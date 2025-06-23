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

// ブザー
const byte BUZZER_PIN = 41;

// トグルスイッチ
const byte TOGGLE_SWITCH_PIN = 11;
// タクトスイッチ
const byte TACT_SWITCH_LEFT_PIN = 13;
const byte TACT_SWITCH_RIGHT_PIN = 12;
// 読み込みピン
const byte PIN_READ[] = { TOGGLE_SWITCH_PIN, TACT_SWITCH_LEFT_PIN, TACT_SWITCH_RIGHT_PIN };

/***************
 * 処理ここから *
 ***************/

/**********
 * ブザー *
 **********/

// ブザー鳴動
const uint16_t BEEP_LOW_FREQ = 220;    // 低音の周波数
const uint16_t BEEP_MID_FREQ = 440;    // 中音の周波数
const uint16_t BEEP_HIGH_FREQ = 880;   // 高音の周波数

// ブザーの音の種類を定義する列挙型
enum BuzzerTone { LO, MI, HI };

// 型と値を同期
const uint16_t BUZZ_FREQ[] = {
  BEEP_LOW_FREQ, // LO に対応
  BEEP_MID_FREQ, // MI に対応
  BEEP_HIGH_FREQ // HI に対応
};

// ブザー鳴動制御
void buzz(BuzzerTone tone_type, float duration = 0.0f) {
  if (duration > 0.0f) {
    // 鳴動
    tone(BUZZER_PIN, BUZZ_FREQ[tone_type], (word)(duration * 1000.0f));
  } else {
    // 消音
    noTone(BUZZER_PIN);
  }
}

/*****************
 * トグルスイッチ *
 *****************/

// トグルスイッチが奥側の時は true
inline boolean isToggleEnabled() {
  return digitalRead(TOGGLE_SWITCH_PIN) == HIGH;
}

// タクト・トグルチャタリング防止遅延 (マイクロ秒)
const byte CHATTER_DEBOUNCE_US = 60;
// トグルスイッチが上げられた時に true
boolean isTogglePulled() {
  // トグルの状態保持用
  static boolean toggle = true;
  // チャタリング防止
  delayMicroseconds(CHATTER_DEBOUNCE_US);
  boolean currently_enabled = isToggleEnabled();
  // トグルの状態を参照し更新
  if (currently_enabled && toggle) {
    // 上げられたので状態を更新
    toggle = false;
    return true;
  } else if (!currently_enabled) {
    // 下げられたので状態をリセット
    toggle = true;
    return false;
  } else {
    // 上げられているが無視
    return false;
  }
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
  pinMode(BUZZER_PIN, OUTPUT);
  // 入力ピンの割り当て
  for (byte i = 0; i < sizeof(PIN_READ) / sizeof(PIN_READ[0]); i++) {
    pinMode(PIN_READ[i], INPUT);
  }
}

#endif // MONO2025_H