#include "mono2025.h"

void initialize() {
}

void loop() {
  static byte seg_l = 0;
  static byte seg_r = 0;
  static boolean back = false;

  seg(num[seg_l], num[seg_r]);

  if (isTactPressed(LS) && !back) {
    if (seg_r == 5 && seg_l == 1) {
      return;
    }
    seg_r++;
    if (seg_r == 10) {
      seg_l = 1;
      seg_r = 0;
    }
  }
  if (isTactPressed(RS) && !back) {
    dc(RT);
    back = true;
  }
  if (back) {
    if (isPhotoPassed(4)) {
      if (seg_l == 1 && seg_r <= 0) {
        seg_l = 0;
        seg_r = 10;
      }
      if (seg_l == 0 && seg_r == 0) {
        dc(F);
        back = false;
        return;
      }
      seg_r--;
    }
  }
}