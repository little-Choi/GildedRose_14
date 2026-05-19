#include "GildedRose.h"
#include <gtest/gtest.h>

class GildedRoseTest : public ::testing::Test {
protected:
    static Item UpdateItem(const std::string& name, int sellIn, int quality) {
        std::vector<Item> items = {Item(name, sellIn, quality)};
        GildedRose app(items);

        app.updateQuality();

        return app.items[0];
    }
};

TEST_F(GildedRoseTest, NormalItemDecreasesQualityAndSellInBeforeSellDate) {
    // Given
    const int initialSellIn = 10;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Normal", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 9);
    EXPECT_EQ(updated.quality, 19);
}

TEST_F(GildedRoseTest, NormalItemQualityDoesNotGoBelowZero) {
    // Given
    const int initialSellIn = 5;
    const int initialQuality = 0;

    // When
    const Item updated = UpdateItem("Normal", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 4);
    EXPECT_EQ(updated.quality, 0);
}

TEST_F(GildedRoseTest, NormalItemDegradesTwiceAsFastOnSellDate) {
    // Given
    const int initialSellIn = 0;
    const int initialQuality = 10;

    // When
    const Item updated = UpdateItem("Normal", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -1);
    EXPECT_EQ(updated.quality, 8);
}

TEST_F(GildedRoseTest, NormalItemDegradesTwiceAsFastAfterSellDate) {
    // Given
    const int initialSellIn = -1;
    const int initialQuality = 10;

    // When
    const Item updated = UpdateItem("Normal", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -2);
    EXPECT_EQ(updated.quality, 8);
}

TEST_F(GildedRoseTest, NormalItemExpiredQualityStopsAtZero) {
    // Given
    const int initialSellIn = -1;
    const int initialQuality = 1;

    // When
    const Item updated = UpdateItem("Normal", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -2);
    EXPECT_EQ(updated.quality, 0);
}

TEST_F(GildedRoseTest, AgedBrieIncreasesQualityBeforeSellDate) {
    // Given
    const int initialSellIn = 10;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Aged Brie", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 9);
    EXPECT_EQ(updated.quality, 21);
}

TEST_F(GildedRoseTest, AgedBrieIncreasesTwiceOnSellDate) {
    // Given
    const int initialSellIn = 0;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Aged Brie", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -1);
    EXPECT_EQ(updated.quality, 22);
}

TEST_F(GildedRoseTest, AgedBrieIncreasesTwiceAfterSellDate) {
    // Given
    const int initialSellIn = -1;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Aged Brie", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -2);
    EXPECT_EQ(updated.quality, 22);
}

TEST_F(GildedRoseTest, AgedBrieQualityDoesNotExceedFifty) {
    // Given
    const int initialSellIn = 5;
    const int initialQuality = 50;

    // When
    const Item updated = UpdateItem("Aged Brie", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 4);
    EXPECT_EQ(updated.quality, 50);
}

TEST_F(GildedRoseTest, AgedBrieExpiredQualityCapsAtFifty) {
    // Given
    const int initialSellIn = 0;
    const int initialQuality = 49;

    // When
    const Item updated = UpdateItem("Aged Brie", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -1);
    EXPECT_EQ(updated.quality, 50);
}

TEST_F(GildedRoseTest, BackstagePassIncreasesByOneWhenSellDateIsMoreThanTenDaysAway) {
    // Given
    const int initialSellIn = 11;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Backstage passes to a TAFKAL80ETC concert", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 10);
    EXPECT_EQ(updated.quality, 21);
}

TEST_F(GildedRoseTest, BackstagePassIncreasesByTwoWhenTenDaysRemain) {
    // Given
    const int initialSellIn = 10;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Backstage passes to a TAFKAL80ETC concert", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 9);
    EXPECT_EQ(updated.quality, 22);
}

TEST_F(GildedRoseTest, BackstagePassIncreasesByThreeWhenFiveDaysRemain) {
    // Given
    const int initialSellIn = 5;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Backstage passes to a TAFKAL80ETC concert", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 4);
    EXPECT_EQ(updated.quality, 23);
}

