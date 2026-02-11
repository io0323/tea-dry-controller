/* main.c
 * TeaDry Controller - メインループ（骨組み）
 * C99
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/sensor.h"
#include "../include/control.h"
#include "../include/state.h"
#include "../include/log.h"

/* 目標値（簡易固定） */
static const float WARMUP_TARGET_TEMP = 60.0f;
static const float DRYING_TARGET_TEMP = 55.0f;
static const float DRYING_TARGET_HUM = 30.0f;

static void parse_args(int argc, char **argv, int *drying_time, char **log_file) {
  *drying_time = 300;
  *log_file = NULL;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
      *log_file = argv[++i];
    } else if (strncmp(argv[i], "--log=", 6) == 0) {
      *log_file = argv[i] + 6;
    } else if (argv[i][0] != '-') {
      *drying_time = atoi(argv[i]);
    }
  }
}

int main(int argc, char **argv) {
  int drying_time;
  char *log_file;
  parse_args(argc, argv, &drying_time, &log_file);

  if (log_file) {
    log_set_file(log_file);
    log_write(LOG_INFO, "ログファイル: %s", log_file);
  }

  SensorData sensor;
  sensor_init(&sensor);

  PID temp_pid;
  pid_init(&temp_pid, 2.0f, 0.01f, 0.5f);

  ControlOutput ctrl = {0.0f, 0.0f};
  DryerState state = STATE_WARMUP;

  /* メインループ：1秒周期で更新・表示 */
  while (state != STATE_FINISH && state != STATE_ERROR) {
    /* 制御目標を状態ごとに決定 */
    float target_temp = DRYING_TARGET_TEMP;
    float target_hum = DRYING_TARGET_HUM;
    switch (state) {
    case STATE_WARMUP:
      target_temp = WARMUP_TARGET_TEMP;
      break;
    case STATE_DRYING:
      target_temp = DRYING_TARGET_TEMP;
      target_hum = DRYING_TARGET_HUM;
      break;
    case STATE_FINISH:
    case STATE_ERROR:
      break;
    }

    /* 制御計算（PID）: target_hum を渡す */
    control_compute(&temp_pid, target_temp, sensor.temperature,
                    target_hum, sensor.humidity, &ctrl);

    /* センサ（環境）を1秒進める */
    sensor_update(&sensor, ctrl.heater_power, ctrl.fan_power);

    /* 単純な状態遷移ロジック（骨組み） */
    switch (state) {
    case STATE_WARMUP:
      if (sensor.temperature >= WARMUP_TARGET_TEMP - 1.0f) {
        state = STATE_DRYING;
        log_write(LOG_INFO, "状態遷移: WARMUP -> DRYING");
      }
      break;
    case STATE_DRYING:
      if (sensor.elapsed_seconds >= drying_time) {
        state = STATE_FINISH;
        log_write(LOG_INFO, "状態遷移: DRYING -> FINISH");
      }
      /* 異常判定（過熱 or 過乾燥） */
      if (sensor.temperature > 120.0f || sensor.humidity < 5.0f) {
        state = STATE_ERROR;
        log_write(LOG_ERROR, "異常検出: ERROR に遷移 (T=%.1f H=%.1f)",
                  sensor.temperature, sensor.humidity);
      }
      break;
    default:
      break;
    }

    /* 画面表示（1秒ごと） */
    printf("T=%5.2fC H=%5.2f%% | Heater=%5.1f%% Fan=%5.1f%% | State=%s | Elapsed=%d\n",
           sensor.temperature, sensor.humidity,
           ctrl.heater_power, ctrl.fan_power,
           state_name(state), sensor.elapsed_seconds);

    /* 終了判定（状態による） */
    if (state == STATE_FINISH) {
      log_write(LOG_INFO, "乾燥完了");
      break;
    }
    if (state == STATE_ERROR) {
      log_write(LOG_ERROR, "異常終了");
      break;
    }

    sleep(1);
  }

  log_shutdown();
  return 0;
}

