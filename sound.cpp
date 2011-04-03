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
    sound_buff_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
    sound_buff = new short[sound_buff_size];
    loaded = true;
    return loaded;
}

AVPacket* Sound::ReadFrame()
{
    AVPacket* packet = new AVPacket();



    bool ok = false;
    while(true)
    {
        if(av_read_frame( pFormatCtx, packet ) < 0)
        {
            ok = false;
            break;
        }
        if(packet->stream_index == videoStream)
        {
            ok = true;
            break;
        }
    }

    if(ok)
    {

        while (packet->size>0)
        {



                if(packet && sound_buff_size < FFMAX(packet->size*sizeof(*sound_buff), AVCODEC_MAX_AUDIO_FRAME_SIZE))
                {
                    sound_buff_size = FFMAX(packet->size*sizeof(*sound_buff), AVCODEC_MAX_AUDIO_FRAME_SIZE);
                    delete []sound_buff;
                    sound_buff = new short[sound_buff_size];
                }
                int decoded_data_size= sound_buff_size;
                int ret = avcodec_decode_audio3 ( pCodecCtx, sound_buff, &decoded_data_size, packet);

                if(ret < 0)
                {
                    av_free_packet(packet);
                    delete packet;
                    return 0;
                }
                packet->data += ret;
                packet->size -= ret;
                if (decoded_data_size <= 0)
                {
                    continue;
                }

                for(int i = 0;i<ret;++i)
                {
                    printf("%d ",sound_buff[i]);
                }


        }
        current = ToSeconds(packet->dts - pStream->start_time);
        return packet;


    }
    av_free_packet(packet);
    delete packet;
    return 0;
}
bool Sound::IsKeyFrame()
{
    return true;
}
void Sound::DecodeFrame()
{

}

void Sound::Dispose()
{
    const ScopedLock myScopedLock (avcodec_critical);
    if(loaded)
    {

        delete []pDataBuffer;

        avcodec_close(pCodecCtx);

        av_close_input_stream(pFormatCtx);

        delete fs;

        delete ByteIOCtx;

        delete []sound_buff;
        sound_buff_size = 0;
    }


    loaded = false;
}

Sound::Sound()
{
    sound_buff_size = 0;
    loaded = false;
}
Sound::~Sound()
{
    Dispose();
}