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

    bool SeekToInternal(int frame);
    int  FindKeyFrame(double back, double dest, bool accurate);
    bool GotoRatioAndRead(double ratio,bool decode, bool accurate);
    bool GotoSecondAndRead(double dest,bool decode, bool accurate);
    bool ReadAndDecodeFrame();
    bool SkipFrame();
    bool GoBack(int frames);
};
#endif
