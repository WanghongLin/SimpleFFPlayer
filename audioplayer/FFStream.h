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

#ifndef FFAUDIOPLAYER_FFSTREAM_H
#define FFAUDIOPLAYER_FFSTREAM_H

#include <string>
#include "Common.h"

using namespace std;

class FFStream {

public:
    FFStream(const string &filePath, AVMediaType mediaType) : filePath(filePath), mediaType(mediaType) {
        av_register_all();
        avcodec_register_all();
    }

    virtual void open(string filePath) = 0;
    virtual void close() = 0;
    virtual void dump() {

    }

    const string &getFilePath() const {
        return filePath;
    }

    void setFilePath(const string &filePath) {
        FFStream::filePath = filePath;
    }

    int getStreamIndex() const {
        return streamIndex;
    }

    void setStreamIndex(int streamIndex) {
        FFStream::streamIndex = streamIndex;
    }

    bool isOpened() const {
        return opened;
    }

    void setOpened(bool opened) {
        FFStream::opened = opened;
    }

    AVMediaType getMediaType() const {
        return mediaType;
    }

    void setMediaType(AVMediaType mediaType) {
        FFStream::mediaType = mediaType;
    }

private:
    string filePath;
    AVMediaType mediaType;
    int streamIndex;
    bool opened;
};


#endif //FFAUDIOPLAYER_FFSTREAM_H
