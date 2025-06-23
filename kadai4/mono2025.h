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

// ステッピングモーター
const byte STEPPER_MOTOR_1_PIN = 23;
const byte STEPPER_MOTOR_2_PIN = 24;
const byte STEPPER_MOTOR_3_PIN = 25;
const byte STEPPER_MOTOR_4_PIN = 26;
// ブザー
const byte BUZZER_PIN = 41;
// クロック
const byte CLOCK_PIN = 44;
// 書き込みピン
const byte PIN_WRITE[] = { STEPPER_MOTOR_1_PIN, STEPPER_MOTOR_2_PIN, STEPPER_MOTOR_3_PIN, STEPPER_MOTOR_4_PIN, BUZZER_PIN, CLOCK_PIN };

// トグルスイッチ
const byte TOGGLE_SWITCH_PIN = 11;
// タクトスイッチ
const byte TACT_SWITCH_RIGHT_PIN = 12;
const byte TACT_SWITCH_LEFT_PIN = 13;
// 読み込みピン
const byte PIN_READ[] = { TOGGLE_SWITCH_PIN, TACT_SWITCH_RIGHT_PIN, TACT_SWITCH_LEFT_PIN };

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

// クロックリセット遅延 (ミリ秒)
const byte CLOCK_RESET_DELAY_MS = 5;
// クロックピンをリセット
void clock_reset() {
  delay(CLOCK_RESET_DELAY_MS);
  ren(CLOCK_PIN, HIGH);
  ren(CLOCK_PIN, LOW);
}

/**********************
 * ステッピングモーター *
 **********************/

// ステッピングモーターを停止
void stepper_stop() {
  ren(STEPPER_MOTOR_1_PIN, LOW);
  ren(STEPPER_MOTOR_2_PIN, LOW);
  ren(STEPPER_MOTOR_3_PIN, LOW);
  ren(STEPPER_MOTOR_4_PIN, LOW);
  clock_reset();
}

// ステッピングモーター
void stepper(boolean reverse = false) {
  // 回数フラグ
  static byte stepper_flag = 0;
  stepper_flag++;

  switch(stepper_flag) {

    case 1:
      ren(STEPPER_MOTOR_1_PIN, HIGH);
      ren(STEPPER_MOTOR_2_PIN, LOW);
      ren(STEPPER_MOTOR_3_PIN, LOW);
      ren(STEPPER_MOTOR_4_PIN, LOW);
      break;

    case 2:
      ren(STEPPER_MOTOR_1_PIN, LOW);
      ren(STEPPER_MOTOR_2_PIN, reverse ? LOW : HIGH);
      ren(STEPPER_MOTOR_3_PIN, LOW);
      ren(STEPPER_MOTOR_4_PIN, reverse ? HIGH : LOW);
      break;

    case 3:
      ren(STEPPER_MOTOR_1_PIN, LOW);
      ren(STEPPER_MOTOR_2_PIN, LOW);
      ren(STEPPER_MOTOR_3_PIN, HIGH);
      ren(STEPPER_MOTOR_4_PIN, LOW);
      break;

    case 4:
      ren(STEPPER_MOTOR_1_PIN, LOW);
      ren(STEPPER_MOTOR_2_PIN, reverse ? HIGH : LOW);
      ren(STEPPER_MOTOR_3_PIN, LOW);
      ren(STEPPER_MOTOR_4_PIN, reverse ? LOW : HIGH);
      stepper_flag = 0;
      break;
  }
  
  // 同期
  clock_reset();
  // 遅延
  delay(7);
}

/**********
 * ブザー *
 **********/

// ブザー鳴動
const uint16_t BEEP_LOW_FREQ = 380;    // 低音の周波数
const uint16_t BEEP_MID_FREQ = 850;    // 中音の周波数
const uint16_t BEEP_HIGH_FREQ = 1250;  // 高音の周波数

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
  // ステッピングモーターを初期化
  stepper_stop();
}

#endif // MONO2025_H