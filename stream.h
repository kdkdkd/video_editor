#ifndef STREAM_H
#define	STREAM_H
#include "juce/juce.h"
#include "config.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

class Stream
{
public:
    int videoStream;
    double ratio_to_internal;
    double ratio_to_seconds;

    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVStream        *pStream;
    AVFormatContext *pFormatCtx;

    double duration;
    double current;
    double fps;

    bool SeekToInternal(int frame);
    int  FindKeyFrame(double back, double dest, bool accurate);
    bool GotoRatioAndRead(double ratio,bool decode = true, bool accurate = true);
    bool GotoSecondAndRead(double dest,bool decode = true, bool accurate = true);
    bool ReadAndDecodeFrame();
    bool SkipFrame();
    bool GoBack(int frames);
    int  ToInternalTime(double seconds);
    double ToSeconds(int internals);



    virtual AVPacket* ReadFrame() = 0;
    virtual bool IsKeyFrame() = 0;
    virtual void DecodeFrame() = 0;

};
#endif
