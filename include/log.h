/* log.h
 * ログ出力モジュール（レベル・ファイル出力対応）
 * C99
 */
#ifndef LOG_H
#define LOG_H

typedef enum {
  LOG_DEBUG = 0,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR
} LogLevel;

/* 最小出力レベルを設定（このレベル以上のものだけ出力） */
void log_set_level(LogLevel min_level);

/* ログファイルを開く。path==NULL ならファイル出力無効 */
void log_set_file(const char *path);

/* ログ出力（printf形式） */
void log_write(LogLevel level, const char *fmt, ...);

/* リソース解放（ファイルを閉じる） */
void log_shutdown(void);

#endif /* LOG_H */
