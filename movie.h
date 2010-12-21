#ifndef MOVIE_H
#define	MOVIE_H

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>

}
#include "ffmpeg_fas.h"
#include <juce.h>

class Movie
{
private:
    AVProbeData *probeData;
    static const long lSize = 32768;
    unsigned char* pDataBuffer;
    ByteIOContext* ByteIOCtx;
    fas_context_ref_type context;
    fas_raw_image_type image_buffer;
    FileInputStream *fs;
public:
    AVFormatContext *pFormatCtx;
    bool loaded;
    Image *image;
    float fps;
    int total_frames;

    double duration;
    double file_size;
    double FrameToDuration(int frame);
    Movie();
    int Open(String &filename);
    double Prepare();
    int GetInfo();
    void Dispose();
    ~Movie();
    void ReadFrame();
    void GotoFrameAndRead(int frame);
};


#endif	/* MOVIE_H */

