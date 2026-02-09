/* state.h
 * ドライヤ状態定義
 */
#ifndef STATE_H
#define STATE_H

typedef enum {
  STATE_WARMUP = 0,
  STATE_DRYING,
  STATE_FINISH,
  STATE_ERROR
} DryerState;

/* 状態名取得（表示用） */
const char *state_name(DryerState s);

#endif /* STATE_H */

