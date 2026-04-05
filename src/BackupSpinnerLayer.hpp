#ifndef SAVEONEXIT_BACKUPSPINNERLAYER_HPP
#define SAVEONEXIT_BACKUPSPINNERLAYER_HPP

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BackupSpinnerPopup : public Popup
{
public:
    static BackupSpinnerPopup* create(const std::string& text) {
        auto ret = new BackupSpinnerPopup();
        if (ret && ret->init(text))
        {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

protected:
    bool init(const std::string& text) {
        constexpr CCSize size = { 160.f, 170.f };

        if (!Popup::init(size))
            return false;

        // Remove the close button
        setCloseButtonSpr(CCSprite::create(), 0.f);

        const CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

        this->setTitle(text);

        LoadingSpinner* loadingSpinner = LoadingSpinner::create(120);
        loadingSpinner->setPosition({screenSize.width / 2.f, screenSize.height / 2.f});
        this->addChild(loadingSpinner);

        return true;
    }
};

#endif //SAVEONEXIT_BACKUPSPINNERLAYER_HPP