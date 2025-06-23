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

// LED (RGB)
const byte LED_RED_PIN = 37;
const byte LED_GREEN_PIN = 39;
const byte LED_BLUE_PIN = 38;
// 書き込みピン
const byte PIN_WRITE[] = { LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN };

// タクトスイッチ
const byte TACT_SWITCH_LEFT_PIN = 13;
const byte TACT_SWITCH_RIGHT_PIN = 12;
// 読み込みピン
const byte PIN_READ[] = { TACT_SWITCH_LEFT_PIN, TACT_SWITCH_RIGHT_PIN };

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

/***********
 * LED制御 *
 ***********/

// LED の色設定
void set_led(byte r_val = LOW, byte g_val = LOW, byte b_val = LOW) {
  // 赤
  ren(LED_RED_PIN, r_val);
  // 緑
  ren(LED_GREEN_PIN, g_val);
  // 青
  ren(LED_BLUE_PIN, b_val);
}

// 各色に対応する列挙型
enum LedColor { R, G, B, W, C, Y, M, K };

// LED制御関数
inline void led(LedColor color = K, boolean low = false) {
  switch (color) {
    case R: set_led(HIGH, LOW, LOW); break;
    case G: set_led(LOW, HIGH, LOW); break;
    case B: set_led(LOW, LOW, HIGH); break;
    case W: set_led(HIGH, HIGH, HIGH); break;
    case C: set_led(LOW, HIGH, HIGH); break;
    case Y: set_led(HIGH, HIGH, LOW); break;
    case M: set_led(HIGH, LOW, HIGH); break;
    case K: set_led(LOW, LOW, LOW); break;
  }
  if (low) {
    delay(1);
    set_led(LOW, LOW, LOW);
    delay(9);
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
}

#endif // MONO2025_H