#include "tasks.h"


vector<task> tasks_list;
EventList OnChangeList;
CriticalSection tasks_list_critical;


task::task(Timeline * timeline, TaskType type, int id, Movie::Info info,String description):Thread("task thread")
{
    this->timeline = timeline;
    this->type = type;
    this->id = id;
    this->info = info;
    this->description = description;
}

task::task(const task& clone_task):Thread("task thread")
{
    this->timeline = clone_task.timeline;
    this->type = clone_task.type;
    this->id = clone_task.id;
    this->info = clone_task.info;
    this->description = clone_task.description;
}
task task::operator=(const task& clone_task)
{
    this->timeline = clone_task.timeline;
    this->type = clone_task.type;
    this->id = clone_task.id;
    this->info = clone_task.info;
    this->description = clone_task.description;
    return *this;
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
                    const ScopedLock myScopedLock (tasks_list_critical);
                    description = T("failed. can't load ") + movie->filename + T(". ") + description;
                    CallEventList(OnChangeList);
                    return;
                }

            }
        }

        String render_result = timeline->Render(info);
        if(render_result==String::empty)
        {
            const ScopedLock myScopedLock (tasks_list_critical);
            description = T("done. ") + description;
        }else
        {
            const ScopedLock myScopedLock (tasks_list_critical);
            description = T("failed. ") + render_result + T(". ") + description;
        }
        CallEventList(OnChangeList);
    }
}

int AddEncodingTask(Timeline * timeline, Movie::Info info)
{
    const ScopedLock myScopedLock (tasks_list_critical);
    int id = 0;
    for(vector<task>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
    {
        if(it->id>id)
        {
            id = it->id;
        }
    }
    task new_task(timeline->CloneIntervals(),task::Encoding,++id,info,info.filename);
    new_task.startThread();
    tasks_list.push_back(new_task);
    CallEventList(OnChangeList);
    return id;
}

void RemoveTask(int id)
{
    const ScopedLock myScopedLock (tasks_list_critical);
    for(vector<task>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
    {
        if(it->id==id)
        {
            it->stopThread(20000);
            delete it->timeline;
            tasks_list.erase(it);
            CallEventList(OnChangeList);
        }
    }
}

task* FindTaskById(int id)
{
    const ScopedLock myScopedLock (tasks_list_critical);
    for(vector<task>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
    {
        if(it->id==id)
        {
            return &*it;
        }
    }
}

task* FindTaskByNumber(int number)
{
    const ScopedLock myScopedLock (tasks_list_critical);
    return &tasks_list.at(number);
}

int GetTaskLength()
{
    const ScopedLock myScopedLock (tasks_list_critical);
    return tasks_list.size();
}

void CleanupTasks()
{
    for(vector<task>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
    {
        delete it->timeline;
    }
}
