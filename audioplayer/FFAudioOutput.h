/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// Created by Wanghong Lin on 8/10/16.
//

#ifndef FFAUDIOPLAYER_FFOUTPUT_H
#define FFAUDIOPLAYER_FFOUTPUT_H

#include "SDL.h"
#include "Common.h"
#include "FFUtils.h"
#include "FFInputStream.h"

class FFAudioOutput {

public:
    FFAudioOutput(AVFormatContext* formatContext, AVCodecContext* codecContext, int streamIndex) : formatContext(
            formatContext), codecContext(codecContext), streamIndex(streamIndex) { initialize(); }

    virtual ~FFAudioOutput() {
        SDL_CloseAudio();
        SDL_AudioQuit();
        swr_free(&swrContext);
        if (obtainedAudioSpec != NULL) {
            av_free(obtainedAudioSpec);
        }
    }

    static FFAudioOutput* create(FFInputStream* inputStream);
    void play(AVFrame* frame);

private:
    void initialize();
    AVFormatContext* formatContext;
    AVCodecContext* codecContext;
    int streamIndex;
    SwrContext* swrContext;
    SDL_AudioSpec* obtainedAudioSpec;
    int64_t startTime;
    bool prepared;
};


#endif //FFAUDIOPLAYER_FFOUTPUT_H
