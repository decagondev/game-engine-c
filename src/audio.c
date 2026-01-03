#include "../include/audio.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define SAMPLE_RATE 44100
#define BLIP_DURATION 0.1f

static const struct {
    float frequency;
    float duration;
    float volume;
} SOUND_PRESETS[] = {
    [AUDIO_SOUND_COIN] = {800.0f, 0.1f, 0.5f},
    [AUDIO_SOUND_DAMAGE] = {200.0f, 0.15f, 0.6f},
    [AUDIO_SOUND_VICTORY] = {523.25f, 0.1f, 0.4f},
    [AUDIO_SOUND_MENU] = {400.0f, 0.08f, 0.3f}
};

/**
 * Generate a simple blip sound using a sine wave.
 * @param frequency Sound frequency in Hz
 * @param duration Sound duration in seconds
 * @param volume Sound volume (0.0 to 1.0)
 * @return Generated wave structure
 */
static Wave generate_blip(float frequency, float duration, float volume) {
    int sampleCount = (int)(SAMPLE_RATE * duration);
    float* samples = (float*)malloc(sampleCount * sizeof(float) * 2);
    
    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / SAMPLE_RATE;
        float envelope = 1.0f;
        if (i < sampleCount * 0.1f) {
            envelope = (float)i / (sampleCount * 0.1f);
        } else if (i > sampleCount * 0.9f) {
            envelope = 1.0f - ((float)(i - sampleCount * 0.9f) / (sampleCount * 0.1f));
        }
        
        float sample = sinf(2.0f * PI * frequency * t) * volume * envelope;
        samples[i * 2] = sample;
        samples[i * 2 + 1] = sample;
    }
    
    Wave wave = {
        .frameCount = sampleCount,
        .sampleRate = SAMPLE_RATE,
        .sampleSize = 32,
        .channels = 2,
        .data = samples
    };
    
    return wave;
}

bool audio_init(void) {
    InitAudioDevice();
    return IsAudioDeviceReady();
}

void audio_cleanup(void) {
    CloseAudioDevice();
}

void audio_play_sound(AudioSoundType sound_type) {
    if (sound_type < 0 || sound_type >= sizeof(SOUND_PRESETS) / sizeof(SOUND_PRESETS[0])) {
        return;
    }
    
    const float* preset = (const float*)&SOUND_PRESETS[sound_type];
    audio_play_blip(preset[0], preset[1], preset[2]);
}

void audio_play_blip(float frequency, float duration, float volume) {
    Wave wave = generate_blip(frequency, duration, volume);
    Sound sound = LoadSoundFromWave(wave);
    PlaySound(sound);
    UnloadWave(wave);
}
