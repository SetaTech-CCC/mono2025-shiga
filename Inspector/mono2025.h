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

/*
 *  関数の説明
 *
 * ・secs()
 *   経過時間を秒単位で返す。小数第一位まで対応。
 * 
 * ・delaySecs(secs_val)
 *   秒単位で遅延させる。小数第一位まで対応。
 * 
 * ・stepper(reverse)
 *   ステッピングモーター制御関数
 *   3°ずつ動く。
 *   引数なしでは時計回りに動く。
 *   引数の reverse を true にすると、反時計回りに動く。
 *   DC モーターとの両立はできない。
 * 
 * ・dc(action)
 *   DCモーターを制御する。
 *   引数には、RT(Right Turn：右回り)、LT(Left Turn：左回り)、S(Stop：即停止)、F(Free：減速)がある。
 *   引数の文字にダブルクォーテーションは不要。
 *   ステッピングモーターとの両立はできない。
 * 
 * ・led(color, low)
 *   LED制御関数。引数に入れた色を点灯する。
 *   R(赤)、G(緑)、B(青)、W(白)、C(水)、Y(黄)、M(紫)、K(消灯)。
 *   引数の文字にダブルクォーテーションは不要。
 *   low は、LED の明るさを調整する。
 *   true を入れると暗くなるが、for や while で回す必要がある。
 *   0.2 秒微点灯したい場合、200(0.2秒) / 10ms(1回の処理時間) = 20 回回す必要がある。
 *   LED 微点灯の処理中、ブザー、DCモーター以外の処理はできない。
 * 
 * ・buzz(tone_type, duration)
 *   ブザー鳴動関数。
 *   tone_type は周波数で、HI(高音)、MI(中音)、LO(低音)がある。
 *   これらにダブルクォーテーションは不要。
 *   音の長さは２つ目の引数である duration に秒数で入れる。小数第一位まで対応。
 * 
 * ・seg(left_data, right_data, pount_left, point_right)
 *   7セグ制御関数。
 *   left_data は左側の７セグ部分、right_data は右側の７セグ部分。
 *   以下は引数の例：
 *   番号：num[8]
 *   アルファベット：SEG_A
 *   特定のセグ：(SEG_C2 + SEG_C3)
 *   point_left は左側の小数点部分、point_right は右側の小数点部分。
 *   point 変数は true で点灯、false で消灯。
 * 
 * ・isPhotoEnabled()
 *   フォトインタラプタが遮光されている間 true を返す。
 * 
 * ・isPhotoPassed(rotation)
 *   羽が引数に指定した回数通過した瞬間 true を返す。
 * 
 * ・isToggleEnabled()
 *   トグルスイッチが上向きの状態なら true を返す。
 *
 * ・isTogglePulled()
 *   トグルスイッチが上向きになった一瞬のみ true を返す。
 * 
 * ・isTactEnabled(side)
 *   タクトスイッチが押され続けている間は true を返す。
 *   引数は、LS(Left Side：左側)、RS(Right Side：右側) の２つがある。
 *   引数の文字にダブルクォーテーションは不要。
 * 
 * ・isTactPressed(side)
 *   タクトスイッチが押された一瞬のみ true を返す。
 *   引数は Enabled 版と同じ。
 * 
 * ・getPot()
 *   可変抵抗器の値を、0～1023 で返す。
 *   
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
// DCモーター
const byte DC_MOTOR_1_PIN = 29;
const byte DC_MOTOR_2_PIN = 30;
// LED (RGB)
const byte LED_RED_PIN = 37;
const byte LED_GREEN_PIN = 39;
const byte LED_BLUE_PIN = 38;
// ブザー
const byte BUZZER_PIN = 41;
// 7セグ
const byte SEG_LEFT_PIN = 43;
const byte SEG_RIGHT_PIN = 42;
// クロック
const byte CLOCK_PIN = 44;
// 書き込みピン
const byte PIN_WRITE[] = { STEPPER_MOTOR_1_PIN, STEPPER_MOTOR_2_PIN, STEPPER_MOTOR_3_PIN, STEPPER_MOTOR_4_PIN, DC_MOTOR_1_PIN, DC_MOTOR_2_PIN, LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN, BUZZER_PIN, SEG_LEFT_PIN, SEG_RIGHT_PIN, CLOCK_PIN };

// フォトインタラプタ
const byte PHOTO_INTERRUPTER_PIN = 10;
// トグルスイッチ
const byte TOGGLE_SWITCH_PIN = 11;
// タクトスイッチ
const byte TACT_SWITCH_LEFT_PIN = 13;
const byte TACT_SWITCH_RIGHT_PIN = 12;
// 可変抵抗器
const byte POTENTIOMETER_PIN = A1 /* 55 */;
// 読み込みピン
const byte PIN_READ[] = { PHOTO_INTERRUPTER_PIN, TOGGLE_SWITCH_PIN, TACT_SWITCH_LEFT_PIN, TACT_SWITCH_RIGHT_PIN, POTENTIOMETER_PIN };

