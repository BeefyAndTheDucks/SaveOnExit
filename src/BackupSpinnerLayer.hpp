#ifndef SAVEONEXIT_BACKUPSPINNERLAYER_HPP
#define SAVEONEXIT_BACKUPSPINNERLAYER_HPP

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BackupSpinnerLayer : public CCLayer
{
public:
    static BackupSpinnerLayer* create() {
        auto ret = new BackupSpinnerLayer();
        if (ret && ret->init())
        {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

protected:
    bool init() {
        if (!CCLayer::init())
            return false;

        const CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        constexpr CCSize size = { 160.f, 170.f };
        constexpr CCPoint center = {size.width / 2,size.height / 2};
        constexpr CCPoint centerAnchor = {0.5f, 0.5f};
        constexpr CCPoint centerTopAnchor = {0.5f, 1.f};

        CCNode* container = CCNode::create();
        container->setID("backing_up_spinner_container");
        container->setContentSize(size);
        container->setAnchorPoint(centerAnchor);
        container->setPosition(
            screenSize.width / 2, screenSize.height / 2);

        CCScale9Sprite* background = CCScale9Sprite::create(
            //"square01_001.png",
            "GJ_square01.png"//,
            /*{
                0.f, 0.f,
                94.f, 94.f
            }*/);

        background->setAnchorPoint(centerAnchor);
        background->setPosition(center);
        background->setPreferredSize(size);

        container->addChild(background);

        CCLabelBMFont* title = CCLabelBMFont::create(
            "Backing up", "goldFont.fnt");
        title->setPosition({ size.width / 2, size.height - 4.f });
        title->setAnchorPoint(centerTopAnchor);
        container->addChild(title);

        TextArea* text = TextArea::create("Hold on...", "chatFont.fnt", 1.0f,
                                          size.width, centerAnchor, 20.0f,
                                          false);
        text->setPosition({size.width / 2, size.height - 40.f});
        title->setAnchorPoint(centerTopAnchor);
        container->addChild(text);

        LoadingSpinner* loadingSpinner = LoadingSpinner::create(120);
        loadingSpinner->setPosition({size.width / 2, size.height / 2 - 12.f});
        container->addChild(loadingSpinner);

        addChild(container);

        return true;
    }
};

#endif //SAVEONEXIT_BACKUPSPINNERLAYER_HPP