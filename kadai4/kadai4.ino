#include "mono2025.h"

void loop() {
  if (isTactPressed(LS)) {
    for (byte i = 1; i <= 15; i++) stepper(!isToggleEnabled());
    buzz(MI, 0.2);
  }
}