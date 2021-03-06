#ifndef CAPABILITIES_H_INCLUDED
#define CAPABILITIES_H_INCLUDED
#include "juce/juce.h"
#include <vector>

namespace capabilities
{
    using namespace std;
    class FFMpegUnit
    {
        public:
        String id;
        String description;
        String display_id;
        int sort_number;
        FFMpegUnit(String id,String description,String display_id,int sort_number);
    };
    class ResolutionPreset
    {
        public:
        int width;
        int height;
        String tag;
        unsigned int toInt32();
        void fromInt32(unsigned int id);
    };
    class VideoCodec;
    class Format:public FFMpegUnit
    {
        public:
        String header;
        bool allowVideo;
        bool allowAudio;
        Format(String id,String description,String display_id,int sort_number,String header,bool AllowVideo,bool AllowAudio);
        vector<VideoCodec*> getCodecs();
        bool hasCompressionPreset();

    };

    class VideoCodec:public FFMpegUnit
    {
        public:
        bool allowTwoPass;
        int qmin;
        int qmax;
        vector<ResolutionPreset> getResolutions(Format &format);
        VideoCodec(String id,String description,String display_id,int sort_number,bool allowTwoPass,int qmin,int qmax);
        bool hasCompressionPreset();
        bool canCQ();
    };


    extern vector<Format>formats;
    extern vector<VideoCodec>video_codecs;
    extern vector<FFMpegUnit>audio_codecs;
    void InitFormats();

}



#endif
