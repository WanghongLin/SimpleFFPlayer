//
//  MediaInput.cpp
//  SimpleFFPlayer
//
//  Created by Wanghong Lin on 11/30/16.
//  Copyright © 2016 Wanghong Lin. All rights reserved.
//

#include "MediaInput.hpp"
#include "MediaUtils.h"

const string MediaInput::TAG = "MediaInput: ";

void* MediaInput::_prepare(void* input) {
    
    MediaInput* pMedia = static_cast<MediaInput*>(input);
    
    cout << TAG << "will play file " << pMedia->getDataSource() << endl;
    
    cout << MediaInput::TAG << "prepare in thread id " << pthread_self() << endl;
    
    int ret = 0;
    pMedia->pFmtCtx = avformat_alloc_context();
    ret = avformat_open_input(&pMedia->pFmtCtx, pMedia->getDataSource().c_str(), NULL, NULL);
    if (ret < 0) {
        cerr << TAG << "open input " << MediaUtils::getFFMPEGErrorString(ret) << endl;
        goto end;
    }
    
    avformat_find_stream_info(pMedia->pFmtCtx, NULL);
    av_dump_format(pMedia->pFmtCtx, 0, pMedia->_dataSource.c_str(), 0);
    if (_prepareForStream(pMedia->pFmtCtx, &pMedia->pVideoCtx, AVMEDIA_TYPE_VIDEO, &pMedia->videoIndex) < 0) {
        cout << TAG << "prepare video stream" << endl;
        goto end;
    }

    if (_prepareForStream(pMedia->pFmtCtx, &pMedia->pAudioCtx, AVMEDIA_TYPE_AUDIO, &pMedia->audioIndex) < 0) {
        cout << TAG << "prepare audio stream" << endl;
        goto end;
    }

    pMedia->pMediaFormat = new MediaFormat(pMedia->pVideoCtx->width, pMedia->pVideoCtx->height,
                                           1, pMedia->pAudioCtx->sample_rate, (uint16_t) pMedia->pAudioCtx->frame_size);

    cout << *(pMedia->getMediaFormat()) << endl;

    pMedia->pSwsCtx = sws_getContext(pMedia->pVideoCtx->width, pMedia->pVideoCtx->height, pMedia->pVideoCtx->pix_fmt,
                   pMedia->pMediaFormat->getWidth(), pMedia->pMediaFormat->getHeight(), AV_PIX_FMT_YUV420P,
                   SWS_BICUBLIN, NULL, NULL, NULL);
    pMedia->pSwrCtx = swr_alloc_set_opts(NULL,
                                         av_get_default_channel_layout(pMedia->pMediaFormat->getChannels()),
                                         AV_SAMPLE_FMT_S16, pMedia->pMediaFormat->getSampleRate(),
                                         av_get_default_channel_layout(pMedia->pAudioCtx->channels),
                                         pMedia->pAudioCtx->sample_fmt, pMedia->pAudioCtx->sample_rate, 0, NULL);
    if (swr_init(pMedia->pSwrCtx) < 0) {
        av_log(NULL, AV_LOG_ERROR, "swr_init");
    }

    if (pMedia->getPrepareCallback() != NULL) {
        pMedia->getPrepareCallback()();
    }
    
end:
    return NULL;
}

int MediaInput::_prepareForStream(AVFormatContext* pFmtCtx, AVCodecContext** pCodecCtx, AVMediaType mediaType, int* streamIndex) {
    
    int ret = 0;

    AVCodec* pCodec = NULL;

    if ((ret = av_find_best_stream(pFmtCtx, mediaType, -1, -1, &pCodec, 0)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "can not find stream for type %d", mediaType);
        return -1;
    }

    bool find_decoder = false;
    *streamIndex = -1;
    if (ret != AVERROR_STREAM_NOT_FOUND) {
        if (ret == AVERROR_DECODER_NOT_FOUND) {
            find_decoder = true;
        }

        for (unsigned i = 0; i < pFmtCtx->nb_streams; ++i) {
            if (pFmtCtx->streams[i]->codecpar->codec_type == mediaType) {
                *streamIndex = i;
                if (find_decoder) {
                    pCodec = avcodec_find_decoder(pFmtCtx->streams[i]->codecpar->codec_id);
                }
                break;
            }
        }
    }
    if (*streamIndex != -1 && pCodec != NULL) {
        *pCodecCtx = avcodec_alloc_context3(pCodec);
        avcodec_parameters_to_context(*pCodecCtx, pFmtCtx->streams[*streamIndex]->codecpar);

        if (*pCodecCtx != NULL) {
            if ((ret = avcodec_open2(*pCodecCtx, pCodec, NULL)) < 0) {
                av_log(NULL, AV_LOG_ERROR, "open decoder");
                avcodec_free_context(pCodecCtx);
                return -1;
            }
        }
    } else {
        av_log(NULL, AV_LOG_ERROR, "prepare for stream type %d error", mediaType);
        return -1;
    }

    return ret;
}

void MediaInput::prepare() {
    _prepare(this);
}

void MediaInput::prepareAsync(PrepareCallback callback) {
    setPrepareCallback(callback);
    
    pthread_t pthread = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    pthread_create(&pthread, &attr, &MediaInput::_prepare, this);

    pthread_attr_destroy(&attr);
}

MediaFormat* MediaInput::getMediaFormat() const {
    return pMediaFormat;
}

