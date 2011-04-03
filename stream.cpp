#include "stream.h"
#include "config.h"
bool Stream::SeekToInternal(int frame)
{
    int dest = ToSeconds(frame);
    int flags = AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME;

    int res = av_seek_frame( pFormatCtx, videoStream, frame, flags);
    avcodec_flush_buffers (pCodecCtx);
    if(res>=0)
    {
        current = dest;
        return true;
    }
    return false;
}

int Stream::FindKeyFrame(double back, double dest, bool accurate)
{
    int keyframe = -1;

    int timestamp = ToInternalTime(dest);

    int back_int = back * ratio_to_internal;

    int timestamp_new = timestamp - back_int;

    bool res = SeekToInternal(timestamp_new);
    if(!res)return false;

    bool eof = false;
    while(!eof)
    {
        AVPacket* packet = ReadFrame();
        if(packet)
        {
            timestamp_new =  packet->dts - pStream->start_time;

            av_free_packet(packet);
            delete packet;
            if(timestamp_new>=timestamp)
            {
                if(!accurate)
                    return keyframe;
                break;
            }
            if(IsKeyFrame() || timestamp_new==0)
            {
                keyframe = timestamp_new;
            }

        }
        else
            eof = true;
    }


    return keyframe;

}

bool Stream::GotoRatioAndRead(double ratio,bool decode, bool accurate)
{
    return GotoSecondAndRead(ratio * duration,decode,accurate);
}


bool Stream::GotoSecondAndRead(double dest,bool decode, bool accurate)
{
    if(dest == current)return true;
    if(dest>duration)
        dest = duration;
    if(dest<0.0)
        dest = 0.0;

    if(dest ==0.)
    {
        SeekToInternal(0);
        ReadAndDecodeFrame();
        return true;
    }

    int found = -1;

    double back = 0.0;
    while(found<0 && back<100.0)
    {
        if(back>dest)
        {
            back = dest;
            found = FindKeyFrame(back,dest,accurate);
            break;
        }
        found = FindKeyFrame(back,dest,accurate);
        if(back==0.0)
            back = 0.5;
        else
            back *= 2.0;

    }

    if(found>=0 && decode)
    {
        DecodeFrame();
    }

    return (found>=0)?true:GotoSecondAndRead(0.);
}

bool Stream::ReadAndDecodeFrame()
{
    AVPacket* packet = ReadFrame();
    if(packet)
    {
        av_free_packet(packet);
        delete packet;
        DecodeFrame();
        return true;
    }
    return false;
}

bool Stream::SkipFrame()
{
    AVPacket* packet = ReadFrame();
    if(packet)
    {
        av_free_packet(packet);
        delete packet;
        return true;
    }
    return false;
}

bool Stream::GoBack(int frames)
{
    double frame = 1.0 / fps;
    double eps = frame/5.0;
    double desired = current - ((double)frames) * frame;
    double guess = desired - 3.0*frame;
    if(guess<0.0)
        guess = 0.0;
    GotoSecondAndRead(guess,false);
    while(desired - current > eps )
    {
        SkipFrame();
    }
    return true;
}


int Stream::ToInternalTime(double seconds)
{
    return (int)(seconds * ratio_to_internal);
}

double Stream::ToSeconds(int internals)
{
    return ((double)(internals)) * ratio_to_seconds;
}
