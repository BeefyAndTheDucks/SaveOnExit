#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/AccountLayer.hpp>

#include "events/BackupSuccessfulEvent.hpp"
#include "events/BackupFailedEvent.hpp"
#include "events/SyncSuccessfulEvent.hpp"
#include "events/SyncFailedEvent.hpp"

class $modify(MyAccountLayer, AccountLayer) {

    struct Fields {
        bool m_wasTriggeredByUser = false;
    };

    // ReSharper disable once CppHidingFunction
    void onBackup(CCObject* sender) {
        m_fields->m_wasTriggeredByUser = true;
        AccountLayer::onBackup(sender);
    }

    // ReSharper disable once CppHidingFunction
    void onSync(CCObject* sender) {
        m_fields->m_wasTriggeredByUser = true;
        AccountLayer::onSync(sender);
    }

    void backupAccountFinished() override {
        BackupSuccessfulEvent().send(m_fields->m_wasTriggeredByUser);
        if (m_fields->m_wasTriggeredByUser)
            AccountLayer::backupAccountFinished();

        // Reset
        m_fields->m_wasTriggeredByUser = false;
    }

    void syncAccountFinished() override {
        SyncSuccessfulEvent().send(m_fields->m_wasTriggeredByUser);

        if (m_fields->m_wasTriggeredByUser)
            AccountLayer::syncAccountFinished();

        // Reset
        m_fields->m_wasTriggeredByUser = false;
    }

    void backupAccountFailed(const BackupAccountError errorType,
                             const int response) override
    {
        BackupFailedEvent().send(m_fields->m_wasTriggeredByUser, errorType,
                                 response);

        if (m_fields->m_wasTriggeredByUser)
            AccountLayer::backupAccountFailed(errorType, response);

        // Reset
        m_fields->m_wasTriggeredByUser = false;
    }

    void syncAccountFailed(const BackupAccountError errorType,
                           const int response) override
    {
        SyncFailedEvent().send(m_fields->m_wasTriggeredByUser, errorType,
                               response);

        if (m_fields->m_wasTriggeredByUser)
            AccountLayer::syncAccountFailed(errorType, response);

        // Reset
        m_fields->m_wasTriggeredByUser = false;
    }
};
