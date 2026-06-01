/* gOS Kernel — Sound Implementation
 * Reference: https://wiki.osdev.org/PC_Speaker
 */

#include "drivers/sound/sound.h"
#include "drivers/pit/pit.h"
#include "gOS/klog.h"

void sound_init(void) {
    klog_info("Sound subsystem initialized (PC speaker available)");
}

void pc_speaker_enable(uint32_t freq) {
    if (freq == 0) return;
    uint32_t div = 1193182 / freq;
    if (div > 65535) div = 65535;

    outb(PIT_COMMAND, 0xB6); /* Channel 2, mode 3, binary */
    io_wait();
    outb(PIT_CHANNEL2, div & 0xFF);
    io_wait();
    outb(PIT_CHANNEL2, (div >> 8) & 0xFF);
    io_wait();

    uint8_t tmp = inb(PC_SPEAKER_PORT);
    tmp |= 0x03; /* Enable speaker gate + data */
    outb(PC_SPEAKER_PORT, tmp);
}

void pc_speaker_disable(void) {
    uint8_t tmp = inb(PC_SPEAKER_PORT);
    tmp &= ~0x03;
    outb(PC_SPEAKER_PORT, tmp);
}

void pc_speaker_beep(uint32_t freq, uint32_t duration_ms) {
    pc_speaker_enable(freq);
    pit_sleep_ms(duration_ms);
    pc_speaker_disable();
}

bool sound_blaster_detect(void) {
    klog_warn("Sound Blaster detection: stub");
    return false;
}

bool sound_blaster_play(const void *data, size_t len) {
    (void)data; (void)len;
    klog_warn("Sound Blaster playback: stub");
    return false;
}
