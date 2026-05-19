# Gilded Rose 테스트 계획서

## 1. 목적과 범위

본 문서는 C++17 기반 Gilded Rose 구현의 회귀 방지와 신규 `Conjured` 요구사항 검증을 위한 단위 테스트 계획을 정의한다. 테스트 대상은 `GildedRose::updateQuality()`이며, Google Test의 `TEST_F` 기반 테스트 픽스처를 사용해 아이템별 비즈니스 규칙, 경계값, 예외 케이스, 커버리지 목표를 체계적으로 검증한다.

### 테스트 대상

- `cpp/include/GildedRose.h`
- `cpp/src/GildedRose.cpp`
- `cpp/test/GildedRoseTest.cpp`

### 테스트 기준 문서

- `GildedRoseRequirements.txt`
- `docs/requirements_analysis.md`

### 제외 범위

- `Item` 클래스 구조 변경 검증은 제외한다. 요구사항상 `Item` 클래스와 `GildedRose::items` 속성은 변경하지 않는 것이 원칙이다.
- UI, 파일 입출력, 외부 시스템 연동 테스트는 대상이 아니다.
- 성능 테스트는 대상이 아니다. 현재 로직은 소규모 인메모리 벡터 순회이며, 단위 테스트와 커버리지 중심으로 검증한다.

## 2. 테스트 전략

### 기본 방향

1. 모든 테스트는 `GildedRose::updateQuality()`를 1회 호출한 뒤 `sellIn`, `quality`의 사후 상태를 검증한다.
2. 아이템 타입별 정상 동작을 먼저 고정한 뒤, `quality`와 `sellIn` 경계값을 조합해 회귀 가능성이 높은 분기를 보강한다.
3. 신규 `Conjured` 규칙은 기존 Normal 아이템과 비교 가능한 케이스로 구성해 "일반 아이템보다 2배 빠른 감소"를 명확히 검증한다.
4. `quality` 하한 0, 일반 상한 50, Sulfuras 예외 상한 80은 모든 관련 타입에서 반복 검증한다.
5. Backstage Pass의 증가 구간은 업데이트 전 `sellIn` 기준으로 판단되므로 `11`, `10`, `6`, `5`, `1`, `0`, `-1`을 우선 검증한다.

### 권장 테스트 구조

기존 `TEST(GildedRoseTest, ...)` 단일 테스트는 다음과 같이 `TEST_F` 기반 픽스처로 확장한다.

```cpp
class GildedRoseTest : public ::testing::Test {
protected:
    std::vector<Item> items;

    void update(const std::string& name, int sellIn, int quality) {
        items = {Item(name, sellIn, quality)};
        GildedRose app(items);
        app.updateQuality();
    }

    const Item& item() const {
        return items[0];
    }
};
```

테스트명은 `ItemType_State_ExpectedBehavior` 패턴을 권장한다.

예:

- `TEST_F(GildedRoseTest, Normal_SellInPositive_DecreasesQualityByOne)`
- `TEST_F(GildedRoseTest, AgedBrie_Quality49AfterSellDate_DoesNotExceed50)`
- `TEST_F(GildedRoseTest, BackstagePass_SellInZero_DropsQualityToZero)`
- `TEST_F(GildedRoseTest, Sulfuras_Expired_DoesNotChange)`
- `TEST_F(GildedRoseTest, Conjured_SellInZero_DecreasesQualityByFour)`

## 3. TEST_F 기반 단위 테스트 범위와 우선순위

### P0: 핵심 요구사항 회귀 방지

