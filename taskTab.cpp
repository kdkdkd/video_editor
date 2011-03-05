#include "taskTab.h"
#include "tasks.h"
#include "toolbox.h"

void taskTab::timerCallback()
{
    table.updateContent();
    repaint();
}
void taskTab::resized()
{
    table.autoSizeAllColumns();
    DocumentWindow::resized();
}
taskTab::taskTab():DocumentWindow(LABEL_TASK_TAB,Colours::whitesmoke,DocumentWindow::allButtons)
{
    setTitleBarHeight (20);
    setResizable (true, false);
    setBounds(30,30,800,600);
    setResizeLimits(100,100,8000,6000);
    setContentComponent(&table);

    table.setModel (this);

    table.setColour (ListBox::outlineColourId, Colours::grey);
    table.setOutlineThickness (1);


    table.getHeader().addColumn(LABEL_TASK_TAB_TYPE,1,130,130,100,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable);
    table.getHeader().addColumn(String::empty,2,26,26,26,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable);
    table.getHeader().addColumn(String::empty,3,26,26,26,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable);
    table.getHeader().addColumn(LABEL_TASK_TAB_DESCRPTION,4,328,328,900,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable | TableHeaderComponent::resizable);
    table.getHeader().addColumn(LABEL_TASK_TAB_TIME_LEFT,5,70,70,70,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable );
    table.getHeader().addColumn(LABEL_TASK_TAB_PROGRESS,6,170,170,900,TableHeaderComponent::visible | TableHeaderComponent::appearsOnColumnMenu | TableHeaderComponent::draggable | TableHeaderComponent::resizable);
    table.setHeaderHeight(30);



    isVisible = false;

    encoding = ImageCache::getFromFile(String("pic\\taskTab\\encoding.png"));
    play = ImageCache::getFromFile(String("pic\\taskTab\\play.png"));
    pause = ImageCache::getFromFile(String("pic\\taskTab\\pause.png"));
    close = ImageCache::getFromFile(String("pic\\taskTab\\close.png"));
    open = ImageCache::getFromFile(String("pic\\taskTab\\open.png"));

}
void taskTab::add()
{
    setVisible(true);
    addToDesktop(ComponentPeer::windowHasCloseButton || ComponentPeer::windowHasTitleBar || ComponentPeer::windowIsResizable);
    isVisible = true;
    startTimer(1000);
}

void taskTab::refresh()
{
    repaint();
}

void taskTab::remove()
{
    removeFromDesktop();
    isVisible = false;
    stopTimer();
}

taskTab::~taskTab()
{

}
int taskTab::getNumRows()
{
    return GetTaskLength();
}

void taskTab::paintRowBackground (Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    /*if (rowIsSelected)
            g.fillAll (Colours::lightblue);*/
}

void taskTab::paintCell (Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    task t_copy;
    {
        const ScopedLock myScopedLock (tasks_list_critical);
        task *t = FindTaskByNumber(rowNumber);
        if(!t)
            return;
        t_copy.copy(t);
    }
    String text_to_draw = String::empty;
    Justification just = Justification::centredLeft;
    switch(columnId)
    {
        case 6:text_to_draw = t_copy.status;break;
        case 4:text_to_draw = t_copy.filename;break;
        case 1:
        {
            switch(t_copy.type)
            {
                case task::Encoding: text_to_draw = LABEL_TASK_TAB_TYPE_ENCODING; break;
                case task::Panorama: text_to_draw = LABEL_TASK_TAB_TYPE_PANORAMA; break;
            }
            g.drawImage(encoding,2,0,27,20,0,0,25,20);
            g.drawText (text_to_draw, 32, 0, width - 34, height, Justification::centredLeft, true);
            text_to_draw = String::empty;
        }
        break;
        case 2:
        {
            g.drawImage(pause,3,1,18,18,0,0,18,18);
        }
        break;
        case 3:
        {
            g.drawImage(close,3,1,18,18,0,0,18,18);
        }
        break;
        case 5:
            just = Justification::centred;
            if(t_copy.millis_left>0)
                text_to_draw = toolbox::format_duration_small((double)t_copy.millis_left * 0.001);
            else
                text_to_draw = "-";
        break;

    }
    if(text_to_draw.length()>0)
        g.drawText (text_to_draw, 2, 0, width - 4, height, just, true);

    g.setColour (Colours::black.withAlpha (0.2f));
    g.fillRect (width - 1, 0, 1, height);
}

void taskTab::closeButtonPressed()
{
    remove();
}
