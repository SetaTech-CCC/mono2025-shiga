#include "mono2025.h"

/*
 * --- 動作検証プログラム ---
 * 実行時は必ずトグルスイッチを下げること。
 * フォトインタラプタモード中にトグルを上げると、高音が鳴り、ステッパー制御モードになる。
 *
 * --- フォトインタラプタモード ---
 * DC モーター が左回転し、その間、フォトインタラプタ の通過数を 7セグ で表示。
 * カウント数が 50 を超えた場合、低音から ブザー を鳴らし、DC モーター は右回転する。カウントはリセット。
 * また超えた場合は、高音から ブザー を鳴らし、DC モーター を停止。
 *
 * --- 可変抵抗器モード ---
 * DCモーター 制御停止時に トグルスイッチ を下げると
 * 7セグ に 可変抵抗器 の状態を表示する。
 * このタイミングで、7セグ は回路の仕様上、微点灯になる。
 * 
 * --- ステッパー制御モード ----
 * DCモーター 制御停止時に トグルスイッチ を上げて
 * タクトスイッチ を押すと ステッピングモーター を制御可能。
 * タクトを押す度に、その方向に3°動く。
 */

void initialize() {
}

// プログラムの動作モードを定義
enum ProgramMode {
  INIT,                // 初期化
  MODE_PHOTO_COUNT_L,  // フォトインタラプタモード（DC 左）
  MODE_PHOTO_COUNT_R,  // フォトインタラプタモード（DC 右）
  MODE_POTENTIOMETER,  // 可変抵抗器モード
  MODE_STEPPER_CONTROL // ステッパー制御モード
};

// 実行
void loop() {
  // 現在のモードを保持する変数
  static ProgramMode currentMode = INIT;
  // ７セグ左の数値
  static byte seg_l = 0; // n
  // ７セグ右の数値
  static byte seg_r = 0; // z
  // トグルスイッチの状態を保持
  boolean toggle = isToggleEnabled();

  // 現在の状態に基づき分岐
  switch (currentMode) {

    case INIT:
      // DC モーターを左回転
      dc(LT);
      // フォトインタラプタモード（DC左）へ
      currentMode = MODE_PHOTO_COUNT_L;

    // フォトインタラプタモード（DC左）
    case MODE_PHOTO_COUNT_L:
      // 7セグ [n][z]
      seg(num[seg_l], num[seg_r]);
      // フォトインタラプタの通過を検知
      if (isPhotoPassed()) {
        // 一の位を＋１ ([n][z+1])
        seg_r++;
        // [n][10] は再現不可なので、[n+1][0]に変換
        if (seg_r == 10) {
          // 一の位を 0 ([n][0])
          seg_r = 0;
          // 十の位を＋１ ([n+1][0])
          seg_l++;
          // ブザー
          buzz(LO, 0.2);
        }
        // フォト検知数が 50 回に到達した時 ([5][0])
        if (seg_l == 5) {
          // 7セグリセット ([0][0])
          seg_l = 0;
          // 50 回検知したら停止
          dc(S);
          // ブザー
          chgBuzz();
          // DC モーター右回り
          dc(RT);
          // フォトインタラプタモード（DC右）へ
          currentMode = MODE_PHOTO_COUNT_R;
        }
      }
      // トグルが上がったらモード移行
      if (toggle) {
        // DC モーターを静止
        dc(S);
        // LED
        led(R);
        // ブザー
        errBuzz();
        // ステッパー制御モードへ
        currentMode = MODE_STEPPER_CONTROL;
      }
      break;

    // フォトインタラプタモード（DC右）
    case MODE_PHOTO_COUNT_R:
      // 7セグ [n][z]
      seg(num[seg_l], num[seg_r]);
      // フォトインタラプタの通過を検知
      if (isPhotoPassed()) {
        // 一の位を＋１ ([n][z+1])
        seg_r++;
        // [n][10] は再現不可なので、[n+1][0]に変換
        if (seg_r == 10) {
          // 一の位を 0 ([n][0])
          seg_r = 0;
          // 十の位を＋１ ([n+1][0])
          seg_l++;
          // ブザー
          buzz(HI, 0.2);
        }
        // フォト検知数が 50 回に到達した時 ([5][0])
        if (seg_l == 5) {
          // 7セグリセット ([0][0])
          seg_l = 0;
          // 100 回検知したら停止
          dc(S);
          // ブザー
          chgBuzz(true);
          // ステッパー制御モードへ
          currentMode = MODE_STEPPER_CONTROL;
        }
      }
      // トグルが上がったらモード移行
      if (toggle) {
        // DC モーターを静止
        dc(S);
        // LED
        led(R);
        // ブザー
        errBuzz();
        // ステッパー制御モードへ
        currentMode = MODE_STEPPER_CONTROL;
      }
      break;

    // 可変抵抗器モード
    case MODE_POTENTIOMETER:
      // 可変抵抗器と７セグを同期
      syncPotSeg();
      // トグルが上がったらステッパー制御モードへ
      if (toggle) {
        // LED 点灯
        led(B);
        // ステッパー制御モードへ
        currentMode = MODE_STEPPER_CONTROL;
      }
      break;

    // ステッパー制御モード
    case MODE_STEPPER_CONTROL:
      if (isTactPressed(LS)) {
        for (byte i = 0; i < 15; i++) {
          stepper(true);
        }
      } else if (isTactPressed(RS)) {
        for (byte i = 0; i < 15; i++) {
          stepper(false);
        }
      }
      // 可変抵抗器モードへ
      if (!toggle) {
        // LED 消灯
        led(K);
        // 可変抵抗器モードへ
        currentMode = MODE_POTENTIOMETER;
      }
      break;
  }
}