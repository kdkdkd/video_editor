#ifndef TOOLBOX_H_INCLUDED
#define TOOLBOX_H_INCLUDED
#include "juce/juce.h"
namespace toolbox
{
String format_duration(double duration);

void show_info_popup(String label,String text,Component *center_component);

void delete_all_child_components(Component * comp);

class TimeVideo
{
public:
    int hour;
    int minute;
    int second;
    int mini_second;
    void fromDouble(double time);
    String toString();
};
}


#endif // TOOLBOX_H_INCLUDED
