#include "config.h"
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
    current_interval_audio = 0;
    current_interval_video = 0;
    disposeMoviesAndSounds = true;
    disposeIntervals = true;
};

Timeline::Interval * Timeline::SetCurrentInterval(Interval *current, int interval_id)
{
    if(interval_id)
    {
        current_interval_audio = current;
    }
    else
    {
        current_interval_video = current;
    }
    return current;
}

Stream* Timeline::GetCurrentStream(int interval_id)
{
    if(interval_id)
    {
        if(!current_interval_audio)
            return 0;
        return current_interval_audio->sound;
    }
    else
    {
        if(!current_interval_video)
            return 0;

        return current_interval_video->movie;
    }

}

void Timeline::Load(String &filename, bool soft, Movie *&movie, Sound *&sound)
{
    movie = new Movie();
    movie->Load(filename,soft);
    bool loaded_local_movie = movie->loaded;
    if(loaded_local_movie)
    {
        movies.push_back(movie);

        if(!loaded)
        {
            Interval * new_interval = new Interval(movie,0,movie->image_preview);
            intervals_video.push_back(new_interval);
            current_interval_video = new_interval;
            duration = movie->duration;
            current = movie->current;
        }

        movies_internal.push_back(movie);

    }
    else
    {
        delete movie;
        movie = 0;
    }

    sound = new Sound();
    sound->Load(filename);
    bool loaded_local_sound = sound->loaded;
    if(loaded_local_sound)
    {
        sounds.push_back(sound);

        if(!loaded)
        {
            Interval * new_interval = new Interval(sound,0);
            intervals_audio.push_back(new_interval);
            current_interval_audio = new_interval;
            if(!loaded_local_movie)
            {
                duration = sound->duration;
                current = sound->current;
            }
        }
        sounds_internal.push_back(sound);
    }
    else
    {
        delete sound;
        sound = 0;
    }


    loaded = loaded || loaded_local_movie || loaded_local_sound;
}

Timeline::Interval* Timeline::GetCurrentInterval(int interval_id)
{
    return (interval_id)?current_interval_audio:current_interval_video;
}

Image* Timeline::GetImage()
{
    return (GetCurrentInterval(0))?GetCurrentInterval(0)->movie->image:&black_image;
}

double Timeline::GetFps()
{
    return (GetCurrentInterval(0))?GetCurrentInterval(0)->movie->fps:25.0;
}

