#include "movie.h"
#include <list>
using namespace std;
Movie::Movie()
{
    loaded = false;
    image = new Image(Image::RGB,1,1,true);
};

int _ReadPacket(void* cookie, uint8_t* buffer, int bufferSize)
{
    FileInputStream* fs = reinterpret_cast<FileInputStream*>(cookie);
    int res = fs->read(buffer,bufferSize);
    return res;
}

int64_t _Seek(void* cookie, int64_t offset, int whence)
{
    FileInputStream* fs = reinterpret_cast<FileInputStream*>(cookie);
    int64_t real_offset = 0;
    switch(whence)
    {
    case AVSEEK_SIZE:
        return fs->getFile().getSize();
    case SEEK_SET:
        real_offset = offset;
        break;
    case SEEK_CUR:
        real_offset = offset + fs->getPosition();
        break;
    case SEEK_END:
        real_offset = fs->getFile().getSize() + offset - 1 ;
        break;

    }


    fs->setPosition(offset);
    return offset;
}

bool Movie::Load(String &filename)
{
    pDataBuffer = new unsigned char[lSize];

    probeData = new AVProbeData();
    probeData->buf = pDataBuffer;
    probeData->buf_size = lSize;
    probeData->filename = "";

    File f(filename);
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

    dump_format(pFormatCtx, 0, filename.toCString(), false);

    // Find the first video stream
    videoStream=-1;
    for(unsigned int i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
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
    if(pCodec->capabilities & CODEC_CAP_TRUNCATED)
        pCodecCtx->flags|=CODEC_FLAG_TRUNCATED;

    // Open codec
    if(avcodec_open(pCodecCtx, pCodec)<0)
        return false; // Could not open codec

    // Allocate video frame
    pFrame=avcodec_alloc_frame();

    // Allocate an AVFrame structure
    pFrameRGB=avcodec_alloc_frame();
    if(pFrameRGB==NULL)
        return false;

    // Determine required buffer size and allocate buffer
    int numBytes=avpicture_get_size(PIX_FMT_BGR24, pCodecCtx->width,
                                    pCodecCtx->height);
    buffer=new uint8_t[numBytes];

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_BGR24,
                   pCodecCtx->width, pCodecCtx->height);


    img_convert_ctx = sws_getContext(pCodecCtx->width,pCodecCtx->height,pCodecCtx->pix_fmt,pCodecCtx->width,pCodecCtx->height,PIX_FMT_BGR24,SWS_BICUBIC, NULL, NULL, NULL);

    /*int fps_num = pStream->r_frame_rate.num;
    int fps_denum = pStream->r_frame_rate.den;

    fps = ((float)fps_num / (float)fps_denum);

    if(fps<=0.0||fps>=1000.0)
    {
        fps = ((float)pCodecCtx->time_base.den / (float)pCodecCtx->time_base.num);
    }*/

    int duration_num = pStream->duration * pStream->time_base.num;
    int duration_denum = pStream->time_base.den;

    duration = (double)duration_num/ (double)duration_denum;

    if(duration<=0.0)
    {
        duration = (double)pFormatCtx->duration / (double)AV_TIME_BASE;
    }

    loaded = true;

    return loaded;

}
int Movie::ToInternalTime(double seconds)
{
    return (int)(seconds * ratio_to_internal);
}

double Movie::ToSeconds(int internals)
{
    return ((double)(internals)) * ratio_to_seconds;
}
/*double Movie::Prepare()
{
    int i = 0;
    while (fas_frame_available(context) )
    {
        fas_step_forward(context);
        i++;
        if(i>=200)
            return (double)fs->getPosition()/file_size;

    }
    return (fas_get_seek_completed(context))?-1.:-2.;
}

int Movie::GetInfo()
{
    int loaded_local = false;
    total_frames=fas_get_frame_count(context);
    loaded_local = total_frames>0;
    if(loaded_local)
    {
        double frame_duration = fas_get_frame_duration(context);
        duration = frame_duration * ((double)total_frames/ 10000000.);
        fps = 10000000./frame_duration;
        fas_seek_to_frame(context,0);
    }
    loaded = loaded_local;
    return loaded;
}*/


void Movie::Dispose()
{
    if(loaded)
    {
        /*fas_close_video(context);
        av_close_input_stream(pFormatCtx);
        delete[] pDataBuffer;
        delete fs;
        delete ByteIOCtx;
        loaded = false;*/

        delete [] buffer;
        av_free(pFrameRGB);

        // Free the YUV frame
        av_free(pFrame);

        // Close the codec
        avcodec_close(pCodecCtx);

        // Close the video file
        av_close_input_stream(pFormatCtx);

        sws_freeContext(img_convert_ctx);

        delete fs;

        delete ByteIOCtx;
    }
    delete image;

    loaded = false;
}

