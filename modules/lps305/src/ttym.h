#ifndef __TTY_M__
#define __TTY_M__
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

struct ttym;
typedef struct ttym* ttym_t;

ttym_t ttym_open(char name[], unsigned int baudrate);
int ttym_close(ttym_t tty);
ssize_t ttym_write(ttym_t tty, void *buf, size_t count);
ssize_t ttym_read(ttym_t tty, void *buf, size_t count);

int ttym_timeout(ttym_t tty, ssize_t time_ms);
ssize_t ttym_writechar(ttym_t tty, char data);
int ttym_readchar(ttym_t tty);

#ifdef __cplusplus
}
#endif

#endif