void Timeline::Dispose()
{
    if(loaded)
    {

        vector<Image *> images;
        //delete movies remember pictures
        for(vector<Movie*>::iterator it = movies_internal.begin(); it != movies_internal.end(); it++)
        {
            images.push_back((*it)->image_preview);
            if(disposeMoviesAndSounds)
                delete *it;
        }

        //delete sounds
        if(disposeMoviesAndSounds)
            for(vector<Sound*>::iterator it = sounds_internal.begin(); it != sounds_internal.end(); it++)
            {
                delete *it;
            }

        //delete intervals remember previews
        for(vector<Interval*>::iterator it = intervals_video.begin(); it != intervals_video.end(); it++)
        {
            images.push_back((*it)->preview);
            if(disposeIntervals)
                delete *it;
        }

        //delete images
        if(disposeMoviesAndSounds)
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

Timeline::Interval * Timeline::FindIntervalBySecond(double second, int interval_id)
{
    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;
    for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
    {
        if(second>=(*it)->absolute_start && second<=(*it)->GetAbsoluteEnd())
            return *it;
    }
    return 0;

}

int Timeline::FindNumberIntervalBySecond(double second, int interval_id)
{
    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;
    int res = 0;
    for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
    {
        if(second>=(*it)->absolute_start && second<=(*it)->GetAbsoluteEnd())
            return res;
        res++;
    }
    return -1;

}

bool Timeline::GotoSecondAndRead(double dest,bool decode,int interval_id)
{
    // audio jumping
    Interval * current_interval = SetCurrentInterval(FindIntervalBySecond(dest,interval_id),interval_id);
    bool res = false;
    if(!current_interval)
    {
        current = dest;
    }
    else
    {
        res = GetCurrentStream(interval_id)->GotoSecondAndRead(dest - current_interval->absolute_start + current_interval->start,decode);
        RecalculateCurrent(interval_id);
    }

    return res;
}

bool Timeline::GotoSecondAndRead(double dest,bool decode)
{
    bool res1 = GotoSecondAndRead(dest,decode,1);
    bool res2 = GotoSecondAndRead(dest,decode,0);
    return res1 || res2;
}


bool Timeline::ContinueToNextFrame(bool decode, bool jump_to_next, int interval_id)
{
    Interval * current_interval = GetCurrentInterval(interval_id);
    if(!current_interval)
    {
        current += 1.0/GetFps();
        current_interval = SetCurrentInterval(FindIntervalBySecond(current,interval_id),interval_id);
        if(current_interval)
        {
            GetCurrentStream(interval_id)->GotoSecondAndRead(current_interval->start,decode);
            current = current_interval->absolute_start;
        }
        return true;
    }
    bool res = false;
    if(current + 1.0/GetFps() - current_interval->absolute_start + current_interval->start <= current_interval->end)
    {
        if(decode)
            res = GetCurrentStream(interval_id)->ReadAndDecodeFrame();
        else
            res = GetCurrentStream(interval_id)->SkipFrame();
    }

    if(res)
    {
        RecalculateCurrent(interval_id);
        return true;
    }

    if(!jump_to_next)
    {
        return false;
    }

    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;
    for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
    {
        if(*it == current_interval)
        {
            it++;
            double frame = 1.0 / GetFps();
            double eps = frame/5.0;
            if(it != intervals.end() && fabs((*it)->absolute_start - current_interval->GetAbsoluteEnd())<eps)
            {
                current_interval = SetCurrentInterval(*it,interval_id);
                GetCurrentStream(interval_id)->GotoSecondAndRead(current_interval->start);
                RecalculateCurrent(interval_id);
                return true;
            }
            break;
        }
    }

    current = current_interval->GetAbsoluteEnd();
    SetCurrentInterval(0,interval_id);
    if(current>=duration)
    {
        return false;
    }
    return true;


}

bool Timeline::ContinueToNextFrame(bool decode, bool jump_to_next)
{
    bool res1 = ContinueToNextFrame(decode,jump_to_next,1);
    bool res2 = ContinueToNextFrame(decode,jump_to_next,0);
    return res1 || res2;
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
    double frame = 1.0 / GetFps();
    double eps = frame/5.0;
    double desired = current - ((double)frames) * frame;
    double guess = desired - 3.0*frame;
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



void Timeline::DecodeFrame(int interval_id)
{
    if(!GetCurrentInterval(interval_id))
        return;
    GetCurrentStream(interval_id)->DecodeFrame();
}


void Timeline::DecodeFrame()
{
    DecodeFrame(0);
    DecodeFrame(1);
}

void Timeline::InsertIntervalIn(Timeline::Interval* insert_interval, int interval_id, double insert_position)
{
    Timeline* timeline_preview = PreviewInsertIntervalIn(insert_interval, interval_id, insert_position);
    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;
    vector<Interval*> &preview_intervals = (interval_id)?timeline_preview->intervals_audio:timeline_preview->intervals_video;
    int size = intervals.size();
    for(int i=0; i<size; ++i)
    {
        delete intervals.back();
        intervals.pop_back();
    }
    for(vector<Timeline::Interval*>::iterator it = preview_intervals.begin(); it!=preview_intervals.end(); it++)
    {
        (*it)->color = Timeline::Interval::usual;
        intervals.push_back(*it);
    }

    timeline_preview->CopyOtherIntervalsPointersAndSetCurrent(this,interval_id,false);

    Interval * current_interval = SetCurrentInterval(timeline_preview->GetCurrentInterval(interval_id),interval_id);

    RecalculateDuration();
    if(!current_interval)
    {
        current = timeline_preview->current;
        GotoSecondAndRead(current);
    }
    else
        RecalculateCurrent(interval_id);

    timeline_preview->disposeIntervals = false;
    delete timeline_preview;
}

void Timeline::RemoveSpaces(int interval_id)
{
    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;

    double prev_end = 0.0;
    for(vector<Timeline::Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
    {
        (*it)->absolute_start = prev_end;
        prev_end = (*it)->GetAbsoluteEnd();
    }
    RecalculateDuration();
    if(GetCurrentInterval(interval_id))
        RecalculateCurrent(interval_id);
    else
        GotoSecondAndRead(current,interval_id);
}

void Timeline::RemoveSpaces()
{
    RemoveSpaces(1);
    RemoveSpaces(0);
}


bool Timeline::IsNearBoundary(int interval_id)
{
    Interval * current_interval = GetCurrentInterval(interval_id);
    return (current - current_interval->absolute_start<0.1) || (- current + current_interval->GetAbsoluteEnd()<0.1);
}

void Timeline::Split(int interval_id)
{
    Interval * current_interval = GetCurrentInterval(interval_id);
    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;
    if(!current_interval || IsNearBoundary(interval_id))return;
    Interval * insert_interval = 0;
    if(interval_id)
        insert_interval = new Interval(current_interval->sound,current - current_interval->absolute_start + current_interval->start,current_interval->end,current);
    else
        insert_interval = new Interval(current_interval->movie,current - current_interval->absolute_start + current_interval->start,current_interval->end,current,current_interval->movie->GeneratePreview());
    current_interval->end = current - current_interval->absolute_start + current_interval->start;

    intervals.insert(find(intervals.begin(), intervals.end(), current_interval)+1,insert_interval);
}


void Timeline::CopyOtherIntervalsPointersAndSetCurrent(Timeline* res,int interval_id, bool deep)
{
    vector<Interval*> &other_intervals = (interval_id)?intervals_video:intervals_audio;
    vector<Interval*> &other_intervals_res = (interval_id)?res->intervals_video:res->intervals_audio;
    Interval * current_interval_other = (interval_id)?current_interval_video:current_interval_audio;


    for(vector<Interval*>::iterator it = other_intervals.begin(); it != other_intervals.end(); it++)
    {
        Interval * new_interval;
        if(deep)
            new_interval = new Interval(*it);
        else
            new_interval = *it;


        other_intervals_res.push_back(new_interval);
        if(interval_id)
        {
            if(*it == current_interval_video)
                res->current_interval_video = new_interval;
        }else
        {
            if(*it == current_interval_audio)
                res->current_interval_audio = new_interval;
        }
    }


}

Timeline* Timeline::PreviewInsertIntervalIn(Timeline::Interval* interval, int interval_id, double insert_position)
{
    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;
    Interval * current_interval = GetCurrentInterval(interval_id);

    // -2 - remove interval
    if(insert_position<-1.5)
    {
        Timeline *res_prepare = new Timeline();
        CopyOtherIntervalsPointersAndSetCurrent(res_prepare,interval_id,true);
        vector<Interval*> &res_prepare_intervals = (interval_id)?res_prepare->intervals_audio:res_prepare->intervals_video;
        res_prepare->disposeIntervals = false;
        res_prepare->disposeMoviesAndSounds = false;
        for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
        {
            if(*it != interval)
            {
                Interval * new_interval = new Interval(*it);
                res_prepare_intervals.push_back(new_interval);
                if(*it == current_interval)
                {
                    if(current_interval == interval)
                    {
                        res_prepare->SetCurrentInterval(0, interval_id);
                    }
                    else
                    {
                        res_prepare->SetCurrentInterval(new_interval, interval_id);
                    }
                }
            }

        }
        res_prepare->loaded = true;
        res_prepare->current = current;
        res_prepare->RecalculateDuration();

        copy(movies.begin(),movies.end(),back_inserter(res_prepare->movies));
        copy(movies_internal.begin(),movies_internal.end(),back_inserter(res_prepare->movies_internal));
        copy(sounds.begin(),sounds.end(),back_inserter(res_prepare->sounds));
        copy(sounds_internal.begin(),sounds_internal.end(),back_inserter(res_prepare->sounds_internal));


        return res_prepare;
    }

    // >0 - move interval to insert_position, interval must be from existing timeline
    if(insert_position>=0.0)
    {
        Timeline *res_prepare = new Timeline();
        vector<Interval*> &res_prepare_intervals = (interval_id)?res_prepare->intervals_audio:res_prepare->intervals_video;
        res_prepare->disposeIntervals = false;
        res_prepare->disposeMoviesAndSounds = false;
        for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
        {
            if(*it != interval)
            {
                res_prepare_intervals.push_back(*it);
            }

        }
        Interval* new_interval = new Interval(interval);
        /*new_interval->color = Timeline::Interval::select;
        new_interval->selected = true;*/
        new_interval->absolute_start = insert_position;
        res_prepare->SetCurrentInterval(current_interval, interval_id);
        if(current_interval == interval)
        {
            res_prepare->SetCurrentInterval(new_interval, interval_id);
        }
        copy(movies.begin(),movies.end(),back_inserter(res_prepare->movies));
        copy(movies_internal.begin(),movies_internal.end(),back_inserter(res_prepare->movies_internal));
        copy(sounds.begin(),sounds.end(),back_inserter(res_prepare->sounds));
        copy(sounds_internal.begin(),sounds_internal.end(),back_inserter(res_prepare->sounds_internal));

        res_prepare->current = current;
        Timeline *res = res_prepare->PreviewInsertIntervalIn(new_interval, interval_id, -1.0);

        delete res_prepare;
        return res;
    }

    // -1 - insert interval at interval->absolute_position, interval must be new
    interval->color = Timeline::Interval::dragg;
    Timeline *timeline_preview = new Timeline;

    CopyOtherIntervalsPointersAndSetCurrent(timeline_preview,interval_id,true);

    vector<Interval*> &timeline_preview_intervals = (interval_id)?timeline_preview->intervals_audio:timeline_preview->intervals_video;
    timeline_preview->disposeMoviesAndSounds = false;
    double diff = 0.0;
    vector<Timeline::Interval*>::iterator it = intervals.begin();
    Timeline::Interval * interval_current = 0;
    bool end = false;
    //push all intervals that are behind inserting interval
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
                timeline_preview->SetCurrentInterval(new_interval,interval_id);
            timeline_preview_intervals.push_back(new_interval);
        }
        else
            break;

    }

    if(!interval_current || end)
    {
        if(interval == current_interval)
            timeline_preview->SetCurrentInterval(interval,interval_id);
        timeline_preview_intervals.push_back(interval);
    }
    else if(interval_current->absolute_start>interval->GetAbsoluteEnd())
    {
        if(interval == current_interval)
            timeline_preview->SetCurrentInterval(interval,interval_id);
        timeline_preview_intervals.push_back(interval);
        Interval * new_interval = new Interval(interval_current);
        if(interval_current == current_interval)
            timeline_preview->SetCurrentInterval(new_interval,interval_id);
        timeline_preview_intervals.push_back(new_interval);
    }
    else if(interval_current->absolute_start + interval_current->GetDuration()/2>interval->absolute_start)
    {
        if(interval == current_interval)
            timeline_preview->SetCurrentInterval(interval,interval_id);
        timeline_preview_intervals.push_back(interval);
        diff = - interval_current->absolute_start + interval->GetAbsoluteEnd();
        Interval * new_interval = new Interval(interval_current->movie,interval_current->start,interval_current->end,interval_current->absolute_start + diff,interval_current->preview);
        if(interval_current == current_interval)
            timeline_preview->SetCurrentInterval(new_interval,interval_id);
        timeline_preview_intervals.push_back(new_interval);
    }
    else
    {
        Interval * new_interval = new Interval(interval_current);
        if(interval_current == current_interval)
            timeline_preview->SetCurrentInterval(new_interval,interval_id);
        timeline_preview_intervals.push_back(new_interval);
        interval->absolute_start = interval_current->GetAbsoluteEnd();
        if(interval == current_interval)
            timeline_preview->SetCurrentInterval(interval,interval_id);
        timeline_preview_intervals.push_back(interval);
        if(it!=intervals.end() && interval->GetAbsoluteEnd()> (*it)->absolute_start)
        {
            diff = interval->GetAbsoluteEnd() - (*it)->absolute_start;
        }
    }

    //push all intervals that are after inserting interval
    while(it!=intervals.end())
    {
        interval_current = *it;
        Interval * new_interval = new Interval(interval_current);
        new_interval->absolute_start += diff;
        if(interval_current == current_interval)
            timeline_preview->SetCurrentInterval(new_interval,interval_id);
        timeline_preview_intervals.push_back(new_interval);
        it++;
    }

    if(!current_interval)
    {
        timeline_preview->current = current;
    }
    else
        timeline_preview->RecalculateCurrent(interval_id);

    timeline_preview->RecalculateDuration();
    timeline_preview->loaded = true;

    copy(movies.begin(),movies.end(),back_inserter(timeline_preview->movies));
    copy(movies_internal.begin(),movies_internal.end(),back_inserter(timeline_preview->movies_internal));
    copy(sounds.begin(),sounds.end(),back_inserter(timeline_preview->sounds));
    copy(sounds_internal.begin(),sounds_internal.end(),back_inserter(timeline_preview->sounds_internal));
    return timeline_preview;

}

int Timeline::RecalculateDuration(int interval_id)
{
    double new_duration = 0.0;
    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;
    for(vector<Timeline::Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
    {
        double duration_candidate = (*it)->GetAbsoluteEnd();
        if(duration_candidate > new_duration)
            new_duration = duration_candidate;
    }

    return new_duration;
}

void Timeline::RecalculateDuration()
{
    int duration1 = RecalculateDuration(0);
    int duration2 = RecalculateDuration(1);
    duration = (duration1>duration2)?duration1:duration2;
}

void Timeline::RecalculateCurrent(int interval_id)
{
    Interval*current_interval = GetCurrentInterval(interval_id);
    current = GetCurrentStream(interval_id)->current - current_interval->start + current_interval->absolute_start;
}

void Timeline::ResetIntervalColor(int interval_id)
{
    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;
    for(vector<Interval*>::iterator it = intervals.begin(); it != intervals.end(); it++)
    {
        (*it)->color = Timeline::Interval::usual;
        (*it)->selected = false;
    }
}

void Timeline::ResetIntervalColor()
{
    ResetIntervalColor(0);
    ResetIntervalColor(1);
}

Timeline::Interval* Timeline::FindSelected(int interval_id)
{
    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;
    for(vector<Timeline::Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
    {
        if((*it)->selected)
            return *it;
    }
    return 0;
}

Timeline::Interval* Timeline::FindSelected()
{
    Interval * int1 = FindSelected(0);
    if(int1)
        return int1;
    Interval * int2 = FindSelected(1);
    return int2;
}

Timeline::Interval* Timeline::FindSelectedOrOver(int interval_id)
{
    Timeline::Interval * over = 0;
    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;
    for(vector<Timeline::Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
    {
        if((*it)->color==Timeline::Interval::over)
            over = *it;
        if((*it)->selected)
            return *it;
    }
    return over;
}

Timeline::Interval* Timeline::FindSelectedOrOver()
{
    Interval * int1 = FindSelectedOrOver(0);
    if(int1)
        return int1;
    Interval * int2 = FindSelectedOrOver(1);
    return int2;
}

bool Timeline::IsEmpty()
{
    return intervals_audio.size()==0 && intervals_video.size()==0;
}

void Timeline::CloneIntervals(int interval_id,Timeline* res)
{
    res->disposeIntervals = true;
    res->disposeMoviesAndSounds = true;
    vector<Interval*> &intervals = (interval_id)?intervals_audio:intervals_video;
    for(vector<Timeline::Interval*>::iterator it = intervals.begin(); it!=intervals.end(); it++)
    {
        Interval *in = new Interval(*it);
        bool found = false;
        if(interval_id)
        {
            for(vector<Sound*>::iterator itm = res->sounds_internal.begin(); itm!=res->sounds_internal.end(); itm++)
            {
                if(in->sound->filename == (*itm)->filename)
                {
                    found = true;
                    in->sound = *itm;
                    break;
                }
            }
            if(!found)
            {
                Sound *sound = new Sound();
                sound->filename = in->sound->filename;
                in->sound = sound;
                res->sounds_internal.push_back(sound);
            }
        }
        {
            for(vector<Movie*>::iterator itm = res->movies_internal.begin(); itm!=res->movies_internal.end(); itm++)
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
                movie->filename = in->movie->filename;
                in->movie = movie;
                res->movies_internal.push_back(movie);
            }
        }
        vector<Interval*> &res_intervals = (interval_id)?res->intervals_audio:res->intervals_video;
        res_intervals.push_back(in);
    }
}

Timeline* Timeline::CloneIntervals()
{
    Timeline* res = new Timeline();
    CloneIntervals(0,res);
    CloneIntervals(1,res);
    return res;
}
