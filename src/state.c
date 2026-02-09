/* state.c
 * 状態関連ユーティリティ
 */
#include "../include/state.h"

const char *state_name(DryerState s) {
  switch (s) {
  case STATE_WARMUP: return "WARMUP";
  case STATE_DRYING: return "DRYING";
  case STATE_FINISH: return "FINISH";
  case STATE_ERROR: return "ERROR";
  default: return "UNKNOWN";
  }
}

