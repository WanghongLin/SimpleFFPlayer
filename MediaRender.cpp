//
//  MediaRender.cpp
//  SimpleFFPlayer
//
//  Created by Wanghong Lin on 11/30/16.
//  Copyright © 2016 Wanghong Lin. All rights reserved.
//

#include "MediaRender.hpp"

const string MediaRender::TAG = "MediaRender: ";

void MediaRender::runInLoopMode() {
    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_WINDOWEVENT
            && event.window.event == SDL_WINDOWEVENT_CLOSE) {
            break;
        }
    }
    cout << TAG << "Quit" << endl;
    SDL_Quit();
}

void MediaRender::writeMediaFrame(MediaFrame* mediaFrame) {
    if (mediaFrame->getMediaType() == AVMEDIA_TYPE_VIDEO) {
        pthread_rwlock_wrlock(&videoFrameLock);
        videoFrameList.push_back(mediaFrame);
        pthread_rwlock_unlock(&videoFrameLock);
    } else if (mediaFrame->getMediaType() == AVMEDIA_TYPE_AUDIO) {
        pthread_rwlock_wrlock(&audioFrameLock);
        audioFrameList.push_back(mediaFrame);
        pthread_rwlock_unlock(&audioFrameLock);
    }
    cout << TAG << "write media frame " << mediaFrame->getPts() << endl;
}

int MediaRender::_videoRenderThread(void* data) {
    MediaRender* pRender = static_cast<MediaRender*>(data);
    int64_t startTime = av_gettime();
    while (true) {
        pthread_rwlock_rdlock(&pRender->videoFrameLock);

        if (pRender->videoFrameList.size() > 0) {
            MediaFrame* frame = pRender->videoFrameList.front();

            if (frame == NULL) {
                pthread_rwlock_unlock(&pRender->videoFrameLock);
                continue;
            }

            if (frame->isEndOfStream()) {
                delete frame;
                pthread_rwlock_unlock(&pRender->videoFrameLock);
                break;
            }

            if (startTime + frame->getPts() > av_gettime()) {
                pthread_rwlock_unlock(&pRender->videoFrameLock);
                continue;
            }

            pRender->videoFrameList.pop_front();
            SDL_UpdateYUVTexture(pRender->sdlTexture, NULL,
                                 frame->getAVFrame()->data[0], frame->getAVFrame()->width,
                                 frame->getAVFrame()->data[1], frame->getAVFrame()->width / 2,
                                 frame->getAVFrame()->data[2], frame->getAVFrame()->width / 2);
            SDL_RenderClear(pRender->sdlRenderer);
            SDL_RenderCopy(pRender->sdlRenderer, pRender->sdlTexture, NULL, NULL);
            SDL_RenderPresent(pRender->sdlRenderer);

            delete frame;
        }

        pthread_rwlock_unlock(&pRender->videoFrameLock);
    }
    return 0;
}

int MediaRender::_audioRenderThread(void* data) {
    MediaRender* pRender = static_cast<MediaRender*>(data);
    SDL_PauseAudio(0);
    int64_t startTime = av_gettime();
    while (true) {
        pthread_rwlock_rdlock(&pRender->audioFrameLock);

        if (pRender->audioFrameList.size() > 0) {
            MediaFrame* frame = pRender->audioFrameList.front();

            if (frame == NULL) {
                pthread_rwlock_unlock(&pRender->audioFrameLock);
                continue;
            }

            if (frame->isEndOfStream()) {
                delete frame;
                pthread_rwlock_unlock(&pRender->audioFrameLock);
                break;
            }

            if (startTime + frame->getPts() > av_gettime()) {
                pthread_rwlock_unlock(&pRender->audioFrameLock);
                continue;
            }

            pRender->audioFrameList.pop_front();
            SDL_QueueAudio(1, frame->getAVFrame()->data[0], (Uint32) frame->getAVFrame()->linesize[0]);
            delete frame;
        }

        pthread_rwlock_unlock(&pRender->audioFrameLock);
    }
    SDL_PauseAudio(1);
    av_log(NULL, AV_LOG_INFO, "audio render thread end\n");
    return 0;
}

MediaRender::~MediaRender() {
    pthread_rwlock_destroy(&videoFrameLock);
    pthread_rwlock_destroy(&audioFrameLock);

    SDL_VideoQuit();
    SDL_AudioQuit();
}
