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
    enum TaskType
    {
        Encoding,
        Panorama
    }type;
    String status;
    String filename;
    Timeline * timeline;
    Movie::Info info;
    int id;
    task(Timeline * timeline, TaskType type, int id, Movie::Info info,String filename, String status);
    task();
    void copy(task*copy_task);
    ~task();
    void run();

};
extern CriticalSection tasks_list_critical;
int AddEncodingTask(Timeline * timeline, Movie::Info info);
void RemoveTask(int id);
task* FindTaskById(int id);
extern EventList OnChangeList;
task* FindTaskByNumber(int number);
int GetTaskLength();
void CleanupTasks();
#endif
