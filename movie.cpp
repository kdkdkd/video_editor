#include "movie.h"

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

int Movie::Open(String &filename)
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

    // Open video file
    //if(av_open_input_file(&pFormatCtx, filename, NULL, 0, NULL)!=0)
    //    return false; // Couldn't open file
    /*
        // Retrieve stream information
        if(av_find_stream_info(pFormatCtx)<0)
            return false; // Couldn't find stream information

        // Dump information about file onto standard error
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

        int fps_num = pStream->r_frame_rate.num;
        int fps_denum = pStream->r_frame_rate.den;

        fps = ((float)fps_num / (float)fps_denum);

        if(fps<=0.0||fps>=1000.0)
        {
            fps = ((float)pCodecCtx->time_base.den / (float)pCodecCtx->time_base.num);
        }

        int64 duration_num = pStream->duration * pStream->time_base.num;
        int duration_denum = pStream->time_base.den;

        duration = (double)duration_num/ (double)duration_denum;

        if(duration<=0.0)
        {
            duration = (double)pFormatCtx->duration / (double)AV_TIME_BASE;
        }

        total_frames = static_cast<int64>(round(duration * fps));
        if(total_frames<=0)
        {
            total_frames = pStream->nb_frames;
        }

        total_frames+=1000;
        current_frame = 0;
        loaded = true;
    */
    fas_error_type video_error;


    fas_initialize (FAS_FALSE, FAS_BGR24);

    video_error = fas_open_video (&context, pFormatCtx);

    return video_error == FAS_SUCCESS;

}

double Movie::Prepare()
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
}


void Movie::Dispose()
{
    if(loaded)
    {
        /*       delete [] buffer;
               av_free(pFrameRGB);

               // Free the YUV frame
               av_free(pFrame);

               // Close the codec
               avcodec_close(pCodecCtx);

               // Close the video file
               av_close_input_stream(pFormatCtx);

               sws_freeContext(img_convert_ctx);



               delete ByteIOCtx;


        */

        fas_close_video(context);
        av_close_input_stream(pFormatCtx);
        delete[] pDataBuffer;
        delete fs;
        delete ByteIOCtx;
        loaded = false;

    }
    delete image;
}

Movie::~Movie()
{
    Dispose();
}

double Movie::FrameToDuration(int frame)
{
    return (double)frame/(double)fps;
}

void Movie::GotoFrameAndRead(int frame)
{
    /*if(frame == current_frame) return ;

    int delta = 3500;
    if(frame<delta)
    {
       delta = frame;
    }
    frame -= delta;


    int flags = 0;
    if(frame<current_frame)
        flags = AVSEEK_FLAG_BACKWARD;

    int64 timestamp = (double)frame * (double)(pStream->time_base.num)/(double)fps * (double)(pStream->time_base.den);
    int res = av_seek_frame( pFormatCtx, videoStream, timestamp, flags);

    avcodec_flush_buffers (pCodecCtx);
    if(res>=0)
    {
        for(int i=0;i<delta-1;i++)
        {
            ReadFrame(false);
        }
        ReadFrame(true);
        current_frame = frame;
    }
    */
    //return res;
    fas_seek_to_frame(context,frame);
    ReadFrame();
}

void Movie::ReadFrame()
{
    /*AVPacket packet;
    int frameFinished = 0;
    //while ( 0 == frameFinished )
    {
        if ( av_read_frame( pFormatCtx, &packet ) >= 0 )
        {

            if ( packet.stream_index == videoStream )
            {
                avcodec_decode_video2( pCodecCtx, pFrame, &frameFinished, &packet);
                if ( frameFinished )
                {
                    if(true){
                    if(image->getHeight() != pCodecCtx->height || image->getWidth() != pCodecCtx->width)
                    {
                        delete image;
                        image = new Image(Image::RGB,pCodecCtx->width,pCodecCtx->height,true);
                    }

                    Image::BitmapData *bd = new Image::BitmapData(*image,0,0,pCodecCtx->width,pCodecCtx->height,true);

                    sws_scale (img_convert_ctx, pFrame->data, pFrame->linesize,0, pCodecCtx->height,&bd->data,pFrameRGB->linesize);

                    delete bd;
                    }

                }

            }
            av_free_packet(&packet);
        }
    }*/
    delete image;
    fas_get_frame (context, &image_buffer);
    image = new Image(Image::RGB,image_buffer.width,image_buffer.height,true);
    Image::BitmapData *bd = new Image::BitmapData(*image,0,0,image_buffer.width,image_buffer.height,true);
    memcpy(bd->data,image_buffer.data,image_buffer.width*image_buffer.height*3);
    fas_free_frame (image_buffer);
    delete bd;

}
