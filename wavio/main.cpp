//
//  main.cpp
//  wavio
//
//  Created by Santi on 23/04/14.
//
//

#include <iostream>
#include "WavFile.h"

int main(int argc, const char * argv[])
{
    if(argc<2)
        std::cerr << "Usage: " << argv[0] << " <input file>" << std::endl;
    WavFile wave(argv[1]);
    wave.printHeader();
    //print file duration in seconds
    WAV_header hdr = wave.getHeader();
    int samples = hdr.data.subChunk2Size/(hdr.fmt.bitsPerSample/8);
    double seconds = (double)samples/(double)hdr.fmt.samplingRate;
    std::cerr << "file duration (seconds): " << seconds << std::endl;
    return 0;
}

