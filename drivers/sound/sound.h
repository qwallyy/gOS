/* gOS Kernel — Sound Blaster / PC Speaker Driver Stubs
 * Reference: https://wiki.osdev.org/Sound_Blaster
 *            https://wiki.osdev.org/PC_Speaker
 */

#ifndef GOS_SOUND_H
#define GOS_SOUND_H

#include "gOS/types.h"

#define PC_SPEAKER_PORT     0x61
#define PIT_CHANNEL2        0x42
#define PIT_COMMAND         0x43

void sound_init(void);
void pc_speaker_enable(uint32_t freq);
void pc_speaker_disable(void);
void pc_speaker_beep(uint32_t freq, uint32_t duration_ms);
bool sound_blaster_detect(void);
bool sound_blaster_play(const void *data, size_t len);

#endif /* GOS_SOUND_H */
