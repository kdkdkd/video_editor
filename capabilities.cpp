#include "capabilities.h"
extern "C" {
#define __STDC_CONSTANT_MACROS
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

bool compareFormatPointer(FFMpegUnit* f1,FFMpegUnit* f2)
{
    return f1->sort_number<f2->sort_number;
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

    if(id == "avi" || id == "asf")
    {
        for(vector<VideoCodec>::iterator it = video_codecs.begin(); it!=video_codecs.end(); it++)
        {
            VideoCodec *vc = &*it;
            vc->sort_number = -1;
            if(it->id == "libxvid")
            {
                vc->sort_number = 10;
            }
            else if(it->id == "wmv2")
            {
                vc->sort_number = 20;
            }
            else if(it->id == "mpeg4")
            {
                vc->sort_number = 30;
            }
            else if(it->id == "mpeg2video")
            {
                vc->sort_number = 40;
            }
            else if(it->id == "h263p")
            {
                vc->sort_number = 50;
            }
            else if(it->id == "mjpeg")
            {
                vc->sort_number = 60;
            }
            if(vc->sort_number>0)
                res.push_back(vc);
        }
    }
    else if(id == "matroska")
    {
        for(vector<VideoCodec>::iterator it = video_codecs.begin(); it!=video_codecs.end(); it++)
        {
            VideoCodec *vc = &*it;
            vc->sort_number = -1;
            if(it->id == "libx264")
            {
                vc->sort_number=10;
            }
            else if(it->id == "libxvid")
            {
                vc->sort_number=10;
            }
            else if(it->id == "mpeg4")
            {
                vc->sort_number=30;
            }
            else if(it->id == "mpeg2video")
            {
                vc->sort_number=40;
            }
            else if(it->id == "wmv2")
            {
                vc->sort_number=50;
            }
            else if(it->id == "flv")
            {
                vc->sort_number=60;
            }
            else if(it->id == "libvpx")
            {
                vc->sort_number=80;
            }
            else if(it->id == "libtheora")
            {
                vc->sort_number=90;
            }
            else if(it->id == "h263p")
            {
                vc->sort_number=120;
            }
            else if(it->id == "mjpeg")
            {
                vc->sort_number=130;
            }
            if(vc->sort_number>0)
                res.push_back(vc);
        }
    }
    else if(id == "mpeg")
    {
        for(vector<VideoCodec>::iterator it = video_codecs.begin(); it!=video_codecs.end(); it++)
        {
            if(it->id == "mpeg2video")
                res.push_back(&*it);
        }
    }
    else if(id == "mp4")
    {
        for(vector<VideoCodec>::iterator it = video_codecs.begin(); it!=video_codecs.end(); it++)
        {
            VideoCodec *vc = &*it;
            vc->sort_number = -1;
            if(it->id == "libx264")
            {
                vc->sort_number = 10;
            }
            else if(it->id == "mpeg4")
            {
                vc->sort_number = 30;
            }
            else if(it->id == "mpeg2video")
            {
                vc->sort_number = 40;
            }
            else if(it->id == "mjpeg")
            {
                vc->sort_number = 50;
            }
            if(vc->sort_number>0)
                res.push_back(vc);
        }
    }
    else if(id == "ogg")
    {
        for(vector<VideoCodec>::iterator it = video_codecs.begin(); it!=video_codecs.end(); it++)
        {
            if(it->id == "libtheora")
                res.push_back(&*it);
        }
    }
    else if(id == "flv")
    {
        for(vector<VideoCodec>::iterator it = video_codecs.begin(); it!=video_codecs.end(); it++)
        {
            VideoCodec *vc = &*it;
            vc->sort_number = -1;
            if(it->id == "libx264")
            {
                vc->sort_number = 10;
            }
            else if(it->id == "flv")
            {
                vc->sort_number = 30;
            }
            if(vc->sort_number>0)
                res.push_back(vc);
        }
    }
    else if(id == "rm")
    {
        for(vector<VideoCodec>::iterator it = video_codecs.begin(); it!=video_codecs.end(); it++)
        {
            VideoCodec *vc = &*it;
            vc->sort_number = -1;
            if(it->id == "rv20")
            {
                vc->sort_number = 10;
            }
            else if(it->id == "rv10")
            {
                vc->sort_number = 30;
            }
            if(vc->sort_number>0)
                res.push_back(vc);
        }
    }
    else if(id == "3gp" || id == "ipod")
    {
        for(vector<VideoCodec>::iterator it = video_codecs.begin(); it!=video_codecs.end(); it++)
        {
            VideoCodec *vc = &*it;
            vc->sort_number = -1;
            if(it->id == "libx264")
            {
                vc->sort_number = 10;
            }
            else if(it->id == "mpeg4")
            {
                vc->sort_number = 30;
            }
            if(vc->sort_number>0)
                res.push_back(vc);
        }
    }
    sort(res.begin(),res.end(),compareFormatPointer);
    return res;
}

VideoCodec::VideoCodec(String id,String description,String display_id,int sort_number,bool allowTwoPass,int qmin,int qmax):FFMpegUnit(id,description,display_id,sort_number)
{
    this->qmin=qmin;
    this->qmax=qmax;
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
bool VideoCodec::canCQ()
{
    return id!="libtheora";
}
vector<ResolutionPreset> VideoCodec::getResolutions(Format &format)
{
    vector<ResolutionPreset> res;
    ResolutionPreset r1;
    r1.width = 1920;
    r1.height = 1080;
    r1.tag = String("Full HD");
    res.push_back(r1);
    r1.width = 1440;
    r1.height = 1080;
    r1.tag = String("Full HD");
    res.push_back(r1);
    r1.width = 1280;
    r1.height = 720;
    r1.tag = String("HD");
    res.push_back(r1);
    r1.width = 800;
    r1.height = 600;
    r1.tag = String("SVGA");
    res.push_back(r1);
    r1.width = 720;
    r1.height = 540;
    r1.tag = String::empty;
    res.push_back(r1);
    r1.width = 720;
    r1.height = 480;
    r1.tag = String::empty;
    res.push_back(r1);
    r1.width = 640;
    r1.height = 480;
    r1.tag = String("VGA");
    res.push_back(r1);
    r1.width = 352;
    r1.height = 288;
    r1.tag = String::empty;
    res.push_back(r1);
    r1.width = 352;
    r1.height = 240;
    r1.tag = String::empty;
    res.push_back(r1);
    r1.width = 320;
    r1.height = 200;
    r1.tag = String::empty;
    res.push_back(r1);
    r1.width = 240;
    r1.height = 180;
    r1.tag = String::empty;
    res.push_back(r1);
    r1.width = 128;
    r1.height = 96;
    r1.tag = String::empty;
    res.push_back(r1);
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
                int qmin = 1;
                int qmax = 31;
                bool allowTwoPass = true;

                if(id_string == "libx264")
                {
                    show_id = "h264";
                    sort_number=10;
                    qmax = 51;
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
                    qmax = 63;
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
                else if(id_string == "mjpeg")
                {
                    sort_number=130;
                }
                if(sort_number>0)
                    video_codecs.push_back(VideoCodec(id_string,p->long_name,show_id,sort_number,allowTwoPass,qmin,qmax));
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


