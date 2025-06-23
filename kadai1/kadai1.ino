#include "mono2025.h"

void loop() {
  // タクト左の状態を取得
  boolean tact_l = isTactEnabled(LS);
  // タクト右の状態を取得
  boolean tact_r = isTactEnabled(RS);
  // トグルの状態を取得
  boolean toggle = isToggleEnabled();

  // タクト同時押し検知回避
  delay(60);

  // ２つ以上押されている場合
  if ((tact_l + tact_r + toggle) >= 2) {
    // 処理を行わない
    return;
  // タクト左が押されている場合
  } else if (tact_l) {
    // 低音で鳴動
    buzz(LO, 0.1);
  // タクト右が押されている場合
  } else if (tact_r) {
    // 中音で鳴動
    buzz(MI, 0.1);
  // トグルがオンの時
  } else if (toggle) {
    // 高音で鳴動
    buzz(HI, 0.1);
  }
}