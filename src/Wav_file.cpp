//
//  Wav_file.cpp
//
//  Created by Santi on 30/07/13.
//

#include "Wav_file.h"

using namespace std;

const char *Wav_file::riff_name = "RIFF";
const char *Wav_file::wave_name = "WAVE";
const char *Wav_file::fmt_name = "fmt ";
const char *Wav_file::data_name = "data";

/* CONSTRUCTOR FROM BUFFER
 **************************/
Wav_file::Wav_file(vector<uint8_t> &data, uint32_t sample_rate, uint16_t bits_per_sample, uint16_t channels){
    signal_data = data;
    set_header(data, sample_rate, bits_per_sample, channels);
}

Wav_file::Wav_file(vector<uint8_t> &data, uint32_t sample_rate, uint16_t channels){
    signal_data = data;
    set_header(data, sample_rate, 8, channels);
}

/* CONSTRUCTOR FROM FILE
 ************************/
Wav_file::Wav_file(std::string file_path, bool verbose):file_path(file_path){
    //read the wave file and store the contents in the signal vector and the header struct
    read_wav_file(verbose);
}

/* READ THE ATTACHED FILE
 *************************/
int Wav_file::read_wav_file(bool verbose){
    if(file_path.empty()){
        cerr << "Error: this object is not attached to any file. Set a pathname first please."
        << endl;
        return -1;
    }
    bool data_header_found=false;
    unsigned int found_header_size = 0;
    //open up a new input stream
    ifstream wav_file(file_path.c_str(), ios::in | ios::binary);
    if(!wav_file){
        cerr << "Error: the file can't be opened" << endl;
        return ERR_READING_FILE;
    }
    //start reading the RIFF header
    wav_file.read((char *) &header.riff, RIFF_HEADER_SIZE);
    if(wav_file.gcount()!=RIFF_HEADER_SIZE || memcmp(header.riff.RIFF, "RIFF", 4)){
        cerr << "Error: not a valid RIFF file" << endl;
        return ERR_FILE_NOT_RIFF;
    }
    found_header_size += wav_file.tellg();
    wav_file.read((char *) &header.fmt, FMT_HEADER_SIZE);
    if(wav_file.gcount()!=FMT_HEADER_SIZE){
        cerr << "Error: couldn't read the fmt header" << endl;
        return ERR_READING_FILE;
    }
    if(header.fmt.audio_format!=1){
        cerr << "Error: this is not in PCM format" << endl;
        return ERR_READING_FILE;
    }
    found_header_size += wav_file.tellg();
    //look for the data header, leaving the other possible headers aside
    while(!data_header_found){
        wav_file.read((char *) &header.data, DATA_HEADER_SIZE);
        if(memcmp(header.data.data, "data", 4)){
            //we don't have a data ID
            cerr << "found a non-data header, so skipping " << header.data.sub_chunk_2_size <<
            " bytes" << endl;
            wav_file.seekg(header.data.sub_chunk_2_size,ios::cur);
        }
        else{
            data_header_found = true;
        }
        found_header_size += wav_file.tellg();
    }
    //and finally, read the signal data
    signal_data.clear();
    signal_data.resize(header.data.sub_chunk_2_size);
    wav_file.read((char*)&signal_data[0], header.data.sub_chunk_2_size);
    if(wav_file.gcount()!=header.data.sub_chunk_2_size){
        cerr << "Error: Something went wrong reading the signal data" << endl;
        return ERR_READING_FILE;
    }
    if(verbose) print_header();
    return 0;
}

/* Print the existing header
 ****************************/
void Wav_file::print_header(){
    if(memcmp(header.riff.RIFF,"RIFF",4)){
        cerr << "The header contains nothing" << endl;
        return;
    }
    cout << "=====================================" << endl;
    cout << "RIFF ID: " << string((char*)header.riff.RIFF).substr(0,4) << endl;
    cout << "File size: " << header.riff.file_size+8 << " Bytes" << endl;
    cout << "WAVE ID: " << string((char*)header.riff.WAVE).substr(0,4) << endl;
    cout << "-------------------------------------" << endl;
    cout << "FMT ID: " << string((char*)header.fmt.fmt) << endl;
    cout << "fmt header size: " << header.fmt.sub_chunk_1_size << " Bytes" << endl;
    switch(header.fmt.audio_format){
        case 1:
            cout << "Audio format: PCM" << endl;
            break;
    }
    cout << "Channels: " << header.fmt.channels << endl;
    cout << "Sample rate: " << header.fmt.sample_rate << " Hz" << endl;
    cout << "Byte rate: " << header.fmt.bytes_per_sec << " Bytes/s" << endl;
    cout << "Block align: " << header.fmt.block_align << " Bytes" << endl;
    cout << "Bits per sample: " << header.fmt.bits_per_sample << " bits"<< endl;
    cout << "-------------------------------------" << endl;
    cout << "DATA ID: " << string((char*)header.data.data).substr(0,4) << endl;
    cout << "Data size (Audio): " << header.data.sub_chunk_2_size << " bytes" << endl;
    cout << "=====================================" << endl;
}

/* Set up the 44 bytes header
 *****************************/
void Wav_file::set_header(vector<uint8_t> &data, uint32_t sample_rate, uint16_t bits_per_sample, uint16_t channels){
    //set up the RIFF header
    memcpy(header.riff.RIFF, riff_name, 4);
    header.riff.file_size = (uint32_t) data.size() + STANDARD_HEADER_SIZE - 8;
    memcpy(header.riff.WAVE, wave_name, 4);
    //set up the fmt header
    memcpy(header.fmt.fmt, fmt_name, 4);
    header.fmt.sub_chunk_1_size = FMT_HEADER_SIZE - 8;
    header.fmt.audio_format = 1; //PCM
    header.fmt.channels = channels;
    header.fmt.sample_rate = sample_rate;
    header.fmt.bytes_per_sec = sample_rate*channels*bits_per_sample/8;
    header.fmt.block_align = channels * bits_per_sample/8;
    header.fmt.bits_per_sample = bits_per_sample;
    //set up the data header
    memcpy(header.data.data, data_name, 4);
    header.data.sub_chunk_2_size = (uint32_t) data.size();
}

/* Save the WAV contents to the attached file
 *********************************************/
bool Wav_file::save(){
    if(file_path.empty()){
        cerr << "Error: this object is not attached to any file. SEt the pathname first" << endl;
        return false;   
    }
    fstream output(file_path.c_str(), ios::out|ios::binary);
    if(output.is_open()){
        output.write((char*) &header, sizeof(header));
        output.write((char*) &signal_data[0], header.data.sub_chunk_2_size);
    }
    else{
        cerr << "Error: can't open " << file_path << " to write" << endl;
        return false;
    }
    return true;
}


