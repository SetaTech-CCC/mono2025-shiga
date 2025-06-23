/*
  Copyright 2025 Syuugo
  
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
      http://www.apache.org/licenses/LICENSE-2.0
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#ifndef MONO2025_H
#define MONO2025_H

/***********
 * 制御ピン *
 ***********/

// DCモーター
const byte DC_MOTOR_1_PIN = 29;
const byte DC_MOTOR_2_PIN = 30;
// 7セグ
const byte SEG_LEFT_PIN = 43;
const byte SEG_RIGHT_PIN = 42;
// クロック
const byte CLOCK_PIN = 44;
// 書き込みピン
const byte PIN_WRITE[] = { DC_MOTOR_1_PIN, DC_MOTOR_2_PIN, SEG_LEFT_PIN, SEG_RIGHT_PIN, CLOCK_PIN };

// フォトインタラプタ
const byte PHOTO_INTERRUPTER_PIN = 10;
// タクトスイッチ
const byte TACT_SWITCH_LEFT_PIN = 13;
const byte TACT_SWITCH_RIGHT_PIN = 12;
// 読み込みピン
const byte PIN_READ[] = { PHOTO_INTERRUPTER_PIN, TACT_SWITCH_LEFT_PIN, TACT_SWITCH_RIGHT_PIN };

/***********
 * 補助関数 *
 ***********/

// デジタルピンに書き込み
inline void ren(byte pin, byte level) {
  digitalWrite(pin, level);
}

/***************
 * 処理ここから *
 ***************/

/***************
 * クロック制御 *
 ***************/

/*************
 * DCモーター *
 *************/

// DCモーターコマンド間遅延 (ミリ秒)
const byte DC_MOTOR_COMMAND_DELAY_MS = 1;
// DC モーターコントローラー
void dc_ctr(boolean left_ctrl = false, boolean right_ctrl = false) {
  // チャタリング防止
  delay(DC_MOTOR_COMMAND_DELAY_MS);
  // 書き換えのため落とす
  ren(CLOCK_PIN, LOW);
  // ２ピンを４パターンで制御
  ren(DC_MOTOR_1_PIN, left_ctrl ? HIGH : LOW);
  ren(DC_MOTOR_2_PIN, right_ctrl ? HIGH : LOW);
  // クロック立ち上げ
  ren(CLOCK_PIN, HIGH);
}

// DCモーターの動作モードを定義する列挙型
enum DCMotor { LT, RT, S, F };

// DCモーター制御
inline void dc(DCMotor action) {
  switch (action) {
    // 左回転
    case LT:  dc_ctr(true, false); break;
    // 右回転
    case RT: dc_ctr(false, true); break;
    // 即座に停止
    case S: dc_ctr(true, true); break;
    // 減速して停止
    case F: dc_ctr(false, false); break;
  }
}

/*********
 * 7セグ *
 *********/

// セグの表示データ
const byte SEG_A0 = B00000000;  // 消灯
const byte SEG_C1 = B00000010;  // 中央上
const byte SEG_R1 = B00000100;  // 右上
const byte SEG_R2 = B00001000;  // 右下
const byte SEG_C3 = B00010000;  // 中央下
const byte SEG_L2 = B00100000;  // 左下
const byte SEG_L1 = B01000000;  // 左上
const byte SEG_C2 = B10000000;  // 中央真ん中

// int で直接できるように数字のみの配列を用意
const byte num[] = {
  /* 0 */ SEG_L1 + SEG_L2 + SEG_C1 + SEG_C3 + SEG_R1 + SEG_R2,
  /* 1 */ SEG_R1 + SEG_R2,
  /* 2 */ SEG_L2 + SEG_C1 + SEG_C2 + SEG_C3 + SEG_R1,
  /* 3 */ SEG_C1 + SEG_C2 + SEG_C3 + SEG_R1 + SEG_R2,
  /* 4 */ SEG_L1 + SEG_C2 + SEG_R1 + SEG_R2,
  /* 5 */ SEG_L1 + SEG_C1 + SEG_C2 + SEG_C3 + SEG_R2,
  /* 6 */ SEG_L1 + SEG_L2 + SEG_C1 + SEG_C2 + SEG_C3 + SEG_R2,
  /* 7 */ SEG_L1 + SEG_C1 + SEG_R1 + SEG_R2,
  /* 8 */ SEG_L1 + SEG_L2 + SEG_C1 + SEG_C2 + SEG_C3 + SEG_R1 + SEG_R2,
  /* 9 */ SEG_L1 + SEG_C1 + SEG_C2 + SEG_C3 + SEG_R1 + SEG_R2
};

