#ifndef TIMELINE_H
#define	TIMELINE_H

#include "juce/juce.h"
#include "movie.h"
#include <vector>

using namespace std;

class Timeline
{
public:
    vector<Movie*> movies;
    vector<Movie*> movies_internal;
    bool loaded;
    double duration;
    double current;
    bool Load(String &filename);
    void Dispose();
    ~Timeline();
    void SkipFrame();
    void ReadAndDecodeFrame();
    bool GotoRatioAndRead(double ratio,bool decode = true);
    bool GotoSecondAndRead(double dest,bool decode = true);
    bool GoBack(int frames);
    Movie* GetCurrentMovie();
    Image* GetImage();
    Timeline();
    void DecodeFrame();

    class Interval
    {
        public:
        Interval(Movie*movie,double absolute_start){ this->movie = movie; start = 0.0; end = movie->duration; this->absolute_start = absolute_start;}
        Interval(Movie*movie,double start,double end,double absolute_start){ this->movie = movie; start = start; end = end; this->absolute_start = absolute_start;}
        double GetDuration(){return end - start;};
        double start,end,absolute_start;
        Movie*movie;
    };

    vector<Interval*> intervals;
};


#endif	/* TIMELINE_H */

