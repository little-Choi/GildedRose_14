# Gilded Rose 요구사항 분석

## 1. 아이템 타입별 비즈니스 규칙

| 아이템 타입 | 이름 판별 기준 | sellIn 변화 | quality 변화: sellIn > 0 | quality 변화: sellIn <= 0 | quality 경계 | 비고 |
| --- | --- | --- | --- | --- | --- | --- |
| Normal | 특수 아이템 이름에 해당하지 않는 일반 아이템 | 매일 1 감소 | 매일 1 감소 | 매일 2 감소 | 최소 0, 최대 50 | `sellIn == 0`인 상태에서 업데이트하면 `sellIn`이 `-1`이 되고, 기한 경과 감소가 적용되어 총 2 감소한다. |
| Aged Brie | `"Aged Brie"`와 정확히 일치 | 매일 1 감소 | 매일 1 증가 | 매일 2 증가 | 최소 0, 최대 50 | 오래될수록 가치가 증가한다. 단, 50을 초과할 수 없다. |
| Backstage Pass | `"Backstage passes to a TAFKAL80ETC concert"`와 정확히 일치 | 매일 1 감소 | `sellIn > 10`: 1 증가<br>`6 <= sellIn <= 10`: 2 증가<br>`1 <= sellIn <= 5`: 3 증가 | 콘서트 이후 0으로 변경 | 최소 0, 최대 50 | 증가량 판단은 업데이트 전 `sellIn` 기준으로 보는 것이 기존 구현 흐름과 맞다. 업데이트 후 `sellIn < 0`이면 quality는 0이 된다. |
| Sulfuras | `"Sulfuras, Hand of Ragnaros"`와 정확히 일치 | 변하지 않음 | 변하지 않음 | 변하지 않음 | 예외적으로 80 유지 | 전설 아이템이므로 `sellIn`, `quality` 모두 변경하지 않는다. 일반 quality 상한 50 규칙의 예외다. |
| Conjured | 신규 Conjured 분류. 예: 이름이 `"Conjured"`로 시작하는 아이템 | 매일 1 감소 | 매일 2 감소 | 매일 4 감소 | 최소 0, 최대 50 | 일반 아이템 감소율의 2배다. 특수 아이템이 아니므로 Aged Brie, Backstage Pass, Sulfuras 규칙보다 별도 분기되어야 한다. |

## 2. 문자열 비교 및 분기 시 주의점

1. 기존 특수 아이템은 정확한 이름으로 분기한다.
   - `"Aged Brie"`
   - `"Backstage passes to a TAFKAL80ETC concert"`
   - `"Sulfuras, Hand of Ragnaros"`

2. `std::string`의 `operator==` 비교는 대소문자, 공백, 쉼표까지 모두 정확히 일치해야 한다. 예를 들어 `"Sulfuras"`와 `"Sulfuras, Hand of Ragnaros"`는 다른 아이템으로 판정된다.

3. Conjured 판별에 `std::string::find()`를 사용할 경우 반환값을 반드시 `std::string::npos`와 비교해야 한다.
   ```cpp
   item.name.find("Conjured") != std::string::npos
   ```
   `if (item.name.find("Conjured"))`처럼 사용하면 `"Conjured"`가 문자열 맨 앞에 있을 때 반환값이 `0`이므로 false가 되고, 찾지 못했을 때 `npos`가 true처럼 평가될 수 있다.

4. Conjured를 카테고리로 볼지 정확한 이름으로 볼지 구현 기준을 고정해야 한다. 요구사항의 `"Conjured" items`는 일반적으로 `"Conjured Mana Cake"`처럼 이름이 `"Conjured"`로 시작하는 아이템군으로 해석하는 것이 자연스럽다.
   ```cpp
   item.name.rfind("Conjured", 0) == 0
   ```

5. 분기 순서는 Sulfuras 예외를 가장 먼저 처리하는 편이 안전하다. Sulfuras는 `sellIn`과 `quality` 모두 변경하지 않아야 하므로 일반 감소 로직에 들어가면 안 된다.

## 3. 예외 및 경계값 조건

1. `quality`는 일반적으로 0 이상 50 이하를 유지해야 한다.

2. `quality`가 0인 아이템은 감소 로직을 적용해도 음수가 되면 안 된다.

3. `quality`가 49 또는 50인 Aged Brie와 Backstage Pass는 증가 후에도 50을 초과하면 안 된다.

4. Sulfuras는 `quality` 80을 유지하는 예외 아이템이다. 50 상한 규칙을 적용하지 않는다.

5. `sellIn > 0`은 판매 기한이 남아 있는 상태다. Normal은 1 감소, Conjured는 2 감소한다.

