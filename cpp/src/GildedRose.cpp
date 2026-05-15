#include "GildedRose.h"
#include "AgedBrieItem.h"
#include "BackstagePassItem.h"
#include "NormalItem.h"
#include "SulfurasItem.h"

GildedRose::GildedRose(std::vector<Item> &items) : items(items) {}

void GildedRose::updateQuality() {
  for (auto &item : items) { // C++11 범위for
    if (item.name == AGED_BRIE)
      AgedBrieItem{item}.updateQuality();
    else if (item.name == BACKSTAGE_PASS)
      BackstagePassItem{item}.updateQuality();
    else if (item.name == SULFURAS)
      SurfurasItem{item}.updateQuality();
    else
      NormalItem{item}.updateQuality();
    updateSellIn(item); // sellIn 업데이트분리!
  }
}

void GildedRose::updateSellIn(Item &item) {
  if (item.name != SULFURAS)
    item.sellIn--;
}