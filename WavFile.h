//
//  WavFile.h
//  wavReader
//
//  Created by Santi on 29/07/13.
//  Copyright (c) 2013 Santi. All rights reserved.
//

#ifndef wavReader_WavFile_h
#define wavReader_WavFile_h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

#define ERR_FILE_NOT_FOUND -1
#define ERR_READING_FILE -2
#define ERR_FILE_NOT_WAV -3
#define NOT_RIFF (RIFF_hdr.RIFF[0]!='R')&&(RIFF_hdr.RIFF[1]!='I')&& \
                (RIFF_hdr.RIFF[2]!='F')&&(RIFF_hdr.RIFF[3]!='F')
#define NOT_DATA (idBuffer[0] != 'd')&&(idBuffer[1] != 'a')&&(idBuffer[2] != 't')\
                &&(idBuffer[3] != 'a')

#define STANDARD_HEADER_SIZE 44
#define FMT_HEADER_SIZE 24
#define RIFF_HEADER_SIZE 12

/**
 HEADER STRUCTS
 **/
typedef struct{
    char RIFF[4];
    unsigned int fileSize;
    char WAVE[4];
}RIFF_h;

typedef struct{
    char fmt[4];
    unsigned int subChunk1Size;
    unsigned short audioFormat;
    unsigned short numOfChan;
    unsigned int samplingRate;
    unsigned int bytesPerSec;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
}fmt_h;

typedef struct{
    char subChunk2ID[4];
    unsigned int subChunk2Size;
}data_h;

typedef struct{
    RIFF_h riff;
    fmt_h fmt;
    data_h data;
}WAV_header;

/**
 END OF HEADER STRUCTS
 **/

class WavFile{

private:
    WAV_header *header;
    std::vector<short> cached_data;
    std::string filePath;

    static const char *riff_name;
    static const char *wave_name;
    static const char *fmt_name;
    static const char *data_name;
public:
    /** Build a wave object replicating a wave file. 
     Call getData() to obtain the cached signal. **/
    int readWavFile();
    /** Store the audio data into a referenced vector **/
    int readWavFile(std::vector<short> &data);
    /** Print the current hold header **/
    void printHeader();
    /** write a Wav file with 'data' **/
    int writeWavFile(std::string path, std::vector<short> &data);
    /** write a Wav file with 'data' specifying 3 header parameters: 
     fm, bits/sample, numOfChan (by default = 1) **/
    int writeWavFile(std::string path, std::vector<short> &data, unsigned int fm,
                     unsigned short bitsPerSample = 16, unsigned short numOfChan = 1);
    /** get the stored data **/
    std::vector<short> getData() const{ return cached_data; }
    /** get the stored header **/
    WAV_header getHeader() const;
    /** set a header, fm and bitsPerSample need to be specified always **/
    int setHeader(unsigned int fm, unsigned short bitsPerSample,int dataSize,
                  unsigned short numOfChan = 1);
    /** set a header copied from another header **/
    void setHeader(WAV_header *header);
    /** blank constructor **/
    WavFile(){ header = NULL; }
    /** specifying the path we're directly reading a new wave file **/
    WavFile(std::string filePath);
    ~WavFile();
    
};

#endif
