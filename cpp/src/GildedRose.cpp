#include <memory>

#include "AgedBrieItem.h"
#include "BackstagePassItem.h"
#include "GildedRose.h"
#include "NormalItem.h"
#include "SulfurasItem.h"

namespace {
constexpr const char *AGED_BRIE = "Aged Brie";
constexpr const char *BACKSTAGE_PASS =
    "Backstage passes to a TAFKAL80ETC concert";
constexpr const char *SULFURAS = "Sulfuras, Hand of Ragnaros";

std::unique_ptr<GildedRoseItem> createItem(Item &item) {
  if (item.name == AGED_BRIE)
    return std::make_unique<AgedBrieItem>(item);
  else if (item.name == BACKSTAGE_PASS)
    return std::make_unique<BackstagePassItem>(item);
  else if (item.name == SULFURAS)
    return std::make_unique<SulfurasItem>(item);
  else
    return std::make_unique<NormalItem>(item);
}
} // namespace

GildedRose::GildedRose(std::vector<Item> &items) : items(items) {}

void GildedRose::updateQuality() {
  for (auto &item : items) {            // C++11 범위for
    auto gildedItem = createItem(item); // Factory
    gildedItem->updateQuality();        // 다형성!
    updateSellIn(item);                 // sellIn 업데이트분리!
  }
}

void GildedRose::updateSellIn(Item &item) {
  if (item.name != SULFURAS)
    item.sellIn--;
}
