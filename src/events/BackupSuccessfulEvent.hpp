#ifndef SAVEONEXIT_BACKUPSUCCESSFULEVENT_H
#define SAVEONEXIT_BACKUPSUCCESSFULEVENT_H

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class BackupSuccessfulEvent : public Event<BackupSuccessfulEvent, bool(
                                               bool triggeredByUser)>
{
public:
    using Event::Event;
};

#endif //SAVEONEXIT_BACKUPSUCCESSFULEVENT_H