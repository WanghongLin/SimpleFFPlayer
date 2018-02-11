//
//  main.cpp
//  SimpleFFPlayer
//
//  Created by Wanghong Lin on 11/30/16.
//  Copyright © 2016 Wanghong Lin. All rights reserved.
//

#include <iostream>
#include "MediaInput.hpp"

using namespace std;

static const char* parseArguments(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " file_to_play" << std::endl;
        exit(EXIT_FAILURE);
    }

    return argv[1];
}

int main(int argc, const char * argv[]) {

    const char* mediaPath = parseArguments(argc, argv);

    MediaInput* pMedia = new MediaInput(mediaPath);
    pMedia->prepare();

    const MediaFormat* pFormat = pMedia->getMediaFormat();
    MediaRender* pRender = new MediaRender(pFormat);
    pRender->prepare();

    MediaFrameAvailableListener* pListener = new MediaFrameAvailableListener(pRender);
    pMedia->setMediaFrameAvailableListener(pListener);
    pMedia->startPlay();

    pRender->runInLoopMode();
    
    delete pMedia;
    delete pRender;
    delete pListener;
    
    return EXIT_SUCCESS;
}
