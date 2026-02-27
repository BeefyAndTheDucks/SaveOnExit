#ifndef SAVEONEXIT_BACKUPFAILEDEVENT_H
#define SAVEONEXIT_BACKUPFAILEDEVENT_H

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class BackupFailedEvent : public Event<BackupFailedEvent, bool(
                                           bool triggeredByUser,
                                           BackupAccountError errorType,
                                           int response)>
{
public:
    using Event::Event;
};

#endif //SAVEONEXIT_BACKUPFAILEDEVENT_H