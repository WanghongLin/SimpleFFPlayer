//
// Created by Wanghong Lin on 12/1/16.
//

#ifndef SIMPLEFFPLAYER_MEDIAFRAME_H
#define SIMPLEFFPLAYER_MEDIAFRAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"
#include "libavutil/time.h"

#ifdef __cplusplus
};
#endif
typedef class MediaFrame {

private:
    MediaFrame(AVFrame* frame, AVMediaType mediaType, int64_t pts);
    bool endOfStream;
    AVFrame* avFrame;
    AVMediaType mediaType;
    int64_t pts;

public:
    virtual ~MediaFrame();
    static MediaFrame* create(AVFrame* frame, AVMediaType mediaType, int64_t pts);

    void setEndOfStream(bool endOfStream);

    bool isEndOfStream() const;

    AVFrame* getAVFrame() const;

    AVMediaType getMediaType() const;

    int64_t getPts() const;
} MediaFrame;


#endif //SIMPLEFFPLAYER_MEDIAFRAME_H
