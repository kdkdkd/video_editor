#include "config.h"
#include "toolbox.h"
#include "PopupWindow.h"
namespace toolbox
{
void delete_all_child_components(Component * comp)
{
    for(int i = 0;i<comp->getNumChildComponents();i++)
    {
        delete_all_child_components(comp->getChildComponent(i));
    }
    comp->getParentComponent()->removeChildComponent(comp);
    delete comp;
}

void show_info_popup(String label,String text,Component *center_component)
{
    TextEditor *te = new TextEditor();
    te->setReadOnly(true);


    Font font = te->getFont();
    font.setHeight(16);
    te->setFont(font);
    te->setText(text);

    int width_text_editor = te->getTextWidth()+20;
    int height_text_editor = te->getTextHeight()+50;
    if(width_text_editor<200)
        width_text_editor = 200;
    if(width_text_editor>600)
        width_text_editor = 600;


    if(height_text_editor>600)
        height_text_editor = 600;


    te->setBounds(0,0,width_text_editor,height_text_editor);
    te->setMultiLine(true,false);
    te->setSelectAllWhenFocused(true);
    te->setPopupMenuEnabled(false);


    PopupWindow *doc = new PopupWindow(label,Colours::whitesmoke,DocumentWindow::closeButton,true);
    doc->setTitleBarHeight(20);
    doc->setResizable(false, false);
    doc->centreAroundComponent(center_component,width_text_editor,height_text_editor);

    doc->setSize(width_text_editor,height_text_editor);
    doc->setContentComponent(te);

    doc->setVisible(true);
    doc->addToDesktop(0);

    te->grabKeyboardFocus();

}
String format_duration(double duration)
{
    String res;
    TimeVideo * tv = new TimeVideo();
    tv->fromDouble(duration);
    res = tv->toString();
    delete tv;
    return res;
}

String format_duration_small(double duration)
{
    String res;
    TimeVideo * tv = new TimeVideo();
    tv->fromDouble(duration);
    res = tv->toStringSmall();
    delete tv;
    return res;
}

void TimeVideo::fromDouble(double time)
{
    int duration_int = time;
    double rest = time - (double)duration_int;
    hour = duration_int/3600;
    duration_int%=3600;
    minute = duration_int/60;
    second = duration_int%60;
    mini_second = (int)(rest*(1000.0));
}

String TimeVideo::toString()
{

    String res;
    if(hour>0)
    {
        res<<hour<<" : ";
    }


    if(minute<10)
        res<<"0";
    res<<minute<<" : ";


    if(second<10)
        res<<"0";
    res<<second;

    res<<" . ";
    if(mini_second<10)
        res<<"00";
    else if(mini_second<100)
        res<<"0";

    res<<mini_second;

    return res;
}

String TimeVideo::toStringSmall()
{

    String res;
    if(hour>0)
    {
        res<<hour<<":";
    }


    if(minute<10)
        res<<"0";
    res<<minute<<":";


    if(second<10)
        res<<"0";
    res<<second;

    return res;
}


}
