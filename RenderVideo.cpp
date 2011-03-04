#include "timeline.h"
#include "movie.h"
#include "localization.h"
extern "C" {
#include <libavcodec/opt.h>
}
#define INT64_C __INT64_C
class RenderContext
{
public:
    AVFrame *picture;
    uint8_t *video_outbuf;
    int frame_count, video_outbuf_size;
    bool end_writing;
    float t, tincr, tincr2;
    int16_t *samples;
    uint8_t *audio_outbuf;
    int audio_outbuf_size;
    int audio_input_frame_size;
    int64_t pts;
    int current_pass;
    int all_pass;
    String pass_info;
    bool is_codec_x264;
    SwsContext *img_convert_ctx;
    bool error;
    String errorText;
    RenderContext()
    {
        picture = 0;
        video_outbuf = 0;
        samples = 0;
        audio_outbuf = 0;
        img_convert_ctx = 0;
    }
    ~RenderContext()
    {
        Dispose();
    }
    void Dispose()
    {
        if(img_convert_ctx != NULL )
        {
            sws_freeContext(img_convert_ctx);
            img_convert_ctx = NULL;
        }
        if(picture)
        {
            av_free(picture->data[0]);
            av_free(picture);
            picture = 0;
        }
        if(video_outbuf)
        {
            av_free(video_outbuf);
            video_outbuf = 0;
        }
        if(samples)
        {
            av_free(samples);
            samples = 0;
        }
        if(audio_outbuf)
        {
            av_free(audio_outbuf);
            audio_outbuf = 0;
        }

    }
};


static int sws_flags = SWS_GAUSS;


extern "C" {
#include <libavutil/intreadwrite.h>
}




int srcW = 0;
int srcH = 0;
int dstW = 0;
int dstH = 0;
PixelFormat srcFormat;
PixelFormat dstFormat;


