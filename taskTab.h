#ifndef TASK_TAB
#define TASK_TAB
#include "juce/juce.h"
#include "localization.h"
using namespace localization;
class taskTab : DocumentWindow
{
    public:
    taskTab();
    ~taskTab();
    void add();
    void remove();
    void closeButtonPressed();
};

#endif
