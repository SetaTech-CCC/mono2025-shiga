#include "mono2025.h"

void loop() {
  boolean tact_l = isTactEnabled(LS);
  boolean tact_r = isTactEnabled(RS);
  if (isToggleEnabled()) {
    if (tact_l) {
      dc(RT);
    } else if (tact_r) {
      dc(LT);
    } else if (!tact_l && !tact_r) {
      dc(F);
    }
  } else {
    if (tact_l) {
      dc(RT);
      dc(S);
    } else if (tact_r) {
      dc(LT);
      dc(S);
    } else if (!tact_l && !tact_r) {
      dc(F);
    }
  } 
}