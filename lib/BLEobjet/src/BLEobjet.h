#pragma once
/******************************************************************************
     Stéphane Lepoutère (c) 2023



******************************************************************************/
#include <Arduino.h>
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>

#include <Govee.h>

class BLEobjet : public BLEAdvertisedDeviceCallbacks
{
private:
    Govee govee[8];
    BLEScan *pBLEScan;

    int scanTime;        // In seconds

    void onResult(BLEAdvertisedDevice advertisedDevice);
    
public:
    BLEobjet();
    ~BLEobjet();
    void init();
    void scan();
};
