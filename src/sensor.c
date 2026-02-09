/* sensor.c
 * 擬似環境モデル（熱・湿度の時間発展）
 */
#include "../include/sensor.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* 内部定数（簡易モデル） */
static const float AMBIENT_TEMP = 25.0f;
static const float AMBIENT_HUMID = 60.0f;

/*
 * sensor_init
 * センサデータの初期値を設定し、乱数を初期化する。
 */
void sensor_init(SensorData *s) {
  if (!s) return;
  s->temperature = AMBIENT_TEMP;
  s->humidity = AMBIENT_HUMID;
  s->elapsed_seconds = 0;
  srand((unsigned)time(NULL));
}

/*
 * sensor_update
 * 非線形を簡略化した 1秒刻みの物理モデル
 * - ヒーターは温度上昇と乾燥促進に寄与
 * - 送風は湿度低下と温度拡散に寄与
 */
void sensor_update(SensorData *s, float heater_power, float fan_power) {
  if (!s) return;

  /* 正規化 */
  float hp = heater_power / 100.0f;
  float fp = fan_power / 100.0f;

  /* ランダムノイズ（小さめ） */
  float noise_t = ((float)rand() / RAND_MAX - 0.5f) * 0.2f;
  float noise_h = ((float)rand() / RAND_MAX - 0.5f) * 0.5f;

  /* 温度更新（単純1次系） */
  float dtemp = hp * 0.6f        /* ヒーターによる加熱 */
                - 0.08f * (s->temperature - AMBIENT_TEMP) /* 熱散失 */
                - fp * 0.03f     /* 風による冷却（空気対流） */
                + noise_t;
  s->temperature += dtemp;

  /* 湿度更新（単純モデル：加熱と送風で乾燥） */
  float dh = - hp * 0.4f       /* 加熱で蒸発 */
              - fp * 0.6f      /* 送風で乾燥促進 */
              + 0.02f * (AMBIENT_HUMID - s->humidity) /* 周囲との回復 */
              + noise_h;
  s->humidity += dh;

  /* 範囲制限 */
  if (s->temperature < -40.0f) s->temperature = -40.0f;
  if (s->temperature > 200.0f) s->temperature = 200.0f;
  if (s->humidity < 0.0f) s->humidity = 0.0f;
  if (s->humidity > 100.0f) s->humidity = 100.0f;

  s->elapsed_seconds += 1;
}

