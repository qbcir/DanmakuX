#ifndef GAMEDESC_H
#define GAMEDESC_H

#include "cocos2d.h"
#include "ScriptComponentDesc.h"

class SceneDesc : public cocos2d::Ref {
public:
    std::string name;
    std::string role;
    std::string filename;
    std::vector<ScriptComponentDesc> components;
};

class GameDesc : public cocos2d::Ref
{
public:
    static GameDesc* fromJson(const std::string& fname);
    std::string version;
    std::string name;
    cocos2d::Vector<SceneDesc*> scenes;
};

#endif // GAMEDESC_H
