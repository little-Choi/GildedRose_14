#pragma once
#include "Item.h"

class NormalItem {
  Item &item_;

public:
  explicit NormalItem(Item &item) : item_(item) {}
  void updateQuality() {
    if (item_.quality > 0)
      item_.quality--;
    if (item_.sellIn < 1 && item_.quality > 0)
      item_.quality--;
  }
};