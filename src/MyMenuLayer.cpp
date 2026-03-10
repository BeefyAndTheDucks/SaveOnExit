#include <Geode/Geode.hpp>

#include "events/SyncFailedEvent.hpp"
#include "events/SyncSuccessfulEvent.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>

#include "events/BackupFailedEvent.hpp"
#include "events/BackupSuccessfulEvent.hpp"

#include "BackupSpinnerLayer.hpp"

// store this globally till I find a better way
static bool g_syncSucceeded = false;
static bool g_hasSynced = false;

class $modify(MyMenuLayer, MenuLayer) {
    struct Fields {
        bool m_backingUp;

        BackupSpinnerPopup* m_backupSpinner;

        ListenerHandle m_backupFailed;
        ListenerHandle m_backupSuccess;
    };

    static void onModify(auto& self)
    {
        /*
         * this is important since we're kinda littering the function call
         * with a nullptr when returning. Prevents a crash.
         */
        self.setHookPriorityPost("MenuLayer::FLAlert_Clicked", Priority::LastPost - 1001);
    }

    bool init() override {
        if (!MenuLayer::init()) return false;

        m_fields->m_backupFailed = BackupFailedEvent().listen(
            [this](const bool wasTriggeredByUser, const BackupAccountError,
                   const int response)
            {
                if (wasTriggeredByUser)
                    return;

                log::warn("Backing up account failed");

                if (Mod::get()->getSettingValue<
                    bool>("show-save-failure-popup"))
                {
                    const gd::string str = CCString::createWithFormat(
                        "Failed to back up. Error Code: %i",
                        response)->getCString();
                    createQuickPopup("Error", str, "Go back", "Close anyway",
                                     [this](auto, const bool is_close_anyway)
                                     {
                                         if (is_close_anyway)
                                             MenuLayer::FLAlert_Clicked(
                                                 nullptr, true);
                                         else {
                                             m_fields->m_backingUp = false;
                                             if (m_fields->m_backupSpinner !=
                                                 nullptr)
                                                 m_fields->m_backupSpinner->
                                                     removeFromParent();
                                         }
                                     });
                } else
                    MenuLayer::FLAlert_Clicked(nullptr, true);
        });

        m_fields->m_backupSuccess = BackupSuccessfulEvent().listen(
            [this](const bool wasTriggeredByUser) {
                if (wasTriggeredByUser)
                    return;

                MenuLayer::FLAlert_Clicked(nullptr, true);

                log::info("Successfully backed up account");

                m_fields->m_backingUp = false;
        });

        return true;
    }

    void Save()
    {
        GJAccountManager* account_manager = GJAccountManager::sharedState();

        log::info("Backing up account data...");

        // Listens to events for us.
        AccountLayer* account_layer = AccountLayer::create();
        account_layer->enterLayer();

        m_fields->m_backupSpinner = BackupSpinnerPopup::create();
        CCDirector::sharedDirector()->getRunningScene()->addChild(
            m_fields->m_backupSpinner);

        m_fields->m_backingUp = true;

        // This triggers backup after getting URL automatically.
        account_manager->getAccountBackupURL();
    }

    void FLAlert_Clicked(FLAlertLayer* layer,
                         const bool is_quit_button) override
    {
        // The layer is nullptr if we triggered it. But also do a sanity check.
        if (layer == nullptr && m_fields->m_backingUp)
        {
            // Create a spoof FLAlertLayer to avoid crashing.
            FLAlertLayer* spoofLayer = FLAlertLayer::create(
                "Quitting",
                "GD is now exiting, if you're seeing this, just ignore it. This only exists due to GD limitations.",
                "Ok");
            spoofLayer->setTag(0);
            MenuLayer::FLAlert_Clicked(spoofLayer, true);
            return;
        }

        if (layer->getTag() != 0)
        {
            MenuLayer::FLAlert_Clicked(layer, is_quit_button);
            return;
        }

        if (!is_quit_button)
            return;

        const auto shouldSaveOption = Mod::get()->getSettingValue<std::string>(
            "save-on-shutdown");

        if (shouldSaveOption == "Always")
        {
            Save();
        } else if (shouldSaveOption == "Never")
        {
            MenuLayer::FLAlert_Clicked(layer, is_quit_button);
        } else if (shouldSaveOption == "Ask")
        {
            createQuickPopup("Save?",
                             "Would you like to save (backup) to RobTop servers before quitting?",
                             "No", "Yes",
                             [this, layer, is_quit_button](FLAlertLayer*,
                                                           const bool btn2)
                             {
                                 if (btn2)
                                     Save();
                                 else
                                      MenuLayer::FLAlert_Clicked(
                                          layer, is_quit_button);
                             });
        } else if (shouldSaveOption == "If Load Succeeded")
        {
            if (g_syncSucceeded)
                Save();
            else
                MenuLayer::FLAlert_Clicked(layer, is_quit_button);
        } else if (shouldSaveOption == "Ask if load Failed")
        {
            if (g_syncSucceeded)
                Save();
            else
            {
                const std::string str = g_hasSynced
                                      ? "Would you like to save (backup) to RobTop servers before quitting? WARNING: The last load failed!"
                                      : "Would you like to save (backup) to RobTop servers before quitting? WARNING: You haven't loaded this session! This will overwrite what's currently on the servers.";
                createQuickPopup("Save?",
                                 str,
                                 "Quit", "Save & Quit",
                                 [this, layer, is_quit_button](FLAlertLayer*,
                                                               const bool btn2)
                                 {
                                     if (btn2)
                                         Save();
                                     else
                                         MenuLayer::FLAlert_Clicked(
                                             layer, is_quit_button);
                                });
            }
        }
    }
};

$execute {
    auto syncFailed = SyncFailedEvent().listen(
            [](const bool, const BackupAccountError, const int) {
                g_syncSucceeded = false;
                g_hasSynced = true;
    });
    syncFailed.leak();

    auto syncSuccess = SyncSuccessfulEvent().listen(
        [](const bool) {
            g_syncSucceeded = true;
            g_hasSynced = true;
    });
    syncSuccess.leak();
}
