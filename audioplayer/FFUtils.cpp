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

#include "FFUtils.h"

AVPacket* FFUtils::allocatePacket() {
    AVPacket* packet = av_packet_alloc();
    av_init_packet(packet);
    packet->data = NULL;
    packet->size = 0;
    return packet;
}

AVFrame* FFUtils::allocateFrame() {
    AVFrame* frame = av_frame_alloc();
    return frame;
}

void FFUtils::freePacket(AVPacket* packet) {
    if (packet != nullptr) {
        av_packet_unref(packet);
    }
}

void FFUtils::freeFrame(AVFrame* frame) {
    if (frame != nullptr) {
        av_frame_unref(frame);
    }
}

void FFUtils::logAVErrorString(const char* str, int err) {
    char errbuf[64] = { 0 };
    av_make_error_string(errbuf, 64, err);
    fprintf(stderr, "%s %s", str, errbuf);
}





