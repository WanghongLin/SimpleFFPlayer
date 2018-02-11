//
// Created by Wanghong Lin on 12/1/16.
//

#include "MediaFrame.h"

MediaFrame::MediaFrame(AVFrame* frame, AVMediaType mediaType, int64_t pts)
        : avFrame(frame), mediaType(mediaType), pts(pts) {
    endOfStream = false;
}

MediaFrame::~MediaFrame() {
    if (avFrame != NULL) {
        av_freep(&avFrame->data[0]);
        av_frame_free(&avFrame);
    }
}

MediaFrame* MediaFrame::create(AVFrame* frame, AVMediaType mediaType, int64_t pts) {
    MediaFrame* pMediaFrame = new MediaFrame(frame, mediaType, pts);
    return pMediaFrame;
}

bool MediaFrame::isEndOfStream() const {
    return endOfStream;
}

AVFrame* MediaFrame::getAVFrame() const {
    return avFrame;
}

AVMediaType MediaFrame::getMediaType() const {
    return mediaType;
}

int64_t MediaFrame::getPts() const {
    return pts;
}

void MediaFrame::setEndOfStream(bool endOfStream) {
    MediaFrame::endOfStream = endOfStream;
}