| 영역 | 테스트 목적 | 대표 케이스 |
| --- | --- | --- |
| Normal | 일반 아이템의 기본 감소와 만료 후 2배 감소 검증 | `sellIn 10, quality 20 -> 9, 19`; `sellIn 0, quality 20 -> -1, 18` |
| 품질 하한 | 어떤 감소 로직도 quality를 0 미만으로 만들지 않음 | Normal `quality 0`; Normal 만료 `quality 1`; Conjured 만료 `quality 1` |
| Aged Brie | 시간 경과에 따라 quality 증가, 만료 후 2배 증가 | `sellIn 10, quality 20 -> 9, 21`; `sellIn 0, quality 20 -> -1, 22` |
| 품질 상한 | 일반 아이템의 quality는 50을 초과하지 않음 | Aged Brie `quality 50`; Backstage Pass `quality 49` |
| Backstage Pass | 구간별 증가량과 콘서트 이후 0 처리 | `sellIn 11`, `10`, `5`, `0`, `-1` |
| Sulfuras | 전설 아이템은 sellIn과 quality가 변하지 않음 | `sellIn 0, quality 80`; `sellIn -1, quality 80` |
| Conjured | 신규 요구사항: Normal의 2배 속도로 감소 | `sellIn 10, quality 20 -> 9, 18`; `sellIn 0, quality 20 -> -1, 16` |

### P1: 경계값과 분기 정확도

| 영역 | 테스트 목적 | 대표 케이스 |
| --- | --- | --- |
| Backstage Pass 구간 경계 | 증가량 전환점 오프바이원 방지 | `sellIn 11/10`, `6/5`, `1/0` |
| Aged Brie 상한 근처 | 만료 후 2 증가 시에도 50 초과 방지 | `quality 49`, `sellIn 0`; `quality 49`, `sellIn -1` |
| Conjured 하한 근처 | 감소량이 quality보다 큰 경우 0으로 보정 | `quality 1`, `sellIn 10`; `quality 3`, `sellIn 0` |
| 이름 판별 | 정확한 특수 이름과 Conjured 접두사 판별 | `"Conjured Mana Cake"`; `"Sulfuras"`는 일반 아이템 취급 |
| 다중 아이템 | 벡터 내 여러 아이템이 서로 독립적으로 갱신됨 | Normal, Aged Brie, Sulfuras, Backstage, Conjured 혼합 |

### P2: 유지보수성 보강

| 영역 | 테스트 목적 | 대표 케이스 |
| --- | --- | --- |
| 빈 목록 | 아이템이 없어도 예외 없이 종료 | empty `std::vector<Item>` |
| 반복 업데이트 | 여러 날 연속 업데이트 시 누적 규칙 유지 | Normal 2일, Backstage concert 이후, Aged Brie 상한 도달 |
| 비표준 초기 quality | 입력 데이터가 이미 범위를 벗어난 경우 현재 정책 명확화 | Sulfuras `quality 80`; 일반 아이템 `quality 51`은 정책 결정 필요 |

## 4. 경계값 케이스 목록

아래 경계값은 테스트 데이터 표 또는 개별 `TEST_F`로 구성한다. 최소 목표는 각 아이템 타입별로 `quality 0, 1, 49, 50`과 `sellIn 0, -1`의 핵심 조합을 포함하는 것이다.

### quality 경계값

| quality | 검증 의미 | 우선 적용 타입 | 기대 결과 |
| --- | --- | --- | --- |
| `0` | 하한 유지 | Normal, Conjured | 감소 후에도 `0` |
| `1` | 하한 근접 감소 | Normal, Conjured | 만료 후 감소량이 커도 `0` 미만 금지 |
| `49` | 상한 근접 증가 | Aged Brie, Backstage Pass | 증가 후 `50` 초과 금지 |
| `50` | 일반 상한 유지 | Aged Brie, Backstage Pass | 증가하지 않고 `50` 유지 |

### sellIn 경계값

| sellIn | 검증 의미 | 우선 적용 타입 | 기대 결과 |
| --- | --- | --- | --- |
| `0` | 판매 기한 당일, 업데이트 후 만료 상태 | 전체 타입 | Normal은 quality 2 감소, Aged Brie는 2 증가, Backstage는 0, Conjured는 4 감소 |
| `-1` | 이미 만료된 상태 | 전체 타입 | Normal은 quality 2 감소, Aged Brie는 2 증가, Backstage는 0, Conjured는 4 감소 |

### 상세 조합

