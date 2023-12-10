#include <BLEobjet.h>

long getLongFromByteArray(byte *bytes, int position)
{
    long result = bytes[position + 1] * 256 + bytes[position];
    return result;
}

// class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
// {
//     void onResult(BLEAdvertisedDevice advertisedDevice)
//     {
//         String deviceName(advertisedDevice.getName().c_str());
//         // devicename is something like Govee_H5074_C0A6
//         // H5074= type of govee device
//         // C0A6 = last four digits of this device address
//         if (deviceName.startsWith("Govee_H5074_"))
//         {
//             unsigned long pll = advertisedDevice.getPayloadLength();
//             uint8_t *payload = advertisedDevice.getPayload();

//             if (pll != 40)
//                 Serial.printf("%s length[%d] advertisement...no sensor data!\n", advertisedDevice.getName().c_str(), pll);

//             // we see length 29, 40, 56 packets for Govees (adv_data_len=29 always, scan_rsp_len=0,11,27 resp.
//             // only the length 40 packets actually contain sensor data in the ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE (0xFF)
//             // Sensor data for govee = 88:EC:00:19:09:5C:15:64:02
//             //                                  ^^^^^ temperature
//             //                                        ^^^^^ humidity
//             //                                              ^^ battery level
//             if (pll == 40)
//             {
//                 double celsius = ((double)getLongFromByteArray(payload, 34)) / 100.0;
//                 double fahrenheit = (celsius * 9.0 / 5.0) + 32.0;
//                 double relativeHumidity = ((double)getLongFromByteArray(payload, 36)) / 100.0;
//                 int battery = payload[38];
//                 Serial.printf("{\"name\":\"%s\",\"celsius\":%.1lf,\"fahrenheit\":%.1lf,\"humidity\":%.1lf,\"battery\":%d}\n",
//                               deviceName.c_str(), celsius, fahrenheit, relativeHumidity, battery);
//             }
//         }
//     }
// };

void BLEobjet::onResult(BLEAdvertisedDevice advertisedDevice)
{
    String deviceName(advertisedDevice.getName().c_str());

    if (deviceName.startsWith("GVH5075_"))
    {
        unsigned long pll = advertisedDevice.getPayloadLength();
        uint8_t *payload = advertisedDevice.getPayload();

        if (pll == 31)
        {
            uint32_t data = ((uint32_t)payload[26] << 16) + ((uint32_t)payload[27] << 8) + (uint32_t)payload[28];
            double temperature = 0;
            double humidite = 0;

            if ((data & 0x00800000) & 0x00800000)
            {
                temperature = (double)(data & !0x800000) * -1;
            }
            else
            {
                temperature = (double)data;
            }

            temperature /= 10000;
            humidite = ((double)(data % 1000)) / 10;

            BLEAddress adresse = advertisedDevice.getAddress();
            int battery = payload[29];
            int rssi = advertisedDevice.getRSSI();

            for (uint8_t i = 0; i < 8; i++)
            {
                if (govee[i].m_adresse->equals(adresse))
                {
                    Serial.println(govee[i].m_nom);
                    govee[i].temperature = temperature;
                    govee[i].hygrometrie = humidite;
                    govee[i].batterie = battery;
                    govee[i].rssi = rssi;
                    Serial.printf("   %.1f°C %.0f%HR batt : %d/100 rssi : %ddB\n", temperature, humidite, battery, rssi);
                }
            }
        }
        // else
        // {
        //     Serial.printf("[%s] length[%d] advertisement...no sensor data!\n", advertisedDevice.getName().c_str(), pll);
        //     Serial.printf("     manufacturer : %x \n", advertisedDevice.getManufacturerData());
        //     for (uint x = 0; x < pll; x++)
        //         Serial.printf("%2x:", payload[x]);
        //     Serial.println("");
        // }
    }
}

BLEobjet::BLEobjet()
{
    // TODO : passer ça dans un fichier csv
    govee[0] = Govee(BLEAddress("A4:C1:38:1A:00:22"), "garage");
    govee[1] = Govee(BLEAddress("A4:C1:38:4B:28:8E"), "chambre Martin");
    govee[2] = Govee(BLEAddress("A4:C1:38:F3:DC:A8"), "salle de bain");
    govee[3] = Govee(BLEAddress("A4:C1:38:3D:F7:8A"), "chambre parents");
    govee[4] = Govee(BLEAddress("A4:C1:38:86:C6:6F"), "Salle à manger");
    govee[5] = Govee(BLEAddress("A4:C1:38:2A:88:5D"), "Cuisine");
    govee[6] = Govee(BLEAddress("A4:C1:38:7B:49:14"), "Chambre Maxine");
    govee[7] = Govee(BLEAddress("A4:C1:38:0F:48:6E"), "Véranda");

    scanTime = 10;        // In seconds
}

BLEobjet::~BLEobjet()
{
}

void BLEobjet::init()
{
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();        // create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new BLEobjet(), true /* wantDuplicates ? yes */);
    pBLEScan->setActiveScan(false);         // active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);                // less or equal setInterval value
}

void BLEobjet::scan()
{
    Serial.printf("\nStarting scan...\n");
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.printf("Nettoyage scan...\n");
    pBLEScan->clearResults();
}
