//
// Created by Wanghong Lin on 12/1/16.
//

#include "MediaFrameAvailableListener.h"

MediaFrameAvailableListener::MediaFrameAvailableListener(MediaRender* pMediaRender) : _pMediaRender(pMediaRender) {}

MediaFrameAvailableListener::~MediaFrameAvailableListener() {

}

void MediaFrameAvailableListener::onMediaFrameAvailable(MediaFrame* mediaFrame) {
    _pMediaRender->writeMediaFrame(mediaFrame);
}
