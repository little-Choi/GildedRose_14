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

// TC2 : 유통기한 지난 일반 아이템 품질 2 감소
TEST(GildedRoseTest, NormalItemQuality2) {
  std::vector<Item> items = {Item("Normal", 0, 5)};
  GildedRose app(items);
  app.updateQuality();
  EXPECT_EQ(app.items[0].sellIn, -1);
  EXPECT_EQ(app.items[0].quality, 3);
}

// TC3 : 전설 아이템 품질 변화 없음
TEST(GildedRoseTest, LegendItemQuality1) {
  std::vector<Item> items = {Item("Sulfuras, Hand of Ragnaros", 3, 5)};
  GildedRose app(items);
  app.updateQuality();
  EXPECT_EQ(app.items[0].sellIn, 3);
  EXPECT_EQ(app.items[0].quality, 5);
}

// TC4 : 유통기한 지난 전설 아이템 품질 변화 없음
TEST(GildedRoseTest, LegendItemQuality2) {
  std::vector<Item> items = {Item("Sulfuras, Hand of Ragnaros", -1, 5)};
  GildedRose app(items);
  app.updateQuality();
  EXPECT_EQ(app.items[0].sellIn, -1);
  EXPECT_EQ(app.items[0].quality, 5);
}

// TC5 : Aged Brie 아이템 유통기한 지나면 품질 2 증가
TEST(GildedRoseTest, AgedBrieItemQuality1) {
  std::vector<Item> items = {Item("Aged Brie", 0, 0)};
  GildedRose app(items);
  app.updateQuality();
  EXPECT_EQ(app.items[0].sellIn, -1);
  EXPECT_EQ(app.items[0].quality, 2);
}

// TC6 : Aged Brie 아이템 최대 품질 50으로 더 이상 증가하지 않음
TEST(GildedRoseTest, AgedBrieItemQuality2) {
  std::vector<Item> items = {Item("Aged Brie", 0, 50)};
  GildedRose app(items);
  app.updateQuality();
  EXPECT_EQ(app.items[0].sellIn, -1);
  EXPECT_LE(app.items[0].quality, 50);
}
