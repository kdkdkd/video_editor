#ifndef MOVIE_H
#define	MOVIE_H

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>

}

#include <juce.h>

class Movie
{
private:
    AVProbeData *probeData;
    static const long lSize = 32768;
    unsigned char* pDataBuffer;
    ByteIOContext* ByteIOCtx;
    FileInputStream *fs;

    uint8_t         *buffer;
    SwsContext *img_convert_ctx;
    AVFrame         *pFrame;
    AVFrame         *pFrameRGB;

    int videoStream;

    int FindKeyFrame(double back, double dest);
    double ratio_to_internal;
    double ratio_to_seconds;

public:
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVStream        *pStream;

    AVFormatContext *pFormatCtx;
    bool loaded;
    Image *image;

    double duration;
    double current;


    double file_size;

    Movie();

    int ToInternalTime(double seconds);
    double ToSeconds(int internals);

    bool SeekToInternal(int frame);

    bool Load(String &filename);
    void Dispose();
    ~Movie();
    AVPacket* ReadFrame();
    void DecodeFrame();
    void ReadAndDecodeFrame();
    bool GotoAndRead(double ratio);
};


#endif	/* MOVIE_H */

