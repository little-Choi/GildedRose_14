#include "GildedRose.h"

GildedRose::GildedRose(std::vector<Item> &items) : items(items) {}

void GildedRose::updateQuality() {
  for (auto &item : items) { // C++11 범위for
    if (item.name == AGED_BRIE)
      updateAgedBrie(item);
    else if (item.name == BACKSTAGE_PASS)
      updateBackstagePass(item);
    else if (item.name == SULFURAS)
      updateSulfuras(item);
    else
      updateNormalItem(item);
    updateSellIn(item); // sellIn 업데이트분리!
  }
}

void GildedRose::updateAgedBrie(Item &item) {
  if (item.quality < MAX_QUALITY)
    item.quality++;
  if (item.sellIn < 1)
    if (item.quality < MAX_QUALITY)
      item.quality++;
}
void GildedRose::updateBackstagePass(Item &item) {
  if (item.quality < MAX_QUALITY)
    item.quality++;
  if (item.sellIn < 11 && item.quality < MAX_QUALITY)
    item.quality++;
  if (item.sellIn < 6 && item.quality < MAX_QUALITY)
    item.quality++;
  if (item.sellIn < 1)
    item.quality = 0;
}
void GildedRose::updateSulfuras(Item &) { /* 변화없음*/ }
void GildedRose::updateNormalItem(Item &item) {
  if (item.quality > 0)
    item.quality--;
  if (item.sellIn < 1 && item.quality > 0)
    item.quality--;
}
void GildedRose::updateSellIn(Item &item) {
  if (item.name != SULFURAS)
    item.sellIn--;
}