/* control.h
 * 制御出力とPIDインターフェース
 */
#ifndef CONTROL_H
#define CONTROL_H

/* 制御出力（ヒーター、ファンともに 0..100） */
typedef struct {
  float heater_power;
  float fan_power;
} ControlOutput;

/* シンプルPID構造体（内部状態は実装側で持つ） */
typedef struct {
  float kp;
  float ki;
  float kd;
  float integrator;
  float last_error;
} PID;

/* PID初期化 */
void pid_init(PID *p, float kp, float ki, float kd);

/* PID計算（1秒刻み想定） */
float pid_update(PID *p, float setpoint, float measured);

/* 高レベル制御：目標温度/湿度から出力を算出する */
void control_compute(PID *temp_pid, float target_temp, float current_temp,
                     float current_hum, ControlOutput *out);

#endif /* CONTROL_H */