| 아이템 타입 | 입력 | 기대 결과 | 우선순위 |
| --- | --- | --- | --- |
| Normal | `sellIn 0, quality 0` | `sellIn -1, quality 0` | P0 |
| Normal | `sellIn 0, quality 1` | `sellIn -1, quality 0` | P0 |
| Normal | `sellIn -1, quality 1` | `sellIn -2, quality 0` | P0 |
| Normal | `sellIn 0, quality 49` | `sellIn -1, quality 47` | P1 |
| Normal | `sellIn -1, quality 50` | `sellIn -2, quality 48` | P1 |
| Aged Brie | `sellIn 0, quality 49` | `sellIn -1, quality 50` | P0 |
| Aged Brie | `sellIn -1, quality 49` | `sellIn -2, quality 50` | P0 |
| Aged Brie | `sellIn 0, quality 50` | `sellIn -1, quality 50` | P0 |
| Backstage Pass | `sellIn 0, quality 49` | `sellIn -1, quality 0` | P0 |
| Backstage Pass | `sellIn -1, quality 50` | `sellIn -2, quality 0` | P0 |
| Backstage Pass | `sellIn 5, quality 49` | `sellIn 4, quality 50` | P0 |
| Backstage Pass | `sellIn 10, quality 49` | `sellIn 9, quality 50` | P0 |
| Sulfuras | `sellIn 0, quality 80` | `sellIn 0, quality 80` | P0 |
| Sulfuras | `sellIn -1, quality 80` | `sellIn -1, quality 80` | P0 |
| Conjured | `sellIn 0, quality 1` | `sellIn -1, quality 0` | P0 |
| Conjured | `sellIn 0, quality 49` | `sellIn -1, quality 45` | P0 |
| Conjured | `sellIn -1, quality 50` | `sellIn -2, quality 46` | P0 |

## 5. 예외 및 특이 케이스 목록

### Sulfuras 예외

- `"Sulfuras, Hand of Ragnaros"`는 `sellIn`과 `quality`가 모두 변경되지 않아야 한다.
- `quality`는 일반 상한 50의 예외로 `80`을 유지한다.
- `sellIn 0`, `sellIn -1`에서도 변경되지 않아야 한다.

### Backstage Pass 특이 케이스

- 업데이트 전 `sellIn > 10`: quality 1 증가.
- 업데이트 전 `6 <= sellIn <= 10`: quality 2 증가.
- 업데이트 전 `1 <= sellIn <= 5`: quality 3 증가.
- 업데이트 전 `sellIn == 0` 또는 이미 `sellIn < 0`: 업데이트 후 quality는 0.
- 증가 구간에서 quality가 49 또는 50이면 50을 초과하지 않아야 한다.

### Conjured 신규 특이 케이스

- `"Conjured Mana Cake"`처럼 `"Conjured"`로 시작하는 이름을 Conjured 아이템으로 처리하는 기준을 권장한다.
- 판매 기한 전에는 quality 2 감소, 판매 기한 당일 및 이후에는 quality 4 감소.
- 감소량보다 quality가 작으면 0으로 보정해야 한다.
- Sulfuras, Aged Brie, Backstage Pass 규칙과 충돌하지 않도록 특수 아이템 분기를 우선해야 한다.

### 이름 판별 특이 케이스

- `"Aged Brie"`, `"Backstage passes to a TAFKAL80ETC concert"`, `"Sulfuras, Hand of Ragnaros"`는 정확히 일치해야 한다.
- `"Sulfuras"` 또는 `"Backstage passes"`처럼 일부만 일치하는 이름은 일반 아이템으로 처리되는지 정책을 명확히 하고 테스트로 고정한다.
- Conjured는 요구사항상 카테고리로 해석할 수 있으므로 `"Conjured"` 접두사 기준 테스트를 우선한다.

### 입력 데이터 특이 케이스

- 빈 `items` 벡터: `updateQuality()`가 예외 없이 종료되어야 한다.
- 여러 아이템이 있는 벡터: 각 아이템의 업데이트가 독립적으로 수행되어야 한다.
- 일반 아이템의 초기 quality가 50을 초과하거나 음수인 경우: 요구사항은 업데이트 후 경계를 보장한다는 관점이므로, 입력 정규화 정책을 별도 결정한 뒤 테스트 추가 여부를 확정한다.

## 6. 커버리지 목표

### 목표

