#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

// Audio service interface (Dependency Inversion Principle)
// Games depend on this abstraction, not concrete implementation

// Initialize audio system
bool audio_init(void);

// Cleanup audio system
void audio_cleanup(void);

// Sound types (Interface Segregation - specific sound types)
typedef enum {
    AUDIO_SOUND_COIN,
    AUDIO_SOUND_DAMAGE,
    AUDIO_SOUND_VICTORY,
    AUDIO_SOUND_MENU
} AudioSoundType;

// Play a specific sound type
void audio_play_sound(AudioSoundType sound_type);

// Play a custom blip sound (for extensibility)
void audio_play_blip(float frequency, float duration, float volume);

#endif // AUDIO_H

