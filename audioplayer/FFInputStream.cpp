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

#include "FFInputStream.h"
#include "FFUtils.h"

void FFInputStream::open(string filePath) {
    setFilePath(filePath);
    open_();
}

void FFInputStream::close() {
    if (isOpened()) {
        avformat_close_input(&formatContext);
        avformat_free_context(formatContext);
    }
}

void FFInputStream::open_() {
    string filePath = getFilePath();

    if (filePath.c_str() != nullptr) {

        formatContext = avformat_alloc_context();
        int ret = avformat_open_input(&formatContext, filePath.c_str(), nullptr, nullptr);
        if (ret < 0) {
            FFUtils::logAVErrorString("open input", ret);
            return;
        }

        if ((ret = avformat_find_stream_info(formatContext, nullptr)) < 0) {
            FFUtils::logAVErrorString("find stream info error", ret);
        }

        ret = av_find_best_stream(formatContext, getMediaType(), -1, -1, &codec, 0);

        if (ret != AVERROR_STREAM_NOT_FOUND) {
            bool shouldFindDecoder = false;
            if (ret == AVERROR_DECODER_NOT_FOUND) {
                shouldFindDecoder = true;
            }

            for (unsigned int i = 0; i < formatContext->nb_streams; ++i) {
                if (formatContext->streams[i]->codecpar->codec_type == getMediaType()) {
                    setStreamIndex(i);
                    codecParameters = formatContext->streams[i]->codecpar;
                    if (shouldFindDecoder) {
                        codec = avcodec_find_decoder(formatContext->streams[i]->codecpar->codec_id);
                    }

                    if (codec != NULL) {
                        codecContext = avcodec_alloc_context3(codec);
                        ret = avcodec_parameters_to_context(codecContext, codecParameters);
                        if (ret < 0) {
                            av_log(NULL, AV_LOG_ERROR, "convert codec parameters to codec context error\n");
                            return;
                        }
                    } else {
                        av_log(NULL, AV_LOG_ERROR, "can not find decoder for stream %d\n", i);
                        return;
                    }
                }
            }

        } else {
            av_log(this, AV_LOG_ERROR, "not stream found\n");
        }
        setOpened(true);
    }
}

void FFInputStream::dump() {
    FFStream::dump();
    if (isOpened()) {
        av_dump_format(formatContext, 0, getFilePath().c_str(), 0);
    }
}

bool FFInputStream::readPacketTo(AVPacket** pkt) {
    if (pkt == NULL) {
        AVPacket* packet = FFUtils::allocatePacket();
        pkt = &packet;
    }

    if (isOpened()) {
        int ret = av_read_frame(formatContext, *pkt);
        return ret >= 0;
    }
    return false;
}

bool FFInputStream::operator>>(AVPacket** pkt) {
    return readPacketTo(pkt);
}
