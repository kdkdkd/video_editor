#ifndef TASKS
#define TASKS
#include "juce/juce.h"
#include "timeline.h"
#include "events.h"
#include <vector>
using namespace std;
class Timeline;


class task : public Thread
{
    public:
    enum TaskState
    {
        Working,
        Done,
        Suspended,
        NotStarted,
        Failed
    }state;
    enum TaskType
    {
        Encoding,
        Panorama
    }type;
    String status;
    String filename;
    Timeline * timeline;
    Movie::Info info;
    task(Timeline * timeline, TaskType type, Movie::Info info,String filename, String status);
    task();
    void copy(task*copy_task);
    ~task();
    void run();

    int64 millis_start;
    int64 millis_worked;
    int64 millis_left;
};

void AddEncodingTask(Timeline * timeline, Movie::Info info);
bool RemoveTask(int number);
bool PauseTask(int number);
bool ResumeTask(int number);
extern EventList OnChangeList;
bool FindTaskByNumberAndCopy(int number,task &t);
int GetTaskLength();
void FindSuspendedTaskAndLaunch();

#endif
