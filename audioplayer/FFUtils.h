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

#ifndef FFAUDIOPLAYER_FFUTILS_H
#define FFAUDIOPLAYER_FFUTILS_H

#include "Common.h"

class FFUtils {

public:
    static AVPacket* allocatePacket();
    static AVFrame* allocateFrame();
    static void freePacket(AVPacket* packet);
    static void freeFrame(AVFrame* frame);
    static void logAVErrorString(const char* str, int err);
};


#endif //FFAUDIOPLAYER_FFUTILS_H
