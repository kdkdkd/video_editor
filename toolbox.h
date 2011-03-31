#ifndef TOOLBOX_H_INCLUDED
#define TOOLBOX_H_INCLUDED
#include "juce/juce.h"
#include <string.h>
#include <stdlib.h>
namespace toolbox
{

String get_compilation_date();
String format_duration(double duration);
String format_duration_small(double duration);
template< typename T > T GCD(T a, T b)
{
    for(;;)
    {
        a = a % b;
		if( a == 0 )
			return b;
		b = b % a;
        if( b == 0 )
			return a;
    }
}

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
    String toStringSmall();
};
}


#endif // TOOLBOX_H_INCLUDED
