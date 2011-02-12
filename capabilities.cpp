#include "capabilities.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <algorithm>
#include "localization.h"

namespace capabilities
{
using namespace localization;
vector<Format> formats;
vector<VideoCodec> video_codecs;
vector<FFMpegUnit> audio_codecs;

bool VideoCodec::hasCompressionPreset()
{
    return id == "libx264";
}

bool compareFormat(FFMpegUnit f1,FFMpegUnit f2)
{
    return f1.sort_number<f2.sort_number;
}

bool equalsFormat(Format f1,int compare)
{
    return f1.sort_number==compare;
}

Format::Format(String id,String description,String display_id,int sort_number,String header,bool allowVideo,bool allowAudio):FFMpegUnit(id,description,display_id,sort_number)
{
    this->header = header;
    this->allowVideo = allowVideo;
    this->allowAudio = allowAudio;
}

vector<VideoCodec*> Format::getCodecs()
{
    vector<VideoCodec*> res;
    for(vector<VideoCodec>::iterator it = video_codecs.begin();it!=video_codecs.end();it++)
    {
        res.push_back(&*it);
    }
    return res;
}

VideoCodec::VideoCodec(String id,String description,String display_id,int sort_number,bool allowTwoPass):FFMpegUnit(id,description,display_id,sort_number)
{
    this->allowTwoPass = allowTwoPass;
}

unsigned int ResolutionPreset::toInt32()
{
    unsigned int res = 0;
    res |= width<<16;
    res |= height;
    return res;
}
void ResolutionPreset::fromInt32(unsigned int id)
{
    height = id&65535;
    width = id>>16;
}
vector<ResolutionPreset> VideoCodec::getResolutions(Format &format)
{
    vector<ResolutionPreset> res;
    ResolutionPreset r1;
    r1.width = 1920;r1.height = 1080;r1.tag = String("Full HD"); res.push_back(r1);
    r1.width = 1440;r1.height = 1080;r1.tag = String("Full HD");res.push_back(r1);
    r1.width = 1280;r1.height = 720;r1.tag = String("HD");res.push_back(r1);
    r1.width = 800;r1.height = 600;r1.tag = String("SVGA");res.push_back(r1);
    r1.width = 720;r1.height = 540;r1.tag = String::empty;res.push_back(r1);
    r1.width = 720;r1.height = 480;r1.tag = String::empty;res.push_back(r1);
    r1.width = 640;r1.height = 480;r1.tag = String("VGA");res.push_back(r1);
    r1.width = 352;r1.height = 288;r1.tag = String::empty;res.push_back(r1);
    r1.width = 352;r1.height = 240;r1.tag = String::empty;res.push_back(r1);
    r1.width = 320;r1.height = 200;r1.tag = String::empty;res.push_back(r1);
    r1.width = 240;r1.height = 180;r1.tag = String::empty;res.push_back(r1);
    r1.width = 128;r1.height = 96;r1.tag = String::empty;res.push_back(r1);
    return res;
}

FFMpegUnit::FFMpegUnit(String id,String description,String display_id,int sort_number)
{
    this->id = id;
    this->description = description;
    this->display_id = display_id;
    this->sort_number = sort_number;
}
void InitFormats()
{
    if(formats.size()>0)
        return;
    /* get all formats */
    AVOutputFormat *ofmt=NULL;
    while((ofmt = av_oformat_next(ofmt)))
    {
        String id_string = ofmt->name;
        int sort_number = -1;
        String show_id = id_string;
        String header = "";
        bool allowVideo = true;
        bool allowAudio = true;
        if(id_string == "avi")
        {
            header = MENU_HEADER_PC;
            sort_number=10;
        }
        else if(id_string == "asf")
        {
            sort_number=20;
            show_id = "wmv";
        }
        else if(id_string == "matroska" && formats.end() == find_if(formats.begin(),formats.end(),bind2nd(pointer_to_binary_function <Format,int,bool> (equalsFormat),30)))
        {
            sort_number=30;
            show_id = "mkv";
        }
        else if(id_string == "mpeg")
        {
            sort_number=40;
        }
        else if(id_string == "mp4")
        {
            sort_number=50;
        }
        else if(id_string == "ogg")
        {
            sort_number=60;
        }

        else if(id_string == "flv")
        {
            header = MENU_HEADER_WEB;
            sort_number=70;
        }
        else if(id_string == "rm")
        {
            sort_number=90;
        }
        else if(id_string == "swf")
        {
            sort_number=100;
        }

        else if(id_string == "3gp")
        {
            header = MENU_HEADER_CELL_PHONE;
            sort_number=110;
        }

        else if(id_string == "mp3")
        {
            allowVideo = false;
            header = MENU_HEADER_AUDIO;
            sort_number=120;
        }

        else if(id_string == "wav")
        {
            allowVideo = false;
            sort_number=130;
        }

        else if(id_string == "aiff")
        {
            allowVideo = false;
            sort_number=140;
        }

        else if(id_string == "ipod")
        {
            header = MENU_HEADER_IPOD;
            sort_number=150;
        }



        if(sort_number>0)
        {
            formats.push_back(Format(id_string,ofmt->long_name,show_id,sort_number,header,allowVideo,allowAudio));
        }

    }
    sort(formats.begin(),formats.end(),compareFormat);
    /* ~ get all formats */
    /* get all codecs */

    AVCodec *p=NULL;
    while((p = av_codec_next(p)))
    {
        if(p->encode)
        {
            switch(p->type)
            {
            case AVMEDIA_TYPE_VIDEO:
            {
                int sort_number=-1;
                String id_string = p->name;
                String show_id = id_string;
                bool allowTwoPass = true;

                if(id_string == "libx264")
                {
                    show_id = "h264";
                    sort_number=10;
                }
                else if(id_string == "libxvid")
                {
                    show_id = "xvid";
                    sort_number=10;
                }
                else if(id_string == "mpeg4")
                {
                    sort_number=30;
                }
                else if(id_string == "mpeg2video")
                {
                    show_id = "mpeg2";
                    sort_number=40;
                }
                else if(id_string == "wmv2")
                {
                    allowTwoPass = false;
                    sort_number=50;
                }
                else if(id_string == "flv")
                {
                    show_id = "flv";
                    sort_number=60;
                }
                else if(id_string == "libvpx")
                {
                    allowTwoPass = false;
                    show_id = "vp8";
                    sort_number=80;
                }
                else if(id_string == "libtheora")
                {
                    allowTwoPass = false;
                    show_id = "theora";
                    sort_number=90;
                }
                else if(id_string == "rv10")
                {
                    sort_number=100;
                }
                else if(id_string == "rv20")
                {
                    sort_number=110;
                }
                else if(id_string == "h263p")
                {
                    sort_number=120;
                }
                if(sort_number>0)
                video_codecs.push_back(VideoCodec(id_string,p->long_name,show_id,sort_number,allowTwoPass));
            }
            break;
            case AVMEDIA_TYPE_AUDIO:
            {
                String id_string = p->name;
                audio_codecs.push_back(FFMpegUnit(id_string,p->long_name,id_string,1));
            }
            break;
            }
        }
    }
    sort(video_codecs.begin(),video_codecs.end(),compareFormat);
    sort(audio_codecs.begin(),audio_codecs.end(),compareFormat);
    /* get all codecs */

}


}

