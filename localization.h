#ifndef LOCALIZATION_H_INCLUDED
#define LOCALIZATION_H_INCLUDED

#include "juce/juce.h"
#include "events.h"
namespace localization
{

extern EventList AfterLocalizationChnaged;
extern String MENU_FILE;
extern String MENU_FILE_OPEN;
extern String MENU_FILE_SAVE;
extern String MENU_QUIT;
extern String MENU_FRAME;
extern String MENU_JUMP;
extern String MENU_SAVE_FRAME;
extern String MENU_SHOW_TASKS;
extern String MENU_RECENT;


extern String CANT_LOAD_FILE;
extern String DIALOG_CHOOSE_FILE_TO_OPEN;
extern String DIALOG_CHOOSE_SCREENSHOT_TO_SAVE;
extern String LABEL_FRAMES;
extern String LABEL_TIME;


extern String LABEL_CHOOSE_JUMP;
extern String LABEL_TOTAL_TIME;

extern String LABEL_OK;
extern String LABEL_CANCEL;

extern String FILE_SAVED;
extern String FILE_NOT_SAVED;

extern String LABEL_PLAY;
extern String LABEL_PAUSE;

extern String LABEL_STOP;
extern String LABEL_NEXT_FRAME;
extern String LABEL_PREV_FRAME;

extern String LABEL_SPECIFIC_TIME;

extern String LABEL_NEXT_FIVE_FRAME;
extern String LABEL_PREV_FIVE_FRAME;

extern String LABEL_NEXT_SECOND;
extern String LABEL_PREV_SECOND;

extern String LABEL_HOURS;
extern String LABEL_MINUTES;
extern String LABEL_SECONDS;
extern String LABEL_MINI_SECONDS;

extern String LABEL_INFO;
extern String LABEL_FILE;
extern String LABEL_DURATION;
extern String LABEL_SIZE;

extern String LABEL_BITRATE;
extern String LABEL_NOT_AVIABLE;
extern String LABEL_KB_PER_SECOND;

extern String LABEL_STREAM;
extern String LABEL_VIDEO;
extern String LABEL_AUDIO;
extern String LABEL_SUBTITLES;
extern String LABEL_FORMAT;


extern String LABEL_CODEC;
extern String LABEL_RESOLUTION;
extern String LABEL_FPS;
extern String LABEL_LANG;
extern String LABEL_COMMENT;

extern String LABEL_SAMPLE_RATE;
extern String LABEL_CHANNELS;

extern String LABEL_MOVIES;
extern String LABEL_DELETE;
extern String LABEL_SCALE;


extern String MENU_VIDEO_PART;
extern String LABEL_DELETE;
extern String LABEL_DELETE_VIDEO_PART;
extern String LABEL_SPLIT;
extern String LABEL_REMOVE_SPACES;


extern String LABEL_SAVE_VIDEO;
extern String LABEL_SAVE;


extern String LABEL_VIDEO_SAVE_PATH;
extern String LABEL_VIDEO_SAVE_FORMAT;

extern String LABEL_VIDEO_SAVE_CODEC;
extern String LABEL_VIDEO_SAVE_SIZE;
extern String LABEL_VIDEO_SAVE_QUALITY;

extern String LABEL_VIDEO_SAVE_QUALITY_BEST;
extern String LABEL_VIDEO_SAVE_QUALITY_BETTER;
extern String LABEL_VIDEO_SAVE_QUALITY_GOOD;
extern String LABEL_VIDEO_SAVE_QUALITY_NORMAL;
extern String LABEL_VIDEO_SAVE_QUALITY_BAD;
extern String LABEL_VIDEO_SAVE_QUALITY_WORSE;
extern String LABEL_VIDEO_SAVE_QUALITY_WORST;
extern String LABEL_VIDEO_SAVE_QUALITY_OTHER;
extern String LABEL_VIDEO_SAVE_SIZE_OTHER;

extern String LABEL_VIDEO_SAVE_BITRATE;
extern String LABEL_VIDEO_SAVE_FPS;
extern String LABEL_VIDEO_SAVE_RESOLUTION;
extern String LABEL_VIDEO_SAVE_COMPRESSION;
extern String LABEL_VIDEO_SAVE_AUTO;
extern String LABEL_VIDEO_SAVE_PASS;
extern String LABEL_VIDEO_SAVE_ADVANCED_MODE;
extern String LABEL_VIDEO_GOP;
extern String LABEL_VIDEO_SAVE_PASS_ONE;
extern String LABEL_VIDEO_SAVE_PASS_TWO;


extern String LABEL_AUDIO_SAVE_CODEC;
extern String LABEL_AUDIO_SAVE_BITRATE;
extern String LABEL_AUDIO_SAVE_SAMPLE_RATE;
extern String LABEL_AUDIO_SAVE_CHANNELS;


extern String MENU_HEADER_PC;
extern String MENU_HEADER_WEB;
extern String MENU_HEADER_CELL_PHONE;
extern String MENU_HEADER_AUDIO;
extern String MENU_HEADER_IPOD;

extern String VALIDATION_SAVE_FPS;
extern String VALIDATION_SAVE_WIDTH;
extern String VALIDATION_SAVE_HEIGHT;
extern String VALIDATION_SAVE_BITRATE;

extern String LABEL_VIDEO_SAVE_COMPRESSION_ULTRAFAST;
extern String LABEL_VIDEO_SAVE_COMPRESSION_SUPERFAST;
extern String LABEL_VIDEO_SAVE_COMPRESSION_VERYFAST;
extern String LABEL_VIDEO_SAVE_COMPRESSION_FASTER;
extern String LABEL_VIDEO_SAVE_COMPRESSION_FAST;
extern String LABEL_VIDEO_SAVE_COMPRESSION_MEDIUM;
extern String LABEL_VIDEO_SAVE_COMPRESSION_SLOW;
extern String LABEL_VIDEO_SAVE_COMPRESSION_SLOWER;
extern String LABEL_VIDEO_SAVE_COMPRESSION_VERYSLOW;
extern String LABEL_VIDEO_SAVE_COMPRESSION_PLACEBO;

extern String LABEL_VIDEO_SAVE_ENABLE_VIDEO;
extern String LABEL_VIDEO_SAVE_ENABLE_AUDIO;

extern String LABEL_VIDEO_SAVE_RATE_CONTROL;
extern String LABEL_VIDEO_SAVE_RATE_CONTROL_BITRATE;
extern String LABEL_VIDEO_SAVE_RATE_CONTROL_CRF;
extern String LABEL_VIDEO_SAVE_RATE_CONTROL_CQ;
extern String LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CRF;
extern String LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CQP;
extern String VALIDATION_SAVE_CRF_MAIN;
extern String VALIDATION_SAVE_CRF_TO;

extern String LABEL_VIDEO_SAVE_FAILED;

extern String LABEL_PREVIEW_VIDEO;
extern String LABEL_VIDEO_SAVE_PREVIEW;
extern String LABEL_LOADING;
extern String LABEL_VIDEO_PREVIEW_ORIGINAL;
extern String LABEL_VIDEO_PREVIEW_ENCODED;
extern String LABEL_VIDEO_PREVIEW_ESTIMATED_FILE_SIZE;
extern String LABEL_VIDEO_PREVIEW_FAILED;
extern String LABEL_VIDEO_PREVIEW_FAILED_TOOLTIP;

extern String LABEL_TASK_TAB;

extern String LABEL_TASK_TAB_TYPE;
extern String LABEL_TASK_TAB_TYPE_ENCODING;
extern String LABEL_TASK_TAB_TYPE_PANORAMA;
extern String LABEL_TASK_TAB_DESCRPTION;
extern String LABEL_TASK_TAB_PROGRESS;
extern String LABEL_TASK_TAB_TIME_LEFT;


extern String LABEL_SAVE_VIDEO_SUSPENDED;

extern String LABEL_TASK_TAB_ERROR_CANT_LOAD_FILE;
extern String LABEL_TASK_TAB_ERROR_CUSTOM;
extern String LABEL_TASK_TAB_DONE;
extern String LABEL_TASK_TAB_BEGIN;
extern String LABEL_TASK_TAB_CONFIRM_DELETE;
extern String LABEL_YES;
extern String LABEL_NO;

extern String LABEL_ASK_QUIT;
extern String LABEL_VIDEO_PREVIEW_ESTIMATED_TIME;

extern String LABEL_SAVE_VIDEO_ERROR_WRITTING;
extern String LABEL_SAVE_VIDEO_ERROR_MEMORY;
extern String LABEL_SAVE_VIDEO_ERROR_HEADER;
extern String LABEL_SAVE_VIDEO_ERROR_TRAILER;
extern String LABEL_SAVE_VIDEO_ERROR_WRITTING_AUDIO_PACKET;
extern String LABEL_SAVE_VIDEO_ERROR_WRITTING_VIDEO_PACKET;
extern String LABEL_SAVE_VIDEO_ERROR_ENCODING_AUDIO_PACKET;
extern String LABEL_SAVE_VIDEO_ERROR_ENCODING_VIDEO_PACKET;
extern String LABEL_SAVE_VIDEO_ERROR_ENCODING_ALLOC_PICTURE;
extern String LABEL_SAVE_VIDEO_ERROR_OPEN_AUDIO_CODEC;
extern String LABEL_SAVE_VIDEO_PAUSED;
extern String LABEL_SAVE_VIDEO_ERROR_OPEN_VIDEO_CODEC;
extern String LABEL_SOUNDS;
extern String LABEL_PICTURES;
extern String PROGRAM_NAME;
extern String VISIT_WEB_SITE;
void load_locale(String name);
extern String current_locale;
}


#endif // LOCALIZATION_H_INCLUDED
