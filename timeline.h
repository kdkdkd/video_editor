#ifndef TIMELINE_H
#define	TIMELINE_H

#include "juce/juce.h"
#include "movie.h"
#include <vector>

using namespace std;

extern Image black_image;

class Timeline
{
private:

    void RecalculateDuration();
    void RecalculateCurrent();
    bool disposeMovies;
    bool disposeIntervals;

public:

    vector<Movie*> movies;
    vector<Movie*> movies_internal;
    bool loaded;
    double duration;
    double current;
    Movie* Load(String &filename,bool soft);
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

    class Interval
    {
        public:
        Interval(Movie*movie,double absolute_start, Image* preview){selected = false;this->preview = preview; color = usual; this->movie = movie; this->start = 0.0; this->end = movie->duration; this->absolute_start = absolute_start;}
        Interval(Movie*movie,double start,double end,double absolute_start, Image* preview){selected = false;this->preview = preview;color = usual; this->movie = movie; this->start = start; this->end = end; this->absolute_start = absolute_start;}
        Interval(Interval *interval){selected = false;this->preview = interval->preview;color = usual; this->movie = interval->movie; this->start = interval->start; this->end = interval->end; this->absolute_start = interval->absolute_start;}
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
        bool selected;
    };

    int FindNumberIntervalBySecond(double second);

    Interval* current_interval;
    Interval* GetCurrentInterval();


    vector<Interval*> intervals;

    vector<Interval*>* GetAllIntervalsIn(double start,double length);

    // -2 - remove interval
    // -1 - insert interval at interval->absolute_position, interval must be new
    // >0 - move interval to insert_position, interval must be from existing timeline
    Timeline* PreviewInsertIntervalIn(Interval* interval, double insert_position = -1.0);
    void InsertIntervalIn(Timeline::Interval* insert_interval, double insert_position = -1.0);

    Interval  * FindIntervalBySecond(double second);

    void ResetIntervalColor();
    Interval* FindSelected();
    Interval* FindSelectedOrOver();

    void RemoveSpaces();
    void Split();
    bool IsNearMovieBoundary();

    String Render(const Movie::Info & info);
    Image RenderImage(const Movie::Info & info);
    bool IsEmpty();
};


#endif	/* TIMELINE_H */

