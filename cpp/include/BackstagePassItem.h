#pragma once
#include "Item.h"

class BackstagePassItem {
  Item &item_;

public:
  explicit BackstagePassItem(Item &item) : item_(item) {}
  void updateQuality() {
    static constexpr int MAX = 50;
    if (item_.quality < MAX)
      item_.quality++;
    if (item_.sellIn < 11 && item_.quality < MAX)
      item_.quality++;
    if (item_.sellIn < 6 && item_.quality < MAX)
      item_.quality++;
    if (item_.sellIn < 1)
      item_.quality = 0;
  }
};