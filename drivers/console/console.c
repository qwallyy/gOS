/* gOS Kernel — Kernel Console Shell Implementation
 * Minimal command-line interface over serial.
 */

#include "drivers/console/console.h"
#include "drivers/serial/serial.h"
#include "drivers/keyboard/keyboard.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"
#include "lib/stdio.h"
#include "proc/proc.h"
#include "power/power.h"

#define MAX_COMMANDS 16

typedef struct console_cmd {
    char name[32];
    char help[128];
    void (*handler)(int argc, char **argv);
} console_cmd_t;

static console_cmd_t g_commands[MAX_COMMANDS];
static int g_cmd_count = 0;

static void cmd_help(int argc, char **argv);
static void cmd_reboot(int argc, char **argv);
static void cmd_shutdown(int argc, char **argv);
static void cmd_meminfo(int argc, char **argv);
static void cmd_pci(int argc, char **argv);
static void cmd_proc(int argc, char **argv);

void console_init(void) {
    klog_info("Kernel console initializing");
    for (int i = 0; i < MAX_COMMANDS; i++) {
        g_commands[i].name[0] = '\0';
    }

    console_register_command("help", "Show available commands", cmd_help);
    console_register_command("reboot", "Reboot the system", cmd_reboot);
    console_register_command("shutdown", "Shutdown the system", cmd_shutdown);
    console_register_command("meminfo", "Show memory info", cmd_meminfo);
    console_register_command("pci", "List PCI devices", cmd_pci);
    console_register_command("proc", "List processes", cmd_proc);
}

void console_register_command(const char *name, const char *help,
                              void (*handler)(int argc, char **argv)) {
    if (g_cmd_count >= MAX_COMMANDS) return;
    console_cmd_t *cmd = &g_commands[g_cmd_count++];
    strncpy(cmd->name, name, 31);
    cmd->name[31] = '\0';
    strncpy(cmd->help, help, 127);
    cmd->help[127] = '\0';
    cmd->handler = handler;
}

void console_prompt(void) {
    serial_puts("\r\ngOS> ");
}

static int parse_args(char *line, char **argv, int max_argv) {
    int argc = 0;
    char *p = line;
    while (*p && argc < max_argv) {
        while (*p == ' ') p++;
        if (!*p) break;
        argv[argc++] = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
    }
    return argc;
}

void console_process_line(const char *line) {
    if (!line || !line[0]) return;

    char buf[CONSOLE_CMD_MAX_LEN];
    strncpy(buf, line, CONSOLE_CMD_MAX_LEN - 1);
    buf[CONSOLE_CMD_MAX_LEN - 1] = '\0';

    char *argv[16];
    int argc = parse_args(buf, argv, 16);
    if (argc == 0) return;

    for (int i = 0; i < g_cmd_count; i++) {
        if (strcmp(g_commands[i].name, argv[0]) == 0) {
            if (g_commands[i].handler) {
                g_commands[i].handler(argc, argv);
            }
            return;
        }
    }
    serial_puts("Unknown command: ");
    serial_puts(argv[0]);
    serial_puts("\r\n");
}

void console_run(void) {
    char line[CONSOLE_CMD_MAX_LEN];
    size_t pos = 0;

    console_prompt();
    for (;;) {
        if (!keyboard_has_input()) {
            proc_yield();
            continue;
        }
        char c = keyboard_get_char();
        if (c == '\0') continue;

        if (c == '\n' || c == '\r') {
            line[pos] = '\0';
            serial_puts("\r\n");
            console_process_line(line);
            pos = 0;
            console_prompt();
        } else if (c == '\b' || c == 0x7F) {
            if (pos > 0) {
                pos--;
                serial_puts("\b \b");
            }
        } else if (pos < CONSOLE_CMD_MAX_LEN - 1) {
            line[pos++] = c;
            serial_putc(c);
        }
    }
}

/* Built-in commands */
static void cmd_help(int argc, char **argv) {
    (void)argc; (void)argv;
    serial_puts("Available commands:\r\n");
    for (int i = 0; i < g_cmd_count; i++) {
        serial_puts("  ");
        serial_puts(g_commands[i].name);
        serial_puts(" - ");
        serial_puts(g_commands[i].help);
        serial_puts("\r\n");
    }
}

static void cmd_reboot(int argc, char **argv) {
    (void)argc; (void)argv;
    power_reboot();
}

static void cmd_shutdown(int argc, char **argv) {
    (void)argc; (void)argv;
    power_shutdown();
}

static void cmd_meminfo(int argc, char **argv) {
    (void)argc; (void)argv;
    extern uintptr_t pmm_get_total_memory(void);
    extern uintptr_t pmm_get_free_memory(void);
    char buf[128];
    snprintf(buf, sizeof(buf), "Total: %llu KiB, Free: %llu KiB\r\n",
             (unsigned long long)(pmm_get_total_memory() / 1024),
             (unsigned long long)(pmm_get_free_memory() / 1024));
    serial_puts(buf);
}

static void cmd_pci(int argc, char **argv) {
    (void)argc; (void)argv;
    extern void pci_dump_devices(void);
    pci_dump_devices();
}

static void cmd_proc(int argc, char **argv) {
    (void)argc; (void)argv;
    extern void proc_dump_all(void);
    proc_dump_all();
}
