#include "events.h"

Event::Event(void* object,void (*func)(void *))
{
    this->object = object;
    this->func = func;
}

Event::Event(const Event& event)
{
    this->object = event.object;
    this->func = event.func;
}

void Event::call()
{
    func(object);
}

void CallEventList(EventList& events)
{
    for(EventList::iterator it = events.begin(); it!=events.end(); ++it)
    {
        it->call();
    }
}
void AddEvent(EventList& events,void* object,void (*func)(void *))
{
    events.push_back(Event(object,func));
}