static void setCompressionPreset(AVCodecContext *c, int preset,int pass_number,RenderContext *rc)
{
    if(rc->is_codec_x264)
    {
        if(pass_number == 1)
        {
            switch(preset)
            {
            case 1:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partp4x4+partb8x8",1,NULL);
                av_set_string3(c,"me_method","tesa",1,NULL);
                av_set_string3(c,"subq","10",1,NULL);
                av_set_string3(c,"me_range","24",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","16",1,NULL);
                av_set_string3(c,"refs","16",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","2",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8-fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 2:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","24",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","8",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 3:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 4:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","50",1,NULL);
            }
            break;
            case 5:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
            }
            break;
            case 6:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","30",1,NULL);
            }
            break;
            case 7:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","1",1,NULL);
                av_set_string3(c,"rc_lookahead","20",1,NULL);
            }
            break;
            case 8:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"rc_lookahead","10",1,NULL);
            }
            break;
            case 9:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","1",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred-dct8x8+fastpskip-mbtree",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"rc_lookahead","0",1,NULL);
            }
            break;
            case 10:
            {
                av_set_string3(c,"coder","0",1,NULL);
                av_set_string3(c,"flags","-loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","0",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","0",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","0",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","0",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","-bpyramid-mixed_refs-wpred-dct8x8+fastpskip-mbtree",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"aq_mode","0",1,NULL);
                av_set_string3(c,"rc_lookahead","0",1,NULL);
            }
            break;
            }
        }
        else
        {
            switch(preset)
            {
            case 1:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partp4x4+partb8x8",1,NULL);
                av_set_string3(c,"me_method","tesa",1,NULL);
                av_set_string3(c,"subq","10",1,NULL);
                av_set_string3(c,"me_range","24",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","16",1,NULL);
                av_set_string3(c,"refs","16",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","2",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8-fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 2:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partp4x4+partb8x8",1,NULL);
                av_set_string3(c,"me_method","umh",1,NULL);
                av_set_string3(c,"subq","10",1,NULL);
                av_set_string3(c,"me_range","24",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","8",1,NULL);
                av_set_string3(c,"refs","16",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","2",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 3:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partp4x4+partb8x8",1,NULL);
                av_set_string3(c,"me_method","umh",1,NULL);
                av_set_string3(c,"subq","9",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","8",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","2",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","60",1,NULL);
            }
            break;
            case 4:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partb8x8",1,NULL);
                av_set_string3(c,"me_method","umh",1,NULL);
                av_set_string3(c,"subq","8",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","2",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","5",1,NULL);
                av_set_string3(c,"directpred","3",1,NULL);
                av_set_string3(c,"trellis","1",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","50",1,NULL);
            }
            break;
            case 5:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partb8x8",1,NULL);
                av_set_string3(c,"me_method","hex",1,NULL);
                av_set_string3(c,"subq","7",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","3",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","1",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
            }
            break;
            case 6:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partb8x8",1,NULL);
                av_set_string3(c,"me_method","hex",1,NULL);
                av_set_string3(c,"subq","6",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","2",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","1",1,NULL);
                av_set_string3(c,"flags2","+bpyramid+mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","2",1,NULL);
                av_set_string3(c,"rc_lookahead","30",1,NULL);
            }
            break;
            case 7:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partb8x8",1,NULL);
                av_set_string3(c,"me_method","hex",1,NULL);
                av_set_string3(c,"subq","4",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","2",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","1",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","1",1,NULL);
                av_set_string3(c,"rc_lookahead","20",1,NULL);
            }
            break;
            case 8:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4+partp8x8+partb8x8",1,NULL);
                av_set_string3(c,"me_method","hex",1,NULL);
                av_set_string3(c,"subq","2",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred+dct8x8+fastpskip",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"rc_lookahead","10",1,NULL);
            }
            break;
            case 9:
            {
                av_set_string3(c,"coder","1",1,NULL);
                av_set_string3(c,"flags","+loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","+parti8x8+parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","1",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","40",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","1",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","3",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","+bpyramid-mixed_refs+wpred+dct8x8+fastpskip-mbtree",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"rc_lookahead","0",1,NULL);
            }
            break;
            case 10:
            {
                av_set_string3(c,"coder","0",1,NULL);
                av_set_string3(c,"flags","-loop",1,NULL);
                av_set_string3(c,"cmp","+chroma",1,NULL);
                av_set_string3(c,"partitions","-parti8x8-parti4x4-partp8x8-partb8x8",1,NULL);
                av_set_string3(c,"me_method","dia",1,NULL);
                av_set_string3(c,"subq","0",1,NULL);
                av_set_string3(c,"me_range","16",1,NULL);
                av_set_string3(c,"g","250",1,NULL);
                av_set_string3(c,"keyint_min","25",1,NULL);
                av_set_string3(c,"sc_threshold","0",1,NULL);
                av_set_string3(c,"i_qfactor","0.71",1,NULL);
                av_set_string3(c,"b_strategy","0",1,NULL);
                av_set_string3(c,"qcomp","0.6",1,NULL);
                av_set_string3(c,"qmin","10",1,NULL);
                av_set_string3(c,"qmax","51",1,NULL);
                av_set_string3(c,"qdiff","4",1,NULL);
                av_set_string3(c,"bf","0",1,NULL);
                av_set_string3(c,"refs","1",1,NULL);
                av_set_string3(c,"directpred","1",1,NULL);
                av_set_string3(c,"trellis","0",1,NULL);
                av_set_string3(c,"flags2","-bpyramid-mixed_refs-wpred-dct8x8+fastpskip-mbtree",1,NULL);
                av_set_string3(c,"wpredp","0",1,NULL);
                av_set_string3(c,"aq_mode","0",1,NULL);
                av_set_string3(c,"rc_lookahead","0",1,NULL);
            }
            break;

            }
        }
    }

}