Movie::~Movie()
{
    Dispose();
}

bool Movie::SeekToInternal(int frame)
{
    int dest = ToSeconds(frame);
    int flags = 0;
    //if(dest<current)
        flags = AVSEEK_FLAG_BACKWARD ;

    int res = av_seek_frame( pFormatCtx, videoStream, frame, flags);
    avcodec_flush_buffers (pCodecCtx);
    if(res>=0)
    {
        current = dest;
        return true;
    }
    return false;
}

int Movie::FindKeyFrame(double back, double dest, unsigned int offset)
{
    list<int> keyframe_array;

    int timestamp = ToInternalTime(dest);

    int back_int = back * ratio_to_internal;

    int timestamp_new = timestamp - back_int;

    int timestamp_prev = 0;


    bool res = SeekToInternal(timestamp_new);
    if(!res)return false;

    bool eof = false;
    while(!eof)
    {
        AVPacket* packet = ReadFrame();
        if(packet)
        {
            timestamp_prev = timestamp_new;
            timestamp_new =  packet->dts - pStream->start_time;

            av_free_packet(packet);
            delete packet;
            if(timestamp_new>timestamp)
            {
                break;
            }
            if(pFrame->key_frame)
            {
                keyframe_array.push_front(timestamp_new);
                if(keyframe_array.size()>offset)
                {
                    keyframe_array.pop_back();
                }
                //DecodeFrame();
            }

        }
        else
            eof = true;
    }


    return (keyframe_array.size()==offset)?keyframe_array.back():0;

}

bool Movie::GotoAndRead(double ratio)
{
    double dest = ratio * duration;
    int dest_int = ToInternalTime(dest);
    /*if(dest == current)return true;
    if(dest ==0.)
    {
        av_seek_frame( pFormatCtx, videoStream, 0, AVSEEK_FLAG_BACKWARD);
        avcodec_flush_buffers (pCodecCtx);
        ReadAndDecodeFrame();
        return true;
    }

    int found = 0;
    int offset = 1;
    while(offset<10)
    {
        found = 0;
        double back = 1.0;

        while(!found && back<100.0)
        {
            if(back>dest)
            {
                back = dest;
                found = FindKeyFrame(back,dest,offset);
                break;
            }
            found = FindKeyFrame(back,dest,offset);
            back *= 2.0;

        }

        if(found)
        {
            printf("found %d;seek %d; back %f\n",found,ToInternalTime(dest),back);*/
            SeekToInternal(dest_int);
            AVPacket* packet = ReadFrame();
            if(packet)
            {

                int position = packet->dts - pStream->start_time;
                printf("want %d;got %d; key %d\n",dest_int,position,pFrame->key_frame);
                av_free_packet(packet);
                delete packet;
                //if(position == found)
                {
                    //ReadAndDecodeFrame();ReadAndDecodeFrame();ReadAndDecodeFrame();ReadAndDecodeFrame();ReadAndDecodeFrame();
                    DecodeFrame();
                    //break;
                }
                /*else
                {
                    offset++;

                }*/
            }
            //else
            //    return GotoAndRead(0.);
            //current = dest;
        /*}
    }
    return (found)?found:GotoAndRead(0.);*/
    return true;
}

AVPacket* Movie::ReadFrame()
{
    AVPacket* packet=new AVPacket();

    int frameFinished = 0;
    while ( 0 == frameFinished )
    {
        if ( av_read_frame( pFormatCtx, packet ) >= 0 )
        {

            if ( packet->stream_index == videoStream )
            {
                avcodec_decode_video2( pCodecCtx, pFrame, &frameFinished, packet);
                if ( frameFinished )
                {
                    current = ToSeconds(packet->dts - pStream->start_time);
                    return packet;a
                }

            }

        }
        else break;
    }
    av_free_packet(packet);
    delete packet;
    return 0;
}

void Movie::ReadAndDecodeFrame()
{
    AVPacket* packet = ReadFrame();
    if(packet)
    {
        printf("dts %d;pts %d\n",int(packet->dts),int(packet->pts));
        av_free_packet(packet);
        delete packet;
        DecodeFrame();

    }
}

void Movie::DecodeFrame()
{
    if(image->getHeight() != pCodecCtx->height || image->getWidth() != pCodecCtx->width)
    {
        delete image;
        image = new Image(Image::RGB,pCodecCtx->width,pCodecCtx->height,true);
    }

    Image::BitmapData *bd = new Image::BitmapData(*image,0,0,pCodecCtx->width,pCodecCtx->height,true);

    sws_scale (img_convert_ctx, pFrame->data, pFrame->linesize,0, pCodecCtx->height,&bd->data,pFrameRGB->linesize);

    delete bd;
}

