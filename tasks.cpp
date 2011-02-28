#include "tasks.h"


vector<task *> tasks_list;
EventList OnChangeList;
CriticalSection tasks_list_critical;


task::task(Timeline * timeline, TaskType type, int id, Movie::Info info,String filename, String status):Thread("task thread")
{
    this->timeline = timeline;
    this->type = type;
    this->id = id;
    this->info = info;
    this->filename = filename;
    this->status = status;
}


task::~task()
{

}



void task::run()
{
    if(type == Encoding)
    {
        for(vector<Timeline::Interval *>::iterator it = timeline->intervals.begin();it!=timeline->intervals.end();it++)
        {
            Movie * movie = (*it)->movie;
            if(!movie->loaded)
            {
                movie->Load(movie->filename,true);
                if(!movie->loaded)
                {


                {
                    const ScopedLock myScopedLock (tasks_list_critical);
                    status = T("failed. can't load ") + movie->filename;

                }
                CallEventList(OnChangeList);
                return;
                }

            }
        }

        String render_result = timeline->Render(info);
        if(render_result==String::empty)
        {
            const ScopedLock myScopedLock (tasks_list_critical);
            status = T("done. ");
        }else
        {
            const ScopedLock myScopedLock (tasks_list_critical);
            status = T("failed. ") + render_result;
        }
        CallEventList(OnChangeList);
    }
}

int AddEncodingTask(Timeline * timeline, Movie::Info info)
{
    const ScopedLock myScopedLock (tasks_list_critical);
    int id = 0;
    for(vector<task*>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
    {
        if((*it)->id>id)
        {
            id = (*it)->id;
        }
    }
    task *new_task = new task(timeline->CloneIntervals(),task::Encoding,++id,info,info.filename,"start");
    new_task->startThread();
    tasks_list.push_back(new_task);
    CallEventList(OnChangeList);
    return id;
}

void RemoveTask(int id)
{
    const ScopedLock myScopedLock (tasks_list_critical);
    for(vector<task*>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
    {
        if((*it)->id==id)
        {
            tasks_list.erase(it);
            if((*it)->isThreadRunning())
                (*it)->stopThread(20000);
            delete (*it)->timeline;
            delete *it;
            CallEventList(OnChangeList);
        }
    }
}

task* FindTaskById(int id)
{
    for(vector<task*>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
    {
        if((*it)->id==id)
        {
            return *it;
        }
    }
}

task* FindTaskByNumber(int number)
{
    return tasks_list.at(number);
}

int GetTaskLength()
{
    const ScopedLock myScopedLock (tasks_list_critical);
    return tasks_list.size();
}

void CleanupTasks()
{
    for(vector<task*>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
    {
        if((*it)->isThreadRunning())
            (*it)->stopThread(20000);
        delete (*it)->timeline;
        delete *it;
    }
}
