#ifndef _ZJUNIX_LOG_H
#define _ZJUNIX_LOG_H

#define LOG_OK 0
#define LOG_FAIL 1
#define LOG_START 2
#define LOG_END 3
#define LOG_STEP 4

void log(int status, const char * format, ...);
void step();
void step_reset();

#endif // ! _ZJUNIX_LOG_H