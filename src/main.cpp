#include <Arduino.h>

#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>


// struct donnees 
// {
//     uint32_t 
// };
// String adresses[] = {'A4:C1:38:7B:49:14' : {'name' : 'GVH5075_4914', 'identifiant' : 'chambre Maxine'},
//                      'A4:C1:38:0F:48:6E' : {'name' : 'GVH5075_486E', 'identifiant' : 'véranda'},
//                      'A4:C1:38:1A:00:22' : {'name' : 'GVH5075_0022', 'identifiant' : 'garage'},
//                      'A4:C1:38:4B:28:8E' : {'name' : 'GVH5075_288E', 'identifiant' : 'chambre Martin'},
//                      'A4:C1:38:F3:DC:A8' : {'name' : 'GVH5075_DCA8', 'identifiant' : 'salle de bain'},
//                      'A4:C1:38:3D:F7:8A' : {'name' : 'GVH5075_F78A', 'identifiant' : 'chambre parents'},
//                      'A4:C1:38:86:C6:6F' : {'name' : 'GVH5075_C66F', 'identifiant' : 'Salle à manger'},
//                      'A4:C1:38:2A:88:5D' : {'name' : 'GVH5075_885D', 'identifiant' : 'mobile'};

BLEScan *pBLEScan;

class Govee
{
    public:
        Govee(){};
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
        friend bool operator==(const Govee &l, const Govee &r)
        {
            return (r.m_adresse == l.m_adresse);
        };
        friend bool operator==(const BLEAddress &l, const Govee &r)
        {
            return (r.m_adresse->equals(l));
        };
};

Govee govee[8];

long getLongFromByteArray(byte *bytes, int position)
{
    long result = bytes[position + 1] * 256 + bytes[position];
    return result;
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        String deviceName(advertisedDevice.getName().c_str());

        if (deviceName.startsWith("GVH5075_"))
        {
            unsigned long pll = advertisedDevice.getPayloadLength();
            uint8_t *payload = advertisedDevice.getPayload();

            if (pll != 31)
            {
                Serial.printf("[%s] length[%d] advertisement...no sensor data!\n", advertisedDevice.getName().c_str(), pll);
                Serial.printf("     manufacturer : %x \n", advertisedDevice.getManufacturerData());
                for (uint x = 0; x < pll; x++)
                    Serial.printf("%2x:", payload[x]);
                Serial.println("");
            }
            else
            {
                Serial.printf("[%s] length[%d] - ", advertisedDevice.getName().c_str(), pll);

                uint32_t data = ((uint32_t)payload[26] << 16) + ((uint32_t)payload[27] << 8) + (uint32_t)payload[28];
                double celsius = 0;
                double relativeHumidity = 0;

                if ((data & 0x00800000) & 0x00800000)
                {
                    celsius = (double)(data & !0x800000) * -1;
                }
                else 
                {
                    celsius = (double)data;
                }
                
                celsius /= 10000;
                relativeHumidity = ((double)(data % 1000)) / 10;

                BLEAddress adresse = advertisedDevice.getAddress();
                int battery = payload[29];
                int rssi = advertisedDevice.getRSSI();

                for (uint8_t i = 0; i < 8; i++)
                {
                    if (govee[i].m_adresse->equals(adresse))
                    {
                        Serial.print(govee[i].m_nom);
                        govee[i].temperature = celsius;
                        govee[i].hygrometrie = relativeHumidity;
                        govee[i].batterie = battery;
                        govee[i].rssi = rssi;
                    }
                }

                Serial.printf(" {\"name\":\"%s\",\"celsius\":%.1lf,\"humidity\":%.1lf,\"battery\":%d,\"rssi\":%d}\n",
                              deviceName.c_str(), celsius, relativeHumidity, battery, rssi);

            }
        }
    }
};

void setup()
{
    govee[0] = Govee(BLEAddress("A4:C1:38:1A:00:22"), "garage");
    govee[1] = Govee(BLEAddress("A4:C1:38:4B:28:8E"), "chambre Martin");
    govee[2] = Govee(BLEAddress("A4:C1:38:F3:DC:A8"), "salle de bain");
    govee[3] = Govee(BLEAddress("A4:C1:38:3D:F7:8A"), "chambre parents");
    govee[4] = Govee(BLEAddress("A4:C1:38:86:C6:6F"), "Salle à manger");
    govee[5] = Govee(BLEAddress("A4:C1:38:2A:88:5D"), "Cuisine");
    govee[6] = Govee(BLEAddress("A4:C1:38:7B:49:14"), "Chambre Maxine");
    govee[7] = Govee(BLEAddress("A4:C1:38:0F:48:6E"), "Véranda");
    // String adresses[] = {'A4:C1:38:7B:49:14' : {'name' : 'GVH5075_4914', 'identifiant' : 'chambre Maxine'},
    //                      'A4:C1:38:0F:48:6E' : {'name' : 'GVH5075_486E', 'identifiant' : 'véranda'},
    //                      'A4:C1:38:1A:00:22' : {'name' : 'GVH5075_0022', 'identifiant' : 'garage'},
    //                      'A4:C1:38:4B:28:8E' : {'name' : 'GVH5075_288E', 'identifiant' : 'chambre Martin'},
    //                      'A4:C1:38:F3:DC:A8' : {'name' : 'GVH5075_DCA8', 'identifiant' : 'salle de bain'},
    //                      'A4:C1:38:3D:F7:8A' : {'name' : 'GVH5075_F78A', 'identifiant' : 'chambre parents'},
    //                      'A4:C1:38:86:C6:6F' : {'name' : 'GVH5075_C66F', 'identifiant' : 'Salle à manger'},
    //                      'A4:C1:38:2A:88:5D' : {'name' : 'GVH5075_885D', 'identifiant' : 'mobile'};

    Serial.begin(115200);
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();        // create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true /* wantDuplicates ? yes */);
    pBLEScan->setActiveScan(false);         // active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);                // less or equal setInterval value
}

void loop()
{
    int scanTime = 20;        // In seconds
    Serial.printf("\nStarting scan...\n");
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    pBLEScan->clearResults();
    delay(1000 * 60 * 60 * 4); // toutes les milli x secondes x minutes x 4 heures
}