#ifndef TASKS
#define TASKS
#include "juce/juce.h"
#include "timeline.h"
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
    String descrition;
    Timeline * timeline;
    int id;
    task(Timeline * timeline, TaskType type);
    task(const task& clone_task);
    task operator=(const task& clone_task);
    ~task();
    void run();

};
extern vector<task> tasks_list;
void AddEncodingTask();
#endif
