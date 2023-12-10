#pragma once
/******************************************************************************
     Stéphane Lepoutère (c) 2023



******************************************************************************/
#include <Arduino.h>
#include <BLEDevice.h>

class Govee
{
private:
    
public:
    Govee() {};
    ~Govee() {};
    Govee(BLEAddress adresse, String nom)
    {
        m_adresse = new BLEAddress(adresse);
        m_nom = nom;
    }
    BLEAddress *m_adresse;
    String m_nom;
    float temperature;
    float hygrometrie;
    char batterie;
    char rssi;
    friend bool operator==(const Govee &l, const Govee &r);
    friend bool operator==(const BLEAddress &l, const Govee &r);
};

