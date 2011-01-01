#include "toolbox.h"
namespace toolbox
{
String format_duration(double duration)
{
    String res;
    TimeVideo * tv = new TimeVideo();
    tv->fromDouble(duration);
    res = tv->toString();
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


    res<<" . "<<mini_second;

    return res;
}


}
