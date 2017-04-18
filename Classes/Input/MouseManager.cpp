#include "../InputManager.h"

MouseManager* MouseManager::getInstance() {
    return InputManager::getInstance()->getMouseManager();
}

void MouseManager::startWithNode(cocos2d::Node* node) {
    auto mouseListener = cocos2d::EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(MouseManager::onMouseMove, this);
    mouseListener->onMouseUp = CC_CALLBACK_1(MouseManager::onMouseUp, this);
    mouseListener->onMouseDown = CC_CALLBACK_1(MouseManager::onMouseDown, this);
    mouseListener->onMouseScroll = CC_CALLBACK_1(MouseManager::onMouseScroll, this);
    auto evtDispatcher = node->getEventDispatcher();
    evtDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, node);
}

void MouseManager::onMouseMove(cocos2d::Event* evt) {
    auto mevt = static_cast<cocos2d::EventMouse*>(evt);
    m_pos = mevt->getLocationInView();
    m_delta = mevt->getDelta();
}

void MouseManager::onMouseUp(cocos2d::Event* evt) {
    auto mevt = static_cast<cocos2d::EventMouse*>(evt);
    m_buttonPressed &= ~(uint64_t(1) << mevt->getMouseButton());
}

void MouseManager::onMouseDown(cocos2d::Event* evt) {
    auto mevt = static_cast<cocos2d::EventMouse*>(evt);
    m_buttonPressed |= (uint64_t(1) << mevt->getMouseButton());
}

void MouseManager::onMouseScroll(cocos2d::Event* evt) {

}

const cocos2d::Vec2& MouseManager::getLocation() const {
    return m_pos;
}

const cocos2d::Vec2& MouseManager::getDelta() const {
    return m_delta;
}

bool MouseManager::isButtonPressed(int button) {
    return m_buttonPressed & (uint64_t(1) << button);
}

uint64_t MouseManager::getButtonsPressed() const {
    return m_buttonPressed;
}
