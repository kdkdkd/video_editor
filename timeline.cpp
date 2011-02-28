#include "timeline.h"
#include "movie.h"
#include "localization.h"
#include "toolbox.h"
#include <algorithm>
using namespace localization;

Image black_image(Image::RGB,4,3,true);

Timeline::Timeline()
{
    loaded = false;
    duration = 0.;
    current = 0.;
    current_interval = 0;
    disposeMovies = true;
    disposeIntervals = true;
};



Movie* Timeline::Load(String &filename, bool soft)
{
    Movie *movie = new Movie();
    movie->Load(filename,soft);
    bool loaded_local = movie->loaded;
    if(loaded_local)
    {
        movies.push_back(movie);

        if(!loaded)
        {
            Interval * new_timeline = new Interval(movie,0,movie->image_preview);
            intervals.push_back(new_timeline);
            current_interval = new_timeline;
            duration = movie->duration;
            current = movie->current;
        }

        movies_internal.push_back(movie);

    }
    else
    {
        delete movie;
    }

    loaded = loaded || loaded_local;
    return (loaded_local)?movie:0;

}

Timeline::Interval* Timeline::GetCurrentInterval()
{
    return current_interval;
}

Image* Timeline::GetImage()
{
    return (GetCurrentInterval())?GetCurrentInterval()->movie->image:&black_image;
}

double Timeline::GetFps()
{
    return (current_interval)?GetCurrentInterval()->movie->fps:25.0;
}

