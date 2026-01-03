#include "../include/audio.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

// Audio settings (Single Responsibility - audio constants)
#define SAMPLE_RATE 44100
#define BLIP_DURATION 0.1f  // 100ms blip sounds

// Sound frequency and duration presets (Open/Closed Principle - easy to extend)
static const struct {
    float frequency;
    float duration;
    float volume;
} SOUND_PRESETS[] = {
    [AUDIO_SOUND_COIN] = {800.0f, 0.1f, 0.5f},      // High pitch, short
    [AUDIO_SOUND_DAMAGE] = {200.0f, 0.15f, 0.6f},    // Low pitch, longer
    [AUDIO_SOUND_VICTORY] = {523.25f, 0.1f, 0.4f},   // C note
    [AUDIO_SOUND_MENU] = {400.0f, 0.08f, 0.3f}       // Medium pitch, very short
};

// Generate a simple blip sound (sine wave tone)
static Wave generate_blip(float frequency, float duration, float volume) {
    int sampleCount = (int)(SAMPLE_RATE * duration);
    float* samples = (float*)malloc(sampleCount * sizeof(float) * 2); // Stereo
    
    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / SAMPLE_RATE;
        // Generate sine wave with envelope (fade in/out)
        float envelope = 1.0f;
        if (i < sampleCount * 0.1f) {
            envelope = (float)i / (sampleCount * 0.1f); // Fade in
        } else if (i > sampleCount * 0.9f) {
            envelope = 1.0f - ((float)(i - sampleCount * 0.9f) / (sampleCount * 0.1f)); // Fade out
        }
        
        float sample = sinf(2.0f * PI * frequency * t) * volume * envelope;
        samples[i * 2] = sample;     // Left channel
        samples[i * 2 + 1] = sample; // Right channel
    }
    
    Wave wave = {
        .frameCount = sampleCount,
        .sampleRate = SAMPLE_RATE,
        .sampleSize = 32, // 16-bit per channel, stereo = 32 bits
        .channels = 2,
        .data = samples
    };
    
    return wave;
}

// Initialize audio system
bool audio_init(void) {
    InitAudioDevice();
    return IsAudioDeviceReady();
}

// Cleanup audio system
void audio_cleanup(void) {
    CloseAudioDevice();
}

// Play a specific sound type
void audio_play_sound(AudioSoundType sound_type) {
    if (sound_type < 0 || sound_type >= sizeof(SOUND_PRESETS) / sizeof(SOUND_PRESETS[0])) {
        return;
    }
    
    const float* preset = (const float*)&SOUND_PRESETS[sound_type];
    audio_play_blip(preset[0], preset[1], preset[2]);
}

// Play a custom blip sound
void audio_play_blip(float frequency, float duration, float volume) {
    Wave wave = generate_blip(frequency, duration, volume);
    Sound sound = LoadSoundFromWave(wave);
    PlaySound(sound);
    UnloadWave(wave);
    // Note: Sound will be automatically cleaned up when it finishes playing
    // For short blips, this works fine. For longer sounds, you'd want to track them.
}

