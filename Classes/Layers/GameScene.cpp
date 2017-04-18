#include "cocostudio/CocoStudio.h"
#include "CCComponentLua.h"
#include "GameScene.h"
#include "../Data/GameDesc.h"
#include "GameManager.h"

cocos2d::Scene* GameScene::createScene(SceneDesc* desc) {
    auto scene = cocos2d::Scene::create();
    auto layer = GameScene::create(desc);
    scene->addChild(layer);
    return scene;
}

GameScene* GameScene::create(SceneDesc* desc) {
    auto gs = new (std::nothrow) GameScene();
    if (gs && gs->initWithSceneDesc(desc)) {
        gs->autorelease();
        return gs;
    }
    CC_SAFE_DELETE(gs);
    return nullptr;
}

bool GameScene::initWithSceneDesc(SceneDesc *desc) {
    cocos2d::Node* sn;
    if (!desc->filename.empty()) {
        sn = cocos2d::CSLoader::createNode(desc->filename);
        this->addChild(sn);
    }
    else {
        sn = this;
    }
    sn->setName(desc->name);
    for (auto scriptComponent : desc->components) {
        auto luaState = cocos2d::LuaEngine::getInstance()->getLuaStack()->getLuaState();
        auto sc = cocos2d::ComponentLua::create(scriptComponent.filename);
        if (!sc) {
            return false;
        }
        sc->setName(scriptComponent.name);
        sn->addComponent(sc);
    }
    return true;
}
