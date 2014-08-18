//
//  Wav_file.h
//
//  Created by Santi on 29/07/13.
//

#ifndef wavReader_WavFile_h
#define wavReader_WavFile_h

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstring>

#define ERR_FILE_NOT_FOUND -1
#define ERR_READING_FILE -2
#define ERR_FILE_NOT_RIFF -3

#define STANDARD_HEADER_SIZE 44
#define FMT_HEADER_SIZE 24
#define RIFF_HEADER_SIZE 12
#define DATA_HEADER_SIZE 8

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

/* HEADER STRUCTS
 *****************/

typedef struct{
    uint8_t RIFF[4];
    uint32_t file_size;
    uint8_t WAVE[4];
}RIFF_h;

typedef struct{
    uint8_t fmt[4];
    uint32_t sub_chunk_1_size;
    uint16_t audio_format;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t bytes_per_sec;
    uint16_t block_align;
    uint16_t bits_per_sample;
}fmt_h;

typedef struct{
    uint8_t data[4];
    uint32_t sub_chunk_2_size;
}data_h;

typedef struct{
    RIFF_h riff;
    fmt_h fmt;
    data_h data;
}WAV_header;

/* END OF HEADER STRUCTS
 ************************/

class Wav_file{
    
private:
    WAV_header header;
    std::vector<uint8_t> signal_data;
    std::string file_path;
    
    static const char *riff_name;
    static const char *wave_name;
    static const char *fmt_name;
    static const char *data_name;
public:
    /// Constructor to build a new wav file from a raw buffer.
    /// @param data array with signal, it can be interleaved to contain 8 or 16 bit samples as well as different channels.
    /// @param sample_rate in Hertz
    /// @param bits_per_sample 8 or 16
    /// @param channels
    Wav_file(std::vector<uint8_t> &data, uint32_t sample_rate, uint16_t bits_per_sample, uint16_t channels);
    
    /// Constructor to build a new wav file from a buffer.
    /// @param data array with signal, it must be an 8 bit PCM signal, though it can contain different channels interleaved
    /// @param sample_rate in Hz
    /// @param channels
    Wav_file(std::vector<uint8_t> &data, uint32_t sample_rate, uint16_t channels);
    
    /// Constructor to build a new wav file from a file
    /// @param file_path to attach the object to a file
    /// @param verbose to show the file properties once the data is readen. True by default.
    Wav_file(std::string file_path, bool verbose=true);
    
    /// Destructor
    ~Wav_file(){}
    
    /// read_wav_file reads the contents of the file attached to the WAV object
    /// @param verbose to show the file properties once the data is readen.
    int read_wav_file(bool verbose);
    
    /// print_header prints the header of this WAV object
    void print_header();
    
    /// get_data gets the raw signal data of this WAV object in an array of bytes.
    /// @return vector reference containing the audio data of the object
    std::vector<uint8_t> &get_data(){ return signal_data; }
    
    /// get_header gets a copy of the header of this WAV object
    WAV_header get_header() const{ return header; }
    
    /// set_pathname sets the pathname of this WAV object so that it gets attached to a file
    /// @param pathname this is the pathname of the file to which we attach the WAV object
    void set_pathname(const std::string &pathname){ this->file_path = pathname; }
    
    /// set_header re-builds this WAV object header with the specified parameters
    /// @param data array with signal
    /// @param sample_rate
    /// @param bits_per_sample
    /// @param channels
    void set_header(std::vector<uint8_t> &data, uint32_t sample_rate, uint16_t bits_per_sample, uint16_t channels);
    
    /// save saves the contents of this WAV object into the file it is attached to
    /// @return true if the save was successful, false otherwise.
    bool save();

};

#endif
