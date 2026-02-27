#ifndef SAVEONEXIT_SYNCFAILEDEVENT_H
#define SAVEONEXIT_SYNCFAILEDEVENT_H

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class SyncFailedEvent : public Event<SyncFailedEvent, bool(
                                         bool triggeredByUser,
                                         BackupAccountError errorType,
                                         int response)>
{
public:
    using Event::Event;
};

#endif //SAVEONEXIT_SYNCFAILEDEVENT_H