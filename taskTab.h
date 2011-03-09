#ifndef TASK_TAB
#define TASK_TAB
#include "juce/juce.h"
#include "localization.h"
using namespace localization;
class taskTab : DocumentWindow, public TableListBoxModel, public Timer
{
    public:
    TableListBox table;
    taskTab();
    ~taskTab();
    void add();
    void remove();
    void closeButtonPressed();
    int getNumRows();
    void paintRowBackground (Graphics& g, int rowNumber, int width, int height, bool rowIsSelected);
    void paintCell (Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected);
    bool isVisible;
    void refresh();
    void resized();
    void timerCallback();
    Image encoding,play,close,pause,open;
    void cellClicked(int rowNumber, int columnId, const MouseEvent& e);
};

#endif
