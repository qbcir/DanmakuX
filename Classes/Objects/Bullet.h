#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h"

class BulletDesc;
class Bullet : public GameObject
{
public:
    static Bullet* createFromDesc(BulletDesc* desc);
    virtual ~Bullet();
    void onCollision(GameObject *obj);
    BulletDesc* getBulletDesc();
protected:
    bool initFromDesc(BulletDesc* desc);
    BulletDesc* m_bulletDesc = nullptr;
};

#endif // BULLET_H
