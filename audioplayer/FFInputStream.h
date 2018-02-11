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

#ifndef FFAUDIOPLAYER_FFINPUTSTREAM_H
#define FFAUDIOPLAYER_FFINPUTSTREAM_H

#include "FFStream.h"
#include "Common.h"

class FFInputStream : public FFStream {

public:
    FFInputStream(const string &filePath, AVMediaType mediaType) : FFStream(filePath, mediaType) { open_(); }

    virtual void dump() override;

    virtual void open(string filePath) override;

    virtual void close() override;


    AVCodecParameters* getCodecParameters() const {
        return codecParameters;
    }

    void setCodecParameters(AVCodecParameters* codecParameters) {
        FFInputStream::codecParameters = codecParameters;
    }

    AVCodecContext* getCodecContext() const {
        return codecContext;
    }

    void setCodecContext(AVCodecContext* codecContext) {
        FFInputStream::codecContext = codecContext;
    }

    AVCodec* getCodec() const {
        return codec;
    }

    void setCodec(AVCodec* codec) {
        FFInputStream::codec = codec;
    }

    bool readPacketTo(AVPacket** pkt);

    bool operator >>(AVPacket** pkt);


    AVFormatContext* getFormatContext() const {
        return formatContext;
    }

private:
    void open_();
    AVFormatContext* formatContext;
    AVCodecParameters* codecParameters;
    AVCodecContext* codecContext;
    AVCodec* codec;
};


#endif //FFAUDIOPLAYER_FFINPUTSTREAM_H