- 라인 커버리지: 90% 이상
- 브랜치 커버리지: 85% 이상
- `GildedRose::updateQuality()` 함수 커버리지: 100%
- 신규 `Conjured` 분기 추가 시 해당 분기 라인/브랜치 커버리지: 100%

현재 `updateQuality()`는 중첩 조건문이 많아 라인 커버리지보다 브랜치 커버리지가 더 중요하다. 특히 아래 분기는 반드시 커버한다.

- Normal 감소 전/후 `quality > 0`
- Aged Brie `quality < 50`
- Backstage Pass의 `sellIn < 11`, `sellIn < 6`, 만료 후 quality 0
- Sulfuras 변경 금지
- Conjured 판매 기한 전/후 감소량

## 7. gcov/lcov 측정 전략

### 권장 빌드 방식

GCC 또는 Clang 계열 컴파일러에서 커버리지 플래그를 활성화한 별도 빌드 디렉터리를 사용한다.

```bash
cmake -S cpp -B build-coverage -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="--coverage -O0 -g" \
  -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build build-coverage
ctest --test-dir build-coverage --output-on-failure
```

Windows 환경에서는 MinGW/MSYS2 또는 WSL의 GCC 기반 도구chain을 사용하는 것을 권장한다. MSVC 사용 시에는 gcov/lcov 대신 Visual Studio Coverage, OpenCppCoverage 등 별도 도구가 필요하다.

### lcov 수집 명령

```bash
lcov --capture --directory build-coverage --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/_deps/*' '*/test/*' --output-file coverage.filtered.info
genhtml coverage.filtered.info --output-directory coverage-report
```

### 확인 기준

```bash
lcov --summary coverage.filtered.info
```

판정 기준:

- `lines......: 90.0%` 이상
- `branches...: 85.0%` 이상
- `cpp/src/GildedRose.cpp`의 함수 커버리지 100%

## 8. 커버리지 개선 전략

1. 라인 미커버리지가 있으면 먼저 해당 라인이 어떤 아이템 타입 분기인지 식별하고 P0/P1 테스트에 추가한다.
2. 브랜치 미커버리지는 `quality == 0`, `quality == 50`, `sellIn == 0`, `sellIn == -1` 케이스로 보강한다.
3. Backstage Pass는 구간 전환점 중심으로 테스트한다. `11 -> 10`, `10 -> 9`, `6 -> 5`, `5 -> 4`, `0 -> -1`이 핵심이다.
4. Conjured 추가 후 Normal 테스트를 복사하지 말고, 감소량 차이를 명시하는 별도 테스트명과 기대값을 사용한다.
5. 커버리지 리포트에서 `_deps`, Google Test, 테스트 소스는 제외하고 제품 코드인 `cpp/src/GildedRose.cpp` 중심으로 측정한다.
6. 커버리지 목표를 만족하더라도 요구사항별 대표 케이스가 누락되면 테스트 완료로 보지 않는다.

## 9. 테스트 작성 순서

1. `TEST_F` 픽스처를 도입하고 기존 테스트를 Normal 하한 케이스로 이전한다.
2. Normal, Aged Brie, Backstage Pass, Sulfuras의 P0 테스트를 먼저 작성해 기존 동작을 고정한다.
3. Conjured P0 테스트를 추가하고 현재 구현에서 실패하는 것을 확인한다.
4. `GildedRose::updateQuality()`에 Conjured 로직을 반영한 뒤 모든 P0 테스트를 통과시킨다.
5. P1 경계값 테스트와 다중 아이템 테스트를 추가한다.
6. gcov/lcov로 커버리지를 측정하고 목표 미달 분기를 보강한다.

## 10. 완료 기준

- 모든 P0 테스트가 통과한다.
- P1 경계값 테스트가 통과한다.
- `ctest --output-on-failure` 실행 결과 실패가 없다.
- `cpp/src/GildedRose.cpp` 기준 라인 커버리지 90% 이상, 브랜치 커버리지 85% 이상을 달성한다.
- 신규 Conjured 요구사항 테스트가 존재하며, 판매 기한 전/당일/이후와 quality 하한을 모두 검증한다.
- 테스트명만 보아도 아이템 타입, 상태, 기대 동작을 이해할 수 있다.
