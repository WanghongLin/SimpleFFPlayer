//
// Created by Wanghong Lin on 12/1/16.
//

#ifndef SIMPLEFFPLAYER_MEDIAFRAMEAVAILABLELISTENER_H
#define SIMPLEFFPLAYER_MEDIAFRAMEAVAILABLELISTENER_H

#include "MediaRender.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"

#ifdef __cplusplus
};
#endif

class MediaFrameAvailableListener {

private:

    MediaRender* _pMediaRender;

public:
    MediaFrameAvailableListener(MediaRender* pMediaRender);

    virtual ~MediaFrameAvailableListener();

    void onMediaFrameAvailable(MediaFrame* mediaFrame);
};


#endif //SIMPLEFFPLAYER_MEDIAFRAMEAVAILABLELISTENER_H