TEST_F(GildedRoseTest, BackstagePassQualityCapsAtFifty) {
    // Given
    const int initialSellIn = 5;
    const int initialQuality = 49;

    // When
    const Item updated = UpdateItem("Backstage passes to a TAFKAL80ETC concert", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 4);
    EXPECT_EQ(updated.quality, 50);
}

TEST_F(GildedRoseTest, BackstagePassQualityDropsToZeroOnSellDate) {
    // Given
    const int initialSellIn = 0;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Backstage passes to a TAFKAL80ETC concert", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -1);
    EXPECT_EQ(updated.quality, 0);
}

TEST_F(GildedRoseTest, BackstagePassQualityStaysZeroAfterSellDate) {
    // Given
    const int initialSellIn = -1;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Backstage passes to a TAFKAL80ETC concert", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -2);
    EXPECT_EQ(updated.quality, 0);
}

TEST_F(GildedRoseTest, SulfurasDoesNotChangeBeforeSellDate) {
    // Given
    const int initialSellIn = 10;
    const int initialQuality = 80;

    // When
    const Item updated = UpdateItem("Sulfuras, Hand of Ragnaros", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 10);
    EXPECT_EQ(updated.quality, 80);
}

TEST_F(GildedRoseTest, SulfurasDoesNotChangeOnSellDate) {
    // Given
    const int initialSellIn = 0;
    const int initialQuality = 80;

    // When
    const Item updated = UpdateItem("Sulfuras, Hand of Ragnaros", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 0);
    EXPECT_EQ(updated.quality, 80);
}

TEST_F(GildedRoseTest, SulfurasDoesNotChangeAfterSellDate) {
    // Given
    const int initialSellIn = -1;
    const int initialQuality = 80;

    // When
    const Item updated = UpdateItem("Sulfuras, Hand of Ragnaros", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -1);
    EXPECT_EQ(updated.quality, 80);
}

TEST_F(GildedRoseTest, SulfurasKeepsQualityEvenWhenQualityIsAtLegendaryBoundary) {
    // Given
    const int initialSellIn = 5;
    const int initialQuality = 80;

    // When
    const Item updated = UpdateItem("Sulfuras, Hand of Ragnaros", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 5);
    EXPECT_EQ(updated.quality, 80);
}

TEST_F(GildedRoseTest, SulfurasKeepsSellInEvenWhenQualityExceedsNormalMaximum) {
    // Given
    const int initialSellIn = -1;
    const int initialQuality = 80;

    // When
    const Item updated = UpdateItem("Sulfuras, Hand of Ragnaros", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -1);
    EXPECT_EQ(updated.quality, 80);
}

TEST_F(GildedRoseTest, ConjuredItemDecreasesQualityTwiceAsFastBeforeSellDate) {
    // Given
    const int initialSellIn = 10;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Conjured Mana Cake", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 9);
    EXPECT_EQ(updated.quality, 18);
}

TEST_F(GildedRoseTest, ConjuredItemDecreasesQualityFourTimesOnSellDate) {
    // Given
    const int initialSellIn = 0;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Conjured Mana Cake", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -1);
    EXPECT_EQ(updated.quality, 16);
}

TEST_F(GildedRoseTest, ConjuredItemDecreasesQualityFourTimesAfterSellDate) {
    // Given
    const int initialSellIn = -1;
    const int initialQuality = 20;

    // When
    const Item updated = UpdateItem("Conjured Mana Cake", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -2);
    EXPECT_EQ(updated.quality, 16);
}

TEST_F(GildedRoseTest, ConjuredItemQualityDoesNotGoBelowZero) {
    // Given
    const int initialSellIn = -1;
    const int initialQuality = 3;

    // When
    const Item updated = UpdateItem("Conjured Mana Cake", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, -2);
    EXPECT_EQ(updated.quality, 0);
}

TEST_F(GildedRoseTest, ConjuredPrefixIsHandledAsConjuredItem) {
    // Given
    const int initialSellIn = 1;
    const int initialQuality = 50;

    // When
    const Item updated = UpdateItem("Conjured Elixir", initialSellIn, initialQuality);

    // Then
    EXPECT_EQ(updated.sellIn, 0);
    EXPECT_EQ(updated.quality, 48);
}
