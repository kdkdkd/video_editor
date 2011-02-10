#include "timeline.h"
#include "movie.h"
#include "localization.h"


#define INT64_C __INT64_C

AVFrame *picture;
uint8_t *video_outbuf;
int frame_count, video_outbuf_size;
bool end_writing;
float t, tincr, tincr2;
int16_t *samples;
uint8_t *audio_outbuf;
int audio_outbuf_size;
int audio_input_frame_size;

static int sws_flags = SWS_BICUBIC;


extern "C" {
#include <libavutil/intreadwrite.h>
}


SwsContext *img_convert_ctx = NULL;

int srcW = 0;
int srcH = 0;
int dstW = 0;
int dstH = 0;
PixelFormat srcFormat;
PixelFormat dstFormat;

static AVStream *add_video_stream(AVFormatContext *oc,const Movie::Info & info)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 0);


    AVCodec *codec = avcodec_find_encoder_by_name(info.videos[0].codec_short.toCString());

    c = st->codec;

    avcodec_get_context_defaults(c);

    c->codec_id = codec->id;
    c->codec_type = AVMEDIA_TYPE_VIDEO;

    if(info.videos[0].codec_short == "libxvid")
    {
        c->codec_tag = AV_RL32("xvid");
        //c->codec_id = CODEC_ID_XVID;
    }else if(info.videos[0].codec_short == "libx264")
    {
        c->codec_tag = AV_RL32("avc1");
        c->codec_id = CODEC_ID_H264;
    }

    /* put sample parameters */
    c->bit_rate = info.videos[0].bit_rate * 1000;

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
    c->time_base = fps;

    c->gop_size = info.videos[0].gop; /* emit one intra frame every twelve frames at most */
    //c->max_b_frames = 2;
    //c->flags |= CODEC_FLAG_QSCALE;
    c->pix_fmt = PIX_FMT_YUV420P;

    // some formats want stream headers to be separate
    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    //c->flags |= CODEC_FLAG2_LOCAL_HEADER;

    if(info.videos[0].codec_short == "libx264")
    {
        c->coder_type = FF_CODER_TYPE_AC;
        c->flags = CODEC_FLAG_LOOP_FILTER;
        c->me_cmp = FF_CMP_CHROMA;
        c->partitions = X264_PART_I8X8 | X264_PART_I4X4 | X264_PART_P8X8 | X264_PART_P8X8 | X264_PART_B8X8;
        c->me_method = 7;
        c->me_subpel_quality = 7;
        c->me_range = 16;
        c->gop_size = 250;
        c->keyint_min = 25;
        c->scenechange_threshold = 40;
        c->i_quant_factor = 0.71;
        c->b_frame_strategy = FF_RC_STRATEGY_XVID;
        c->qcompress = 0.6;
        c->qmin = 10;
        c->qmax = 51;
        c->max_qdiff=4;
        c->max_b_frames = 3;
        c->refs=3;
        c->directpred=1;
        c->trellis=1;
        c->flags2 = CODEC_FLAG2_BPYRAMID | CODEC_FLAG2_MIXED_REFS | CODEC_FLAG2_WPRED | CODEC_FLAG2_8X8DCT | CODEC_FLAG2_FASTPSKIP;
        c->weighted_p_pred = 2;
    }
    avcodec_open(c, codec);

    return st;
}

static AVStream *add_audio_stream(AVFormatContext *oc, enum CodecID codec_id,const Movie::Info & info)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 1);

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

static void open_audio(AVFormatContext *oc, AVStream *st)
{
    AVCodecContext *c;
    AVCodec *codec;

    c = st->codec;

    /* find the audio encoder */
    codec = avcodec_find_encoder(c->codec_id);

    /* open it */
    avcodec_open(c, codec) < 0;

    /* init signal generator */
    t = 0;
    tincr = 2 * M_PI * 110.0 / c->sample_rate;
    /* increment frequency by 110 Hz per second */
    tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    audio_outbuf_size = 1000000;
    audio_outbuf = (uint8_t *)av_malloc(audio_outbuf_size);

    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
       support to compute the input frame size in samples */
    if (c->frame_size <= 1)
    {
        audio_input_frame_size = audio_outbuf_size / c->channels;
        switch(st->codec->codec_id)
        {
        case CODEC_ID_PCM_S16LE:
        case CODEC_ID_PCM_S16BE:
        case CODEC_ID_PCM_U16LE:
        case CODEC_ID_PCM_U16BE:
            audio_input_frame_size >>= 1;
            break;
        default:
            break;
        }
    }
    else
    {
        audio_input_frame_size = c->frame_size;
    }
    samples = (int16_t *)av_malloc(audio_input_frame_size * 2 * c->channels);
}

