void logger(char *message, int level);
void debug(char *message);
void debug_s(char *message, char *string);
void info(char* message);

#define  DEBUG_FATAL 0
#define  DEBUG_ERROR 1
#define  DEBUG_WARN  2
#define  DEBUG_INFO  3
#define  DEBUG_DEBUG 6