static AVStream *add_video_stream(AVFormatContext *oc,const Movie::Info & info,RenderContext *rc)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 0);
    if (!st)
    {
        rc->error = true;
        rc->errorText =  "Could not allocate stream";
        return st;
    }

    AVCodec *codec = avcodec_find_encoder_by_name(info.videos[0].codec_short.toCString());
    if (!codec)
    {
        rc->error = true;
        rc->errorText =  "Could not find encoder";
        return st;
    }

    c = st->codec;

    avcodec_get_context_defaults(c);

    c->codec_id = codec->id;
    c->codec_type = AVMEDIA_TYPE_VIDEO;

    if(info.videos[0].codec_short == "libxvid")
    {
        c->codec_tag = AV_RL32("xvid");
        //c->codec_id = CODEC_ID_XVID;
    }
    else if(info.videos[0].codec_short == "libx264")
    {
        rc->is_codec_x264 = true;
    }

    /* put sample parameters */
    if(info.videos[0].is_bitrate_or_crf)
    {
        c->bit_rate = info.videos[0].bit_rate * 1000;
    }
    else
    {
        if(rc->is_codec_x264)
        {
            c->crf = info.videos[0].bit_rate;
        }
        else
        {
            c->qmin = 1;
            c->qmax = info.videos[0].bit_rate;
        }
    }

    //c->flags |= CODEC_FLAG_QSCALE;
    //c->global_quality = st->quality = FF_QP2LAMBDA * (float)info.videos[0].bit_rate;

    /* resolution must be a multiple of two */
    c->width = info.videos[0].width;
    c->height = info.videos[0].height;

    AVRational sample_aspect_ratio;
    sample_aspect_ratio.den = 1;
    sample_aspect_ratio.num = 1;
    st->sample_aspect_ratio =
        c->sample_aspect_ratio = st->sample_aspect_ratio = sample_aspect_ratio;
    /* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
    AVRational fps = av_d2q(info.videos[0].fps,65535);

    if (codec->supported_framerates)
        fps = codec->supported_framerates[av_find_nearest_q_idx(fps, codec->supported_framerates)];
    swapVariables(fps.num,fps.den);
    c->time_base = st->time_base = st->r_frame_rate = fps;

    if(info.videos[0].codec_short == "mjpeg")
        c->pix_fmt = PIX_FMT_YUVJ420P;
    else
        c->pix_fmt = PIX_FMT_YUV420P;

    setCompressionPreset(c,info.videos[0].compressionPreset,(rc->all_pass>1)?rc->current_pass:2,rc);
    if(info.videos[0].codec_short == "mjpeg")
        c->gop_size = 0;
    else
        c->gop_size = info.videos[0].gop;

    // some formats want stream headers to be separate
    if(oc->oformat && oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    //c->flags |= CODEC_FLAG2_LOCAL_HEADER;

    if(rc->all_pass>1)
    {
        if(rc->current_pass == 1)
            c->flags |= CODEC_FLAG_PASS1;
        else
        {
            c->flags |= CODEC_FLAG_PASS2;

            c->stats_in = const_cast<char *>(rc->pass_info.toCString());
        }
    }
    {
        const ScopedLock myScopedLock (avcodec_critical);
        if(avcodec_open(c, codec)<0)
        {
            rc->error = true;
            rc->errorText =  "Could not open video codec";
            return st;
        }
    }

    return st;
}

static AVStream *add_audio_stream(AVFormatContext *oc, enum CodecID codec_id,const Movie::Info & info, RenderContext *rc)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 1);
    if (!st)
    {
        rc->error = true;
        rc->errorText =  "Could not alloc stream";
        return st;
    }

    c = st->codec;
    c->codec_id = codec_id;
    c->codec_type = AVMEDIA_TYPE_AUDIO;



    /* put sample parameters */
    c->sample_fmt = AV_SAMPLE_FMT_S16;
    c->bit_rate = 64000;
    c->sample_rate = 44100;
    c->channels = 2;

    // some formats want stream headers to be separate
    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return st;
}

