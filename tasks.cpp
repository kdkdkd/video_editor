#include "tasks.h"

vector<task> tasks_list;

task::task(Timeline * timeline, TaskType type):Thread("task thread")
{
    this->timeline = timeline;
    this->type = type;

}

task::task(const task& clone_task):Thread("task thread")
{
    this->timeline = clone_task.timeline;
    this->type = clone_task.type;
}
task task::operator=(const task& clone_task)
{
    this->timeline = clone_task.timeline;
    this->type = clone_task.type;
    return *this;
}
task::~task()
{
    delete timeline;
}

void task::run()
{

}
void AddEncodingTask(Timeline * timeline)
{
    tasks_list.push_back(task(timeline->CloneIntervals(),task::Encoding));
}
