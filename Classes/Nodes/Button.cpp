#include "Button.h"

Button* Button::createWithSprites(
        const std::string& defSp,
        const std::string& pSp,
        const std::string& disSp,
        const std::string& aSp) {
    auto b = new (std::nothrow) Button();
    if (b && b->initWithSprites(defSp, pSp, disSp, aSp)) {
        b->autorelease();
        return b;
    }
    CC_SAFE_DELETE(b);
    return nullptr;
}

bool Button::initWithSprites(
        const std::string& defaultSprite,
        const std::string& pressedSprite,
        const std::string& disabledSprite,
        const std::string& activeSprite) {
    auto defSp = cocos2d::Sprite::create(defaultSprite);
    auto pSp = cocos2d::Sprite::create(pressedSprite);
    auto disSp = cocos2d::Sprite::create(disabledSprite);
    auto aSp = cocos2d::Sprite::create(activeSprite);
    return this->initWithSprites(defSp, pSp, disSp, aSp);
}

bool Button::initWithSprites(
        cocos2d::Sprite* defSp,
        cocos2d::Sprite* pSp,
        cocos2d::Sprite* disSp,
        cocos2d::Sprite* aSp) {
    if (defSp) {
        m_defaultSprite = defSp;
        this->addChild(defSp, 0);
        this->setContentSize(defSp->getContentSize());
        setRadius(defSp->boundingBox().size.width/2);
    }
    if (pSp) {
        m_pressedSprite = pSp;
        this->addChild(pSp, 1);
        this->setContentSize(pSp->getContentSize());
    }
    if (disSp) {
        m_disabledSprite = disSp;
        this->addChild(disSp, 2);
        this->setContentSize(disSp->getContentSize());
    }
    if (aSp) {
        m_activeSprite = aSp;
        this->addChild(aSp, 3);
        this->setContentSize(aSp->getContentSize());
    }
    updateSprites();
    auto evtListener = cocos2d::EventListenerTouchOneByOne::create();
    evtListener->setSwallowTouches(true);
    evtListener->onTouchBegan = CC_CALLBACK_2(Button::onTouchBegan, this);
    evtListener->onTouchMoved = CC_CALLBACK_2(Button::onTouchMoved, this);;
    evtListener->onTouchEnded = CC_CALLBACK_2(Button::onTouchEnded, this);;
    evtListener->onTouchCancelled = CC_CALLBACK_2(Button::onTouchCancelled, this);;
    _eventDispatcher->addEventListenerWithSceneGraphPriority(evtListener, this);
    return true;
}

bool Button::onTouchBegan(cocos2d::Touch* t, cocos2d::Event* evt) {
    auto tgt = static_cast<Button*>(evt->getCurrentTarget());
    if (tgt->m_active) {
        return false;
    }
    auto pos = cocos2d::Director::getInstance()->convertToGL(t->getLocationInView());
    pos = tgt->convertToNodeSpace(pos);
    if (pos.x < -tgt->m_radius
            || pos.x > tgt->m_radius
            || pos.y < -tgt->m_radius
            || pos.y > tgt->m_radius) {
        return false;
    } else {
        if (tgt->m_radiusSquared > pos.length()) {
            tgt->m_active = true;
            if (!tgt->m_holdable && !tgt->m_toggleable) {
                tgt->m_pressed = true;
            }
            if (tgt->m_holdable) {
                tgt->m_pressed = true;
            }
            if (tgt->m_toggleable) {
                tgt->m_pressed = !tgt->m_pressed;
            }
            updateSprites();
            return true;
        }
    }
    return false;
}

void Button::onTouchMoved(cocos2d::Touch* t, cocos2d::Event* evt) {
    auto tgt = static_cast<Button*>(evt->getCurrentTarget());
    if (tgt->m_active) {
        return;
    }
    auto pos = cocos2d::Director::getInstance()->convertToGL(t->getLocationInView());
    pos = tgt->convertToNodeSpace(pos);
    if (pos.x < -tgt->m_radius
            || pos.x > tgt->m_radius
            || pos.y < -tgt->m_radius
            || pos.y > tgt->m_radius) {
        return;
    } else {
       if (tgt->m_radiusSquared > pos.length()) {
           if (tgt->m_holdable) {
               tgt->m_pressed = false;
           }
       } else if (tgt->m_holdable) {
           tgt->m_pressed = false;
       }
    }
    updateSprites();
}

void Button::onTouchEnded(cocos2d::Touch* t, cocos2d::Event* evt) {
    auto tgt = static_cast<Button*>(evt->getCurrentTarget());
    if (!tgt->m_active) {
        return;
    }
    if (tgt->m_holdable) {
        tgt->m_pressed = false;
    }
    if (tgt->m_holdable || tgt->m_toggleable) {
        tgt->m_active = false;
    }
    updateSprites();
}

void Button::onTouchCancelled(cocos2d::Touch* t, cocos2d::Event* evt) {
    auto tgt = static_cast<Button*>(evt->getCurrentTarget());
    tgt->onTouchEnded(t, evt);
}

void Button::setRadius(float r) {
    m_radius = r;
    m_radiusSquared = r * r;
}

void Button::setHoldable(bool h) {
    m_holdable = h;
}

void Button::setToggleable(bool t) {
    m_toggleable = t;
}

bool Button::getPressed() const {
    return m_pressed;
}

void Button::updateSprites() {
    if (!m_enabled) {
        m_disabledSprite->setVisible(true);
    } else {
        m_disabledSprite->setVisible(false);
        if (!m_active) {
            m_pressedSprite->setVisible(false);
            if (!m_pressed) {
                m_activeSprite->setVisible(false);
                m_defaultSprite->setVisible(true);
            } else {
                m_activeSprite->setVisible(true);
            }
        } else {
            m_defaultSprite->setVisible(false);
            m_pressedSprite->setVisible(true);
        }
    }
}
