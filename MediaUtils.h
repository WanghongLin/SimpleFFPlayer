//
// Created by Wanghong Lin on 12/1/16.
//

#ifndef SIMPLEFFPLAYER_MEDIAUTILS_H
#define SIMPLEFFPLAYER_MEDIAUTILS_H

#include <string>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#include "libavutil/avutil.h"

#ifdef __cplusplus
};
#endif

class MediaUtils {

public:
    static string getFFMPEGErrorString(int error) {
        char* errbuf = new char[1024];
        memset(errbuf, 0, 1024);
        av_make_error_string(errbuf, 1024, error);
        return string(errbuf);
    }
};


#endif //SIMPLEFFPLAYER_MEDIAUTILS_H
