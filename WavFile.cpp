//
//  WavFile.cpp
//  wavReader
//
//  Created by Santi on 30/07/13.
//  Copyright (c) 2013 Santi. All rights reserved.
//

#include "WavFile.h"
#include <cstdlib>
#include <cstring>

using namespace std;

WavFile::~WavFile(){
    if(header){
        free(header);
    }
}

WavFile::WavFile(std::string filePath):filePath(filePath){
    //read the wave file in header and data
    header = NULL;
    readWavFile();
}

int WavFile::readWavFile(std::vector<short> &data){
    readWavFile();
    copy(cached_data.begin(), cached_data.end(), data.begin());
    return 0;
}

int WavFile::readWavFile(){
    FILE *sound;
    RIFF_h RIFF_hdr;
    fmt_h fmt_hdr;
    data_h data_hdr;
    unsigned int sizeBuffer = 0, fileSize = 0, dataSize = 0;
    unsigned long int firstChunksSize = 0, headerSize = 0;
    char idBuffer[4];
    bool otherChunks = false, dataHeaderFound = false;
    
    if(!(sound=fopen(filePath.c_str(),"rb"))){
        cerr << endl << "The file does not exist" << endl << endl;
        return ERR_FILE_NOT_FOUND;
    }
    if(fread(&RIFF_hdr, RIFF_HEADER_SIZE, 1, sound) < 1){
        cerr << "Failed reading the file, it might be corrupted" << endl;
        return ERR_READING_FILE;
    }
    if(NOT_RIFF){
        cerr << "Not a valid RIFF format file." << endl;
        return ERR_FILE_NOT_WAV;
    }
    fileSize = RIFF_hdr.fileSize;
    //read fmt header->composed of 24 bytes
    fread(&fmt_hdr, FMT_HEADER_SIZE, 1, sound);
    if(fmt_hdr.subChunk1Size == 18) fseek(sound, 2, SEEK_CUR);
    if(fmt_hdr.subChunk1Size == 40) fseek(sound, 28, SEEK_CUR);
    while(!dataHeaderFound){
        fread(idBuffer, 4, 1, sound);
        if(NOT_DATA){
            if(fread(&sizeBuffer, 4, 1, sound)<1){
                cerr << "Not readen correctly the subChunk2Size" << endl;
                return -1;
            }
            fseek(sound, sizeBuffer, SEEK_CUR);
            firstChunksSize = ftell(sound);
            otherChunks = true;
            
        }
        else{
            if(!otherChunks){
                fseek(sound, -4, SEEK_CUR);
                headerSize = sizeof(RIFF_hdr) + sizeof(fmt_hdr) + sizeof(data_hdr);
                if(fmt_hdr.subChunk1Size == 18) headerSize += 2;
                if(fmt_hdr.subChunk1Size == 40) headerSize += 28;
                
            }
            else {
                fseek(sound, firstChunksSize, SEEK_SET);
                headerSize = firstChunksSize + (long)sizeof(data_hdr);
            }
            
            fread(&data_hdr.subChunk2ID, 4, 1, sound);
            fread(&data_hdr.subChunk2Size, 4, 1, sound);
            dataSize = data_hdr.subChunk2Size;
            dataHeaderFound = true;
            if(fmt_hdr.subChunk1Size == 18) dataSize += 2;
            if(fmt_hdr.subChunk1Size == 40) dataSize += 28;
        }
        
    }
    if(fmt_hdr.audioFormat>1){
        cerr << "Error: can't read this format" << endl;
        return -1;
    }
    //Save data in dynamic array
    cached_data.clear();
    cached_data.resize(dataSize/(fmt_hdr.bitsPerSample/8));
    if(fread(&cached_data[0],1,dataSize,sound) == dataSize){
        cerr << endl << "Readen successfully: " << dataSize << " bytes" << endl;
    }
    fclose(sound);
    //Save the header
    if(header){
        cerr << "freeing memory" << endl;
        free(header);
    }
    header = (WAV_header *)malloc(sizeof(WAV_header));
    header->riff = RIFF_hdr;
    header->fmt = fmt_hdr;
    header->data = data_hdr;
    return dataSize;
}

