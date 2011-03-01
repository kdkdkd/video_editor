#include "tasks.h"
#include "localization.h"
using namespace localization;

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

task::task():Thread("task thread")
{

}

void task::copy(task*copy_task)
{
    this->type = copy_task->type;
    this->id = copy_task->id;
    this->filename = copy_task->filename;
    this->status = copy_task->status;
}

task::~task()
{

}

void _ReportProgress(task* thread,double progress)
{
    {
        const ScopedLock myScopedLock (tasks_list_critical);
        thread->status = "    " + String(int(progress*100.0)) + "%";
    }
    CallEventList(OnChangeList);
}

void task::run()
{
    if(type == Encoding)
    {
        for(vector<Timeline::Interval *>::iterator it = timeline->intervals.begin(); it!=timeline->intervals.end(); it++)
        {
            Movie * movie = (*it)->movie;
            if(!movie->loaded)
            {
                movie->Load(movie->filename,true);
                if(!movie->loaded)
                {


                    {
                        const ScopedLock myScopedLock (tasks_list_critical);
                        status = LABEL_TASK_TAB_ERROR_CANT_LOAD_FILE + movie->filename;

                    }
                    CallEventList(OnChangeList);
                    return;
                }

            }
        }
        timeline->RecalculateDuration();

        String render_result = timeline->Render(info,this,_ReportProgress);
        if(render_result==String::empty)
        {
            const ScopedLock myScopedLock (tasks_list_critical);
            status = LABEL_TASK_TAB_DONE;
        }
        else
        {
            const ScopedLock myScopedLock (tasks_list_critical);
            status = LABEL_TASK_TAB_ERROR_CUSTOM + " " + render_result;
        }
        CallEventList(OnChangeList);
    }
}

int AddEncodingTask(Timeline * timeline, Movie::Info info)
{
    int id = 0;
    task *new_task = 0;
    {
        const ScopedLock myScopedLock (tasks_list_critical);
        for(vector<task*>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
        {
            if((*it)->id>id)
            {
                id = (*it)->id;
            }
        }
        new_task = new task(timeline->CloneIntervals(),task::Encoding,++id,info,info.filename,LABEL_TASK_TAB_BEGIN);
        tasks_list.push_back(new_task);
    }
    new_task->startThread();
    CallEventList(OnChangeList);
    return id;
}

void RemoveTask(int id)
{
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

            }
        }
    }
    CallEventList(OnChangeList);
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
