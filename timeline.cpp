#include "timeline.h"
#include "movie.h"
#include "localization.h"
#include "toolbox.h"
using namespace localization;
Timeline::Timeline()
{
    loaded = false;
    duration = 0.;
    current = 0.;
};

vector<Timeline::Interval*> Timeline::GetAllIntervalsIn(double start,double length)
{
    vector<Interval*> res;
    for(vector<Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
    {
        double end = start + length,start1=(*it)->absolute_start,end1 = (*it)->GetAbsoluteEnd();
        if(start1<=end&&end1>=start)
        {
            res.push_back(*it);
        }
    }
    return res;
}

bool Timeline::Load(String &filename)
{
    Movie *movie = new Movie();
    movie->Load(filename);
    bool loaded_local = movie->loaded;
    if(loaded_local)
    {
        movies.push_back(movie);

        if(!loaded)
            intervals.push_back(new Interval(movie,0));

        movies_internal.push_back(movie);
        duration = movie->duration;
        current = movie->current;
    }
    else
    {
        delete movie;
    }

    loaded = loaded || loaded_local;
    return loaded_local;

}

Movie* Timeline::GetCurrentMovie()
{
    return movies_internal.back();
}

Image* Timeline::GetImage()
{
    return GetCurrentMovie()->image;
}


void Timeline::Dispose()
{
    if(loaded)
    {
        for(vector<Movie*>::iterator it = movies_internal.begin(); it!=movies_internal.end(); it++)
        {
            delete *it;
        }

        for(vector<Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
        {
            delete *it;
        }
    }
    loaded = false;
}

Timeline::~Timeline()
{
    Dispose();
}

bool Timeline::GotoRatioAndRead(double ratio,bool decode)
{
    return GotoSecondAndRead(ratio * duration,decode);
}


bool Timeline::GotoSecondAndRead(double dest,bool decode)
{
    bool res = GetCurrentMovie()->GotoSecondAndRead(dest,decode);
    current = GetCurrentMovie()->current;
    return res;
}

void Timeline::ReadAndDecodeFrame()
{
    GetCurrentMovie()->ReadAndDecodeFrame();
    current = GetCurrentMovie()->current;
}

void Timeline::SkipFrame()
{
    GetCurrentMovie()->SkipFrame();
    current = GetCurrentMovie()->current;
}

bool Timeline::GoBack(int frames)
{
    bool res = GetCurrentMovie()->GoBack(frames);
    current = GetCurrentMovie()->current;
    return res;
}

void Timeline::DecodeFrame()
{
    GetCurrentMovie()->DecodeFrame();
}