void WavFile::printHeader(){
    if(!header) return;
    cerr << "=====================================" << endl;
    cerr << "RIFF ID: " << header->riff.RIFF[0] << header->riff.RIFF[1] <<
    header->riff.RIFF[2] << header->riff.RIFF[3] << endl;
    cerr << "WAVE ID: " << header->riff.WAVE[0] << header->riff.WAVE[1] <<
    header->riff.WAVE[2] << header->riff.WAVE[3] << endl;
    cerr << "FMT ID: " << header->fmt.fmt[0] << header->fmt.fmt[1] <<
    header->fmt.fmt[2] << header->fmt.fmt[3] << endl;
    switch(header->fmt.audioFormat){
        case 1:
            cerr << "Format: PCM" << endl;
            break;
    }
    cerr << "Channels: " << header->fmt.numOfChan << endl;
    cerr << "Sampling rate: " << header->fmt.samplingRate << " Hz" << endl;
    cerr << "Bytes per sec: " << header->fmt.bytesPerSec << " Bytes/s" << endl;
    cerr << "Data size (Audio): " << header->data.subChunk2Size << " bytes" << endl;
    cerr << "Header size: " << sizeof(header->fmt) + sizeof(header->data) + sizeof(header->riff) << " bytes" << endl;
    cerr << "=====================================" << endl;
}

/* Function to set a 44 bytes header->specifying: fm, bits/sample, dataSize and #channels */
int WavFile::setHeader(unsigned int samplingRate, unsigned short bitsPerSample, int dataSize,
                       unsigned short numOfChan){
    int datasize = dataSize;
    if(bitsPerSample == 16) datasize *= 2;
    header = (WAV_header *)malloc(sizeof(RIFF_h)+sizeof(data_h)+sizeof(fmt_h));
    strcpy(header->riff.RIFF, h_names.riff_name);
    header->riff.fileSize = datasize+STANDARD_HEADER_SIZE;
    strcpy(header->riff.WAVE, h_names.wave_name);
    strcpy(header->fmt.fmt, h_names.fmt_name);
    header->fmt.subChunk1Size = 16;
    header->fmt.audioFormat = 1; //PCM
    header->fmt.numOfChan = numOfChan;
    header->fmt.samplingRate = samplingRate;
    header->fmt.bytesPerSec = 2*samplingRate;
    header->fmt.blockAlign = 2;
    header->fmt.bitsPerSample = bitsPerSample;
    strcpy(header->data.subChunk2ID, h_names.data_name);
    header->data.subChunk2Size = datasize;
    return 0;
}

void WavFile::setHeader(WAV_header *header){
    unsigned long size = sizeof(RIFF_h)+sizeof(fmt_h)+sizeof(data_h);
    this->header = (WAV_header *) malloc(size);
    memcpy(this->header, header, size);
}

int WavFile::writeWavFile(std::string path, vector<short> &data){
    ofstream output(path.c_str(), ios::binary);
    if(!header){
        cerr << "Need a header to write the Wave file" << endl;
        return -1;
    }
    if(output.is_open()){
        output.write((char *) header, sizeof(header->riff)+sizeof(header->data) +
                     sizeof(header->fmt));
        output.write((char *) &data[0], (header->fmt.bitsPerSample/8)*data.size());
    }else{
        cerr << "Couldn't open the output file" << endl;
        return ERR_FILE_NOT_FOUND;
    }
    return 0;
}
int WavFile::writeWavFile(std::string path, std::vector<short> &data, unsigned int samplingRate, unsigned short bitsPerSample, unsigned short numOfChan){
    this->setHeader(samplingRate, bitsPerSample, cached_data.size());
    return this->writeWavFile(path, data);
}

WAV_header WavFile::getHeader() const{
    WAV_header to_return;
    memcpy(&to_return, header, sizeof(header->fmt) +
           sizeof(header->data) + sizeof(header->riff));
    return to_return;
}