static AVFrame *alloc_picture(enum PixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture)
        return NULL;
    size = avpicture_get_size(pix_fmt, width, height);
    picture_buf = (uint8_t *)av_malloc(size);
    if (!picture_buf)
    {
        av_free(picture);
        return NULL;
    }
    avpicture_fill((AVPicture *)picture, picture_buf,
                   pix_fmt, width, height);
    return picture;
}

static void open_video(AVFormatContext *oc, AVStream *st)
{
    AVCodecContext *c;

    c = st->codec;


    video_outbuf = NULL;
    if (!(oc->oformat->flags & AVFMT_RAWPICTURE))
    {
        /* allocate output buffer */
        /* XXX: API change will be done */
        /* buffers passed into lav* can be allocated any way you prefer,
           as long as they're aligned enough for the architecture, and
           they're freed appropriately (such as using av_free for buffers
           allocated with av_malloc) */
        video_outbuf_size = 20000000;
        video_outbuf = (uint8_t *)av_malloc(video_outbuf_size);
    }

    /* allocate the encoded raw picture */
    picture = alloc_picture(c->pix_fmt, c->width, c->height);


    /* if the output format is not YUV420P, then a temporary YUV420P
       picture is needed too. It is then converted to the required
       output format */
}

static void close_video(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);
    if(img_convert_ctx != NULL )
    {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = NULL;
    }
    av_free(picture->data[0]);
    av_free(picture);
    av_free(video_outbuf);
}

static void close_audio(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);

    av_free(samples);
    av_free(audio_outbuf);
}

/* prepare a dummy image */
static void fill_frame(AVFrame *pict, int frame_index, const Movie::Info& info, Timeline * timeline,PixelFormat pix_fmt)
{
    if(!timeline->current_interval)
    {
        //Make black image
        end_writing = !timeline->SkipFrame();
        return;
    }
    Movie * movie = timeline->current_interval->movie;
    Movie::VideoInfo video_info = info.videos[0];
    if(
        img_convert_ctx == NULL
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

        if(img_convert_ctx != NULL )
        {
            sws_freeContext(img_convert_ctx);
            img_convert_ctx = NULL;
        }
        img_convert_ctx = sws_getContext(srcW, srcH,
                                         srcFormat,
                                         dstW, dstH,
                                         dstFormat,
                                         sws_flags, NULL, NULL, NULL);
    }


    sws_scale(img_convert_ctx, movie->pFrame->data, movie->pFrame->linesize,
              0, video_info.height, pict->data, pict->linesize);

    end_writing = !timeline->SkipFrame();

}

static bool write_video_frame(AVFormatContext *oc, AVStream *st, const Movie::Info& info, Timeline * timeline)
{
    int out_size, ret;
    AVCodecContext *c;
    c = st->codec;


    {
        fill_frame(picture, frame_count, info, timeline, c->pix_fmt);
    }
    if (end_writing)
    {
        return false;
    }
    //picture->quality = info.videos[0].bit_rate;

    if (oc->oformat->flags & AVFMT_RAWPICTURE)
    {
        /* raw video case. The API will change slightly in the near
           futur for that */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.stream_index= st->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);

        ret = av_interleaved_write_frame(oc, &pkt);
    }
    else
    {
        /* encode the image */
        out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
        /* if zero size, it means the image was buffered */
        if (out_size > 0)
        {
            AVPacket pkt;
            av_init_packet(&pkt);

            if (c->coded_frame->pts != AV_NOPTS_VALUE)
                pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
            if(c->coded_frame->key_frame)
                pkt.flags |= AV_PKT_FLAG_KEY;
            pkt.stream_index= st->index;
            pkt.data = video_outbuf;
            pkt.size = out_size;

            /* write the compressed frame in the media file */
            ret = av_interleaved_write_frame(oc, &pkt);
        }
        else
        {
            ret = 0;
        }
    }
    if (ret != 0)
    {
        false;
    }
    frame_count++;
    return true;
}