static void open_audio(AVFormatContext *oc, AVStream *st,RenderContext *rc)
{
    AVCodecContext *c;
    AVCodec *codec;

    c = st->codec;

    /* find the audio encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec)
    {
        rc->error = true;
        rc->errorText =  "Audio codec not found";
        return;
    }

    /* open it */
    {
        const ScopedLock myScopedLock (avcodec_critical);
        if(avcodec_open(c, codec) < 0)
        {
            rc->error = true;
            rc->errorText =  "Could not open audio codec";
            return;
        }
    }
    /* init signal generator */
    rc->t = 0;
    rc->tincr = 2 * M_PI * 110.0 / c->sample_rate;
    /* increment frequency by 110 Hz per second */
    rc->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    rc->audio_outbuf_size = 262144;
    rc->audio_outbuf = (uint8_t *)av_malloc(rc->audio_outbuf_size);
    if(!rc->audio_outbuf)
    {
        rc->error = true;
        rc->errorText = "Could not allocate audio buffer size";
        return;
    }
    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
       support to compute the input frame size in samples */
    if (c->frame_size <= 1)
    {
        rc->audio_input_frame_size = rc->audio_outbuf_size / c->channels;
        switch(st->codec->codec_id)
        {
        case CODEC_ID_PCM_S16LE:
        case CODEC_ID_PCM_S16BE:
        case CODEC_ID_PCM_U16LE:
        case CODEC_ID_PCM_U16BE:
            rc->audio_input_frame_size >>= 1;
            break;
        default:
            break;
        }
    }
    else
    {
        rc->audio_input_frame_size = c->frame_size;
    }
    int size_samples = rc->audio_input_frame_size * 2 * c->channels;
    rc->samples = (int16_t *)av_malloc(size_samples);
    if(!rc->samples)
    {
        rc->error = true;
        rc->errorText =  "Could not alocate audio buffer size";
        return;
    }
}

static AVFrame *alloc_picture(enum PixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture)
        return 0;
    size = avpicture_get_size(pix_fmt, width, height);
    if(size<0)
    {
        av_free(picture);
        return 0;
    }
    picture_buf = (uint8_t *)av_malloc(size);
    if (!picture_buf)
    {
        av_free(picture);
        return 0;
    }
    avpicture_fill((AVPicture *)picture, picture_buf, pix_fmt, width, height);
    return picture;
}

static void open_video(AVFormatContext *oc, AVStream *st, RenderContext* rc)
{
    AVCodecContext *c;

    c = st->codec;


    rc->video_outbuf = NULL;
    if (!oc->oformat || !(oc->oformat->flags & AVFMT_RAWPICTURE))
    {
        int video_outbuf_size_candidate = 6 * c->width * c->height + 200;
        rc->video_outbuf_size = (video_outbuf_size_candidate>262144)?video_outbuf_size_candidate:262144;
        rc->video_outbuf = (uint8_t *)av_malloc(rc->video_outbuf_size);
        if(!rc->video_outbuf)
        {
            rc->error = true;
            rc->errorText =  "Could not alocate video buffer";
            return;
        }
    }

    rc->picture = alloc_picture(c->pix_fmt, c->width, c->height);
    if(!rc->picture)
    {
        rc->error = true;
        rc->errorText = "Could not allocate picture";
        return;
    }
}

static void fill_frame(AVFrame *pict, int frame_index, const Movie::Info& info, Timeline * timeline,PixelFormat pix_fmt, RenderContext *rc)
{

    Movie::VideoInfo video_info = info.videos[0];
    if(!timeline->current_interval)
    {
        //Make black image

        int height = video_info.height;
        int width = video_info.width;

        /* Y */
        memset(pict->data[0],0,(height-1) * pict->linesize[0] + width-1);
        /* U */
        memset(pict->data[1],128,(height/2-1) * pict->linesize[1] + width/2-1);
        /* V */
        memset(pict->data[2],128,(height/2-1) * pict->linesize[2] + width/2-1);

        rc->end_writing = !timeline->SkipFrame();
        return;
    }
    Movie * movie = timeline->current_interval->movie;

    if(
        rc->img_convert_ctx == NULL
        || srcW != movie->width
        || srcH != movie->height
        || dstW != video_info.width
        || dstH != video_info.height
        || srcFormat != movie->GetMovieInfo()->videos[0].pix_fmt
        || dstFormat != pix_fmt
    )
    {
        srcW = movie->width;
        srcH = movie->height;
        dstW = video_info.width;
        dstH = video_info.height;
        srcFormat = movie->GetMovieInfo()->videos[0].pix_fmt;
        dstFormat = pix_fmt;

        if(rc->img_convert_ctx != NULL )
        {
            sws_freeContext(rc->img_convert_ctx);
            rc->img_convert_ctx = NULL;
        }
        rc->img_convert_ctx = sws_getContext(srcW, srcH,
                                             srcFormat,
                                             dstW, dstH,
                                             dstFormat,
                                             sws_flags, NULL, NULL, NULL);
        if (rc->img_convert_ctx == NULL)
        {
            rc->errorText = "Can't initialize the conversion context";
            rc->error = true;
            return;
        }
    }


    int scale_res = sws_scale(rc->img_convert_ctx, movie->pFrame->data, movie->pFrame->linesize,
                              0, movie->height, pict->data, pict->linesize);

}


