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
    class Format:public FFMpegUnit
    {
        public:
        String header;
        Format(String id,String description,String display_id,int sort_number,String header);
    };

    extern vector<Format>formats;
    extern vector<FFMpegUnit>video_codecs;
    extern vector<FFMpegUnit>audio_codecs;
    void InitFormats();

}



#endif