/* prepare a 16 bit dummy audio frame of 'frame_size' samples and
   'nb_channels' channels */
static void get_audio_frame(int16_t *samples, int frame_size, int nb_channels, const Movie::Info& info, Timeline * timeline)
{
    int j, i, v;
    int16_t *q;

    q = samples;
    for(j=0; j<frame_size; j++)
    {
        v = (int)(sin(t) * 10000);
        for(i = 0; i < nb_channels; i++)
            *q++ = v;
        t += tincr;
        tincr += tincr2;
    }
}

static bool write_audio_frame(AVFormatContext *oc, AVStream *st, const Movie::Info& info, Timeline * timeline)
{
    AVCodecContext *c;
    AVPacket pkt;
    av_init_packet(&pkt);

    c = st->codec;
    get_audio_frame(samples, audio_input_frame_size, c->channels, info, timeline);

    pkt.size= avcodec_encode_audio(c, audio_outbuf, audio_outbuf_size, samples);

    if (c->coded_frame && c->coded_frame->pts != AV_NOPTS_VALUE)
        pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
    pkt.flags |= AV_PKT_FLAG_KEY;
    pkt.stream_index= st->index;
    pkt.data= audio_outbuf;

    /* write the compressed frame in the media file */
    if (av_interleaved_write_frame(oc, &pkt) != 0)
    {
        return false;
    }
    return true;
}

bool Timeline::Render(const Movie::Info & info)
{

        bool video_enabled = info.videos.size()>0;
        bool audio_enabled = info.audios.size()>0;
        end_writing = false;
        AVOutputFormat *fmt;
        AVFormatContext *oc;
        AVStream *audio_st, *video_st;
        double audio_pts, video_pts;
        int i;

        const char * c_string_filename = info.filename.toCString();
        fmt = av_guess_format(info.format_short.toCString(), NULL, NULL);

        oc = avformat_alloc_context();
        oc->oformat = fmt;

        snprintf(oc->filename, sizeof(oc->filename), "%s", c_string_filename);
        video_st = NULL;
        audio_st = NULL;
        if (video_enabled)
        {
            GotoSecondAndRead(0.0,false);
            video_st = add_video_stream(oc, info);
        }
        if (fmt->audio_codec != CODEC_ID_NONE && audio_enabled)
        {
            audio_st = add_audio_stream(oc, fmt->audio_codec,info);
        }
        av_set_parameters(oc, NULL);

        if (video_st)
        {
            open_video(oc, video_st);
        }
        if (audio_st)
        {
            open_audio(oc, audio_st);
        }

        url_fopen(&oc->pb, c_string_filename, URL_WRONLY);

        av_write_header(oc);



        for(;;)
        {
            /* compute current audio and video time */
            if (audio_st)
                audio_pts = (double)audio_st->pts.val * audio_st->time_base.num / audio_st->time_base.den;
            else
                audio_pts = 0.0;

            if (video_st)
                video_pts = (double)video_st->pts.val * video_st->time_base.num / video_st->time_base.den;
            else
                video_pts = 0.0;

            /*if ((!audio_st || audio_pts >= STREAM_DURATION) &&
                (!video_st || video_pts >= STREAM_DURATION))
                break;*/
            if(end_writing)
                break;

            /* write interleaved audio and video frames */
            if (!video_st || (video_st && audio_st && audio_pts < video_pts))
            {

                write_audio_frame(oc, audio_st,info,this);
            }
            else
            {
                bool res = write_video_frame(oc, video_st,info,this);
                if(!res)
                {
                    break;
                }
            }
        }
        av_write_trailer(oc);

        if (video_st)
        {
            close_video(oc, video_st);
        }
        if (audio_st)
        {
            close_audio(oc, audio_st);
        }

        for(i = 0; i < oc->nb_streams; i++)
        {
            av_freep(&oc->streams[i]->codec);
            av_freep(&oc->streams[i]);
        }

        if (!(fmt->flags & AVFMT_NOFILE))
        {

            url_fclose(oc->pb);
        }


        av_free(oc);
    return true;
}
