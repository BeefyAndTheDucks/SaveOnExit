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

class $modify(MyMenuLayer, MenuLayer) {
    struct Fields {
        bool m_backingUp;

        BackupSpinnerLayer* m_backupSpinner;

        ListenerHandle m_backupFailed;
        ListenerHandle m_backupSuccess;

        /*
         * Last used FLAlertLayer so we can actually quit the game
         * after a sync.
         */
        FLAlertLayer* m_alertLayer;
    };

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
                                                 m_fields->m_alertLayer, true);
                                         else {
                                             m_fields->m_backingUp = false;
                                             if (m_fields->m_backupSpinner !=
                                                 nullptr)
                                                 m_fields->m_backupSpinner->
                                                     removeFromParent();
                                         }
                                     });
                } else
                    MenuLayer::FLAlert_Clicked(m_fields->m_alertLayer, true);
        });

        m_fields->m_backupSuccess = BackupSuccessfulEvent().listen(
            [this](const bool wasTriggeredByUser) {
                if (wasTriggeredByUser)
                    return;

                MenuLayer::FLAlert_Clicked(m_fields->m_alertLayer, true);

                log::info("Successfully backed up account");

                m_fields->m_backingUp = false;
        });

        return true;
    }

    void Save(FLAlertLayer* layer)
    {
        GJAccountManager* account_manager = GJAccountManager::sharedState();

        log::info("Backing up account data...");

        // Listens to events for us.
        AccountLayer* account_layer = AccountLayer::create();
        account_layer->enterLayer();

        m_fields->m_backupSpinner = BackupSpinnerLayer::create();
        CCDirector::sharedDirector()->getRunningScene()->addChild(
            m_fields->m_backupSpinner);

        m_fields->m_backingUp = true;
        m_fields->m_alertLayer = layer;

        // This triggers backup after getting URL automatically.
        account_manager->getAccountBackupURL();
    }

    void FLAlert_Clicked(FLAlertLayer* layer,
                         const bool is_quit_button) override
    {
        /*
         * Stays like this until we get a crash
         * telling me what part of this errors out.
         */
        const int tag = layer->getTag();
        const bool isBackingUp = m_fields->m_backingUp;
        if (tag != 0 || isBackingUp)
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
            Save(layer);
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
                                     Save(layer);
                                 else
                                      MenuLayer::FLAlert_Clicked(
                                          layer, is_quit_button);
                             });
        } else if (shouldSaveOption == "If Load Succeeded")
        {
            if (g_syncSucceeded)
                Save(layer);
            else
                MenuLayer::FLAlert_Clicked(layer, is_quit_button);
        } else if (shouldSaveOption == "Ask if load Failed")
        {
            if (g_syncSucceeded)
                Save(layer);
            else
            {
                createQuickPopup("Save?",
                                 "Would you like to save (backup) to RobTop servers before quitting? WARNING: The last load failed!",
                                 "No", "Yes",
                                 [this, layer, is_quit_button](FLAlertLayer*,
                                                               const bool btn2)
                                 {
                                     if (btn2)
                                         Save(layer);
                                     else
                                         MenuLayer::FLAlert_Clicked(
                                             layer, is_quit_button);
                                });
            }
        }
    }

    // Disable buttons if we're backing up
#define DISABLE_BUTTON_IF_BACKING_UP(func) \
    void func(CCObject* sender) {          \
        if (m_fields->m_backingUp)         \
            return;                        \
                                           \
        MenuLayer::func(sender);           \
    }

    DISABLE_BUTTON_IF_BACKING_UP(onAchievements)
    DISABLE_BUTTON_IF_BACKING_UP(onCreator)
    DISABLE_BUTTON_IF_BACKING_UP(onDaily)
    DISABLE_BUTTON_IF_BACKING_UP(onDiscord)
    //DISABLE_BUTTON_IF_BACKING_UP(onEveryplay)
    DISABLE_BUTTON_IF_BACKING_UP(onFacebook)
    //DISABLE_BUTTON_IF_BACKING_UP(onFreeLevels)
    //DISABLE_BUTTON_IF_BACKING_UP(onFullVersion)
    //DISABLE_BUTTON_IF_BACKING_UP(onGameCenter)
    DISABLE_BUTTON_IF_BACKING_UP(onGarage)
    //DISABLE_BUTTON_IF_BACKING_UP(onGooglePlayGames)
    DISABLE_BUTTON_IF_BACKING_UP(onMoreGames)
    DISABLE_BUTTON_IF_BACKING_UP(onMyProfile)
    DISABLE_BUTTON_IF_BACKING_UP(onNewgrounds)
    DISABLE_BUTTON_IF_BACKING_UP(onOptions)
    DISABLE_BUTTON_IF_BACKING_UP(onPlay)
    DISABLE_BUTTON_IF_BACKING_UP(onQuit)
    DISABLE_BUTTON_IF_BACKING_UP(onRobTop)
    DISABLE_BUTTON_IF_BACKING_UP(onStats)
    //DISABLE_BUTTON_IF_BACKING_UP(onTrailer)
    DISABLE_BUTTON_IF_BACKING_UP(onTwitch)
    DISABLE_BUTTON_IF_BACKING_UP(onTwitter)
    DISABLE_BUTTON_IF_BACKING_UP(onYouTube)
    DISABLE_BUTTON_IF_BACKING_UP(openOptions)
};

$execute {
    auto syncFailed = SyncFailedEvent().listen(
            [](const bool, const BackupAccountError, const int) {
                g_syncSucceeded = false;
    });
    syncFailed.leak();

    auto syncSuccess = SyncSuccessfulEvent().listen(
        [](const bool) {
            g_syncSucceeded = true;
    });
    syncSuccess.leak();
}
