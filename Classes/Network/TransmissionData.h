#ifndef TRANSMISSIONDATA_H
#define TRANSMISSIONDATA_H

#include "cocos2d.h"

class DeliveryNotificationManager;
class TransmissionData : public cocos2d::Ref {
public:
    virtual void onDeliveryFail(DeliveryNotificationManager*) const = 0;
    virtual void onDeliverySucc(DeliveryNotificationManager*) const = 0;
};

#endif // TRANSMISSIONDATA_H
