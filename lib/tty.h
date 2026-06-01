/* gOS Kernel — TTY Abstraction Layer
 * Provides terminal I/O over serial and VGA.
 */

#ifndef GOS_TTY_H
#define GOS_TTY_H

#include "gOS/types.h"

#define TTY_MAX_TTYS    4

typedef struct tty {
    int id;
    bool active;
    void (*write)(const char *data, size_t len);
    int (*read)(char *buf, size_t len);
} tty_t;

void tty_init(void);
void tty_set_active(int tty_id);
tty_t *tty_get_active(void);
void tty_write(const char *data, size_t len);
void tty_puts(const char *str);
int  tty_read(char *buf, size_t len);
void tty_switch(int tty_id);

#endif /* GOS_TTY_H */
