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
// クロック
const byte CLOCK_PIN = 44;
// 書き込みピン
const byte PIN_WRITE[] = { DC_MOTOR_1_PIN, DC_MOTOR_2_PIN, CLOCK_PIN };

// トグルスイッチ
const byte TOGGLE_SWITCH_PIN = 11;
// タクトスイッチ
const byte TACT_SWITCH_LEFT_PIN = 13;
const byte TACT_SWITCH_RIGHT_PIN = 12;
// 読み込みピン
const byte PIN_READ[] = { TOGGLE_SWITCH_PIN, TACT_SWITCH_LEFT_PIN, TACT_SWITCH_RIGHT_PIN };

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
    case LT: dc_ctr(true, false); break;
    // 右回転
    case RT: dc_ctr(false, true); break;
    // 即座に停止
    case S: dc_ctr(true, true); break;
    // 減速して停止
    case F:  dc_ctr(false, false); break;
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