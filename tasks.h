#ifndef TASKS
#define TASKS
#include "juce/juce.h"
#include "timeline.h"
#include "events.h"
#include <vector>
using namespace std;
class task : public Thread
{
    public:
    enum TaskType
    {
        Encoding,
        Panorama
    }type;
    String description;
    Timeline * timeline;
    Movie::Info info;
    int id;
    task(Timeline * timeline, TaskType type, int id, Movie::Info info,String description);
    task(const task& clone_task);
    task operator=(const task& clone_task);
    ~task();
    void run();

};
int AddEncodingTask(Timeline * timeline, Movie::Info info);
void RemoveTask(int id);
task* FindTaskById(int id);
extern EventList OnChangeList;
task* FindTaskByNumber(int number);
int GetTaskLength();
void CleanupTasks();
#endif