6. `sellIn == 0`은 업데이트가 끝나면 `-1`이 되므로 기한 경과 상태로 처리된다. 기존 구현 흐름 기준으로 Normal은 총 2 감소, Conjured는 총 4 감소, Aged Brie는 총 2 증가, Backstage Pass는 0이 된다.

7. `sellIn < 0`은 이미 기한이 지난 상태다. Normal은 2 감소, Conjured는 4 감소, Aged Brie는 2 증가, Backstage Pass는 0이 된다.

8. Backstage Pass의 증가 구간은 업데이트 전 `sellIn` 기준으로 판단한다.
   - `sellIn >= 11`: 1 증가
   - `6 <= sellIn <= 10`: 2 증가
   - `1 <= sellIn <= 5`: 3 증가
   - 업데이트 후 `sellIn < 0`: 0

9. `Item` 클래스와 `GildedRose::items` 속성은 요구사항상 수정하지 않는 것이 원칙이다. C++17 구현에서는 `updateQuality()` 내부 로직이나 보조 함수 추가로 해결한다.

## 4. Conjured 신규 요구사항 명세

1. Conjured 아이템은 일반 아이템과 동일하게 매일 `sellIn`이 1 감소한다.

2. Conjured 아이템은 일반 아이템보다 quality가 2배 빠르게 감소한다.

3. 판매 기한 전(`sellIn > 0`) Conjured의 quality 감소량은 2다.

4. 판매 기한 당일 또는 이후(`sellIn <= 0`) Conjured의 quality 감소량은 4다.

5. Conjured의 quality도 0 미만으로 내려가면 안 된다.

6. Conjured는 Aged Brie, Backstage Pass, Sulfuras의 특수 규칙을 대체하지 않는다. 이름 판별 충돌이 생기면 Sulfuras처럼 변경 금지인 예외 규칙을 우선해야 한다.

7. 권장 테스트용 대표 이름은 `"Conjured Mana Cake"`이다. 구현에서는 `"Conjured"` 접두사 기준으로 판별하면 신규 Conjured 품목 추가에 대응하기 쉽다.

## 5. Google Test 기준 테스트 시나리오

1. Normal: `sellIn > 0`, `quality > 0`이면 업데이트 후 `sellIn`은 1 감소하고 `quality`는 1 감소한다.

2. Normal: `quality == 0`이면 업데이트 후에도 `quality`는 0이다.

3. Normal: `sellIn == 0`이면 업데이트 후 `sellIn == -1`, `quality`는 2 감소한다.

4. Normal: `sellIn < 0`이면 업데이트 후 `quality`는 2 감소한다.

5. Normal: `sellIn < 0`, `quality == 1`이면 업데이트 후 `quality`는 0보다 작아지지 않는다.

6. Aged Brie: `sellIn > 0`이면 업데이트 후 `quality`는 1 증가한다.

7. Aged Brie: `sellIn == 0`이면 업데이트 후 `quality`는 2 증가한다.

8. Aged Brie: `sellIn < 0`이면 업데이트 후 `quality`는 2 증가한다.

9. Aged Brie: `quality == 50`이면 업데이트 후에도 `quality`는 50이다.

10. Aged Brie: `quality == 49`, `sellIn <= 0`이면 업데이트 후 `quality`는 50을 초과하지 않는다.

11. Backstage Pass: `sellIn >= 11`이면 업데이트 후 `quality`는 1 증가한다.

12. Backstage Pass: `sellIn == 10`이면 업데이트 후 `quality`는 2 증가한다.

13. Backstage Pass: `sellIn == 5`이면 업데이트 후 `quality`는 3 증가한다.

14. Backstage Pass: `quality == 49`이고 증가 구간에 있으면 업데이트 후 `quality`는 50을 초과하지 않는다.

15. Backstage Pass: `sellIn == 0`이면 업데이트 후 `quality`는 0이 된다.

16. Backstage Pass: `sellIn < 0`이면 업데이트 후 `quality`는 0이다.

17. Sulfuras: 업데이트 후 `sellIn`은 변하지 않는다.

18. Sulfuras: 업데이트 후 `quality`는 80으로 유지된다.

19. Sulfuras: `sellIn <= 0`이어도 `sellIn`과 `quality`는 변하지 않는다.

20. Conjured: `sellIn > 0`이면 업데이트 후 `quality`는 2 감소한다.

21. Conjured: `sellIn == 0`이면 업데이트 후 `sellIn == -1`, `quality`는 4 감소한다.

22. Conjured: `sellIn < 0`이면 업데이트 후 `quality`는 4 감소한다.

23. Conjured: `quality`가 감소량보다 작으면 업데이트 후 `quality`는 0이다.

24. Conjured: `"Conjured Mana Cake"`처럼 `"Conjured"`로 시작하는 이름이 Conjured 규칙으로 처리된다.
