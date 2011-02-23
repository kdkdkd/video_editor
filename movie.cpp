#include "movie.h"
#include "localization.h"
#include "toolbox.h"
using namespace localization;
Movie::Movie()
{
    loaded = false;
    image = new Image();
    bitmapData = 0;
    image_preview=new Image();
    info = 0;
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
    File f(filename);
    if(!f.exists())
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
    /*if(pCodec->capabilities & CODEC_CAP_TRUNCATED)
        pCodecCtx->flags|=CODEC_FLAG_TRUNCATED;*/

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
    fps = ((double)fps_num / (double)fps_denum);

    if(fps<=0.0||fps>=1000.0)
    {
        fps = ((double)pCodecCtx->time_base.den / (double)pCodecCtx->time_base.num);
    }

    int duration_num = pStream->duration * pStream->time_base.num;
    int duration_denum = pStream->time_base.den;

    duration = (double)duration_num/ (double)duration_denum;

    if(duration<=0.0)
    {
        duration = (double)pFormatCtx->duration / (double)AV_TIME_BASE;
    }



    width = pCodecCtx->width;
    height = pCodecCtx->height;

    //Generate preview
    GotoRatioAndRead(.1,true,false);

    image_preview = GeneratePreview();

    GotoSecondAndRead(.0);
    //~Generate preview

    loaded = true;
    return loaded;

}

Image * Movie::GeneratePreview()
{
    Image * res = new Image();
    int preview_width = 128;
    int preview_height = 96;
    *res = image->rescaled(preview_width,preview_height);
    return res;
}

int Movie::ToInternalTime(double seconds)
{
    return (int)(seconds * ratio_to_internal);
}

double Movie::ToSeconds(int internals)
{
    return ((double)(internals)) * ratio_to_seconds;
}


void Movie::Dispose()
{
    if(loaded)
    {

        delete [] buffer;
        av_free(pFrameRGB);

        delete []pDataBuffer;


        // Free the YUV frame
        av_free(pFrame);

        // Close the codec
        avcodec_close(pCodecCtx);

        // Close the video file
        av_close_input_stream(pFormatCtx);

        sws_freeContext(img_convert_ctx);

        delete fs;

        delete ByteIOCtx;
        if(info)
            delete info;
    }


    if(image)
        delete image;

    //delete image_preview;
    if(bitmapData)
        delete bitmapData;

    loaded = false;
}

Movie::~Movie()
{
    Dispose();
}

bool Movie::SeekToInternal(int frame)
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

int Movie::FindKeyFrame(double back, double dest, bool accurate)
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
            if(pFrame->key_frame || timestamp_new==0)
            {
                keyframe = timestamp_new;
            }

        }
        else
            eof = true;
    }


    return keyframe;

}

bool Movie::GotoRatioAndRead(double ratio,bool decode, bool accurate)
{
    return GotoSecondAndRead(ratio * duration,decode,accurate);
}


bool Movie::GotoSecondAndRead(double dest,bool decode, bool accurate)
{
    if(dest == current)return true;
    if(dest>duration)
        dest = duration;
    if(dest<0.0d)
        dest = 0.0d;

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
                    return packet;
                }

            }

        }
        else break;
    }
    av_free_packet(packet);
    delete packet;
    return 0;
}

bool Movie::ReadAndDecodeFrame()
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
bool Movie::SkipFrame()
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

bool Movie::GoBack(int frames)
{
    double frame = 1.0d / fps;
    double eps = frame/5.0d;
    double desired = current - ((double)frames) * frame;
    double guess = desired - 3.0d*frame;
    if(guess<0.0)
        guess = 0.0;
    GotoSecondAndRead(guess,false);
    while(desired - current > eps )
    {
        SkipFrame();
    }
    return true;
}

void Movie::DecodeFrame()
{
    if(image->getHeight() != pCodecCtx->height || image->getWidth() != pCodecCtx->width)
    {
        delete image;
        image = new Image(Image::RGB,pCodecCtx->width,pCodecCtx->height,true);
        delete bitmapData;
        bitmapData = new Image::BitmapData(*image,0,0,pCodecCtx->width,pCodecCtx->height,true);
    }



    sws_scale (img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height,&bitmapData->data,pFrameRGB->linesize);


}

