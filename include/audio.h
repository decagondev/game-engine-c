#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

typedef enum {
    AUDIO_SOUND_COIN,
    AUDIO_SOUND_DAMAGE,
    AUDIO_SOUND_VICTORY,
    AUDIO_SOUND_MENU
} AudioSoundType;

/**
 * Initialize the audio system.
 * @return true if initialization successful, false otherwise
 */
bool audio_init(void);

/**
 * Cleanup and shutdown the audio system.
 */
void audio_cleanup(void);

/**
 * Play a predefined sound type.
 * @param sound_type The type of sound to play
 */
void audio_play_sound(AudioSoundType sound_type);

/**
 * Play a custom blip sound with specified parameters.
 * @param frequency Sound frequency in Hz
 * @param duration Sound duration in seconds
 * @param volume Sound volume (0.0 to 1.0)
 */
void audio_play_blip(float frequency, float duration, float volume);

#endif
