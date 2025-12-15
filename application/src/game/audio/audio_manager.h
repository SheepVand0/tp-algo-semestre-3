#pragma once

#include "SDL3/SDL_mixer.h"
#include "SDL3/SDL.h"
#include "settings.h"

#define AUDIO_MAX_QUEUE 15

typedef struct SAudio
{
    MIX_Audio* TargetAudio;
    char* AudioName;

} SAudio;

typedef struct AudioManager
{
    MIX_Mixer* Mixer;
    SAudio* AudioQueue[AUDIO_MAX_QUEUE];

    char* AssetPath;

} AudioManager;

AudioManager* AudioManager_create();

SAudio* AudioManager_loadWav(AudioManager* manager, char* fileName, char* objectName);

SAudio* SAudio_create();

bool AudioManager_play(AudioManager* manager, SAudio* audio);
