#ifndef TOOLBOX_H_INCLUDED
#define TOOLBOX_H_INCLUDED
#include <juce.h>
namespace toolbox
{
String format_duration(double duration);
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
