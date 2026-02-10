/* control.c
 * PID 制御の簡易実装
 */
#include "../include/control.h"
#include <math.h>

/* PID 初期化 */
void pid_init(PID *p, float kp, float ki, float kd) {
  if (!p) return;
  p->kp = kp;
  p->ki = ki;
  p->kd = kd;
  p->integrator = 0.0f;
  p->last_error = 0.0f;
}

/*
 * pid_update
 * 単純な離散PID。dt=1秒想定。
 * - 出力を 0..100 にクリップして返す（呼び出し側で再クリップ不要）
 * - アンチワインドアップ: 積分項は出力が飽和しない時のみコミットする
 */
float pid_update(PID *p, float setpoint, float measured) {
  if (!p) return 0.0f;
  float err = setpoint - measured;
  float derivative = err - p->last_error;

  /* 積分の仮適用で将来出力を試算 */
  float prospective_integrator = p->integrator + err;
  float out_temp = p->kp * err + p->ki * prospective_integrator + p->kd * derivative;

  /* 出力クリッピング（0..100） */
  float out = out_temp;
  if (out < 0.0f) out = 0.0f;
  if (out > 100.0f) out = 100.0f;

  /* アンチワインドアップ: 仮出力が飽和していなければ積分を確定 */
  if (out_temp >= 0.0f && out_temp <= 100.0f) {
    p->integrator = prospective_integrator;
  }

  /* 次回の微分計算のためエラーは必ず更新 */
  p->last_error = err;
  return out;
}

/*
 * control_compute
 * 温度PIDでヒーター出力を決定し、湿度は単純に風量で補正する。
 */
void control_compute(PID *temp_pid, float target_temp, float current_temp,
                     float target_hum, float current_hum, ControlOutput *out) {
  if (!temp_pid || !out) return;

  /* PID から 0..100 のヒーター割合を得る（アンチワインドアップ済） */
  float heater = pid_update(temp_pid, target_temp, current_temp);

  /* 湿度差に比例したファン制御（単純比例制御） */
  float hum_err = current_hum - target_hum; /* 正 -> 実測が目標より高い */
  float fan = 30.0f + hum_err * 1.5f;       /* ゼロ基準を 30% に設定 */
  if (fan < 0.0f) fan = 0.0f;
  if (fan > 100.0f) fan = 100.0f;

  out->heater_power = heater;
  out->fan_power = fan;
}

