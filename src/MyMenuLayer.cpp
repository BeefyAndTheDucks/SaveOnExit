#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "events/SyncFailedEvent.hpp"
#include "events/SyncSuccessfulEvent.hpp"

#include "events/BackupFailedEvent.hpp"
#include "events/BackupSuccessfulEvent.hpp"

#include "BackupSpinnerLayer.hpp"

using namespace geode::prelude;

// store this globally till I find a better way
static bool g_syncSucceeded = false;
static bool g_hasSynced = false;

class $modify(MyMenuLayer, MenuLayer) {
    struct Fields {
        bool m_forceExit;

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
        const bool success = static_cast<bool>(self.setHookPriorityPost("MenuLayer::FLAlert_Clicked", Priority::LastPost - 1001));
        if (!success)
            log::error("Failed to change hook order of modify \"MenuLayer::FLAlert_Clicked\"!");
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
                                             Quit();
                                         else {
                                             if (m_fields->m_backupSpinner != nullptr)
                                                 m_fields->m_backupSpinner->close();
                                         }
                                     });
                } else
                    Quit();
        });

        m_fields->m_backupSuccess = BackupSuccessfulEvent().listen(
            [this](const bool wasTriggeredByUser) {
                if (wasTriggeredByUser)
                    return;

                log::info("Successfully backed up account");

                Quit();
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

        m_fields->m_backupSpinner = BackupSpinnerPopup::create("Backing up...");
        m_fields->m_backupSpinner->show();

        // This triggers backup after getting URL automatically.
        account_manager->getAccountBackupURL();
    }

    void Quit() {
        // Create a spoof FLAlertLayer to avoid crashing.
        FLAlertLayer* spoofLayer = FLAlertLayer::create(
            "Quitting",
            "GD is now exiting, if you're seeing this, just ignore it. This only exists due to GD limitations.",
            "Ok");
        spoofLayer->setTag(0);
        m_fields->m_forceExit = true;

        MenuLayer::FLAlert_Clicked(spoofLayer, true);
    }

    void CreateQuitPopup(const std::string& content) {
        createQuickPopup("Save?", content, "No", "Yes",
                         [this](FLAlertLayer*, const bool btn2)
                         {
                             if (btn2)
                                 Save();
                             else
                                 Quit();
                         });
    }

    void FLAlert_Clicked(FLAlertLayer* layer,
                         const bool is_quit_button) override
    {
        if (layer->getTag() != 0 || m_fields->m_forceExit)
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
            Quit();
        } else if (shouldSaveOption == "Ask")
        {
            CreateQuitPopup("Would you like to save (backup) to RobTop servers before quitting?");
        } else if (shouldSaveOption == "If Load Succeeded")
        {
            if (g_syncSucceeded)
                Save();
            else
                Quit();
        } else if (shouldSaveOption == "Ask if load Failed")
        {
            if (g_syncSucceeded)
                Save();
            else
            {
                const std::string str = g_hasSynced
                                      ? "Would you like to save (backup) to RobTop servers before quitting? WARNING: The last load failed!"
                                      : "Would you like to save (backup) to RobTop servers before quitting? WARNING: You haven't loaded this session! This will overwrite what's currently on the servers.";
                CreateQuitPopup(str);
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