Movie::Info* Movie::GetMovieInfo()
{
    if(!loaded)return 0;
    if(info)return info;

    Movie::Info * res = new Movie::Info;
    File f(filename);
    res->filename = f.getFileName();
    res->duration = duration;
    res->size = fs->getFile().getSize();
    res->bit_rate = pFormatCtx->bit_rate / 1000;
    res->format_long = pFormatCtx->iformat->long_name;
    res->format_short = pFormatCtx->iformat->name;

    for(unsigned int i=0; i<pFormatCtx->nb_streams; i++)
    {
        AVStream * stream = pFormatCtx->streams[i];
        if(stream->codec->codec_type==CODEC_TYPE_VIDEO)
        {
            VideoInfo video_info;
            AVCodec* codec = avcodec_find_decoder(stream->codec->codec_id);
            video_info.codec_long = codec->long_name;
            video_info.codec_short = codec->name;

            char * buf = new char[12];
            av_get_codec_tag_string(buf,12,stream->codec->codec_tag);
            video_info.codec_tag = String(buf);
            if(video_info.codec_tag.indexOf("[")>=0)
                video_info.codec_tag = String::empty;
            delete []buf;
            video_info.width = stream->codec->width;
            video_info.height = stream->codec->height;
            video_info.bit_rate = stream->codec->bit_rate/1000;
            video_info.fps = ((double)stream->r_frame_rate.num / (double)stream->r_frame_rate.den);
            video_info.pix_fmt = stream->codec->pix_fmt;
            AVMetadataTag *lang = av_metadata_get(stream->metadata, "language", NULL, 0);

            if(lang)
                video_info.language = String::fromUTF8(lang->value);
            else
                video_info.language = String::empty;
            AVMetadataTag *title = av_metadata_get(stream->metadata, "title", NULL, 0);
            if(title)
                video_info.title = String::fromUTF8(title->value);
            else
                video_info.language = String::empty;
            res->videos.push_back(video_info);
        }else if(stream->codec->codec_type==CODEC_TYPE_AUDIO)
        {
            AudioInfo audio_info;
            AVCodec* codec = avcodec_find_decoder(stream->codec->codec_id);

            audio_info.codec_long = codec->long_name;
            audio_info.codec_short = codec->name;

            char * buf = new char[12];
            av_get_codec_tag_string(buf,12,stream->codec->codec_tag);
            audio_info.codec_tag = String(buf);
            if(audio_info.codec_tag.indexOf("[")>=0)
                audio_info.codec_tag = String::empty;
            delete []buf;

            audio_info.bit_rate = -1;
            audio_info.sample_rate = stream->codec->sample_rate;
            audio_info.channels = stream->codec->channels;

            AVMetadataTag *lang = av_metadata_get(stream->metadata, "language", NULL, 0);
            if(lang)
                audio_info.language = String::fromUTF8(lang->value);
            else
                audio_info.language = String::empty;
            AVMetadataTag *title = av_metadata_get(stream->metadata, "title", NULL, 0);
            if(title)
                audio_info.title = String::fromUTF8(title->value);
            else
                audio_info.language = String::empty;

            res->audios.push_back(audio_info);
        }else if(stream->codec->codec_type==CODEC_TYPE_SUBTITLE)
        {
            SubInfo sub_info;
            AVMetadataTag *lang = av_metadata_get(stream->metadata, "language", NULL, 0);
            if(lang)
                sub_info.language = String::fromUTF8(lang->value);
            else
                sub_info.language = String::empty;
            AVMetadataTag *title = av_metadata_get(stream->metadata, "title", NULL, 0);
            if(title)
                sub_info.title = String::fromUTF8(title->value);
            else
                sub_info.language = String::empty;
            res->subs.push_back(sub_info);
        }
    }
    return res;
}

String Movie::PrintMovieInfo()
{
    Info * inf = GetMovieInfo();

    String text;

    text<<"["<<LABEL_FILE<<"] "<<inf->filename<<"\n";
    text<<"["<<LABEL_DURATION<<"] "<<toolbox::format_duration(inf->duration)<<"\n";
    text<<"["<<LABEL_SIZE<<"] "<<File::descriptionOfSizeInBytes(inf->size)<<"\n";
    String bit_rate;
    int bit_rate_int = inf->bit_rate;
    if(bit_rate_int)
    {
        bit_rate = String(bit_rate_int) + " " +  LABEL_KB_PER_SECOND;
    }
    else
    {
        bit_rate = LABEL_NOT_AVIABLE;
    }
    text<<"["<<LABEL_BITRATE<<"] "<<bit_rate<<"\n";
    text<<"["<<LABEL_FORMAT<<"] "<<inf->format_short<<","<<inf->format_long<<"\n\n";

    int display_index = 1;
    for(vector<VideoInfo>::iterator it = inf->videos.begin();it!=inf->videos.end();it++)
    {
        text<<LABEL_STREAM<<" #"<<display_index++<<" ("<<LABEL_VIDEO<<")"<<"\n";
        text<<"   ["<<LABEL_CODEC<<"] "<<it->codec_short;
        if(it->codec_tag!=String::empty)
            text<<" ("<<it->codec_tag<<") ";
        text<<","<<it->codec_long<<"\n";

        text<<"   ["<<LABEL_RESOLUTION<<"] "<<it->width<<"x"<<it->height<<"\n";
        text<<"   ["<<LABEL_FPS<<"] "<<it->fps<<"\n";
        if(it->bit_rate)
            text<<"   ["<<LABEL_BITRATE<<"] "<<it->bit_rate<<" "<<LABEL_KB_PER_SECOND<<"\n";

        if(it->language!=String::empty)
            text<<"   ["<<LABEL_LANG<<"] "<<it->language<<"\n";

        if(it->title!=String::empty)
            text<<"   ["<<LABEL_COMMENT<<"] "<<it->title<<"\n";

        text<<"\n";
    }

    for(vector<AudioInfo>::iterator it = inf->audios.begin();it!=inf->audios.end();it++)
    {

        text<<LABEL_STREAM<<" #"<<display_index++<<" ("<<LABEL_AUDIO<<")"<<"\n";

        text<<"   ["<<LABEL_CODEC<<"] "<<it->codec_short;
        if(it->codec_tag!=String::empty)
            text<<" ("<<it->codec_tag<<") ";
        text<<","<<it->codec_long<<"\n";

        text<<"   ["<<LABEL_SAMPLE_RATE<<"] "<<it->sample_rate<<" Hz"<<"\n";
        text<<"   ["<<LABEL_CHANNELS<<"] "<<it->channels<<"\n";

        if(it->language!=String::empty)
            text<<"   ["<<LABEL_LANG<<"] "<<it->language<<"\n";

        if(it->title!=String::empty)
            text<<"   ["<<LABEL_COMMENT<<"] "<<it->title<<"\n";

        text<<"\n";
    }

    for(vector<SubInfo>::iterator it = inf->subs.begin();it!=inf->subs.end();it++)
    {
        if(it->language!=String::empty)
            text<<"   ["<<LABEL_LANG<<"] "<<it->language<<"\n";

        if(it->title!=String::empty)
            text<<"   ["<<LABEL_COMMENT<<"] "<<it->title<<"\n";

        text<<"\n";
    }
    return text;

}