void MediaInput::setMediaFrameAvailableListener(MediaFrameAvailableListener* pMediaFrameAvailableListener) {
    _pMediaFrameAvailableListener = pMediaFrameAvailableListener;
}

void MediaInput::startPlay() {

    pthread_t pthread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&pthread, &attr, &MediaInput::_start, this);

    pthread_attr_destroy(&attr);
}

void* MediaInput::_start(void* input) {

    MediaInput* pMedia = static_cast<MediaInput*>(input);

    AVPacket* packet = av_packet_alloc();
    av_init_packet(packet);
    packet->data = NULL;
    packet->size = 0;
    av_log(NULL, AV_LOG_INFO, "playback start\n");
    while (av_read_frame(pMedia->pFmtCtx, packet) == 0) {
        if (packet->stream_index == pMedia->videoIndex) {
            MediaInput::_decode(pMedia, pMedia->pVideoCtx, packet, &MediaInput::_onVideoDecoded);
        } else if (packet->stream_index == pMedia->audioIndex) {
            MediaInput::_decode(pMedia, pMedia->pAudioCtx, packet, &MediaInput::_onAudioDecoded);
        }
    }

    MediaFrame* videoEndFrame = MediaFrame::create(NULL, AVMEDIA_TYPE_VIDEO, 0);
    videoEndFrame->setEndOfStream(true);
    pMedia->_pMediaFrameAvailableListener->onMediaFrameAvailable(videoEndFrame);

    MediaFrame* audioEndFrame = MediaFrame::create(NULL, AVMEDIA_TYPE_AUDIO, 0);
    audioEndFrame->setEndOfStream(true);
    pMedia->_pMediaFrameAvailableListener->onMediaFrameAvailable(audioEndFrame);

    av_log(NULL, AV_LOG_INFO, "playback end\n");

    av_packet_free(&packet);
    return nullptr;
}

void
MediaInput::_decode(MediaInput* pMediaInput, AVCodecContext* pCodecCtx, AVPacket* packet, DecodeCallback callback) {
    int packet_ret = 0;
    do {
        packet_ret = avcodec_send_packet(pCodecCtx, packet);
        if (packet_ret == 0) {
            int frame_ret = 0;
            do {
                AVFrame* frame = av_frame_alloc();
                frame_ret = avcodec_receive_frame(pCodecCtx, frame);
                if (frame_ret == 0) {
                    callback(pMediaInput, frame);
                }
                av_frame_free(&frame);
            } while (frame_ret == 0);
        }
    } while (packet_ret == AVERROR(EAGAIN));
}

void MediaInput::_onVideoDecoded(MediaInput* pMediaInput, AVFrame* frame) {
    int64_t pts = av_frame_get_best_effort_timestamp(frame);
    pts = av_rescale_q(pts, pMediaInput->pFmtCtx->streams[pMediaInput->videoIndex]->time_base, AV_TIME_BASE_Q);

    frame->width = pMediaInput->pVideoCtx->width;
    frame->height = pMediaInput->pVideoCtx->height;

    int ret = 0;
    AVFrame* frameYUV = av_frame_alloc();
    frameYUV->width = pMediaInput->pMediaFormat->getWidth();
    frameYUV->height = pMediaInput->pMediaFormat->getHeight();
    frameYUV->format = AV_PIX_FMT_YUV420P;
    ret = av_image_alloc(frameYUV->data, frameYUV->linesize, pMediaInput->pMediaFormat->getWidth(), pMediaInput->pMediaFormat->getHeight(),
                   AV_PIX_FMT_YUV420P, 32);

    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "allocate image\n");
    }

    ret = sws_scale(pMediaInput->pSwsCtx, (const uint8_t* const*) frame->data, frame->linesize, 0, frame->height, frameYUV->data, frameYUV->linesize);
    if (ret > 0) {
        if (pMediaInput->_pMediaFrameAvailableListener != NULL) {
            MediaFrame* mediaFrame = MediaFrame::create(frameYUV, AVMEDIA_TYPE_VIDEO, pts);
            pMediaInput->_pMediaFrameAvailableListener->onMediaFrameAvailable(mediaFrame);
        }
    }
}

void MediaInput::_onAudioDecoded(MediaInput* pMediaInput, AVFrame* frame) {
    int64_t pts = av_frame_get_best_effort_timestamp(frame);
    pts = av_rescale_q(pts, pMediaInput->pFmtCtx->streams[pMediaInput->audioIndex]->time_base, AV_TIME_BASE_Q);

    AVFrame* frameS16 = av_frame_alloc();
    frameS16->channels = pMediaInput->getMediaFormat()->getChannels();
    frameS16->nb_samples = frame->nb_samples;
    frameS16->sample_rate = pMediaInput->getMediaFormat()->getSampleRate();

    int ret = 0;
    ret = av_samples_alloc(frameS16->data, frameS16->linesize, frameS16->channels, frameS16->nb_samples, AV_SAMPLE_FMT_S16, 32);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "allocate samples\n");
    }

    ret = swr_convert(pMediaInput->pSwrCtx, frameS16->data, frameS16->nb_samples, (const uint8_t**) frame->data, frame->nb_samples);
    if (ret > 0) {
        if (pMediaInput->_pMediaFrameAvailableListener != NULL) {
            MediaFrame* mediaFrame = MediaFrame::create(frameS16, AVMEDIA_TYPE_AUDIO, pts);
            pMediaInput->_pMediaFrameAvailableListener->onMediaFrameAvailable(mediaFrame);
        }
    }
}