/***********
 * 補助関数 *
 ***********/

// デジタルピンに書き込み
inline void ren(byte pin, byte level) {
  digitalWrite(pin, level);
}

// 初回遅延（秒）：処理の重複を防ぐため
const byte DELAY_FIRST_S = 2;

// 秒数計算
inline float secs() {
  return millis() / 1000.0f - DELAY_FIRST_S;
}

// 秒単位で遅延
void delaySecs(float secs_val) {
  delay((word)(secs_val * 1000.0f));
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
  
  // DC モーターの暴走を阻止
  ren(DC_MOTOR_1_PIN, LOW);
  ren(DC_MOTOR_2_PIN, LOW);
  // 同期
  clock_reset();
  // 遅延
  delay(5);
}

/*************
 * DCモーター *
 *************/

// DCモーターコマンド間遅延 (ミリ秒)
const byte DC_MOTOR_COMMAND_DELAY_MS = 1;
// DC モーターコントローラー
void dc_ctr(boolean left_ctrl = false, boolean right_ctrl = false) {
  // ステッピングモーターとの競合を阻止
  stepper_stop();
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
    case LT: // 左回転
      dc_ctr(true, false);
      break;
    case RT: // 右回転
      dc_ctr(false, true);
      break;
    case S:  // 即座に停止
      dc_ctr(true, true);
      break;
    case F:  // 減速して停止
      dc_ctr(false, false);
      break;
  }
}

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

/**********
 * ブザー *
 **********/

// ブザー鳴動
const uint16_t BEEP_LOW_FREQ = 400;    // 低音の周波数
const uint16_t BEEP_MID_FREQ = 800;    // 中音の周波数
const uint16_t BEEP_HIGH_FREQ = 1200;  // 高音の周波数

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

// エラー時ブザー
void errBuzz() {
  delaySecs(0.2);
  buzz(HI, 0.1);
  delaySecs(0.1);
  buzz(HI, 0.1);
  delaySecs(0.1);
  buzz(HI, 0.1);
  delaySecs(0.1);
  buzz(HI, 0.1);
  delaySecs(0.1);
  buzz(HI, 0.1);
}

// 処理移行時ブザー
void chgBuzz(boolean second = false) {
  delaySecs(0.1);
  buzz(second ? HI : LO, 0.2);
  delaySecs(0.3);
  buzz(MI, 0.2);
  delaySecs(0.3);
  buzz(second ? LO : HI, 0.2);
  delaySecs(0.3);
  buzz(second ? HI : LO, 0.7);
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
// アルファベット
#define SEG_A (SEG_L1 + SEG_L2 + SEG_C1 + SEG_C2 + SEG_R1 + SEG_R2)
#define SEG_B (SEG_L1 + SEG_L2 + SEG_C2 + SEG_C3 + SEG_R2)
//#define SEG_C (SEG_L1 + SEG_L2 + SEG_C1 + SEG_C3) // C 大文字
#define SEG_C (SEG_L2 + SEG_C2 + SEG_C3)
#define SEG_D (SEG_L2 + SEG_C2 + SEG_C3 + SEG_R1 + SEG_R2)
#define SEG_E (SEG_L1 + SEG_L2 + SEG_C1 + SEG_C2 + SEG_C3)
#define SEG_F (SEG_L1 + SEG_L2 + SEG_C1 + SEG_C2)
#define SEG_H (SEG_L1 + SEG_L2 + SEG_C2 + SEG_R1 + SEG_R2)
#define SEG_L (SEG_L1 + SEG_L2 + SEG_C3)
#define SEG_P (SEG_L1 + SEG_L2 + SEG_C1 + SEG_C2 + SEG_R1)
#define SEG_U (SEG_L1 + SEG_L2 + SEG_C3 + SEG_R1 + SEG_R2)

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
  byte scaled_value = (byte)(inverted_value * 99 / 1023);
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

// 任意初期化
void initialize(void);

// 初期化
void setup() {
  // ボードレートを指定
  Serial.begin(9600);
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
  // ステッピングモーターを初期化
  stepper_stop();
  // 初期遅延
  delaySecs(DELAY_FIRST_S);
  // オプション関数
  initialize();
}

#endif // MONO2025_H