#include <homekit/homekit.h>
#include <homekit/characteristics.h>

bool identify = false;

// Function to handle accessory identification
void roseBushIdentify(homekit_value_t _value) {
  identify = true;
}

// Light Group All
homekit_characteristic_t lightName = HOMEKIT_CHARACTERISTIC_(NAME, "Roses"); // Name of the accessory
homekit_characteristic_t lightOn = HOMEKIT_CHARACTERISTIC_(ON, false); // On/Off state
homekit_characteristic_t lightBrightness = HOMEKIT_CHARACTERISTIC_(BRIGHTNESS, 100); // Brightness level
homekit_characteristic_t lightSaturation = HOMEKIT_CHARACTERISTIC_(SATURATION, (float)0); // Saturation level
homekit_characteristic_t lightHue = HOMEKIT_CHARACTERISTIC_(HUE, (float)180); // Hue (color) value

// Function 1
homekit_characteristic_t function1Name = HOMEKIT_CHARACTERISTIC_(NAME, "Bounce"); // Name of function 1
homekit_characteristic_t function1Switch = HOMEKIT_CHARACTERISTIC_(ON, false); // On/Off state for function 1

// Function 2
homekit_characteristic_t function2Name = HOMEKIT_CHARACTERISTIC_(NAME, "Colour Cycle"); // Name of function 2
homekit_characteristic_t function2Switch = HOMEKIT_CHARACTERISTIC_(ON, false); // On/Off state for function 2

// Function 3
homekit_characteristic_t function3Name = HOMEKIT_CHARACTERISTIC_(NAME, "Function 3"); // Name of function 3
homekit_characteristic_t function3Switch = HOMEKIT_CHARACTERISTIC_(ON, false); // On/Off state for function 3

// Function 4
homekit_characteristic_t function4Name = HOMEKIT_CHARACTERISTIC_(NAME, "Function 4"); // Name of function 4
homekit_characteristic_t function4Switch = HOMEKIT_CHARACTERISTIC_(ON, false); // On/Off state for function 4

// Define the accessories and services
homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_lightbulb, .services=(homekit_service_t*[]) {
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "Rose Bush"), // Accessory name
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "matthewpicone.com"), // Manufacturer
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "SCU-2023-001"), // Serial number
            HOMEKIT_CHARACTERISTIC(MODEL, "ROSEBUSH_V2"), // Model
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.5"), // Firmware revision
            HOMEKIT_CHARACTERISTIC(IDENTIFY, roseBushIdentify), // Identify function
            NULL
        }),
        HOMEKIT_SERVICE(LIGHTBULB, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
            &lightName,
            &lightOn,
            &lightBrightness,
            &lightSaturation,
            &lightHue,
            NULL
        }),

        HOMEKIT_SERVICE(SWITCH, .primary=false, .characteristics=(homekit_characteristic_t*[]) {
            &function1Name,
            &function1Switch,
            NULL
        }),
        HOMEKIT_SERVICE(SWITCH, .primary=false, .characteristics=(homekit_characteristic_t*[]) {
            &function2Name,
            &function2Switch,
            NULL
        }),
        HOMEKIT_SERVICE(SWITCH, .primary=false, .characteristics=(homekit_characteristic_t*[]) {
            &function3Name,
            &function3Switch,
            NULL
        }),
        HOMEKIT_SERVICE(SWITCH, .primary=false, .characteristics=(homekit_characteristic_t*[]) {
            &function4Name,
            &function4Switch,
            NULL
        }),
        NULL
    }), 
    NULL
};

// Configure the HomeKit server
homekit_server_config_t accessory_config = {
    .accessories = accessories,
    .password = "111-11-111" // HomeKit setup code
};
