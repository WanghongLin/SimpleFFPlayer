//
//  MediaRender.hpp
//  SimpleFFPlayer
//
//  Created by Wanghong Lin on 11/30/16.
//  Copyright © 2016 Wanghong Lin. All rights reserved.
//

#ifndef MediaRender_hpp
#define MediaRender_hpp

#include <iostream>
#include <string>
#include <list>
#include "SDL.h"
#include "MediaFormat.hpp"
#include "MediaFrame.h"

using namespace std;

class MediaRender {
private:
    static const string TAG;
    const MediaFormat* _pMediaFormat;
    std::list<MediaFrame*> videoFrameList;
    std::list<MediaFrame*> audioFrameList;
    pthread_rwlock_t videoFrameLock;
    pthread_rwlock_t audioFrameLock;

    static int _videoRenderThread(void* data);
    static int _audioRenderThread(void* data);

    SDL_Renderer* sdlRenderer;
    SDL_Texture* sdlTexture;
public:
    
    MediaRender() {

    }
    
    MediaRender(const MediaFormat*& format) : _pMediaFormat(format) {
        pthread_rwlockattr_t video_rwlockattr{};
        pthread_rwlockattr_t audio_rwlockattr{};

        pthread_rwlockattr_init(&video_rwlockattr);
        pthread_rwlockattr_init(&audio_rwlockattr);

        pthread_rwlock_init(&videoFrameLock, &video_rwlockattr);
        pthread_rwlock_init(&audioFrameLock, &audio_rwlockattr);

        pthread_rwlockattr_destroy(&video_rwlockattr);
        pthread_rwlockattr_destroy(&audio_rwlockattr);
    }

    virtual ~MediaRender();

    void prepare() {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
        SDL_Window* window = SDL_CreateWindow("Simple FFPlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                              _pMediaFormat->getWidth(), _pMediaFormat->getHeight(), 0);
        sdlRenderer = SDL_CreateRenderer(window, -1, 0);
        sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING,
                                       _pMediaFormat->getWidth(), _pMediaFormat->getHeight());
        SDL_ShowWindow(window);

        SDL_AudioSpec sdlAudioSpec;
        sdlAudioSpec.callback = NULL;
        sdlAudioSpec.channels = _pMediaFormat->getChannels();
        sdlAudioSpec.format = AUDIO_S16SYS;
        sdlAudioSpec.freq = _pMediaFormat->getSampleRate();
        sdlAudioSpec.samples = _pMediaFormat->getBufferSizeInSamples();
        if (SDL_OpenAudio(&sdlAudioSpec, NULL) < 0) {
            av_log(NULL, AV_LOG_ERROR, "SDL open video error\n");
        }

        SDL_CreateThread(&MediaRender::_videoRenderThread, "video", this);
        SDL_CreateThread(&MediaRender::_audioRenderThread, "audio", this);
    }

    void writeMediaFrame(MediaFrame* mediaFrame);
    
    void runInLoopMode();
};

#endif /* MediaRender_hpp */
