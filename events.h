#ifndef EVENTS
#define EVENTS
#include <vector>
using namespace std;

    class Event
    {
        private:
        void* object;
        void (*func)(void * obj);
        public:
        Event(void* object,void (*func)(void * obj));
        Event(const Event& event);
        void call();
    };

    typedef vector<Event> EventList;
    void CallEventList(EventList& events);
    void AddEvent(EventList& events,void* object,void (*func)(void *));

#endif
