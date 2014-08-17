//
//  main.cpp
//  waveh
//
//  Created by Santi on 23/04/14.
//

#include <iostream>
#include "Wav_file.h"

int main(int argc, const char * argv[])
{
    if(argc<2){
        std::cerr << "Usage: " << argv[0] << " <input file>" << std::endl;
        return -1;
    }
    Wav_file wave(argv[1]);
    //compute and play the file duration in seconds
    WAV_header hdr = wave.get_header();
    int samples = hdr.data.sub_chunk_2_size/((hdr.fmt.bits_per_sample/8)*hdr.fmt.channels);
    double seconds = (double)samples/(double)hdr.fmt.sample_rate;
    std::cerr << "file duration (seconds): " << seconds << std::endl;
    return 0;
}

