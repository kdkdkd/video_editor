#ifndef TIMELINE_H
#define	TIMELINE_H

#include <juce.h>
#include "movie.h"
#include <list>

using namespace std;

class Timeline
{
public:
    list<Movie*> movies;
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
};


#endif	/* TIMELINE_H */

