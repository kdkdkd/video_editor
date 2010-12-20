/*
 * File:   movie.h
 * Author: kd
 *
 * Created on 24 Октябрь 2010 г., 14:59
 */

#ifndef MOVIE_H
#define	MOVIE_H

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>

}
#include "ffmpeg_fas.h"
#include <juce.h>

class Movie
{
private:
    AVProbeData *probeData;
    static const long lSize = 32768;
    unsigned char* pDataBuffer;
    ByteIOContext* ByteIOCtx;
    /*int videoStream;
    uint8_t         *buffer;
    SwsContext *img_convert_ctx;
    AVFrame         *pFrame;
    AVFrame         *pFrameRGB;
    ByteIOContext* ByteIOCtx;
    int current_frame;*/
    //int _ReadPacket(void* cookie, uint8_t* buffer, int bufferSize);
    //int64_t _Seek(void* cookie, int64_t offset, int whence);
    fas_context_ref_type context;
    fas_raw_image_type image_buffer;
    FileInputStream *fs;
public:
    /*
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVStream        *pStream;*/
    AVFormatContext *pFormatCtx;
    bool loaded;
    Image *image;
    float fps;
    int total_frames;

    double duration;
    double file_size;
    double FrameToDuration(int frame);
    Movie();
    int Open(String &filename);
    double Prepare();
    int GetInfo();
    void Dispose();
    ~Movie();
    void ReadFrame();
    void GotoFrameAndRead(int frame);
};


#endif	/* MOVIE_H */

