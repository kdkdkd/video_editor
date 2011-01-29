#include "timeline.h"
#include "movie.h"
#include "localization.h"


#define INT64_C __INT64_C

AVFrame *picture, *tmp_picture;
uint8_t *video_outbuf;
int frame_count, video_outbuf_size;
bool end_writing;
float t, tincr, tincr2;
int16_t *samples;
uint8_t *audio_outbuf;
int audio_outbuf_size;
int audio_input_frame_size;

static int sws_flags = SWS_BICUBIC;

static AVStream *add_video_stream(AVFormatContext *oc, enum CodecID codec_id)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 0);

    c = st->codec;
    c->codec_id = codec_id;
    c->codec_type = AVMEDIA_TYPE_VIDEO;

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    //c->width = 800;
    c->width = 800;
    c->height = 600;
    /* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
    c->time_base.den = 25;
    c->time_base.num = 1;
    c->gop_size = 12; /* emit one intra frame every twelve frames at most */
    c->pix_fmt = PIX_FMT_YUV420P;
    if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
        /* just for testing, we also add B frames */
        c->max_b_frames = 2;
    }
    if (c->codec_id == CODEC_ID_MPEG1VIDEO){
        /* Needed to avoid using macroblocks in which some coeffs overflow.
           This does not happen with normal video, it just happens here as
           the motion of the chroma plane does not match the luma plane. */
        c->mb_decision=2;
    }
    // some formats want stream headers to be separate
    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return st;
}

static AVStream *add_audio_stream(AVFormatContext *oc, enum CodecID codec_id)
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

    audio_outbuf_size = 10000;
    audio_outbuf = (uint8_t *)av_malloc(audio_outbuf_size);

    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
       support to compute the input frame size in samples */
    if (c->frame_size <= 1) {
        audio_input_frame_size = audio_outbuf_size / c->channels;
        switch(st->codec->codec_id) {
        case CODEC_ID_PCM_S16LE:
        case CODEC_ID_PCM_S16BE:
        case CODEC_ID_PCM_U16LE:
        case CODEC_ID_PCM_U16BE:
            audio_input_frame_size >>= 1;
            break;
        default:
            break;
        }
    } else {
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
    if (!picture_buf) {
        av_free(picture);
        return NULL;
    }
    avpicture_fill((AVPicture *)picture, picture_buf,
                   pix_fmt, width, height);
    return picture;
}

static void open_video(AVFormatContext *oc, AVStream *st)
{
    AVCodec *codec;
    AVCodecContext *c;

    c = st->codec;

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);


    /* open the codec */
    avcodec_open(c, codec) < 0;

    video_outbuf = NULL;
    if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
        /* allocate output buffer */
        /* XXX: API change will be done */
        /* buffers passed into lav* can be allocated any way you prefer,
           as long as they're aligned enough for the architecture, and
           they're freed appropriately (such as using av_free for buffers
           allocated with av_malloc) */
        video_outbuf_size = 200000;
        video_outbuf = (uint8_t *)av_malloc(video_outbuf_size);
    }

    /* allocate the encoded raw picture */
    picture = alloc_picture(c->pix_fmt, c->width, c->height);


    /* if the output format is not YUV420P, then a temporary YUV420P
       picture is needed too. It is then converted to the required
       output format */
    tmp_picture = NULL;
    if (c->pix_fmt != PIX_FMT_YUV420P) {
        tmp_picture = alloc_picture(PIX_FMT_YUV420P, c->width, c->height);

    }
}

static void close_video(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);
    av_free(picture->data[0]);
    av_free(picture);
    if (tmp_picture) {
        av_free(tmp_picture->data[0]);
        av_free(tmp_picture);
    }
    av_free(video_outbuf);
}

static void close_audio(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);

    av_free(samples);
    av_free(audio_outbuf);
}

