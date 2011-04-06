#ifndef TIMELINE_H
#define	TIMELINE_H

#include "juce/juce.h"
#include "movie.h"
#include "sound.h"
#include "tasks.h"
#include <vector>

using namespace std;

extern Image black_image;
class task;
class Timeline
{

public:
    class Interval
    {
        public:
        Interval(Movie*movie,double absolute_start, Image* preview)
        {
            selected = false;
            this->preview = preview;
            color = usual;
            this->movie = movie;
            this->start = 0.0;
            this->end = movie->duration;
            this->absolute_start = absolute_start;
            this->sound = 0;
        }
        Interval(Sound*sound,double absolute_start)
        {
            selected = false;
            this->preview = 0;
            color = usual;
            this->movie = 0;
            this->start = 0.0;
            this->end = sound->duration;
            this->absolute_start = absolute_start;
            this->sound = sound;
        }
        Interval(Movie*movie,double start,double end,double absolute_start, Image* preview)
        {
            selected = false;
            this->preview = preview;
            color = usual;
            this->movie = movie;
            this->start = start;
            this->end = end;
            this->absolute_start = absolute_start;
            this->sound = 0;
        }
        Interval(Sound*sound,double start,double end,double absolute_start)
        {
            selected = false;
            this->preview = 0;
            color = usual;
            this->movie = 0;
            this->start = start;
            this->end = end;
            this->absolute_start = absolute_start;
            this->sound = sound;
        }
        Interval(Interval *interval)
        {
            selected = false;
            this->preview = interval->preview;
            color = usual;
            this->movie = interval->movie;
            this->start = interval->start;
            this->end = interval->end;
            this->absolute_start = interval->absolute_start;
            this->sound = interval->sound;
        }
        double GetDuration(){return end - start;};
        double GetAbsoluteEnd(){return end - start + absolute_start;};
        double start,end,absolute_start;
        Image * preview;
        enum IntervalColor
        {
            usual = 0,
            dragg = 1,
            select = 2,
            over = 3

        }color;
        Movie*movie;
        Sound*sound;
        bool selected;
    };

    Interval * SetCurrentInterval(Interval *current, int interval_id);
    Stream* GetCurrentStream(int interval_id);

    void RecalculateDuration();
    void RecalculateCurrent(int interval_id);
    //used on gui
    vector<Movie*> movies;
    vector<Sound*> sounds;
    //list of movies/sounds present on timeline
    vector<Movie*> movies_internal;
    vector<Sound*> sounds_internal;
    bool loaded;
    double duration;
    double current;
    //movie and sound are out parameters
    void Load(String &filename, bool soft, Movie*movie, Sound*sound);
    void Dispose();
    ~Timeline();
    bool SkipFrame(bool jump_to_next = true);
    bool ReadAndDecodeFrame(bool jump_to_next = true);
    double GetFps();

    bool ContinueToNextFrame(bool decode, bool jump_to_next = true);

    bool GotoRatioAndRead(double ratio,bool decode = true);
    bool GotoSecondAndRead(double dest,bool decode = true);
    bool GoBack(int frames);

    Image* GetImage();
    Timeline();
    void DecodeFrame();


    int FindNumberIntervalBySecond(double second, int interval_id);

    Interval* current_interval_video;
    Interval* current_interval_audio;

    Interval* GetCurrentInterval(int interval_id);

    // interval_id == 0
    vector<Interval*> intervals_video;
    // interval_id == 1
    vector<Interval*> intervals_audio;

    vector<Interval*>* GetAllIntervalsIn(double start, double length, int interval_id);

    // -2 - remove interval
    // -1 - insert interval at interval->absolute_position, interval must be new
    // >0 - move interval to insert_position, interval must be from existing timeline
    Timeline* PreviewInsertIntervalIn(Interval* interval, int interval_id, double insert_position = -1.0);
    void InsertIntervalIn(Timeline::Interval* insert_interval, int interval_id, double insert_position = -1.0);

    Interval * FindIntervalBySecond(double second, int interval_id);

    void ResetIntervalColor();
    Interval* FindSelected();
    Interval* FindSelectedOrOver();

    void RemoveSpaces();
    void Split(int interval_id);
    // split everything
    void Split();
    bool IsNearBoundary(int interval_id);

    String Render(const Movie::Info & info, Thread * thread, void (* reportProgress)(task*,double), task* t);
    bool IsEmpty();

    Timeline* CloneIntervals();
    private:


    bool disposeMoviesAndSounds;
    bool disposeIntervals;
    bool GotoSecondAndRead(double dest, bool decode, int interval_id);
    bool ContinueToNextFrame(bool decode, bool jump_to_next, int interval_id);
    void DecodeFrame(int interval_id);
    void RemoveSpaces(int interval_id);
    int RecalculateDuration(int interval_id);
    void ResetIntervalColor(int interval_id);
    Interval* FindSelected(int interval_id);
    Interval* FindSelectedOrOver(int interval_id);
    void CloneIntervals(int interval_id,Timeline* res);
    void CopyOtherIntervalsPointersAndSetCurrent(Timeline* res,int interval_id, bool deep);
};


#endif	/* TIMELINE_H */

