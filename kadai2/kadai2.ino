#include "mono2025.h"

void loop() {
  byte seg_l = isTactEnabled(LS) ? SEG_C3 : SEG_L2 + SEG_C2 + SEG_R2;
  byte seg_r = isTactEnabled(RS) ? SEG_C3 : SEG_L2 + SEG_C2 + SEG_R2;
  seg(seg_l, seg_r);
}