#include "toolbox.h"
namespace toolbox
{
String format_duration(unsigned int duration)
{
    String res;
    if(duration>3600)
    {
        res<<duration/3600<<" : ";
        duration%=3600;
    }
    unsigned int add = 0;
    add = duration/60;
    if(add<10)
        res<<"0";
    res<<add<<" : ";
    duration%=60;

    if(duration<10)
        res<<"0";
    res<<duration;
    duration%=60;


    return res;
}

}
