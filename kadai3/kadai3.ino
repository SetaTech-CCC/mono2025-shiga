#include "mono2025.h"

void loop() {
  static byte tact_count = 1;
  if (isTactPressed(LS)) tact_count++;
  switch(tact_count) {
    case 1: led(G); break;
    case 2: led(B); break;
    case 3: led(C); break;
    case 4: led(R); break;
    case 5: led(Y); break;
    case 6: led(M); break;
    case 7: led(W); tact_count = 0; break;
  }
}