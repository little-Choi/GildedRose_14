#include "GildedRose.h"

GildedRose::GildedRose(std::vector<Item>& items) : items(items) {}

namespace {
const char* const AGED_BRIE = "Aged Brie";
const char* const BACKSTAGE_PASS = "Backstage passes to a TAFKAL80ETC concert";
const char* const SULFURAS = "Sulfuras, Hand of Ragnaros";
const char* const CONJURED_PREFIX = "Conjured";
const int MIN_QUALITY = 0;
const int MAX_QUALITY = 50;

bool IsConjured(const Item& item) {
    return item.name.rfind(CONJURED_PREFIX, 0) == 0;
}

void IncreaseQuality(Item& item, int amount) {
    item.quality += amount;
    if (item.quality > MAX_QUALITY) {
        item.quality = MAX_QUALITY;
    }
}

void DecreaseQuality(Item& item, int amount) {
    item.quality -= amount;
    if (item.quality < MIN_QUALITY) {
        item.quality = MIN_QUALITY;
    }
}
}

void GildedRose::updateQuality() {
    for (Item& item : items) {
        if (item.name == SULFURAS) {
            continue;
        }

        if (item.name == AGED_BRIE) {
            IncreaseQuality(item, item.sellIn <= 0 ? 2 : 1);
            item.sellIn -= 1;
            continue;
        }

        if (item.name == BACKSTAGE_PASS) {
            if (item.sellIn <= 0) {
                item.quality = MIN_QUALITY;
            } else if (item.sellIn <= 5) {
                IncreaseQuality(item, 3);
            } else if (item.sellIn <= 10) {
                IncreaseQuality(item, 2);
            } else {
                IncreaseQuality(item, 1);
            }
            item.sellIn -= 1;
            continue;
        }

        DecreaseQuality(item, IsConjured(item) ? (item.sellIn <= 0 ? 4 : 2) : (item.sellIn <= 0 ? 2 : 1));
        item.sellIn -= 1;
    }
}
