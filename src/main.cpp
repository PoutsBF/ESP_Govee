#include <Arduino.h>

#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>

#include <BLEobjet.h>

#include <Govee.h>

#include <U8x8lib.h>
#include <Wire.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);        // OLEDs without Reset of the Display

BLEobjet bleObjet;

void print_wakeup_reason();

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



void setup()
{
    Serial.begin(115200);
    // Print the wakeup reason for ESP32
    print_wakeup_reason();

    u8x8.begin();
    u8x8.setFlipMode(1);        // set number from 1 to 3, the screen word will rotary 180
    u8x8.setFont(u8x8_font_chroma48medium8_r);

    u8x8.clearDisplay();
    u8x8.setCursor(0, 0);
    u8x8.print("Stephane\n     Lepoutere");
    delay(2000);

    bleObjet.init();

}

void loop()
{
    u8x8.clearDisplay();
    u8x8.setCursor(0, 0);
    u8x8.print("Scan...\n");
    
    bleObjet.scan();

    u8x8.print("fin.");

    delay(1000 * 60 * 60 * 4);        // toutes les milli x secondes x minutes x 4 heures

    

}

void print_wakeup_reason()
{
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
        case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
        case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
        case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
        case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
        default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
    }

    
}