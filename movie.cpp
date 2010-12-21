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

    fas_seek_to_frame(context,frame);
    ReadFrame();
}

void Movie::ReadFrame()
{
    delete image;
    fas_get_frame (context, &image_buffer);
    image = new Image(Image::RGB,image_buffer.width,image_buffer.height,true);
    Image::BitmapData *bd = new Image::BitmapData(*image,0,0,image_buffer.width,image_buffer.height,true);
    memcpy(bd->data,image_buffer.data,image_buffer.width*image_buffer.height*3);
    fas_free_frame (image_buffer);
    delete bd;

}
