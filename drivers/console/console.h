/* gOS Kernel — Kernel Console Shell
 * Minimal command-line interface over serial.
 */

#ifndef GOS_CONSOLE_H
#define GOS_CONSOLE_H

#include "gOS/types.h"

#define CONSOLE_CMD_MAX_LEN     256
#define CONSOLE_HISTORY_SIZE    16

void console_init(void);
void console_run(void);
void console_prompt(void);
void console_process_line(const char *line);
void console_register_command(const char *name, const char *help,
                              void (*handler)(int argc, char **argv));

#endif /* GOS_CONSOLE_H */
