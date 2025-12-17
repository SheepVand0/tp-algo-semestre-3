#include "audio_manager.h"

AudioManager* AudioManager_create()
{
    AudioManager* l_Manager = (AudioManager*)malloc(sizeof(AudioManager));

    assert(l_Manager);
    l_Manager->AssetPath = "../../../assets/music/";
    if (!MIX_Init())
    {
        printf("\nCannot init mixer librarie!\n");
        printf("\n%s\n", SDL_GetError());
    }
    

    l_Manager->Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!l_Manager->Mixer)
    {
        printf("\nError while creating mixer\n");
        printf("\n%s\n", SDL_GetError());
    }


    return l_Manager;
}

SAudio* AudioManager_loadWav(AudioManager* manager, char* fileName, char* objectName)
{
    SAudio* l_Audio = SAudio_create();

    char* l_Path = calloc(1024, sizeof(char));
    assert(l_Path);
    sprintf(l_Path, "%s%s", manager->AssetPath, fileName);
    //sprintf(l_Path, "%s", fileName);

    printf("\nAudio path: %s\n", l_Path);

    l_Audio->TargetAudio = MIX_LoadAudio(manager->Mixer, l_Path, true);
    assert(l_Audio->TargetAudio);

    l_Audio->AudioName = objectName;

    return l_Audio;
}

SAudio* SAudio_create()
{
    SAudio* l_Audio = malloc(sizeof(SAudio));
    assert(l_Audio);

    return l_Audio;
}

bool AudioManager_play(AudioManager* manager, SAudio* audio)
{
    return MIX_PlayAudio(manager->Mixer, audio->TargetAudio);
}

void AudioManager_stopAll(AudioManager* manager)
{
    MIX_StopAllTracks(manager->Mixer, 0);
}
