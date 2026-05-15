#include "GildedRose.h"

GildedRose::GildedRose(std::vector<Item> &items) : items(items) {}

void GildedRose::updateQuality() {
  for (size_t i = 0; i < items.size(); i++) {
    Item &item = items[i];
    if (item.name != AGED_BRIE && item.name != BACKSTAGE_PASS) {
      if (item.quality > MIN_QUALITY) {
        if (item.name != SULFURAS) {
          item.quality = item.quality - 1;
        }
      }
    } else {
      if (item.quality < MAX_QUALITY) {
        item.quality = item.quality + 1;

        if (item.name == BACKSTAGE_PASS) {
          if (item.sellIn < 11) {
            if (item.quality < MAX_QUALITY) {
              item.quality = item.quality + 1;
            }
          }

          if (item.sellIn < 6) {
            if (item.quality < MAX_QUALITY) {
              item.quality = item.quality + 1;
            }
          }
        }
      }
    }

    if (item.name != SULFURAS) {
      item.sellIn = item.sellIn - 1;
    }

    if (item.sellIn < 0) {
      if (item.name != AGED_BRIE) {
        if (item.name != BACKSTAGE_PASS) {
          if (item.quality > MIN_QUALITY) {
            if (item.name != SULFURAS) {
              item.quality = item.quality - 1;
            }
          }
        } else {
          item.quality = item.quality - item.quality;
        }
      } else {
        if (item.quality < MAX_QUALITY) {
          item.quality = item.quality + 1;
        }
      }
    }
  }
}