/* prepare a dummy image */
static void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height)
{
    int x, y, i;

    i = frame_index;

    /* Y */
    for(y=0;y<height;y++) {
        for(x=0;x<width;x++) {
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;
        }
    }

    /* Cb and Cr */
    for(y=0;y<height/2;y++) {
        for(x=0;x<width/2;x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }

    end_writing = frame_index>=25*10;
}

static bool write_video_frame(AVFormatContext *oc, AVStream *st)
{
    int out_size, ret;
    AVCodecContext *c;
    static struct SwsContext *img_convert_ctx;
    c = st->codec;

    if (end_writing) {
        /* no more frame to compress. The codec has a latency of a few
           frames if using B frames, so we get the last frames by
           passing the same picture again */
    } else {
        if (c->pix_fmt != PIX_FMT_YUV420P) {
            /* as we only generate a YUV420P picture, we must convert it
               to the codec pixel format if needed */
            if (img_convert_ctx == NULL) {
                img_convert_ctx = sws_getContext(c->width, c->height,
                                                 PIX_FMT_YUV420P,
                                                 c->width, c->height,
                                                 c->pix_fmt,
                                                 sws_flags, NULL, NULL, NULL);
            }
            fill_yuv_image(tmp_picture, frame_count, c->width, c->height);
            sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
                      0, c->height, picture->data, picture->linesize);
        } else {
            fill_yuv_image(picture, frame_count, c->width, c->height);
        }
    }


    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
        /* raw video case. The API will change slightly in the near
           futur for that */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.stream_index= st->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);

        ret = av_interleaved_write_frame(oc, &pkt);
    } else {
        /* encode the image */
        out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
        /* if zero size, it means the image was buffered */
        if (out_size > 0) {
            AVPacket pkt;
            av_init_packet(&pkt);

            if (c->coded_frame->pts != AV_NOPTS_VALUE)
                pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
            if(c->coded_frame->key_frame)
                pkt.flags |= AV_PKT_FLAG_KEY;
            pkt.stream_index= st->index;
            pkt.data= video_outbuf;
            pkt.size= out_size;

            /* write the compressed frame in the media file */
            ret = av_interleaved_write_frame(oc, &pkt);
        } else {
            ret = 0;
        }
    }
    if (ret != 0) {
        false;
    }
    frame_count++;
    return true;
}

/* prepare a 16 bit dummy audio frame of 'frame_size' samples and
   'nb_channels' channels */
static void get_audio_frame(int16_t *samples, int frame_size, int nb_channels)
{
    int j, i, v;
    int16_t *q;

    q = samples;
    for(j=0;j<frame_size;j++) {
        v = (int)(sin(t) * 10000);
        for(i = 0; i < nb_channels; i++)
            *q++ = v;
        t += tincr;
        tincr += tincr2;
    }
}

static bool write_audio_frame(AVFormatContext *oc, AVStream *st)
{
    AVCodecContext *c;
    AVPacket pkt;
    av_init_packet(&pkt);

    c = st->codec;

    get_audio_frame(samples, audio_input_frame_size, c->channels);

    pkt.size= avcodec_encode_audio(c, audio_outbuf, audio_outbuf_size, samples);

    if (c->coded_frame && c->coded_frame->pts != AV_NOPTS_VALUE)
        pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
    pkt.flags |= AV_PKT_FLAG_KEY;
    pkt.stream_index= st->index;
    pkt.data= audio_outbuf;

    /* write the compressed frame in the media file */
    if (av_interleaved_write_frame(oc, &pkt) != 0) {
        return false;
    }
    return true;
}

bool Timeline::Render(String filename)
{
    end_writing = false;
    printf(filename.toCString());
    AVOutputFormat *fmt;
    AVFormatContext *oc;
    AVStream *audio_st, *video_st;
    double audio_pts, video_pts;
    int i;

    const char * c_string_filename = filename.toCString();
    fmt = av_guess_format("psp", NULL, NULL);
    oc = avformat_alloc_context();
    oc->oformat = fmt;
    snprintf(oc->filename, sizeof(oc->filename), "%s", c_string_filename);
    video_st = NULL;
    audio_st = NULL;
    if (fmt->video_codec != CODEC_ID_NONE)
    {
        video_st = add_video_stream(oc, fmt->video_codec);
    }
    if (fmt->audio_codec != CODEC_ID_NONE)
    {
        audio_st = add_audio_stream(oc, fmt->audio_codec);
    }
    av_set_parameters(oc, NULL);
    if (video_st)
        open_video(oc, video_st);
    if (audio_st)
        open_audio(oc, audio_st);

    url_fopen(&oc->pb, c_string_filename, URL_WRONLY);

    av_write_header(oc);

    for(;;) {
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
        if (!video_st || (video_st && audio_st && audio_pts < video_pts)) {
            write_audio_frame(oc, audio_st);
        } else {
            write_video_frame(oc, video_st);
        }
    }

    av_write_trailer(oc);

    /* free the streams */
    for(i = 0; i < oc->nb_streams; i++) {
        av_freep(&oc->streams[i]->codec);
        av_freep(&oc->streams[i]);
    }

    if (!(fmt->flags & AVFMT_NOFILE)) {
        /* close the output file */
        url_fclose(oc->pb);
    }

    /* free the stream */
    av_free(oc);
    return true;
}
