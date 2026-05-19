# GildedRose `updateQuality()` Code Quality Report

대상 파일:

- `cpp/include/GildedRose.h`
- `cpp/src/GildedRose.cpp`

분석 기준: SOLID, Code Smell, C++17 리팩토링 가능성  
우선순위 기준: `1`이 가장 시급하고, `5`가 가장 낮음

## 문제점 분석

| 문제점 | 위반 원칙/스멜 | 영향 | 개선 방향 | 우선순위 |
|---|---|---|---|---|
| `updateQuality()`가 재고 순회, 상품 판별, 품질 변경, 판매기한 감소, 만료 후 처리, 품질 상한/하한 보정을 모두 담당함 | SRP 위반, Long Method | 하나의 변경이 전체 메서드 조건문에 영향을 주며, 새 상품 규칙 추가 시 회귀 가능성이 커짐 | `updateQuality()`는 순회만 담당하고, `updateItem(Item&)`, `updateSellIn(Item&)`, `adjustQuality(Item&, int)` 또는 상품별 정책 객체로 책임 분리 | 1 |
| 상품명이 문자열 비교로 하드코딩되어 있고, 상품별 규칙이 `if/else` 체인에 직접 포함됨 | OCP 위반, Stringly Typed Code | `Conjured` 같은 신규 상품을 추가하려면 기존 메서드를 수정해야 하므로 기존 동작을 깨뜨릴 위험이 큼 | `ItemPolicy` 전략 인터페이스와 상품명 기반 정책 레지스트리, 또는 `classifyItem()` + `switch`/테이블 기반 디스패치 도입 | 1 |
| `0`, `50`, `11`, `6`, `1` 같은 도메인 값이 코드에 직접 노출됨 | Magic Number | 숫자의 의미가 코드만으로 드러나지 않고, 백스테이지 임계값처럼 `sellIn < 11`이 "10일 이하"를 뜻하는 규칙이 숨겨짐 | `constexpr int MinQuality = 0;`, `MaxQuality = 50;`, `BackstageDoubleThreshold = 10;`, `BackstageTripleThreshold = 5;`, `NormalDegrade = 1;` 등으로 상수화 | 2 |
| `quality < 50`, `quality > 0`, `quality = quality +/- 1` 패턴이 여러 위치에 반복됨 | Duplicated Code | 품질 경계값 보정이 분산되어 누락되기 쉽고, 증가/감소 규칙 변경 시 여러 지점을 함께 수정해야 함 | `adjustQuality(Item&, int delta)`에서 `std::clamp`로 경계 보정, `setQualityToMin(Item&)` 같은 명시적 헬퍼 도입 | 2 |
| 중첩 조건문이 깊고 부정 조건(`!=`)이 많음 | 조건문 복잡도, High Cognitive Complexity | 정상 상품, 브리, 백스테이지, 설퍼라스의 흐름을 한 번에 추적해야 하며, 만료 전/후 동작을 검증하기 어려움 | 조기 반환, 상품별 함수 분리, 정책 디스패치로 중첩 제거. 예: `if (isLegendary(item)) return;` 이후 일반 흐름 처리 | 2 |
| 판매기한 감소 전/후의 품질 변경 순서가 메서드 내부 흐름에 암묵적으로 묶여 있음 | Temporal Coupling, Hidden Business Rule | 만료 후 두 배 감소 규칙이 `sellIn--` 이후 `sellIn < 0` 검사에 의존해 읽기 어렵고, 순서 변경 시 동작이 달라질 수 있음 | "기본 품질 변경 -> 판매기한 감소 -> 만료 후 보정" 같은 단계 이름을 함수로 분리하거나 정책 내부에 명시 | 3 |
| `Sulfuras` 예외 처리가 품질 변경과 판매기한 감소 조건에 반복됨 | Duplicated Special Case, Shotgun Surgery | 전설 아이템 예외가 여러 위치에 흩어져 있어 누락 가능성이 있고, 다른 불변 상품이 생기면 수정 지점이 증가함 | `SulfurasPolicy`에서 아무 작업도 하지 않게 하거나 `isImmutableItem()` 가드로 한 곳에서 처리 | 3 |
| 상품 타입이 `std::string` 값 자체로 표현됨 | Primitive Obsession, Stringly Typed Code | 오타와 이름 변경에 취약하고, 이름과 동작 규칙이 강하게 결합됨 | `enum class ItemKind { Normal, AgedBrie, BackstagePass, Sulfuras, Conjured }`로 분류 후 정책 선택. 단, `Item` 클래스 변경 금지 조건 때문에 외부 분류 함수로 처리 | 4 |
| 새 규칙 추가 방식이 테스트 가능한 작은 단위가 아니라 전체 메서드 검증에 의존함 | Low Testability | `Conjured` 추가 시 전체 `updateQuality()` 시나리오 테스트가 필요하고, 상품별 경계 조건 테스트가 어려움 | 상품별 정책 함수/클래스를 독립 테스트 가능하게 분리하고, 공통 경계 보정 함수에 단위 테스트 집중 | 4 |
| 모든 규칙을 객체 다형성으로만 풀 경우 현재 규모 대비 구조가 과해질 수 있음 | Overengineering Risk | 작은 kata 코드에서 과도한 계층 구조는 가독성을 오히려 떨어뜨릴 수 있음 | 1차 리팩토링은 헬퍼 함수 + 테이블 기반 디스패치로 시작하고, 규칙 수가 늘어날 때 전략 패턴으로 확장 | 5 |

