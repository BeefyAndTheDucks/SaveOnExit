#ifndef SAVEONEXIT_SYNCSUCCESSFULEVENT_H
#define SAVEONEXIT_SYNCSUCCESSFULEVENT_H

#include <Geode/loader/Event.hpp> // Event

using namespace geode::prelude;

class SyncSuccessfulEvent : public Event<SyncSuccessfulEvent, bool(
                                             bool triggeredByUser)>
{
public:
    using Event::Event;
};

#endif //SAVEONEXIT_SYNCSUCCESSFULEVENT_H