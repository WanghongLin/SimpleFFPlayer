//
//  MediaFormat.hpp
//  SimpleFFPlayer
//
//  Created by Wanghong Lin on 12/1/16.
//  Copyright © 2016 Wanghong Lin. All rights reserved.
//

#ifndef MediaFormat_hpp
#define MediaFormat_hpp

#include <iostream>
#include <iomanip>

class MediaFormat {
private:
    
    int _width;
    int _height;
    uint8_t _channels;
    int32_t _sampleRate;
    uint16_t _bufferSizeInSamples;
    
public:

    MediaFormat() {}

    MediaFormat(int width, int height, uint8_t channels, int32_t sampleRate, uint16_t bufferSizeInSamples)
    : _width(width), _height(height),
    _channels(channels), _sampleRate(sampleRate), _bufferSizeInSamples(bufferSizeInSamples) {
        std::cout << "channels " << _channels << std::endl;
    }

    friend std::ostream& operator<<(std::ostream& os, const MediaFormat& format) {
        os << "_width: " << format._width << " _height: " << format._height << " _channels: " << static_cast<unsigned>(format._channels)
           << " _sampleRate: " << format._sampleRate << " _bufferSizeInSamples: " << format._bufferSizeInSamples;
        return os;
    }

    int getWidth() const {
        return _width;
    }
    
    int getHeight() const {
        return _height;
    }
    
    uint8_t getChannels() const {
        return _channels;
    }
    
    int32_t getSampleRate() const {
        return _sampleRate;
    }
    
    uint16_t getBufferSizeInSamples() const {
        return _bufferSizeInSamples;
    }
};

#endif /* MediaFormat_hpp */
