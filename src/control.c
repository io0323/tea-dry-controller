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
 */
float pid_update(PID *p, float setpoint, float measured) {
  if (!p) return 0.0f;
  float err = setpoint - measured;
  p->integrator += err;
  float derivative = err - p->last_error;
  p->last_error = err;
  float out = p->kp * err + p->ki * p->integrator + p->kd * derivative;
  /* 出力は任意のスケール。呼び出し側で 0..100 に変換する */
  return out;
}

/*
 * control_compute
 * 温度PIDでヒーター出力を決定し、湿度は単純に風量で補正する。
 */
void control_compute(PID *temp_pid, float target_temp, float current_temp,
                     float current_hum, ControlOutput *out) {
  if (!temp_pid || !out) return;

  float pid_val = pid_update(temp_pid, target_temp, current_temp);

  /* PID出力をヒーター割合に変換し、クリッピング */
  float heater = pid_val;
  if (heater < 0.0f) heater = 0.0f;
  if (heater > 100.0f) heater = 100.0f;

  /* 湿度が高ければファン強め、低ければ弱め（単純ルール） */
  float fan = 0.0f;
  if (current_hum > 65.0f) fan = 70.0f;
  else if (current_hum > 55.0f) fan = 50.0f;
  else fan = 20.0f;

  out->heater_power = heater;
  out->fan_power = fan;
}

