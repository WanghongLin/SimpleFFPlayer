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

#include <iostream>
#include "FFInputStream.h"
#include "FFUtils.h"
#include "FFCodec.h"
#include "FFAudioOutput.h"

using namespace std;

static void onFrameAvailable(AVFrame* frame);
static FFAudioOutput* ffAudioOutput;

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " file_to_play" << endl;
        return EXIT_FAILURE;
    }

    string filePath(argv[1]);

    FFInputStream* inputStream = new FFInputStream(filePath, AVMEDIA_TYPE_AUDIO);
    FFCodec* ffCodec = FFCodec::create(inputStream);

    inputStream->dump();

    ffAudioOutput = FFAudioOutput::create(inputStream);
    AVPacket* pkt = FFUtils::allocatePacket();
    while ((*inputStream) >> (&pkt)) {
        ffCodec->decode(pkt, &onFrameAvailable);
    }

    FFUtils::freePacket(pkt);
    return 0;
}

static void onFrameAvailable(AVFrame* frame) {
    ffAudioOutput->play(frame);
    FFUtils::freeFrame(frame);
}