static bool write_video_frame(AVFormatContext *oc, AVStream *st, const Movie::Info& info, Timeline * timeline,RenderContext *rc)
{
    int out_size;
    AVCodecContext *c;
    c = st->codec;

    fill_frame(rc->picture, rc->frame_count, info, timeline, c->pix_fmt, rc);
    rc->end_writing = !timeline->SkipFrame();

    if(rc->error)
    {
        return false;
    }

    if (oc->oformat->flags & AVFMT_RAWPICTURE)
    {
        /* raw video case. The API will change slightly in the near
           futur for that */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.stream_index= st->index;
        pkt.data= (uint8_t *)rc->picture;
        pkt.size= sizeof(AVPicture);

        if(av_interleaved_write_frame(oc, &pkt)<0)
        {
            rc->errorText =  "Error while writing video packet";
            rc->error = true;
            return false;
        }
    }
    else
    {
        /* encode the image */

        //if(rc->is_codec_x264)
        rc->picture->pts = rc->pts++;
        for(;;)
        {
            /* flushing buffers */
            AVFrame * send_picture = (rc->end_writing)?NULL:rc->picture;

            out_size = avcodec_encode_video(c, rc->video_outbuf, rc->video_outbuf_size, send_picture);
            /* if zero size, it means the image was buffered */

            if (out_size < 0)
            {
                rc->errorText =  "Error while encoding video packet";
                rc->error = true;
                return false;
            }

            if(out_size == 0)
                break;

            AVPacket pkt;
            av_init_packet(&pkt);

            if (c->coded_frame->pts != AV_NOPTS_VALUE)
                pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
            if(c->coded_frame->key_frame)
                pkt.flags |= AV_PKT_FLAG_KEY;
            pkt.stream_index= st->index;
            pkt.data = rc->video_outbuf;
            pkt.size = out_size;

            /* write the compressed frame in the media file */
            if(av_interleaved_write_frame(oc, &pkt)<0)
            {
                rc->errorText =  "Error while writing video packet";
                rc->error = true;
                return false;
            }
            if(rc->all_pass>1 && c->stats_out)
                rc->pass_info<<c->stats_out;
            if (!rc->end_writing)
                break;

        }

    }

    rc->frame_count++;
    return true;
}

/* prepare a 16 bit dummy audio frame of 'frame_size' samples and
   'nb_channels' channels */
static void get_audio_frame(int16_t *samples, int frame_size, int nb_channels, const Movie::Info& info, Timeline * timeline,RenderContext* rc)
{
    int j, i, v;
    int16_t *q;

    q = samples;
    for(j=0; j<frame_size; j++)
    {
        v = (int)(sin(rc->t) * 10000);
        for(i = 0; i < nb_channels; i++)
            *q++ = v;
        rc->t += rc->tincr;
        rc->tincr += rc->tincr2;
    }
}

