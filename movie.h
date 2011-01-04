#ifndef MOVIE_H
#define	MOVIE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

}

#include "juce/juce.h"

class Movie
{
private:
    AVProbeData *probeData;
    static const long lSize = 32768;
    unsigned char* pDataBuffer;
    ByteIOContext* ByteIOCtx;


    uint8_t         *buffer;
    SwsContext *img_convert_ctx;
    AVFrame         *pFrame;
    AVFrame         *pFrameRGB;

    int videoStream;

    int FindKeyFrame(double back, double dest);
    double ratio_to_internal;
    double ratio_to_seconds;
    bool SeekToInternal(int frame);

public:
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVStream        *pStream;

    AVFormatContext *pFormatCtx;
    bool loaded;
    Image *image;
    Image::BitmapData *bitmapData;

    double duration;
    double current;
    double fps;

    int width;
    int height;

    String filename;

    FileInputStream *fs;

    double file_size;

    Movie();

    int ToInternalTime(double seconds);
    double ToSeconds(int internals);

    bool Load(String &filename);
    void Dispose();
    ~Movie();
    AVPacket* ReadFrame();
    void SkipFrame();
    void DecodeFrame();
    void ReadAndDecodeFrame();
    bool GotoRatioAndRead(double ratio,bool decode = true);
    bool GotoSecondAndRead(double dest,bool decode = true);
    bool GoBack(int frames);
    String GetMovieInfo();
};


#endif	/* MOVIE_H */

