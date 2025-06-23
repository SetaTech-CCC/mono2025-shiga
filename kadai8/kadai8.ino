#include "mono2025.h"

void loop() {
  // カウンターが動作中かどうかのフラグ
  static boolean timer = false;
  // 7セグLEDに表示する0-99のカウンター
  static byte count = 0;
  // 最後にカウントアップした時刻
  static float last_time;
  // カウンターを停止した時刻
  static float stop_time;

  // 左タクトスイッチが押された時
  if (isTactPressed(LS)) {
    // 停止状態から開始する場合のみ、カウント開始時刻をリセット
    if (!timer) {
      last_time = secs();
    }
    // タイマーフラグを立ち上げ
    timer = true;
  }

  // 右タクトスイッチが押された時
  if (isTactPressed(RS)) {
    // 時刻を記録
    if (timer) {
      stop_time = secs();
    }
    // フラグを折る
    timer = false;
  }

  // 右タクトスイッチが押され続け、かつ停止から2秒経過したらカウンターを0にリセット
  // 更にタイマーが予め停止していることが前提
  if (!timer && isTactEnabled(RS) && secs() - stop_time > 2.0) {
    // カウンターを初期化
    count = 0;
  }

  // カウンターが有効で、かつ前回のカウントから0.1秒以上経過した場合に実行
  if (timer && secs() - last_time >= 0.1) {
    // 基準時刻を更新
    last_time = secs();
    // カウント増加
    count++;
    // カウンターオーバーフロー時
    if (count == 100) {
      // 値を初期化
      count = 0;
    }
  }

  // LEDの状態を更新 (カウンター動作中:点灯, 停止中:微灯)
  if (timer) {
    led(B);
  } else {
    led(B, true);
  }

  // トグルスイッチがONなら、常に7セグを点灯
  if (isToggleEnabled()) {
    // count の10の位と1の位を計算して表示
    seg(num[count / 10], num[count % 10], true);
  }
}
