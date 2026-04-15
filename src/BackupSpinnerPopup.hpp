#ifndef SAVEONEXIT_BACKUPSPINNERLAYER_HPP
#define SAVEONEXIT_BACKUPSPINNERLAYER_HPP

#include <Geode/Geode.hpp>
#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class QuitAnywayEvent : public Event<QuitAnywayEvent, bool()>
{
public:
    using Event::Event;
};

class BackupSpinnerPopup : public Popup
{
public:
    static BackupSpinnerPopup* create(const std::string& text, const bool enableQuitAnywayButton) {
        auto ret = new BackupSpinnerPopup();
        if (ret && ret->init(text, enableQuitAnywayButton))
        {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void close()
    {
        onClose(nullptr);
    }

protected:
    // Don't close with esc/equivalent
    void keyBackClicked() override {}

    bool init(const std::string& text, const bool enableQuitAnywayButton) {
        constexpr CCSize size = { 225.f, 200.f };

        if (!Popup::init(size))
            return false;

        // Remove the close button
        setCloseButtonSpr(CCSprite::create(), 0.f);

        const CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

        this->setTitle(text);

        LoadingSpinner* loadingSpinner = LoadingSpinner::create(120);
        loadingSpinner->setPosition({screenSize.width / 2.f, screenSize.height / 2.f});
        this->addChild(loadingSpinner);

        this->setID("BackupSpinner"_spr);

        constexpr float delayBeforeQuitAnywayButtonSeconds = 10.0f;
        if (enableQuitAnywayButton)
            scheduleOnce(schedule_selector(BackupSpinnerPopup::showQuitAnywayButton), delayBeforeQuitAnywayButtonSeconds);

        return true;
    }

    void showQuitAnywayButton(float)
    {
        CCMenu* menu = CCMenu::create();

        const auto sprite = ButtonSprite::create("Quit");
        const auto button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(BackupSpinnerPopup::onQuitAnyway));

        CCLabelBMFont* label = CCLabelBMFont::create("It's taking a long time, quit now?", "chatFont.fnt");
        label->setPositionY(-50);
        button->setPositionY(-76);

        menu->addChild(label);
        menu->addChild(button);

        this->addChild(menu);
    }

    void onQuitAnyway(CCObject*)
    {
        QuitAnywayEvent().send();
    }
};

#endif //SAVEONEXIT_BACKUPSPINNERLAYER_HPP