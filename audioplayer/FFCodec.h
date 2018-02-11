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

#ifndef FFAUDIOPLAYER_FFCODEC_H
#define FFAUDIOPLAYER_FFCODEC_H

#include "FFInputStream.h"

class FFCodec {

public:
    static FFCodec* create(FFInputStream* inputStream);
    void decode(const AVPacket* packet, FrameDecodeCallback* callback);

    FFCodec(AVCodecParameters* codecParameters, AVCodecContext* codecContext, AVCodec* codec) : codecParameters(
            codecParameters), codecContext(codecContext), codec(codec) {
        openCodec_();
    }

    bool isOpened() const {
        return opened;
    }

    void setOpened(bool opened) {
        FFCodec::opened = opened;
    }

private:
    AVCodecParameters* codecParameters;
    AVCodecContext* codecContext;
    AVCodec* codec;
    bool opened;
    void openCodec_();
};


#endif //FFAUDIOPLAYER_FFCODEC_H
