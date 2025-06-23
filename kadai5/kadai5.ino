#include "mono2025.h"

void loop() {
  boolean tact_l = isTactEnabled(LS);
  boolean tact_r = isTactEnabled(RS);
  boolean toggle = isToggleEnabled();
  if (tact_l) {
    dc(RT);
    if (toggle) dc(S);
  } else if (tact_r) {
    dc(LT);
    if (toggle) dc(S);
  } else if (!tact_l && !tact_r) {
    dc(F);
  }
}
