#include "sound.h"



bool Sound::Load(String &filename)
{
    File f(filename);
    if(!f.existsAsFile())
        return false;

    pDataBuffer = new unsigned char[lSize];

    probeData = new AVProbeData();
    probeData->buf = pDataBuffer;
    probeData->buf_size = lSize;
    probeData->filename = "";

    this->filename = filename;
    fs = f.createInputStream();
    fs->read(pDataBuffer,lSize);
    fs->setPosition(0);
    file_size = (double)fs->getFile().getSize();

    AVInputFormat* pAVInputFormat = av_probe_input_format(probeData,1);
    if(!pAVInputFormat)
    {
        return false;
    }
    delete probeData;

    ByteIOCtx = new ByteIOContext();
    if(init_put_byte(ByteIOCtx, pDataBuffer, lSize, 0, fs, _ReadPacket, NULL, _Seek) < 0)
    {
        return false;
    }

    if(av_open_input_stream(&pFormatCtx, ByteIOCtx, "", pAVInputFormat, NULL) < 0)
    {
        return false;
    }

    if(av_find_stream_info(pFormatCtx)<0)
        return false;

    //dump_format(pFormatCtx, 0, filename.toCString(), false);

    // Find the first video stream
    videoStream=-1;
    for(unsigned int i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_AUDIO)
        {
            videoStream=i;
            break;
        }
    if(videoStream==-1)
        return false; // Didn't find a video stream

    pStream = pFormatCtx->streams[videoStream];

    ratio_to_internal = (double)(pStream->time_base.den)/ (double)(pStream->time_base.num);
    ratio_to_seconds = (double)(pStream->time_base.num)/ (double)(pStream->time_base.den);

    // Get a pointer to the codec context for the video stream
    pCodecCtx=pStream->codec;

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL)
        return false; // Codec not found

    // Inform the codec that we can handle truncated bitstreams -- i.e.,
    // bitstreams where frame boundaries can fall in the middle of packets
    /*if(pCodec->capabilities & CODEC_CAP_TRUNCATED)
        pCodecCtx->flags|=CODEC_FLAG_TRUNCATED;*/

    // Open codec
    {
        const ScopedLock myScopedLock (avcodec_critical);
        if(avcodec_open(pCodecCtx, pCodec)<0)
            return false; // Could not open codec
    }

    duration = (double)pFormatCtx->duration / (double)AV_TIME_BASE;


    if(duration<=0.0)
    {
        int duration_num = pStream->duration * pStream->time_base.num;
        int duration_denum = pStream->time_base.den;
        duration = (double)duration_num / (double)duration_denum;
    }


    loaded = true;
    return loaded;
}

AVPacket* Sound::ReadFrame()
{
  return new AVPacket();
}
bool Sound::IsKeyFrame()
{
    return true;
}
void Sound::DecodeFrame()
{

}
