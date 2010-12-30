#include "toolbox.h"
namespace toolbox
{
String format_duration(double duration)
{
    int duration_int = duration;
    double rest = duration - (double)duration_int;


    String res;
    if(duration_int>3600)
    {
        res<<duration_int/3600<<" : ";
        duration_int%=3600;
    }
    unsigned int add = 0;
    add = duration_int/60;
    if(add<10)
        res<<"0";
    res<<add<<" : ";
    duration_int%=60;

    if(duration_int<10)
        res<<"0";
    res<<duration_int;
    duration_int%=60;

    res<<" . "<<(int)(rest*(1000.0));

    return res;
}

}
