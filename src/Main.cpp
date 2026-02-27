#include <Geode/Geode.hpp>

#include "events/SyncFailedEvent.hpp"
#include "events/SyncSuccessfulEvent.hpp"

using namespace geode::prelude;

$on_game(Loaded) {
	if (!Mod::get()->getSettingValue<bool>("load-on-startup"))
		return;

	GJAccountManager* account_manager = GJAccountManager::sharedState();

	log::info("Syncing account data...");

	// Listens to events for us.
	AccountLayer* account_layer = AccountLayer::create();
	account_layer->enterLayer();

	auto syncFailed = SyncFailedEvent().listen(
		[](const bool wasTriggeredByUser, const BackupAccountError,
		   const int response) {
			if (wasTriggeredByUser)
				return;

			if (Mod::get()->getSettingValue<bool>("show-load-failure-popup"))
			{
				const gd::string str = CCString::createWithFormat(
					"Failed to sync. Error Code: %i", response)->getCString();
				FLAlertLayer* alert =
					FLAlertLayer::create("Error", str, "Okay");
				alert->show();
			}

			log::warn("Syncing account failed");
	});
	syncFailed.leak();

	auto syncSuccess = SyncSuccessfulEvent().listen(
		[](const bool wasTriggeredByUser) {
			if (wasTriggeredByUser)
				return;

			if (Mod::get()->getSettingValue<bool>("show-load-success-popup"))
			{
				FLAlertLayer* alert = FLAlertLayer::create(
					"Load successful",
					"Successfully loaded (synced) save data from RobTob servers.",
					"Okay");
				alert->show();
			};

			log::info("Successfully synced account");
	});
	syncSuccess.leak();

	// This triggers sync after getting URL automatically.
	account_manager->getAccountSyncURL();
}
