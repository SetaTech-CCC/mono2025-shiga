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

// 7セグ
const byte SEG_LEFT_PIN = 43;
const byte SEG_RIGHT_PIN = 42;
// 書き込みピン
const byte PIN_WRITE[] = { SEG_LEFT_PIN, SEG_RIGHT_PIN };

// 可変抵抗器
const byte POTENTIOMETER_PIN = A1 /* 55 */;

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

/*************
 * 可変抵抗器 *
 *************/

// 可変抵抗器の値を 0 ~ 1023 で取得
inline uint16_t getPot() {
  return analogRead(POTENTIOMETER_PIN);
}

// 1023~0 -> 0~99
byte map_volume_value() {
  // アナログ値を取得
  uint16_t raw_value = getPot();
  // 値を逆転
  long inverted_value = 1023 - raw_value;
  // 変換
  byte scaled_value = (byte)(inverted_value * 31 / 1023);
  // 戻す
  return scaled_value;
}

// 可変抵抗器と7セグを同期
void syncPotSeg() {
  // 値を２桁に変換
  byte val = map_volume_value();
  // ７セグに表示
  seg(num[val / 10], num[val % 10]);
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
  pinMode(POTENTIOMETER_PIN, INPUT);
}

#endif // MONO2025_H