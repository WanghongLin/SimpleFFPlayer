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

#include "FFCodec.h"
#include "FFUtils.h"

FFCodec* FFCodec::create(FFInputStream* inputStream) {
    if (inputStream != NULL) {
        FFCodec* ffCodec = new FFCodec(inputStream->getCodecParameters(),
                                       inputStream->getCodecContext(), inputStream->getCodec());
        return ffCodec;
    } else {
        return nullptr;
    }
}

void FFCodec::decode(const AVPacket* packet, FrameDecodeCallback* callback) {
    if (isOpened()) {
        int packet_ret;
        do {
            packet_ret = avcodec_send_packet(codecContext, packet);
            if (packet_ret == 0) {
                int frame_ret;
                do {
                    AVFrame* frame = FFUtils::allocateFrame();
                    frame_ret = avcodec_receive_frame(codecContext, frame);
                    if (frame_ret == 0 && callback != NULL) {
                        (*callback)(frame);
                    }
                } while (frame_ret == 0);
            }
        } while (packet_ret == AVERROR(EAGAIN));
    }
}

void FFCodec::openCodec_() {
    if (codecContext != nullptr && codec != nullptr) {
        int ret = avcodec_open2(codecContext, codec, nullptr);
        if (ret < 0) {
            setOpened(false);
            FFUtils::logAVErrorString("could not open codec", ret);
        } else {
            setOpened(true);
        }
    }
}


