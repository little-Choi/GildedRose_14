#pragma once

#include "GildedRoseItem.h"
#include "Item.h"

class SulfurasItem : public GildedRoseItem {
public:
  using GildedRoseItem::GildedRoseItem;
  void updateQuality() override {}
};
