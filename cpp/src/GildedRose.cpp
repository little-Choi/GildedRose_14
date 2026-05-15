#include "GildedRose.h"

GildedRose::GildedRose(std::vector<Item> &items) : items(items) {}

void GildedRose::updateQuality() {
  for (size_t i = 0; i < items.size(); i++) {
    Item &item = items[i];
    if (item.name == AGED_BRIE || item.name == BACKSTAGE_PASS) {
      if (item.quality < MAX_QUALITY) {
        item.quality++;

        if (item.name == BACKSTAGE_PASS) {
          if (item.sellIn < 11) {
            if (item.quality < MAX_QUALITY) {
              item.quality++;
            }
          }
          if (item.sellIn < 6) {
            if (item.quality < MAX_QUALITY) {
              item.quality++;
            }
          }
        }
      }
    } else if (item.name == SULFURAS) {
    } else {
      if (item.quality > MIN_QUALITY) {
        item.quality--;
      }
    }

    if (item.name != SULFURAS) {
      item.sellIn--;
    }

    if (item.sellIn < 0) {
      if (item.name == AGED_BRIE) {
        if (item.quality < MAX_QUALITY) {
          item.quality++;
        }
      } else {
        if (item.name != BACKSTAGE_PASS) {
          if (item.quality > MIN_QUALITY) {
            if (item.name != SULFURAS) {
              item.quality--;
            }
          }
        } else {
          item.quality = MIN_QUALITY;
        }
      }
    }
  }
}
