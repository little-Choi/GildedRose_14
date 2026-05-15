#pragma once

#include "GildedRoseItem.h"
#include "Item.h"

class AgedBrieItem : public GildedRoseItem {
public:
  using GildedRoseItem::GildedRoseItem;
  void updateQuality() override {
    static constexpr int MAX = 50;
    if (item_.quality < MAX)
      item_.quality++;
    if (item_.sellIn < 1)
      if (item_.quality < MAX)
        item_.quality++;
  }
};