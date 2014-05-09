wave_io
=======

Read a wave file:
```
WavFile wave("file.wav");
```
Print header:
```
wave.printHeader();
```
Write a wave file:
```
std::vector<short> data;
fillDataArray(data); //some function where we put raw short samples into the buffer
WavFile wave;
wave.setHeader(48000, 16, 400000, 1); //sampling rate, bits per sample, data size in bytes, # channels
wave.writeWavFile("outputfile.wav", data);
```
Replicate a wave file:
```
WavFile in_wave("input.wav");
WavFile out_wave;
out_wave.setHeader(in_wave.getHeader());
out_wave.writeWavFile("output.wav", in_wave.getData());
```
