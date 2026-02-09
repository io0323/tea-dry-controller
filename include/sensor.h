/* sensor.h
 * 擬似センサ用データ構造とAPI
 * C99
 */
#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/* センサデータ（温度 ℃、湿度 %、経過秒） */
typedef struct {
  float temperature;
  float humidity;
  int elapsed_seconds;
} SensorData;

/* 初期化 */
void sensor_init(SensorData *s);

/*
 * 1秒ごとのセンサ更新
 * heater_power, fan_power は 0..100 の割合
 */
void sensor_update(SensorData *s, float heater_power, float fan_power);

#endif /* SENSOR_H */

