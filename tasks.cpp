#include "tasks.h"
#include "localization.h"
using namespace localization;

vector<task *> tasks_list;
CriticalSection tasks_list_critical;


task::task(Timeline * timeline, TaskType type, int id, Movie::Info info,String filename, String status):Thread("task thread")
{
    this->timeline = timeline;
    this->type = type;
    this->id = id;
    this->info = info;
    this->filename = filename;
    this->status = status;
    this->millis_worked = 0;
    this->millis_left = 0;
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

    this->millis_start = copy_task->millis_start;
    this->millis_worked = copy_task->millis_worked;
    this->millis_left = copy_task->millis_left;

    if(!copy_task->isThreadRunning())
        this->millis_left = 0;
}

task::~task()
{

}

void _ReportProgress(task* thread,double progress)
{
    {
        const ScopedLock myScopedLock (tasks_list_critical);
        if(progress>0.01)
            thread->millis_left = ((double)(thread->millis_worked + Time::currentTimeMillis() - thread->millis_start)) * (1.0-progress) / progress;
        else
            thread->millis_left = 0;
        thread->status = "    " + String(int(progress*100.0)) + "%";
    }
}
void FindSuspendedTaskAndLaunch()
{
    const ScopedLock myScopedLock (tasks_list_critical);
    for(vector<task*>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
    {
        if((*it)->state == task::NotStarted)
        {
            (*it)->state = task::Working;
            (*it)->startThread();
            break;
        }else if((*it)->state == task::Suspended)
        {
            (*it)->state = task::Working;
            (*it)->notify();
            break;
        }
    }

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
                    return;
                }

            }
        }
        {
            const ScopedLock myScopedLock (tasks_list_critical);
            millis_start = Time::currentTimeMillis();
        }
        timeline->RecalculateDuration();

        String render_result = timeline->Render(info,this,_ReportProgress,this);
        if(render_result==String::empty)
        {
            const ScopedLock myScopedLock (tasks_list_critical);
            status = LABEL_TASK_TAB_DONE;
            state = Done;
        }
        else
        {
            const ScopedLock myScopedLock (tasks_list_critical);
            status = LABEL_TASK_TAB_ERROR_CUSTOM + " " + render_result;
            state = Failed;
        }
        FindSuspendedTaskAndLaunch();
    }
}

int AddEncodingTask(Timeline * timeline, Movie::Info info)
{
    int id = 0;
    int number_of_working_task = 0;
    task *new_task = 0;
    {
        const ScopedLock myScopedLock (tasks_list_critical);
        for(vector<task*>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
        {
            if((*it)->id>id)
            {
                id = (*it)->id;
            }
            if((*it)->state == task::Working)
                number_of_working_task++;
        }
        new_task = new task(timeline->CloneIntervals(),task::Encoding,++id,info,info.filename,LABEL_TASK_TAB_BEGIN);
        tasks_list.push_back(new_task);
        if(number_of_working_task<3)
        {
            new_task->startThread();
            new_task->state = task::Working;
        }
        else
            new_task->state = task::NotStarted;
    }

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
}

task* FindTaskById(int id)
{
    {
        const ScopedLock myScopedLock (tasks_list_critical);
        for(vector<task*>::iterator it = tasks_list.begin(); it!=tasks_list.end(); it++)
        {
            if((*it)->id==id)
            {
                return *it;
            }
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