void Timeline::Dispose()
{
    if(loaded)
    {

        vector<Image *> images;
        for(vector<Movie*>::iterator it = movies_internal.begin(); it!=movies_internal.end(); it++)
        {
            images.push_back((*it)->image_preview);
            if(disposeMovies)
                delete *it;
        }

        for(vector<Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
        {
            images.push_back((*it)->preview);
            if(disposeIntervals)
                delete *it;
        }
        if(disposeMovies)
        {
            sort(images.begin(), images.end());
            vector<Image *> ::iterator end = unique(images.begin(), images.end());
            for(vector<Image *> ::iterator it = images.begin(); it!=end; it++)
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

Timeline::Interval * Timeline::FindIntervalBySecond(double second)
{
    for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
    {
        if(second>=(*it)->absolute_start && second<=(*it)->GetAbsoluteEnd())
            return *it;
    }
    return 0;

}

int Timeline::FindNumberIntervalBySecond(double second)
{
    int res = 0;
    for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
    {
        if(second>=(*it)->absolute_start && second<=(*it)->GetAbsoluteEnd())
            return res;
        res++;
    }
    return -1;

}

bool Timeline::GotoSecondAndRead(double dest,bool decode)
{
    current_interval = FindIntervalBySecond(dest);
    if(!current_interval)
    {
        current = dest;
        return false;
    }
    bool res = current_interval->movie->GotoSecondAndRead(dest-current_interval->absolute_start + current_interval->start,decode);
    RecalculateCurrent();
    return res;
}

bool Timeline::ContinueToNextFrame(bool decode, bool jump_to_next)
{
    if(!current_interval)
    {
        current += 1.0/GetFps();
        current_interval = FindIntervalBySecond(current);
        if(current_interval)
        {
            current_interval->movie->GotoSecondAndRead(current_interval->start,decode);
            current = current_interval->absolute_start;
        }
        return true;
    }
    bool res = false;
    if(current + 1.0/GetFps() - current_interval->absolute_start + current_interval->start <= current_interval->end)
    {
        if(decode)
            res = current_interval->movie->ReadAndDecodeFrame();
        else
            res = current_interval->movie->SkipFrame();
    }

    if(res)
    {
        RecalculateCurrent();
        return true;
    }

    if(!jump_to_next)
    {
        return false;
    }

    for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
    {
        if(*it == current_interval)
        {
            it++;
            double frame = 1.0d / GetFps();
            double eps = frame/5.0d;
            if(it != intervals.end() && fabs((*it)->absolute_start - current_interval->GetAbsoluteEnd())<eps)
            {
                current_interval = *it;
                current_interval->movie->GotoSecondAndRead(current_interval->start);
                RecalculateCurrent();
                return true;
            }
            break;
        }
    }

    current = current_interval->GetAbsoluteEnd();
    current_interval = 0;
    if(current>=duration)
    {
        return false;
    }
    return true;


}

bool Timeline::ReadAndDecodeFrame(bool jump_to_next)
{
    return ContinueToNextFrame(true,jump_to_next);
}

bool Timeline::SkipFrame(bool jump_to_next)
{
    return ContinueToNextFrame(false,jump_to_next);
}

bool Timeline::GoBack(int frames)
{
    double frame = 1.0d / GetFps();
    double eps = frame/5.0d;
    double desired = current - ((double)frames) * frame;
    double guess = desired - 3.0d*frame;
    if(guess<0.0)
        guess = 0.0;
    GotoSecondAndRead(guess,false);
    while(desired - current > eps )
    {
        if(!SkipFrame(false))
            break;
    }
    return true;
}

void Timeline::DecodeFrame()
{
    if(!GetCurrentInterval())
        return;
    GetCurrentInterval()->movie->DecodeFrame();
}


void Timeline::InsertIntervalIn(Timeline::Interval* insert_interval, double insert_position)
{

    Timeline* timeline_preview = PreviewInsertIntervalIn(insert_interval, insert_position);
    int size = this->intervals.size();
    for(int i=0; i<size; ++i)
    {
        delete intervals.back();
        this->intervals.pop_back();
    }
    for(vector<Timeline::Interval*>::iterator it = timeline_preview->intervals.begin(); it!=timeline_preview->intervals.end(); it++)
    {
        (*it)->color = Timeline::Interval::usual;
        this->intervals.push_back(*it);
    }
    this->current_interval = timeline_preview->current_interval;

    RecalculateDuration();
    if(!current_interval)
    {
        current = timeline_preview->current;
        GotoSecondAndRead(current);
    }
    else
        RecalculateCurrent();

    timeline_preview->disposeIntervals = false;
    delete timeline_preview;
}

void Timeline::RemoveSpaces()
{
    double prev_end = 0.0;
    for(vector<Timeline::Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
    {
        (*it)->absolute_start = prev_end;
        prev_end = (*it)->GetAbsoluteEnd();
    }
    RecalculateDuration();
    if(current_interval)
        RecalculateCurrent();
    else
        GotoSecondAndRead(current);

}
bool Timeline::IsNearMovieBoundary()
{
    return current - current_interval->absolute_start<0.1 || -current + current_interval->GetAbsoluteEnd()<0.1;
}

void Timeline::Split()
{
    if(!current_interval || IsNearMovieBoundary())return;
    Interval * insert_interval = new Interval(current_interval->movie,current - current_interval->absolute_start + current_interval->start,current_interval->end,current,current_interval->movie->GeneratePreview());
    current_interval->end = current - current_interval->absolute_start + current_interval->start;
    //current_interval = insert_interval;
    intervals.insert(find(intervals.begin(), intervals.end(), current_interval)+1,insert_interval);
}

Timeline* Timeline::PreviewInsertIntervalIn(Timeline::Interval* interval, double insert_position)
{
    // -2 - remove interval
    if(insert_position<-1.5)
    {
        Timeline *res_prepare = new Timeline();
        res_prepare->disposeIntervals = false;
        res_prepare->disposeMovies = false;
        for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
        {
            if(*it != interval)
            {
                Interval * new_interval = new Interval(*it);
                res_prepare->intervals.push_back(new_interval);
                if(*it == current_interval)
                {
                    if(current_interval == interval)
                    {
                        res_prepare->current_interval = 0;
                    }
                    else
                    {
                        res_prepare->current_interval = new_interval;
                    }
                }
            }

        }
        res_prepare->loaded = true;
        res_prepare->current = current;
        res_prepare->RecalculateDuration();

        copy(movies.begin(),movies.end(),back_inserter(res_prepare->movies));
        copy(movies_internal.begin(),movies_internal.end(),back_inserter(res_prepare->movies_internal));

        return res_prepare;
    }

    // >0 - move interval to insert_position, interval must be from existing timeline
    if(insert_position>=0.0)
    {
        Timeline *res_prepare = new Timeline();
        res_prepare->disposeIntervals = false;
        res_prepare->disposeMovies = false;
        for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
        {
            if(*it != interval)
            {
                res_prepare->intervals.push_back(*it);
            }

        }
        Interval* new_interval = new Interval(interval);
        /*new_interval->color = Timeline::Interval::select;
        new_interval->selected = true;*/
        new_interval->absolute_start = insert_position;
        res_prepare->current_interval = current_interval;
        if(current_interval == interval)
        {
            res_prepare->current_interval = new_interval;
        }
        copy(movies.begin(),movies.end(),back_inserter(res_prepare->movies));
        copy(movies_internal.begin(),movies_internal.end(),back_inserter(res_prepare->movies_internal));
        res_prepare->current = current;
        Timeline *res = res_prepare->PreviewInsertIntervalIn(new_interval,-1.0);

        delete res_prepare;
        return res;
    }

    // -1 - insert interval at interval->absolute_position, interval must be new
    interval->color=Timeline::Interval::dragg;
    Timeline *timeline_preview = new Timeline;
    timeline_preview->disposeMovies = false;
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
            Interval * new_interval = new Interval(interval_current);
            if(interval_current == current_interval)
                timeline_preview->current_interval = new_interval;
            timeline_preview->intervals.push_back(new_interval);
        }
        else
            break;

    }
    if(!interval_current || end)
    {
        if(interval == current_interval)
            timeline_preview->current_interval = interval;
        timeline_preview->intervals.push_back(interval);
    }
    else if(interval_current->absolute_start>interval->GetAbsoluteEnd())
    {
        if(interval == current_interval)
            timeline_preview->current_interval = interval;
        timeline_preview->intervals.push_back(interval);
        Interval * new_interval = new Interval(interval_current);
        if(interval_current == current_interval)
            timeline_preview->current_interval = new_interval;
        timeline_preview->intervals.push_back(new_interval);
    }
    else if(interval_current->absolute_start + interval_current->GetDuration()/2>interval->absolute_start)
    {
        if(interval == current_interval)
            timeline_preview->current_interval = interval;
        timeline_preview->intervals.push_back(interval);
        diff = - interval_current->absolute_start + interval->GetAbsoluteEnd();
        Interval * new_interval = new Interval(interval_current->movie,interval_current->start,interval_current->end,interval_current->absolute_start + diff,interval_current->preview);
        if(interval_current == current_interval)
            timeline_preview->current_interval = new_interval;
        timeline_preview->intervals.push_back(new_interval);
    }
    else
    {
        Interval * new_interval = new Interval(interval_current);
        if(interval_current == current_interval)
            timeline_preview->current_interval = new_interval;
        timeline_preview->intervals.push_back(new_interval);
        interval->absolute_start = interval_current->GetAbsoluteEnd();
        if(interval == current_interval)
            timeline_preview->current_interval = interval;
        timeline_preview->intervals.push_back(interval);
        if(it!=intervals.end() && interval->GetAbsoluteEnd()> (*it)->absolute_start)
        {
            diff = interval->GetAbsoluteEnd() - (*it)->absolute_start;
        }
    }

    while(it!=intervals.end())
    {
        interval_current = *it;
        Interval * new_interval = new Interval(interval_current->movie,interval_current->start,interval_current->end,interval_current->absolute_start + diff,interval_current->preview);
        if(interval_current == current_interval)
            timeline_preview->current_interval = new_interval;
        timeline_preview->intervals.push_back(new_interval);
        it++;
    }
    if(!current_interval)
    {
        timeline_preview->current = current;
    }
    else
        timeline_preview->RecalculateCurrent();
    timeline_preview->RecalculateDuration();
    timeline_preview->loaded = true;

    copy(movies.begin(),movies.end(),back_inserter(timeline_preview->movies));
    copy(movies_internal.begin(),movies_internal.end(),back_inserter(timeline_preview->movies_internal));

    return timeline_preview;

}

void Timeline::RecalculateDuration()
{
    double new_duration = 0.0;
    for(vector<Timeline::Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
    {
        double duration_candidate = (*it)->GetAbsoluteEnd();
        if(duration_candidate > new_duration)
            new_duration = duration_candidate;
    }
    duration = new_duration;
}

void Timeline::RecalculateCurrent()
{
    current = current_interval->movie->current - current_interval->start + current_interval->absolute_start;
}

void Timeline::ResetIntervalColor()
{
    for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
    {
        (*it)->color = Timeline::Interval::usual;
        (*it)->selected = false;
    }
}

Timeline::Interval* Timeline::FindSelected()
{
    for(vector<Timeline::Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
    {
        if((*it)->selected)
            return *it;
    }
    return 0;
}

Timeline::Interval* Timeline::FindSelectedOrOver()
{
    Timeline::Interval * over = 0;
    for(vector<Timeline::Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
    {
        if((*it)->color==Timeline::Interval::over)
            over = *it;
        if((*it)->selected)
            return *it;
    }
    return over;
}

bool Timeline::IsEmpty()
{
    return intervals.size()==0;
}

Timeline* Timeline::CloneIntervals()
{
    Timeline* res = new Timeline();
    res->disposeIntervals = true;
    res->disposeMovies = true;

    for(vector<Timeline::Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
    {
        Interval *in = new Interval(*it);
        bool found = false;
        for(vector<Movie*>::iterator itm = movies_internal.begin(); itm!=movies_internal.end(); itm++)
        {
            if(in->movie->filename == (*itm)->filename)
            {
                found = true;
                in->movie = *itm;
                break;
            }
        }
        if(!found)
        {
            Movie *movie = new Movie();
            movie ->filename = in->movie->filename;
            in->movie = movie;
            movies_internal.push_back(movie);
        }

        res->intervals.push_back(in);
    }

}
