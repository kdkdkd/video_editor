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

vector<Timeline::Interval*>* Timeline::PreviewInsertIntervalIn(Timeline::Interval* interval)
{
    vector<Interval*> *intervals_preview = new vector<Interval*>();

    double diff = 0.0d;
    vector<Timeline::Interval*>::iterator it = intervals.begin();
    Timeline::Interval * interval_current = 0;
    bool end = false;
    while(true)
    {
      if(it==intervals.end())
      {
        end = true;
        break;
      }
      interval_current = *it;
      it++;
      if(interval_current->GetAbsoluteEnd()<interval->absolute_start)
      {
        intervals_preview->push_back(new Interval(interval_current));
      }else
      break;

    }
    if(!interval_current || end)
    {
        intervals_preview->push_back(interval);
    }else
    if(interval_current->absolute_start>interval->GetAbsoluteEnd())
    {
        intervals_preview->push_back(interval);
        intervals_preview->push_back(new Interval(interval_current));
    }else if(interval_current->absolute_start + interval_current->GetDuration()/2>interval->absolute_start)
    {
        intervals_preview->push_back(interval);
        diff = - interval_current->absolute_start + interval->GetAbsoluteEnd();
        intervals_preview->push_back(new Interval(interval_current->movie,interval_current->start,interval_current->end,interval_current->absolute_start + diff));
    }else
    {
        intervals_preview->push_back(new Interval(interval_current));
        interval->absolute_start = interval_current->GetAbsoluteEnd();
        intervals_preview->push_back(interval);
        if(it!=intervals.end() && interval->GetAbsoluteEnd()> (*it)->absolute_start)
        {
            diff = interval->GetAbsoluteEnd() - (*it)->absolute_start;
        }
    }

    while(it!=intervals.end())
    {
      interval_current = *it;
      intervals_preview->push_back(new Interval(interval_current->movie,interval_current->start,interval_current->end,interval_current->absolute_start + diff));
      it++;
    }

    return intervals_preview;

}



