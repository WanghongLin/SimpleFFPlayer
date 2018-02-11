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

#include "FFAudioOutput.h"

void FFAudioOutput::play(AVFrame* frame) {
    if (!prepared) {
        av_log(NULL, AV_LOG_ERROR, "output not prepared, skip play frame\n");
        return;
    }

    int64_t pts = av_frame_get_best_effort_timestamp(frame);
    pts = av_rescale_q(pts, formatContext->streams[streamIndex]->time_base, AV_TIME_BASE_Q);
    av_log(NULL, AV_LOG_INFO, "play frame with pts %ld\n", pts);

    while ((startTime + pts) > av_gettime()) {
        ;
    }

    AVFrame* sdlFrame = av_frame_alloc();

    sdlFrame->nb_samples = frame->nb_samples;
    sdlFrame->sample_rate = obtainedAudioSpec->freq;
    sdlFrame->format = AV_SAMPLE_FMT_S16;
    sdlFrame->channels = obtainedAudioSpec->channels;

    av_samples_alloc(sdlFrame->data, sdlFrame->linesize, obtainedAudioSpec->channels, frame->nb_samples, AV_SAMPLE_FMT_S16, 0);
    int ret = swr_convert(swrContext, sdlFrame->data, frame->nb_samples, (const uint8_t**) frame->data, frame->nb_samples);
    if (ret > 0) {
        SDL_QueueAudio(1, sdlFrame->data[0], (Uint32) sdlFrame->linesize[0]);
    } else {
        FFUtils::logAVErrorString("convert frame error", ret);
    }

    av_frame_free(&sdlFrame);
}

FFAudioOutput* FFAudioOutput::create(FFInputStream* inputStream) {
    return new FFAudioOutput(inputStream->getFormatContext(), inputStream->getCodecContext(),
                             inputStream->getStreamIndex());
}

void FFAudioOutput::initialize() {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);

    SDL_AudioSpec* desired = (SDL_AudioSpec*) av_mallocz(sizeof(SDL_AudioSpec));
    desired->format = AUDIO_S16SYS;
    desired->channels = (Uint8) codecContext->channels;
    desired->freq = codecContext->sample_rate;
    desired->samples = (Uint16) (codecContext->frame_size * 2);

    obtainedAudioSpec = (SDL_AudioSpec*) av_mallocz(sizeof(SDL_AudioSpec));
    SDL_OpenAudio(desired, obtainedAudioSpec);

    swrContext = swr_alloc_set_opts(nullptr,
                                    av_get_default_channel_layout(obtainedAudioSpec->channels), AV_SAMPLE_FMT_S16, obtainedAudioSpec->freq,
                                    av_get_default_channel_layout(codecContext->channels), codecContext->sample_fmt, codecContext->sample_rate,
                                    0, nullptr);
    int ret = swr_init(swrContext);
    if (ret < 0) {
        FFUtils::logAVErrorString("initialize swr context error", ret);
        prepared = false;
        return;
    }

    prepared = true;
    SDL_PauseAudio(0);

    this->startTime = av_gettime();
}