static bool write_audio_frame(AVFormatContext *oc, AVStream *st, const Movie::Info& info, Timeline * timeline,RenderContext* rc)
{
    AVCodecContext *c;
    AVPacket pkt;
    av_init_packet(&pkt);

    c = st->codec;
    get_audio_frame(rc->samples, rc->audio_input_frame_size, c->channels, info, timeline, rc);

    pkt.size = avcodec_encode_audio(c, rc->audio_outbuf, rc->audio_outbuf_size, rc->samples);
    if(pkt.size<0)
    {
        rc->errorText =  "Error while encoding audio packet";
        rc->error = true;
        return false;
    }
    if (c->coded_frame && c->coded_frame->pts != AV_NOPTS_VALUE)
        pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
    pkt.flags |= AV_PKT_FLAG_KEY;
    pkt.stream_index = st->index;
    pkt.data = rc->audio_outbuf;

    /* write the compressed frame in the media file */
    if (av_interleaved_write_frame(oc, &pkt) <0)
    {
        rc->errorText =  "Error while writing audio packet";
        rc->error = true;
        return false;
    }
    return true;
}

int _WritePacket(void* cookie, uint8_t* buffer, int bufferSize)
{
    FileOutputStream* fs = reinterpret_cast<FileOutputStream*>(cookie);
    bool res = fs->write(buffer,bufferSize);
    fs->flush();
    return (res)?bufferSize:0;
}

