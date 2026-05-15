#include "GildedRose.h"
#include <gtest/gtest.h>

// TC1 : 품질 0인 일반 아이템 품질 감소하지 않음
TEST(GildedRoseTest, NormalItemQuality1) {
  std::vector<Item> items = {Item("Normal", 0, 0)};
  GildedRose app(items);
  app.updateQuality();
  EXPECT_EQ(app.items[0].sellIn, -1);
  EXPECT_GE(app.items[0].quality, 0);
}
