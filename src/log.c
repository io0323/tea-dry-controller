/* log.c
 * ログ出力の実装（stderr + 任意でファイル）
 */
#include "../include/log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

static LogLevel g_min_level = LOG_INFO;
static FILE *g_log_file = NULL;

static const char *level_str(LogLevel level) {
  switch (level) {
  case LOG_DEBUG: return "DEBUG";
  case LOG_INFO: return "INFO";
  case LOG_WARN: return "WARN";
  case LOG_ERROR: return "ERROR";
  default: return "?";
  }
}

void log_set_level(LogLevel min_level) {
  g_min_level = min_level;
}

void log_set_file(const char *path) {
  if (g_log_file) {
    fclose(g_log_file);
    g_log_file = NULL;
  }
  if (path && path[0] != '\0') {
    g_log_file = fopen(path, "a");
  }
}

void log_write(LogLevel level, const char *fmt, ...) {
  if (level < g_min_level) return;

  char buf[256];
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int n = 0;
  if (t) {
    n = snprintf(buf, sizeof(buf), "[%04d-%02d-%02d %02d:%02d:%02d] [%s] ",
                 t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                 t->tm_hour, t->tm_min, t->tm_sec, level_str(level));
  } else {
    n = snprintf(buf, sizeof(buf), "[%s] ", level_str(level));
  }
  if (n < 0 || (size_t)n >= sizeof(buf)) return;

  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf + n, sizeof(buf) - (size_t)n, fmt, ap);
  va_end(ap);

  fprintf(stderr, "%s\n", buf);
  if (g_log_file) {
    fprintf(g_log_file, "%s\n", buf);
    fflush(g_log_file);
  }
}

void log_shutdown(void) {
  if (g_log_file) {
    fclose(g_log_file);
    g_log_file = NULL;
  }
}