int64_t _SeekWithOutputStream(void* cookie, int64_t offset, int whence)
{
    FileOutputStream* fs = reinterpret_cast<FileOutputStream*>(cookie);
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

class CloseRender
{
public:
    AVStream* video_st;
    AVStream* audio_st;
    AVFormatContext *oc;
    ByteIOContext* ByteIOCtx;
    unsigned char* pDataBuffer;
    FileOutputStream* fs;
    CloseRender()
    {
        video_st = 0;
        audio_st = 0;
        oc = 0;
        ByteIOCtx = 0;
        pDataBuffer = 0;
        fs = 0;
    }
    ~CloseRender()
    {
        if(video_st)
        {
            const ScopedLock myScopedLock (avcodec_critical);
            avcodec_close(video_st->codec);
            video_st = 0;
        }
        if(audio_st)
        {
            const ScopedLock myScopedLock (avcodec_critical);
            avcodec_close(audio_st->codec);
            audio_st = 0;
        }

        if(oc)
        {
            for(int i = 0; i < oc->nb_streams; i++)
            {
                av_freep(&oc->streams[i]->codec);
                av_freep(&oc->streams[i]);
            }
            av_free(oc);
            oc = 0;
        }
        if(pDataBuffer)
        {
            delete []pDataBuffer;
            pDataBuffer = 0;
        }
        if(ByteIOCtx)
        {
            delete ByteIOCtx;
            ByteIOCtx = 0;
        }
        if(fs)
        {
            fs->flush();
            delete fs;
            fs = 0;
        }

    }
};



String Timeline::Render(const Movie::Info & info, task * thread, void (* reportProgress)(task*,double))
{
    bool video_enabled = info.videos.size()>0;
    RenderContext rc,*rcp = &rc;
    rcp->pass_info=String::empty;
    rcp->is_codec_x264 = false;
    rcp->error = false;


    rcp->all_pass = (video_enabled)?info.videos[0].pass:1;
    for(rcp->current_pass=1; rcp->current_pass<=rcp->all_pass; ++rcp->current_pass)
    {
        CloseRender deleter;

        bool audio_enabled = info.audios.size()>0 && rcp->all_pass==rcp->current_pass;

        rcp->pts = 0;

        rcp->end_writing = false;
        AVOutputFormat *fmt;


        double audio_pts, video_pts;
        int i;

        const char * c_string_filename = info.filename.toCString();
        fmt = av_guess_format(info.format_short.toCString(), NULL, NULL);
        if (!fmt)
        {
            return "Could not deduce output format";
        }
        deleter.oc = avformat_alloc_context();
        if (!deleter.oc)
        {
            return "Could not avformat_alloc_context";
        }
        deleter.oc->oformat = fmt;

        snprintf(deleter.oc->filename, sizeof(deleter.oc->filename), "%s", c_string_filename);
        deleter.video_st = NULL;
        deleter.audio_st = NULL;
        if (video_enabled)
        {
            GotoSecondAndRead(0.0,false);
            deleter.video_st = add_video_stream(deleter.oc, info, rcp);
            if(rcp->error)
                return rcp->errorText;
        }
        if (fmt->audio_codec != CODEC_ID_NONE && audio_enabled)
        {
            deleter.audio_st = add_audio_stream(deleter.oc, fmt->audio_codec, info, rcp);
            if(rcp->error)
                return rcp->errorText;
        }
        if (av_set_parameters(deleter.oc, NULL) < 0)
        {
            return "Invalid output format parameters";
        }

        if (deleter.video_st)
        {
            open_video(deleter.oc, deleter.video_st, rcp);
            if(rcp->error)
                return rcp->errorText;
        }
        if (deleter.audio_st)
        {
            open_audio(deleter.oc, deleter.audio_st, rcp);
            if(rcp->error)
                return rcp->errorText;
        }

        File f(info.filename);
        if(f.exists())
        {
            if(!f.deleteFile())
            {
                return "Not able to delete file";
            }
        }
        if(!f.hasWriteAccess())
        {
            return "No write access";
        }




        deleter.fs = f.createOutputStream();
        int lSize = 32768;
        try
        {
            deleter.ByteIOCtx = new ByteIOContext();
        }
        catch(std::bad_alloc& ex)
        {
            deleter.ByteIOCtx = 0;
            return "Memory allocation error";
        }

        try
        {
            deleter.pDataBuffer = new unsigned char[lSize];
        }
        catch(std::bad_alloc& ex)
        {
            deleter.pDataBuffer = 0;
            return "Memory allocation error";
        }

        init_put_byte(deleter.ByteIOCtx, deleter.pDataBuffer, lSize, 1, deleter.fs, NULL, _WritePacket, _SeekWithOutputStream);
        deleter.oc->pb = deleter.ByteIOCtx;

        if(av_write_header(deleter.oc))
        {
            return "Can't write header";
        }

        for(;;)
        {
            if(reportProgress && rcp->pts % 10 == 0)
            {
                double pos = ((double)rcp->pts * (double)deleter.video_st->r_frame_rate.num / (double)deleter.video_st->r_frame_rate.den);
                if(rcp->all_pass==1)
                    reportProgress(thread, pos / duration);
                else if(rcp->current_pass==2)
                    reportProgress(thread, (pos + duration) / (2.0 * duration));
                else
                    reportProgress(thread, (pos) / (2.0 * duration));
            }

            if(thread && thread->threadShouldExit())
                return localization::LABEL_SAVE_VIDEO_SUSPENDED;

            /* compute current audio and video time */
            if (deleter.audio_st)
                audio_pts = (double)deleter.audio_st->pts.val * deleter.audio_st->time_base.num / deleter.audio_st->time_base.den;
            else
                audio_pts = 0.0;

            if (deleter.video_st)
                video_pts = (double)deleter.video_st->pts.val * deleter.video_st->time_base.num / deleter.video_st->time_base.den;
            else
                video_pts = 0.0;

            /*if ((!audio_st || audio_pts >= STREAM_DURATION) &&
                (!video_st || video_pts >= STREAM_DURATION))
                break;*/
            if(rcp->end_writing)
                break;

            /* write interleaved audio and video frames */
            if (!deleter.video_st || (deleter.video_st && deleter.audio_st && audio_pts < video_pts))
            {

                write_audio_frame(deleter.oc, deleter.audio_st,info,this, rcp);
                if(rcp->error)
                {
                    return rcp->errorText;
                }
            }
            else
            {
                write_video_frame(deleter.oc, deleter.video_st,info,this, rcp);
                if(rcp->error)
                {
                    return rcp->errorText;
                }
            }
        }

        if(av_write_trailer(deleter.oc))
        {
            return "Can't write trailer";
        }


        rcp->Dispose();
    }
    return String::empty;
}