## C++17 스타일 개선 방향 요약

1. **상수화부터 적용**  
   품질 경계값, 백스테이지 임계값, 감소/증가 단위, 상품명 문자열을 `constexpr` 또는 `std::string_view` 상수로 분리한다. 가장 작고 안전한 변경이며, 이후 리팩토링의 의도를 선명하게 만든다.

2. **품질 보정 공통 함수 도입**  
   `adjustQuality(Item&, int delta)`를 만들고 내부에서 `std::clamp`를 사용한다. `quality < 50`, `quality > 0` 반복을 제거하고 품질 불변식(`0 <= quality <= 50`)을 한 곳에 모은다. 단, `Sulfuras`의 품질 80 예외는 별도 정책으로 분리한다.

3. **상품별 업데이트 함수로 분리**  
   `updateNormal`, `updateAgedBrie`, `updateBackstagePass`, `updateSulfuras`, `updateConjured`처럼 규칙 단위를 나눈다. 이 단계만으로도 SRP 위반과 Long Method 문제가 크게 줄어든다.

4. **테이블 기반 디스패치 또는 전략 패턴 적용**  
   C++17에서는 `std::unordered_map<std::string_view, UpdateFunction>` 형태의 테이블 기반 디스패치가 간결하다. 규칙이 더 복잡해지면 `ItemPolicy` 인터페이스와 정책 클래스로 확장해 OCP를 강화할 수 있다.

5. **`std::variant`는 선택적으로 검토**  
   `std::variant<NormalItem, AgedBrie, BackstagePass, Sulfuras, Conjured>`는 타입 안정성이 높지만, 현재 요구사항은 `Item` 클래스와 `items` 속성을 변경하지 말라는 제약이 있다. 따라서 현 단계에서는 직접 적용보다 `ItemKind` 분류 함수나 정책 디스패치가 현실적이다.

## 권장 리팩토링 순서

1. `constexpr` 상수와 상품명 상수를 도입한다.
2. `adjustQuality()`로 품질 경계값 보정을 중앙화한다.
3. `Sulfuras`를 조기 반환 또는 별도 정책으로 분리한다.
4. 상품별 업데이트 함수를 분리해 `updateQuality()`를 순회와 디스패치 중심으로 축소한다.
5. `Conjured` 추가 및 향후 확장을 위해 테이블 기반 디스패치 또는 전략 패턴으로 전환한다.
