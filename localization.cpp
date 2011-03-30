#include "config.h"
#include "localization.h"


namespace localization
{
EventList AfterLocalizationChnaged;
String MENU_FILE;
String MENU_FILE_OPEN;
String MENU_FILE_SAVE;
String MENU_QUIT;
String MENU_FRAME;
String MENU_JUMP;
String MENU_SAVE_FRAME;
String MENU_RECENT;
String CANT_LOAD_FILE;
String DIALOG_CHOOSE_FILE_TO_OPEN;
String DIALOG_CHOOSE_SCREENSHOT_TO_SAVE;
String LABEL_FRAMES;
String LABEL_TIME;

String LABEL_CHOOSE_JUMP;
String LABEL_TOTAL_TIME;
String LABEL_OK;
String LABEL_CANCEL;
String FILE_SAVED;
String FILE_NOT_SAVED;
String LABEL_PLAY;
String LABEL_PAUSE;
String LABEL_STOP;
String LABEL_NEXT_FRAME;
String LABEL_PREV_FRAME;
String LABEL_NEXT_FIVE_FRAME;
String LABEL_PREV_FIVE_FRAME;
String LABEL_NEXT_SECOND;
String LABEL_PREV_SECOND;
String LABEL_SPECIFIC_TIME;
String LABEL_HOURS;
String LABEL_MINUTES;
String LABEL_SECONDS;
String LABEL_MINI_SECONDS;
String LABEL_INFO;
String LABEL_FILE;
String LABEL_DURATION;
String LABEL_SIZE;
String LABEL_FORMAT;
String LABEL_CODEC;
String LABEL_RESOLUTION;
String LABEL_FPS;
String LABEL_SAMPLE_RATE;
String LABEL_CHANNELS;
String LABEL_LANG;
String LABEL_COMMENT;
String LABEL_BITRATE;
String LABEL_NOT_AVIABLE;
String LABEL_KB_PER_SECOND;
String LABEL_STREAM;
String LABEL_VIDEO;
String LABEL_AUDIO;
String LABEL_SUBTITLES;
String LABEL_MOVIES;
String LABEL_SOUNDS;
String LABEL_PICTURES;
String LABEL_SCALE;
String MENU_VIDEO_PART;
String LABEL_DELETE;
String LABEL_DELETE_VIDEO_PART;
String LABEL_SPLIT;
String LABEL_REMOVE_SPACES;
String LABEL_SAVE_VIDEO;
String LABEL_SAVE;
String LABEL_VIDEO_SAVE_PATH;
String LABEL_VIDEO_SAVE_FORMAT;
String LABEL_VIDEO_SAVE_CODEC;
String LABEL_VIDEO_SAVE_SIZE;
String LABEL_VIDEO_SAVE_QUALITY;
String LABEL_VIDEO_SAVE_COMPRESSION;
String LABEL_VIDEO_SAVE_COMPRESSION_ULTRAFAST;
String LABEL_VIDEO_SAVE_COMPRESSION_SUPERFAST;
String LABEL_VIDEO_SAVE_COMPRESSION_VERYFAST;
String LABEL_VIDEO_SAVE_COMPRESSION_FASTER;
String LABEL_VIDEO_SAVE_COMPRESSION_FAST;
String LABEL_VIDEO_SAVE_COMPRESSION_MEDIUM;
String LABEL_VIDEO_SAVE_COMPRESSION_SLOW;
String LABEL_VIDEO_SAVE_COMPRESSION_SLOWER;
String LABEL_VIDEO_SAVE_COMPRESSION_VERYSLOW;
String LABEL_VIDEO_SAVE_COMPRESSION_PLACEBO;
String LABEL_VIDEO_SAVE_QUALITY_BEST;
String LABEL_VIDEO_SAVE_QUALITY_BETTER;
String LABEL_VIDEO_SAVE_QUALITY_GOOD;
String LABEL_VIDEO_SAVE_QUALITY_NORMAL;
String LABEL_VIDEO_SAVE_QUALITY_BAD;
String LABEL_VIDEO_SAVE_QUALITY_WORSE;
String LABEL_VIDEO_SAVE_QUALITY_WORST;
String LABEL_VIDEO_SAVE_QUALITY_OTHER;
String LABEL_VIDEO_SAVE_SIZE_OTHER;
String LABEL_VIDEO_SAVE_PASS;
String LABEL_VIDEO_SAVE_ADVANCED_MODE;
String LABEL_VIDEO_SAVE_BITRATE;
String LABEL_VIDEO_SAVE_FPS;
String LABEL_VIDEO_SAVE_RESOLUTION;
String LABEL_VIDEO_SAVE_AUTO;
String LABEL_VIDEO_GOP;
String LABEL_VIDEO_SAVE_PASS_ONE;
String LABEL_VIDEO_SAVE_PASS_TWO;
String LABEL_AUDIO_SAVE_CODEC;
String LABEL_AUDIO_SAVE_BITRATE;
String LABEL_AUDIO_SAVE_SAMPLE_RATE;
String LABEL_AUDIO_SAVE_CHANNELS;
String MENU_HEADER_PC;
String MENU_HEADER_WEB;
String MENU_HEADER_CELL_PHONE;
String MENU_HEADER_AUDIO;
String MENU_HEADER_IPOD;
String VALIDATION_SAVE_FPS;
String VALIDATION_SAVE_WIDTH;
String VALIDATION_SAVE_HEIGHT;
String VALIDATION_SAVE_BITRATE;
String VALIDATION_SAVE_CRF_MAIN;
String VALIDATION_SAVE_CRF_TO;
String LABEL_VIDEO_SAVE_ENABLE_VIDEO;
String LABEL_VIDEO_SAVE_ENABLE_AUDIO;
String LABEL_VIDEO_SAVE_RATE_CONTROL;
String LABEL_VIDEO_SAVE_RATE_CONTROL_BITRATE;
String LABEL_VIDEO_SAVE_RATE_CONTROL_CRF;
String LABEL_VIDEO_SAVE_RATE_CONTROL_CQ;
String LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CRF;
String LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CQP;
String LABEL_VIDEO_SAVE_FAILED;
String LABEL_PREVIEW_VIDEO;
String LABEL_VIDEO_SAVE_PREVIEW;
String LABEL_LOADING;
String LABEL_VIDEO_PREVIEW_ORIGINAL;
String LABEL_VIDEO_PREVIEW_ENCODED;
String LABEL_VIDEO_PREVIEW_ESTIMATED_FILE_SIZE;
String LABEL_VIDEO_PREVIEW_ESTIMATED_TIME;
String LABEL_VIDEO_PREVIEW_FAILED;
String LABEL_VIDEO_PREVIEW_FAILED_TOOLTIP;
String MENU_SHOW_TASKS;
String LABEL_TASK_TAB;
String LABEL_TASK_TAB_TYPE;
String LABEL_TASK_TAB_TYPE_ENCODING;
String LABEL_TASK_TAB_TYPE_PANORAMA;
String LABEL_TASK_TAB_DESCRPTION;
String LABEL_TASK_TAB_TIME_LEFT;
String LABEL_TASK_TAB_PROGRESS;
String LABEL_SAVE_VIDEO_SUSPENDED;
String LABEL_TASK_TAB_ERROR_CANT_LOAD_FILE;
String LABEL_TASK_TAB_ERROR_CUSTOM;
String LABEL_TASK_TAB_DONE;
String LABEL_TASK_TAB_BEGIN;
String LABEL_TASK_TAB_CONFIRM_DELETE;
String LABEL_YES;
String LABEL_NO;
String LABEL_ASK_QUIT;
String LABEL_SAVE_VIDEO_ERROR_WRITTING;
String LABEL_SAVE_VIDEO_ERROR_MEMORY;
String LABEL_SAVE_VIDEO_ERROR_HEADER;
String LABEL_SAVE_VIDEO_ERROR_TRAILER;
String LABEL_SAVE_VIDEO_ERROR_WRITTING_AUDIO_PACKET;
String LABEL_SAVE_VIDEO_ERROR_WRITTING_VIDEO_PACKET;
String LABEL_SAVE_VIDEO_ERROR_ENCODING_AUDIO_PACKET;
String LABEL_SAVE_VIDEO_ERROR_ENCODING_VIDEO_PACKET;
String LABEL_SAVE_VIDEO_ERROR_ENCODING_ALLOC_PICTURE;
String LABEL_SAVE_VIDEO_ERROR_OPEN_AUDIO_CODEC;
String LABEL_SAVE_VIDEO_ERROR_OPEN_VIDEO_CODEC;
String LABEL_SAVE_VIDEO_PAUSED;
String current_locale;
    void load_locale(String name)
    {
            if(current_locale == name)
                return;
            if(name!="en" && current_locale!="en")
                load_locale("en");

            if(!File("../localization/" + name).isDirectory())
            {
                return;
            }
            PropertiesFile video_editor_localization(File("../localization/" + name + "/translate.xml"),0,PropertiesFile::storeAsXML,0);

            if(video_editor_localization.containsKey("MENU_FILE"))
                MENU_FILE = video_editor_localization.getValue("MENU_FILE");
            if(video_editor_localization.containsKey("MENU_FILE_OPEN"))
                MENU_FILE_OPEN = video_editor_localization.getValue("MENU_FILE_OPEN");
            if(video_editor_localization.containsKey("MENU_FILE_SAVE"))
                MENU_FILE_SAVE = video_editor_localization.getValue("MENU_FILE_SAVE");
            if(video_editor_localization.containsKey("MENU_QUIT"))
                MENU_QUIT = video_editor_localization.getValue("MENU_QUIT");
            if(video_editor_localization.containsKey("MENU_FRAME"))
                MENU_FRAME = video_editor_localization.getValue("MENU_FRAME");
            if(video_editor_localization.containsKey("MENU_JUMP"))
                MENU_JUMP = video_editor_localization.getValue("MENU_JUMP");
            if(video_editor_localization.containsKey("MENU_SAVE_FRAME"))
                MENU_SAVE_FRAME = video_editor_localization.getValue("MENU_SAVE_FRAME");
            if(video_editor_localization.containsKey("MENU_RECENT"))
                MENU_RECENT = video_editor_localization.getValue("MENU_RECENT");
            if(video_editor_localization.containsKey("CANT_LOAD_FILE"))
                CANT_LOAD_FILE = video_editor_localization.getValue("CANT_LOAD_FILE");
            if(video_editor_localization.containsKey("DIALOG_CHOOSE_FILE_TO_OPEN"))
                DIALOG_CHOOSE_FILE_TO_OPEN = video_editor_localization.getValue("DIALOG_CHOOSE_FILE_TO_OPEN");
            if(video_editor_localization.containsKey("DIALOG_CHOOSE_SCREENSHOT_TO_SAVE"))
                DIALOG_CHOOSE_SCREENSHOT_TO_SAVE = video_editor_localization.getValue("DIALOG_CHOOSE_SCREENSHOT_TO_SAVE");
            if(video_editor_localization.containsKey("LABEL_FRAMES"))
                LABEL_FRAMES = video_editor_localization.getValue("LABEL_FRAMES");
            if(video_editor_localization.containsKey("LABEL_TIME"))
                LABEL_TIME = video_editor_localization.getValue("LABEL_TIME");
            if(video_editor_localization.containsKey("LABEL_CHOOSE_JUMP"))
                LABEL_CHOOSE_JUMP = video_editor_localization.getValue("LABEL_CHOOSE_JUMP");
            if(video_editor_localization.containsKey("LABEL_TOTAL_TIME"))
                LABEL_TOTAL_TIME = video_editor_localization.getValue("LABEL_TOTAL_TIME");
            if(video_editor_localization.containsKey("LABEL_OK"))
                LABEL_OK = video_editor_localization.getValue("LABEL_OK");
            if(video_editor_localization.containsKey("LABEL_CANCEL"))
                LABEL_CANCEL = video_editor_localization.getValue("LABEL_CANCEL");
            if(video_editor_localization.containsKey("FILE_SAVED"))
                FILE_SAVED = video_editor_localization.getValue("FILE_SAVED");
            if(video_editor_localization.containsKey("FILE_NOT_SAVED"))
                FILE_NOT_SAVED = video_editor_localization.getValue("FILE_NOT_SAVED");
            if(video_editor_localization.containsKey("LABEL_PLAY"))
                LABEL_PLAY = video_editor_localization.getValue("LABEL_PLAY");
            if(video_editor_localization.containsKey("LABEL_PAUSE"))
                LABEL_PAUSE = video_editor_localization.getValue("LABEL_PAUSE");
            if(video_editor_localization.containsKey("LABEL_STOP"))
                LABEL_STOP = video_editor_localization.getValue("LABEL_STOP");
            if(video_editor_localization.containsKey("LABEL_NEXT_FRAME"))
                LABEL_NEXT_FRAME = video_editor_localization.getValue("LABEL_NEXT_FRAME");
            if(video_editor_localization.containsKey("LABEL_PREV_FRAME"))
                LABEL_PREV_FRAME = video_editor_localization.getValue("LABEL_PREV_FRAME");
            if(video_editor_localization.containsKey("LABEL_NEXT_FIVE_FRAME"))
                LABEL_NEXT_FIVE_FRAME = video_editor_localization.getValue("LABEL_NEXT_FIVE_FRAME");
            if(video_editor_localization.containsKey("LABEL_PREV_FIVE_FRAME"))
                LABEL_PREV_FIVE_FRAME = video_editor_localization.getValue("LABEL_PREV_FIVE_FRAME");
            if(video_editor_localization.containsKey("LABEL_NEXT_SECOND"))
                LABEL_NEXT_SECOND = video_editor_localization.getValue("LABEL_NEXT_SECOND");
            if(video_editor_localization.containsKey("LABEL_PREV_SECOND"))
                LABEL_PREV_SECOND = video_editor_localization.getValue("LABEL_PREV_SECOND");
            if(video_editor_localization.containsKey("LABEL_SPECIFIC_TIME"))
                LABEL_SPECIFIC_TIME = video_editor_localization.getValue("LABEL_SPECIFIC_TIME");
            if(video_editor_localization.containsKey("LABEL_HOURS"))
                LABEL_HOURS = video_editor_localization.getValue("LABEL_HOURS");
            if(video_editor_localization.containsKey("LABEL_MINUTES"))
                LABEL_MINUTES = video_editor_localization.getValue("LABEL_MINUTES");
            if(video_editor_localization.containsKey("LABEL_SECONDS"))
                LABEL_SECONDS = video_editor_localization.getValue("LABEL_SECONDS");
            if(video_editor_localization.containsKey("LABEL_MINI_SECONDS"))
                LABEL_MINI_SECONDS = video_editor_localization.getValue("LABEL_MINI_SECONDS");
            if(video_editor_localization.containsKey("LABEL_INFO"))
                LABEL_INFO = video_editor_localization.getValue("LABEL_INFO");
            if(video_editor_localization.containsKey("LABEL_FILE"))
                LABEL_FILE = video_editor_localization.getValue("LABEL_FILE");
            if(video_editor_localization.containsKey("LABEL_DURATION"))
                LABEL_DURATION = video_editor_localization.getValue("LABEL_DURATION");
            if(video_editor_localization.containsKey("LABEL_SIZE"))
                LABEL_SIZE = video_editor_localization.getValue("LABEL_SIZE");
            if(video_editor_localization.containsKey("LABEL_FORMAT"))
                LABEL_FORMAT = video_editor_localization.getValue("LABEL_FORMAT");
            if(video_editor_localization.containsKey("LABEL_CODEC"))
                LABEL_CODEC = video_editor_localization.getValue("LABEL_CODEC");
            if(video_editor_localization.containsKey("LABEL_RESOLUTION"))
                LABEL_RESOLUTION = video_editor_localization.getValue("LABEL_RESOLUTION");
            if(video_editor_localization.containsKey("LABEL_FPS"))
                LABEL_FPS = video_editor_localization.getValue("LABEL_FPS");
            if(video_editor_localization.containsKey("LABEL_SAMPLE_RATE"))
                LABEL_SAMPLE_RATE = video_editor_localization.getValue("LABEL_SAMPLE_RATE");
            if(video_editor_localization.containsKey("LABEL_CHANNELS"))
                LABEL_CHANNELS = video_editor_localization.getValue("LABEL_CHANNELS");
            if(video_editor_localization.containsKey("LABEL_LANG"))
                LABEL_LANG = video_editor_localization.getValue("LABEL_LANG");
            if(video_editor_localization.containsKey("LABEL_COMMENT"))
                LABEL_COMMENT = video_editor_localization.getValue("LABEL_COMMENT");
            if(video_editor_localization.containsKey("LABEL_BITRATE"))
                LABEL_BITRATE = video_editor_localization.getValue("LABEL_BITRATE");
            if(video_editor_localization.containsKey("LABEL_NOT_AVIABLE"))
                LABEL_NOT_AVIABLE = video_editor_localization.getValue("LABEL_NOT_AVIABLE");
            if(video_editor_localization.containsKey("LABEL_KB_PER_SECOND"))
                LABEL_KB_PER_SECOND = video_editor_localization.getValue("LABEL_KB_PER_SECOND");
            if(video_editor_localization.containsKey("LABEL_STREAM"))
                LABEL_STREAM = video_editor_localization.getValue("LABEL_STREAM");
            if(video_editor_localization.containsKey("LABEL_VIDEO"))
                LABEL_VIDEO = video_editor_localization.getValue("LABEL_VIDEO");
            if(video_editor_localization.containsKey("LABEL_AUDIO"))
                LABEL_AUDIO = video_editor_localization.getValue("LABEL_AUDIO");
            if(video_editor_localization.containsKey("LABEL_SUBTITLES"))
                LABEL_SUBTITLES = video_editor_localization.getValue("LABEL_SUBTITLES");
            if(video_editor_localization.containsKey("LABEL_MOVIES"))
                LABEL_MOVIES = video_editor_localization.getValue("LABEL_MOVIES");
            if(video_editor_localization.containsKey("LABEL_SOUNDS"))
                LABEL_SOUNDS = video_editor_localization.getValue("LABEL_SOUNDS");
            if(video_editor_localization.containsKey("LABEL_PICTURES"))
                LABEL_PICTURES = video_editor_localization.getValue("LABEL_PICTURES");
            if(video_editor_localization.containsKey("LABEL_SCALE"))
                LABEL_SCALE = video_editor_localization.getValue("LABEL_SCALE");
            if(video_editor_localization.containsKey("MENU_VIDEO_PART"))
                MENU_VIDEO_PART = video_editor_localization.getValue("MENU_VIDEO_PART");
            if(video_editor_localization.containsKey("LABEL_DELETE"))
                LABEL_DELETE = video_editor_localization.getValue("LABEL_DELETE");
            if(video_editor_localization.containsKey("LABEL_DELETE_VIDEO_PART"))
                LABEL_DELETE_VIDEO_PART = video_editor_localization.getValue("LABEL_DELETE_VIDEO_PART");
            if(video_editor_localization.containsKey("LABEL_SPLIT"))
                LABEL_SPLIT = video_editor_localization.getValue("LABEL_SPLIT");
            if(video_editor_localization.containsKey("LABEL_REMOVE_SPACES"))
                LABEL_REMOVE_SPACES = video_editor_localization.getValue("LABEL_REMOVE_SPACES");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO"))
                LABEL_SAVE_VIDEO = video_editor_localization.getValue("LABEL_SAVE_VIDEO");
            if(video_editor_localization.containsKey("LABEL_SAVE"))
                LABEL_SAVE = video_editor_localization.getValue("LABEL_SAVE");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_PATH"))
                LABEL_VIDEO_SAVE_PATH = video_editor_localization.getValue("LABEL_VIDEO_SAVE_PATH");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_FORMAT"))
                LABEL_VIDEO_SAVE_FORMAT = video_editor_localization.getValue("LABEL_VIDEO_SAVE_FORMAT");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_CODEC"))
                LABEL_VIDEO_SAVE_CODEC = video_editor_localization.getValue("LABEL_VIDEO_SAVE_CODEC");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_SIZE"))
                LABEL_VIDEO_SAVE_SIZE = video_editor_localization.getValue("LABEL_VIDEO_SAVE_SIZE");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_QUALITY"))
                LABEL_VIDEO_SAVE_QUALITY = video_editor_localization.getValue("LABEL_VIDEO_SAVE_QUALITY");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_COMPRESSION"))
                LABEL_VIDEO_SAVE_COMPRESSION = video_editor_localization.getValue("LABEL_VIDEO_SAVE_COMPRESSION");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_COMPRESSION_ULTRAFAST"))
                LABEL_VIDEO_SAVE_COMPRESSION_ULTRAFAST = video_editor_localization.getValue("LABEL_VIDEO_SAVE_COMPRESSION_ULTRAFAST");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_COMPRESSION_SUPERFAST"))
                LABEL_VIDEO_SAVE_COMPRESSION_SUPERFAST = video_editor_localization.getValue("LABEL_VIDEO_SAVE_COMPRESSION_SUPERFAST");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_COMPRESSION_VERYFAST"))
                LABEL_VIDEO_SAVE_COMPRESSION_VERYFAST = video_editor_localization.getValue("LABEL_VIDEO_SAVE_COMPRESSION_VERYFAST");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_COMPRESSION_FASTER"))
                LABEL_VIDEO_SAVE_COMPRESSION_FASTER = video_editor_localization.getValue("LABEL_VIDEO_SAVE_COMPRESSION_FASTER");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_COMPRESSION_FAST"))
                LABEL_VIDEO_SAVE_COMPRESSION_FAST = video_editor_localization.getValue("LABEL_VIDEO_SAVE_COMPRESSION_FAST");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_COMPRESSION_MEDIUM"))
                LABEL_VIDEO_SAVE_COMPRESSION_MEDIUM = video_editor_localization.getValue("LABEL_VIDEO_SAVE_COMPRESSION_MEDIUM");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_COMPRESSION_SLOW"))
                LABEL_VIDEO_SAVE_COMPRESSION_SLOW = video_editor_localization.getValue("LABEL_VIDEO_SAVE_COMPRESSION_SLOW");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_COMPRESSION_SLOWER"))
                LABEL_VIDEO_SAVE_COMPRESSION_SLOWER = video_editor_localization.getValue("LABEL_VIDEO_SAVE_COMPRESSION_SLOWER");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_COMPRESSION_VERYSLOW"))
                LABEL_VIDEO_SAVE_COMPRESSION_VERYSLOW = video_editor_localization.getValue("LABEL_VIDEO_SAVE_COMPRESSION_VERYSLOW");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_COMPRESSION_PLACEBO"))
                LABEL_VIDEO_SAVE_COMPRESSION_PLACEBO = video_editor_localization.getValue("LABEL_VIDEO_SAVE_COMPRESSION_PLACEBO");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_QUALITY_BEST"))
                LABEL_VIDEO_SAVE_QUALITY_BEST = video_editor_localization.getValue("LABEL_VIDEO_SAVE_QUALITY_BEST");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_QUALITY_BETTER"))
                LABEL_VIDEO_SAVE_QUALITY_BETTER = video_editor_localization.getValue("LABEL_VIDEO_SAVE_QUALITY_BETTER");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_QUALITY_GOOD"))
                LABEL_VIDEO_SAVE_QUALITY_GOOD = video_editor_localization.getValue("LABEL_VIDEO_SAVE_QUALITY_GOOD");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_QUALITY_NORMAL"))
                LABEL_VIDEO_SAVE_QUALITY_NORMAL = video_editor_localization.getValue("LABEL_VIDEO_SAVE_QUALITY_NORMAL");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_QUALITY_BAD"))
                LABEL_VIDEO_SAVE_QUALITY_BAD = video_editor_localization.getValue("LABEL_VIDEO_SAVE_QUALITY_BAD");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_QUALITY_WORSE"))
                LABEL_VIDEO_SAVE_QUALITY_WORSE = video_editor_localization.getValue("LABEL_VIDEO_SAVE_QUALITY_WORSE");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_QUALITY_WORST"))
                LABEL_VIDEO_SAVE_QUALITY_WORST = video_editor_localization.getValue("LABEL_VIDEO_SAVE_QUALITY_WORST");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_QUALITY_OTHER"))
                LABEL_VIDEO_SAVE_QUALITY_OTHER = video_editor_localization.getValue("LABEL_VIDEO_SAVE_QUALITY_OTHER");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_SIZE_OTHER"))
                LABEL_VIDEO_SAVE_SIZE_OTHER = video_editor_localization.getValue("LABEL_VIDEO_SAVE_SIZE_OTHER");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_PASS"))
                LABEL_VIDEO_SAVE_PASS = video_editor_localization.getValue("LABEL_VIDEO_SAVE_PASS");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_ADVANCED_MODE"))
                LABEL_VIDEO_SAVE_ADVANCED_MODE = video_editor_localization.getValue("LABEL_VIDEO_SAVE_ADVANCED_MODE");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_BITRATE"))
                LABEL_VIDEO_SAVE_BITRATE = video_editor_localization.getValue("LABEL_VIDEO_SAVE_BITRATE");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_FPS"))
                LABEL_VIDEO_SAVE_FPS = video_editor_localization.getValue("LABEL_VIDEO_SAVE_FPS");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_RESOLUTION"))
                LABEL_VIDEO_SAVE_RESOLUTION = video_editor_localization.getValue("LABEL_VIDEO_SAVE_RESOLUTION");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_AUTO"))
                LABEL_VIDEO_SAVE_AUTO = video_editor_localization.getValue("LABEL_VIDEO_SAVE_AUTO");
            if(video_editor_localization.containsKey("LABEL_VIDEO_GOP"))
                LABEL_VIDEO_GOP = video_editor_localization.getValue("LABEL_VIDEO_GOP");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_PASS_ONE"))
                LABEL_VIDEO_SAVE_PASS_ONE = video_editor_localization.getValue("LABEL_VIDEO_SAVE_PASS_ONE");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_PASS_TWO"))
                LABEL_VIDEO_SAVE_PASS_TWO = video_editor_localization.getValue("LABEL_VIDEO_SAVE_PASS_TWO");
            if(video_editor_localization.containsKey("LABEL_AUDIO_SAVE_CODEC"))
                LABEL_AUDIO_SAVE_CODEC = video_editor_localization.getValue("LABEL_AUDIO_SAVE_CODEC");
            if(video_editor_localization.containsKey("LABEL_AUDIO_SAVE_BITRATE"))
                LABEL_AUDIO_SAVE_BITRATE = video_editor_localization.getValue("LABEL_AUDIO_SAVE_BITRATE");
            if(video_editor_localization.containsKey("LABEL_AUDIO_SAVE_SAMPLE_RATE"))
                LABEL_AUDIO_SAVE_SAMPLE_RATE = video_editor_localization.getValue("LABEL_AUDIO_SAVE_SAMPLE_RATE");
            if(video_editor_localization.containsKey("LABEL_AUDIO_SAVE_CHANNELS"))
                LABEL_AUDIO_SAVE_CHANNELS = video_editor_localization.getValue("LABEL_AUDIO_SAVE_CHANNELS");
            if(video_editor_localization.containsKey("MENU_HEADER_PC"))
                MENU_HEADER_PC = video_editor_localization.getValue("MENU_HEADER_PC");
            if(video_editor_localization.containsKey("MENU_HEADER_WEB"))
                MENU_HEADER_WEB = video_editor_localization.getValue("MENU_HEADER_WEB");
            if(video_editor_localization.containsKey("MENU_HEADER_CELL_PHONE"))
                MENU_HEADER_CELL_PHONE = video_editor_localization.getValue("MENU_HEADER_CELL_PHONE");
            if(video_editor_localization.containsKey("MENU_HEADER_AUDIO"))
                MENU_HEADER_AUDIO = video_editor_localization.getValue("MENU_HEADER_AUDIO");
            if(video_editor_localization.containsKey("MENU_HEADER_IPOD"))
                MENU_HEADER_IPOD = video_editor_localization.getValue("MENU_HEADER_IPOD");
            if(video_editor_localization.containsKey("VALIDATION_SAVE_FPS"))
                VALIDATION_SAVE_FPS = video_editor_localization.getValue("VALIDATION_SAVE_FPS");
            if(video_editor_localization.containsKey("VALIDATION_SAVE_WIDTH"))
                VALIDATION_SAVE_WIDTH = video_editor_localization.getValue("VALIDATION_SAVE_WIDTH");
            if(video_editor_localization.containsKey("VALIDATION_SAVE_HEIGHT"))
                VALIDATION_SAVE_HEIGHT = video_editor_localization.getValue("VALIDATION_SAVE_HEIGHT");
            if(video_editor_localization.containsKey("VALIDATION_SAVE_BITRATE"))
                VALIDATION_SAVE_BITRATE = video_editor_localization.getValue("VALIDATION_SAVE_BITRATE");
            if(video_editor_localization.containsKey("VALIDATION_SAVE_CRF_MAIN"))
                VALIDATION_SAVE_CRF_MAIN = video_editor_localization.getValue("VALIDATION_SAVE_CRF_MAIN");
            if(video_editor_localization.containsKey("VALIDATION_SAVE_CRF_TO"))
                VALIDATION_SAVE_CRF_TO = video_editor_localization.getValue("VALIDATION_SAVE_CRF_TO");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_ENABLE_VIDEO"))
                LABEL_VIDEO_SAVE_ENABLE_VIDEO = video_editor_localization.getValue("LABEL_VIDEO_SAVE_ENABLE_VIDEO");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_ENABLE_AUDIO"))
                LABEL_VIDEO_SAVE_ENABLE_AUDIO = video_editor_localization.getValue("LABEL_VIDEO_SAVE_ENABLE_AUDIO");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_RATE_CONTROL"))
                LABEL_VIDEO_SAVE_RATE_CONTROL = video_editor_localization.getValue("LABEL_VIDEO_SAVE_RATE_CONTROL");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_RATE_CONTROL_BITRATE"))
                LABEL_VIDEO_SAVE_RATE_CONTROL_BITRATE = video_editor_localization.getValue("LABEL_VIDEO_SAVE_RATE_CONTROL_BITRATE");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_RATE_CONTROL_CRF"))
                LABEL_VIDEO_SAVE_RATE_CONTROL_CRF = video_editor_localization.getValue("LABEL_VIDEO_SAVE_RATE_CONTROL_CRF");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_RATE_CONTROL_CQ"))
                LABEL_VIDEO_SAVE_RATE_CONTROL_CQ = video_editor_localization.getValue("LABEL_VIDEO_SAVE_RATE_CONTROL_CQ");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CRF"))
                LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CRF = video_editor_localization.getValue("LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CRF");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CQP"))
                LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CQP = video_editor_localization.getValue("LABEL_VIDEO_SAVE_RATE_CONTROL_COEFF_CQP");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_FAILED"))
                LABEL_VIDEO_SAVE_FAILED = video_editor_localization.getValue("LABEL_VIDEO_SAVE_FAILED");
            if(video_editor_localization.containsKey("LABEL_PREVIEW_VIDEO"))
                LABEL_PREVIEW_VIDEO = video_editor_localization.getValue("LABEL_PREVIEW_VIDEO");
            if(video_editor_localization.containsKey("LABEL_VIDEO_SAVE_PREVIEW"))
                LABEL_VIDEO_SAVE_PREVIEW = video_editor_localization.getValue("LABEL_VIDEO_SAVE_PREVIEW");
            if(video_editor_localization.containsKey("LABEL_LOADING"))
                LABEL_LOADING = video_editor_localization.getValue("LABEL_LOADING");
            if(video_editor_localization.containsKey("LABEL_VIDEO_PREVIEW_ORIGINAL"))
                LABEL_VIDEO_PREVIEW_ORIGINAL = video_editor_localization.getValue("LABEL_VIDEO_PREVIEW_ORIGINAL");
            if(video_editor_localization.containsKey("LABEL_VIDEO_PREVIEW_ENCODED"))
                LABEL_VIDEO_PREVIEW_ENCODED = video_editor_localization.getValue("LABEL_VIDEO_PREVIEW_ENCODED");
            if(video_editor_localization.containsKey("LABEL_VIDEO_PREVIEW_ESTIMATED_FILE_SIZE"))
                LABEL_VIDEO_PREVIEW_ESTIMATED_FILE_SIZE = video_editor_localization.getValue("LABEL_VIDEO_PREVIEW_ESTIMATED_FILE_SIZE");
            if(video_editor_localization.containsKey("LABEL_VIDEO_PREVIEW_ESTIMATED_TIME"))
                LABEL_VIDEO_PREVIEW_ESTIMATED_TIME = video_editor_localization.getValue("LABEL_VIDEO_PREVIEW_ESTIMATED_TIME");
            if(video_editor_localization.containsKey("LABEL_VIDEO_PREVIEW_FAILED"))
                LABEL_VIDEO_PREVIEW_FAILED = video_editor_localization.getValue("LABEL_VIDEO_PREVIEW_FAILED");
            if(video_editor_localization.containsKey("LABEL_VIDEO_PREVIEW_FAILED_TOOLTIP"))
                LABEL_VIDEO_PREVIEW_FAILED_TOOLTIP = video_editor_localization.getValue("LABEL_VIDEO_PREVIEW_FAILED_TOOLTIP");
            if(video_editor_localization.containsKey("MENU_SHOW_TASKS"))
                MENU_SHOW_TASKS = video_editor_localization.getValue("MENU_SHOW_TASKS");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB"))
                LABEL_TASK_TAB = video_editor_localization.getValue("LABEL_TASK_TAB");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB_TYPE"))
                LABEL_TASK_TAB_TYPE = video_editor_localization.getValue("LABEL_TASK_TAB_TYPE");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB_TYPE_ENCODING"))
                LABEL_TASK_TAB_TYPE_ENCODING = video_editor_localization.getValue("LABEL_TASK_TAB_TYPE_ENCODING");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB_TYPE_PANORAMA"))
                LABEL_TASK_TAB_TYPE_PANORAMA = video_editor_localization.getValue("LABEL_TASK_TAB_TYPE_PANORAMA");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB_DESCRPTION"))
                LABEL_TASK_TAB_DESCRPTION = video_editor_localization.getValue("LABEL_TASK_TAB_DESCRPTION");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB_TIME_LEFT"))
                LABEL_TASK_TAB_TIME_LEFT = video_editor_localization.getValue("LABEL_TASK_TAB_TIME_LEFT");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB_PROGRESS"))
                LABEL_TASK_TAB_PROGRESS = video_editor_localization.getValue("LABEL_TASK_TAB_PROGRESS");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_SUSPENDED"))
                LABEL_SAVE_VIDEO_SUSPENDED = video_editor_localization.getValue("LABEL_SAVE_VIDEO_SUSPENDED");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB_ERROR_CANT_LOAD_FILE"))
                LABEL_TASK_TAB_ERROR_CANT_LOAD_FILE = video_editor_localization.getValue("LABEL_TASK_TAB_ERROR_CANT_LOAD_FILE");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB_ERROR_CUSTOM"))
                LABEL_TASK_TAB_ERROR_CUSTOM = video_editor_localization.getValue("LABEL_TASK_TAB_ERROR_CUSTOM");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB_DONE"))
                LABEL_TASK_TAB_DONE = video_editor_localization.getValue("LABEL_TASK_TAB_DONE");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB_BEGIN"))
                LABEL_TASK_TAB_BEGIN = video_editor_localization.getValue("LABEL_TASK_TAB_BEGIN");
            if(video_editor_localization.containsKey("LABEL_TASK_TAB_CONFIRM_DELETE"))
                LABEL_TASK_TAB_CONFIRM_DELETE = video_editor_localization.getValue("LABEL_TASK_TAB_CONFIRM_DELETE");
            if(video_editor_localization.containsKey("LABEL_YES"))
                LABEL_YES = video_editor_localization.getValue("LABEL_YES");
            if(video_editor_localization.containsKey("LABEL_NO"))
                LABEL_NO = video_editor_localization.getValue("LABEL_NO");
            if(video_editor_localization.containsKey("LABEL_ASK_QUIT"))
                LABEL_ASK_QUIT = video_editor_localization.getValue("LABEL_ASK_QUIT");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_ERROR_WRITTING"))
                LABEL_SAVE_VIDEO_ERROR_WRITTING = video_editor_localization.getValue("LABEL_SAVE_VIDEO_ERROR_WRITTING");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_ERROR_MEMORY"))
                LABEL_SAVE_VIDEO_ERROR_MEMORY = video_editor_localization.getValue("LABEL_SAVE_VIDEO_ERROR_MEMORY");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_ERROR_HEADER"))
                LABEL_SAVE_VIDEO_ERROR_HEADER = video_editor_localization.getValue("LABEL_SAVE_VIDEO_ERROR_HEADER");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_ERROR_TRAILER"))
                LABEL_SAVE_VIDEO_ERROR_TRAILER = video_editor_localization.getValue("LABEL_SAVE_VIDEO_ERROR_TRAILER");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_ERROR_WRITTING_AUDIO_PACKET"))
                LABEL_SAVE_VIDEO_ERROR_WRITTING_AUDIO_PACKET = video_editor_localization.getValue("LABEL_SAVE_VIDEO_ERROR_WRITTING_AUDIO_PACKET");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_ERROR_WRITTING_VIDEO_PACKET"))
                LABEL_SAVE_VIDEO_ERROR_WRITTING_VIDEO_PACKET = video_editor_localization.getValue("LABEL_SAVE_VIDEO_ERROR_WRITTING_VIDEO_PACKET");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_ERROR_ENCODING_AUDIO_PACKET"))
                LABEL_SAVE_VIDEO_ERROR_ENCODING_AUDIO_PACKET = video_editor_localization.getValue("LABEL_SAVE_VIDEO_ERROR_ENCODING_AUDIO_PACKET");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_ERROR_ENCODING_VIDEO_PACKET"))
                LABEL_SAVE_VIDEO_ERROR_ENCODING_VIDEO_PACKET = video_editor_localization.getValue("LABEL_SAVE_VIDEO_ERROR_ENCODING_VIDEO_PACKET");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_ERROR_ENCODING_ALLOC_PICTURE"))
                LABEL_SAVE_VIDEO_ERROR_ENCODING_ALLOC_PICTURE = video_editor_localization.getValue("LABEL_SAVE_VIDEO_ERROR_ENCODING_ALLOC_PICTURE");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_ERROR_OPEN_AUDIO_CODEC"))
                LABEL_SAVE_VIDEO_ERROR_OPEN_AUDIO_CODEC = video_editor_localization.getValue("LABEL_SAVE_VIDEO_ERROR_OPEN_AUDIO_CODEC");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_ERROR_OPEN_VIDEO_CODEC"))
                LABEL_SAVE_VIDEO_ERROR_OPEN_VIDEO_CODEC = video_editor_localization.getValue("LABEL_SAVE_VIDEO_ERROR_OPEN_VIDEO_CODEC");
            if(video_editor_localization.containsKey("LABEL_SAVE_VIDEO_PAUSED"))
                LABEL_SAVE_VIDEO_PAUSED = video_editor_localization.getValue("LABEL_SAVE_VIDEO_PAUSED");



            current_locale = name;
            CallEventList(AfterLocalizationChnaged);
    }

}
