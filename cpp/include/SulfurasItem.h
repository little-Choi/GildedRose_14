#pragma once
#include "Item.h"

class SurfurasItem {
  Item &item_;

public:
  explicit SurfurasItem(Item &item) : item_(item) {}
  void updateQuality() { /* 변화없음*/ }
};