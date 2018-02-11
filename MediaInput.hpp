//
//  MediaInput.hpp
//  SimpleFFPlayer
//
//  Created by Wanghong Lin on 11/30/16.
//  Copyright © 2016 Wanghong Lin. All rights reserved.
//

#ifndef MediaInput_hpp
#define MediaInput_hpp

#include <string>
#include <iostream>
#include "MediaFormat.hpp"
#include "MediaFrameAvailableListener.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"

#ifdef __cplusplus
};
#endif

using namespace std;

class MediaInput;

typedef void (*PrepareCallback)(void);
typedef void (*DecodeCallback)(MediaInput* pMediaInput, AVFrame* frame);

class MediaInput {
    
private:
    
    static const string TAG;
    
    string _dataSource;
    
    PrepareCallback _prepareCallback;
    
    static void* _prepare(void* input);
    static int _prepareForStream(AVFormatContext* pFmtCtx, AVCodecContext** pCodecCtx, AVMediaType mediaType, int* streamIndex);

    static void* _start(void* input);
    static void _decode(MediaInput* pMediaInput, AVCodecContext* pCodecCtx, AVPacket* packet, DecodeCallback callback);
    static void _onVideoDecoded(MediaInput* pMediaInput, AVFrame* frame);
    static void _onAudioDecoded(MediaInput* pMediaInput, AVFrame* frame);
    
    void setPrepareCallback(PrepareCallback callback) {
        _prepareCallback = callback;
    }
    
    PrepareCallback getPrepareCallback() {
        return _prepareCallback;
    }

    MediaFrameAvailableListener* _pMediaFrameAvailableListener;

    MediaFormat* pMediaFormat;

    // FFmpeg stuffs
    AVFormatContext* pFmtCtx;
    AVCodecContext* pVideoCtx;
    AVCodecContext* pAudioCtx;
    int videoIndex;
    int audioIndex;
    SwsContext* pSwsCtx;
    SwrContext* pSwrCtx;
public:
    
    MediaInput() {
        
    }
    
    MediaInput(const string& dataSource) : _dataSource(dataSource) {
        av_register_all();
        avcodec_register_all();
    }
    ~MediaInput() {
        if (pMediaFormat != NULL) {
            delete pMediaFormat;
        }
    }
    
    string getDataSource() {
        return _dataSource;
    }
    
    void setDataSource(string dataSource) {
        _dataSource = dataSource;
    }

    void setMediaFrameAvailableListener(MediaFrameAvailableListener* _pMediaFrameAvailableListener);

    MediaFormat* getMediaFormat() const;

    void prepare();
    void prepareAsync(PrepareCallback prepareCallback);

    void startPlay();
};

#endif /* MediaInput_hpp */