// セグメント切り替え遅延 (マイクロ秒)
const byte SEGMENT_SWITCH_DELAY_US = 30;
// セグメント実行
void seg(byte left_data = SEG_A0, byte right_data = SEG_A0, byte point_left = LOW, byte point_right = LOW) {
  // 左側のセグの内容
  PORTA = left_data;
  // 左側のセグの右下のドット
  PORTC = point_left ? (byte)B10000000 : (byte)B00000000;
  // 左側のセグを点灯
  ren(SEG_LEFT_PIN, HIGH);
  // 左側のセグを即消灯
  ren(SEG_LEFT_PIN, LOW);
  // 表示の競合を避けるために一旦遅延
  delayMicroseconds(SEGMENT_SWITCH_DELAY_US);
  // 右側のセグの内容
  PORTA = right_data;
  // 右側のセグの右下のドット
  PORTC = point_right ? (byte)B10000000 : (byte)B00000000;
  // 右側のセグを点灯
  ren(SEG_RIGHT_PIN, HIGH);
  // 右側のセグを即消灯
  ren(SEG_RIGHT_PIN, LOW);
}

/********************
 * フォトインタラプタ *
 ********************/

// フォトインタラプタが反応し続けている時は true
inline boolean isPhotoEnabled() {
  return digitalRead(PHOTO_INTERRUPTER_PIN) == LOW;
}

// フォトインタラプタの羽が指定回数通過した瞬間に true
boolean isPhotoPassed(byte rotation = 1) {
  // 瞬間検知のための状態保持フラグ
  static boolean photo_state = false;
  // 通過回数カウンター
  static byte photo_passed_count = 0;
  // 羽が1枚通過した瞬間かを判定
  boolean one_rotate = false;
  // 現在の遮光状態を一度だけ読み取る
  boolean currently_enabled = isPhotoEnabled();
  // 通過の瞬間を検知
  if (!photo_state && currently_enabled) {
    // 状態を検知済みに更新
    photo_state = true;
    // 1回通過したと判定
    one_rotate = true;
  }
  // 状態をリセット
  else if (!currently_enabled) {
    // 状態を未検知にリセット
    photo_state = false;
  }  
  // 1回通過を検知した場合のみ回数判断へ
  if (!one_rotate) {
    return false;
  }
  // 1回通過が検知された場合
  if (rotation == 1) {
    // 目標が1回なら、そのまま true
    return true;
  } else {
    // 目標が複数回なら、カウンター処理を行う
    photo_passed_count++;
    if (photo_passed_count >= rotation) {
      photo_passed_count = 0;
      // 目標回数に到達した
      return true;
    }
  }
  // 目標回数に達していない場合は false
  return false;
}

/*****************
 * タクトスイッチ *
 *****************/

// タクトスイッチの左右を識別する列挙型
enum TactSwitch { LS, RS };

// 指定された側のタクトスイッチが押され続けている時は true
boolean isTactEnabled(TactSwitch side) {
  return digitalRead((side == LS) ? TACT_SWITCH_LEFT_PIN : TACT_SWITCH_RIGHT_PIN) == LOW;
}

// タクトチャタリング防止遅延 (マイクロ秒)
const byte CHATTER_DEBOUNCE_US = 60;

// 指定された側のタクトスイッチが１回押された時に true
boolean isTactPressed(TactSwitch side) {
  // スイッチの状態保持用 (L, R に対応)
  static boolean tact_pressed_state[2] = {true, true};
  // チャタリング防止
  delayMicroseconds(CHATTER_DEBOUNCE_US);
  boolean currently_enabled = isTactEnabled(side);
  // 指定されたスイッチの状態を参照し更新
  if (currently_enabled && tact_pressed_state[side]) {
    // 押されたので状態を更新
    tact_pressed_state[side] = false;
    return true;
  } else if (!currently_enabled) {
    // 離されたので状態をリセット
    tact_pressed_state[side] = true;
    return false;
  } else {
    // 押され続けているが無視
    return false;
  }
}

/***********
 * 実行準備 *
 ***********/

// 初期化
void setup() {
  // 出力ピンの割り当て
  for (byte i = 0; i < sizeof(PIN_WRITE) / sizeof(PIN_WRITE[0]); i++) {
    pinMode(PIN_WRITE[i], OUTPUT);
  }
  // 入力ピンの割り当て
  for (byte i = 0; i < sizeof(PIN_READ) / sizeof(PIN_READ[0]); i++) {
    pinMode(PIN_READ[i], INPUT);
  }
  // DC モーターの暴走を阻止
  dc(S); // 即座に停止
}

#endif // MONO2025_H