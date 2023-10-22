// This Arduino sketch controls a NeoPixel-based lighting system with HomeKit integration.

#include <Arduino.h>
#include <arduino_homekit_server.h>
#include <Adafruit_NeoPixel.h>
#include "wifi_info.h"

// Constants
#define LED_PIN D6   // Pin to which the NeoPixel LED strip is connected
#define NUM_LEDS 18  // Number of NeoPixel LEDs in the strip

// NeoPixel setup
Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// HomeKit Accessory State Variable References
extern "C" homekit_server_config_t accessory_config;
extern "C" homekit_characteristic_t lightOn;
extern "C" homekit_characteristic_t lightBrightness;
extern "C" homekit_characteristic_t lightSaturation;
extern "C" homekit_characteristic_t lightHue;
extern "C" homekit_characteristic_t function1Switch;
extern "C" homekit_characteristic_t function2Switch;
extern "C" homekit_characteristic_t function3Switch;
extern "C" homekit_characteristic_t function4Switch;

homekit_value_t homeKitFalse;
homekit_value_t homeKitTrue;
homekit_value_t homeKitZeroInt;
homekit_value_t homeKitZeroFloat;

// An array of function switches for HomeKit
homekit_characteristic_t* functionSwitches[] = {
  &function1Switch, &function2Switch, &function3Switch, &function4Switch
};

// Timer arrays for various timing intervals
int timerPrevious[] = { 0, 0, 0, 0, 0 };           // Previous time stamps
int timerIntervals[] = { 10, 100, 100, 100, 50 };  // Intervals in milliseconds

// Arrays defining LED positions and patterns
int bounceSequence[] = { 11, 5, 12, 6, 13, 7, 14, 8, 15, 9, 16, 10 };
int frontPlate[] = { 0, 1, 2, 3, 4 };
int outerRing[] = { 5, 6, 7, 8, 9, 10 };
int innerRing[] = { 11, 12, 13, 14, 15, 16 };
int centre = 17;
int row1[] = { 10, 11, 5 };
int row2[] = { 16, 17, 12 };
int row3[] = { 9, 13, 14, 15, 6 };
int row4[] = { 8, 7 };
int sequencePosition = 0;
int currentOn = 0;
int direction = 1;
int currentOnInner = 1;
int directionInner = 1;
int colourDirRed = 1;
int colourDirGreen = 1;
int colourDirBlue = 1;
bool colourLockRed = true;
bool colourLockGreen = false;
bool colourLockBlue = false;
int colourCycleState = 0;

// Variables for controlling colors and brightness
int red = 0;
int green = 0;
int blue = 0;
int bright = 0;
int hue = 0;
int saturation = 0;
int value = 0;
bool on = false;
bool updateRequired = true;

// Function switch states
bool bounceSwitchBool = false;
bool colourCycleSwitchBool = false;
bool ringChaserSwitchBool = false;
bool spiralChaserSwitchBool = false;

// Function to check if a specified timer interval has passed
bool checkTimer(int timerIndex) {
  int currentTime = millis();
  if (currentTime - timerPrevious[timerIndex] > timerIntervals[timerIndex]) {
    timerPrevious[timerIndex] = currentTime;
    return true;
  } else {
    return false;
  }
}

// Function to convert HSV color to RGB color
void convertHSVToRGB(float h, float s, float v) {
  float h1 = h;
  float s1 = s;
  float v1 = v;
  float r, g, b;
  s /= 100.0;
  v /= 100.0;

  if (s == 0.0) {
    r = g = b = v;
  } else {
    int i;
    float f, p, q, t;

    h /= 60.0;
    i = floor(h);
    Serial.print("I: ");
    Serial.println(i);
    f = h - i;
    p = v * (1.0 - s);
    q = v * (1.0 - s * f);
    t = v * (1.0 - s * (1.0 - f));

    switch (i) {
      case 0:
        r = v;
        g = t;
        b = p;
        break;
      case 1:
        r = q;
        g = v;
        b = p;
        break;
      case 2:
        r = p;
        g = v;
        b = t;
        break;
      case 3:
        r = p;
        g = q;
        b = v;
        break;
      case 4:
        r = t;
        g = p;
        b = v;
        break;
      default:
        r = v;
        g = p;
        b = q;
        break;
    }
  }
  red = round(r * 255);
  green = round(g * 255);
  blue = round(b * 255);
  Serial.print("Hue: ");
  Serial.println(h1);
  Serial.print("Sat: ");
  Serial.println(s1);
  Serial.print("Value: ");
  Serial.println(v1);
  Serial.print("Red: ");
  Serial.println(red);
  Serial.print("Green: ");
  Serial.println(green);
  Serial.print("Blue: ");
  Serial.println(blue);
}

// Function to set the state of a function switch
void setFunctionSwitch(int functionNumber, const homekit_value_t v) {
  for (int i = 0; i < 4; i++) {
    if (i == functionNumber) {
      functionSwitches[i]->value.bool_value = v.bool_value;
    } else {
      functionSwitches[i]->value.bool_value = false;
    }
    homekit_characteristic_notify(functionSwitches[i], functionSwitches[i]->value);
    delay(10);
  }
}

// Setter functions for HomeKit characteristics
void setOn(const homekit_value_t v) {
  on = v.bool_value;
  lightOn.value.bool_value = on;
  homekit_characteristic_notify(&lightOn, lightOn.value);
  updateRequired = true;
}

void setBrightness(const homekit_value_t v) {
  value = v.int_value;
  bright = map(value, 0, 100, 55, 255);
  pixels.setBrightness(bright);

  lightBrightness.value.int_value = value;
  homekit_characteristic_notify(&lightBrightness, lightBrightness.value);
  updateRequired = true;
}

void setSaturation(const homekit_value_t v) {
  saturation = v.float_value;
  lightSaturation.value.float_value = saturation;
  homekit_characteristic_notify(&lightSaturation, lightSaturation.value);
  updateRequired = true;
}

void setHue(const homekit_value_t v) {
  hue = v.float_value;
  lightHue.value.float_value = hue;
  homekit_characteristic_notify(&lightHue, lightHue.value);
  updateRequired = true;
}

// Setter functions for function switches
void setFunction1Switch(const homekit_value_t v) {
  setFunctionSwitch(0, v);
  bounceSwitchBool = v.bool_value;
  colourCycleSwitchBool = false;
  ringChaserSwitchBool = false;
  spiralChaserSwitchBool = false;
}

void setFunction2Switch(const homekit_value_t v) {
  setFunctionSwitch(1, v);
  colourCycleSwitchBool = v.bool_value;
  bounceSwitchBool = false;
  ringChaserSwitchBool = false;
  spiralChaserSwitchBool = false;
}

void setFunction3Switch(const homekit_value_t v) {
  setFunctionSwitch(2, v);
  ringChaserSwitchBool = v.bool_value;
  bounceSwitchBool = false;
  colourCycleSwitchBool = false;
  spiralChaserSwitchBool = false;
}

void setFunction4Switch(const homekit_value_t v) {
  setFunctionSwitch(3, v);
  spiralChaserSwitchBool = v.bool_value;
  ringChaserSwitchBool = false;
  bounceSwitchBool = false;
  colourCycleSwitchBool = false;
}

// Function to set random RGB color values
void setRandomColor() {
  red = random(0, 255);
  green = random(0, 255);
  blue = random(0, 255);
}

// Function to update all NeoPixel lights with specified RGB color and brightness
void updateAllLights(int green, int red, int blue, int brightness = 255) {
  pixels.fill(pixels.Color(green, red, blue), 0);
}

// Function to update the front panel NeoPixel lights with specified RGB color and brightness
void updateFrontPanel(int green, int red, int blue, int brightness = 255) {
  for (int lightIndex = 0; lightIndex < 5; lightIndex++) {
    pixels.setPixelColor(lightIndex, pixels.Color(green, red, blue));
  }
}

// Function to update all rose-shaped NeoPixel lights with specified RGB color and brightness
void updateAllRoses(int green, int red, int blue, int brightness = 255) {
  for (int lightIndex = 5; lightIndex < NUM_LEDS; lightIndex++) {
    pixels.setPixelColor(lightIndex, pixels.Color(green, red, blue));
  }
}

// HomeKit setup function
void homekit_setup() {
  lightOn.setter = setOn;
  lightBrightness.setter = setBrightness;
  lightSaturation.setter = setSaturation;
  lightHue.setter = setHue;
  function1Switch.setter = setFunction1Switch;
  function2Switch.setter = setFunction2Switch;
  function3Switch.setter = setFunction3Switch;
  function4Switch.setter = setFunction4Switch;
  arduino_homekit_setup(&accessory_config);
}

// Function to control a specific lighting pattern
void bounce() {
  if (checkTimer(1) == true) {
    if (sequencePosition >= 12) {
      sequencePosition = 0;
      setRandomColor();  // Set a random RGB color
    } else {
      sequencePosition++;
    }
    for (int i = 5; i < 17; i++) {
      if (i == bounceSequence[sequencePosition]) {
        pixels.setPixelColor(i, pixels.Color(green, red, blue));  // Set specific LED to the selected color
      } else {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));  // Turn off other LEDs
      }
    }
  }
}

void incrementColors() {
  if (colourLockRed == false) {
    if (red >= 255) {
      colourDirRed = -1;
      colourCycleState++;
    } else if (red <= 0) {
      colourDirRed = 1;
      colourCycleState++;
    }
    red += colourDirRed;
  }
  if (colourLockGreen == false) {
    if (green >= 255) {
      colourDirGreen = -1;
      colourCycleState++;
    } else if (green <= 0) {
      colourDirGreen = 1;
      colourCycleState++;
    }
    green += colourDirGreen;
  }
  if (colourLockBlue == false) {
    if (blue >= 255) {
      colourDirBlue = -1;
      colourCycleState++;
    } else if (blue <= 0) {
      colourDirBlue = 1;
      colourCycleState++;
    }
    blue += colourDirBlue;
  }

  switch (colourCycleState) {
    case 0:
      colourLockRed = true;
      colourLockGreen = false;
      colourLockBlue = false;
      break;
    case 1:
      colourLockRed = false;
      colourLockGreen = true;
      colourLockBlue = false;
      break;
    case 2:
      colourLockRed = false;
      colourLockGreen = false;
      colourLockBlue = true;
      break;
    case 3:
      colourLockRed = true;
      colourLockGreen = true;
      colourLockBlue = false;
      break;
    case 4:
      colourLockRed = false;
      colourLockGreen = true;
      colourLockBlue = true;
      break;
    case 5:
      colourLockRed = true;
      colourLockGreen = false;
      colourLockBlue = true;
      break;
    default:
      colourCycleState = 0;
  }
}

void spiralChaser() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i == currentOn) {
      pixels.setPixelColor(i, pixels.Color(green, red, blue));
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }
  currentOn += direction;
  if (currentOn >= NUM_LEDS) {
    direction = -1;
    setRandomColor();
  } else if (currentOn <= 5) {
    direction = 1;
    setRandomColor();
  }
}

void ringChaser() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i == currentOn || i == currentOnInner) {
      pixels.setPixelColor(i, pixels.Color(green, red, blue));
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }
  currentOn += direction;
  currentOnInner += directionInner;

  if (currentOn >= 10) {
    direction = -1;
    setRandomColor();
  } else if (currentOn <= 5) {
    direction = 1;
    setRandomColor();
  }

  if (currentOnInner >= 18) {
    directionInner = -1;
  } else if (currentOnInner <= 11) {
    directionInner = 1;
  }
}

// Arduino setup function
void setup() {
  Serial.begin(115200);         // Initialize serial communication
  wifi_connect();               // Connect to Wi-Fi network
  pixels.begin();               // Initialize NeoPixel
  updateAllLights(0, 0, 0, 0);  // Initialize NeoPixel lights
  homekit_setup();              // Initialize HomeKit configuration
  homeKitFalse.format = homekit_format_bool;
  homeKitFalse.bool_value = 0;
  homeKitTrue.format = homekit_format_bool;
  homeKitTrue.bool_value = 1;
  homeKitZeroInt.format = homekit_format_int;
  homeKitZeroInt.int_value = 0;
  homeKitZeroFloat.format = homekit_format_float;
  homeKitZeroFloat.float_value = 0;
  pixels.setBrightness(255);
  pixels.show();

  setFunction1Switch(homeKitFalse);
  setFunction2Switch(homeKitFalse);
  setFunction3Switch(homeKitFalse);
  setFunction4Switch(homeKitFalse);
  setOn(homeKitFalse);
  setBrightness(homeKitZeroInt);
  setSaturation(homeKitZeroInt);
  setHue(homeKitZeroInt);
}

// Arduino main loop
void loop() {
  if (checkTimer(0)) {
    arduino_homekit_loop();  // Handle HomeKit events

    if (updateRequired == true) {
      convertHSVToRGB(hue, saturation, value);  // Convert HSV color to RGB
      updateRequired = false;
    }
    if (bounceSwitchBool == true) {
      bounce();                                 // Execute a lighting sequence
      updateFrontPanel(green, red, blue, 255);  // Update front panel lights
      pixels.setBrightness(bright);
      pixels.show();
    } else if (colourCycleSwitchBool == true) {
      if (checkTimer(2) == true) {
        incrementColors();
        updateFrontPanel(green, red, blue, 255);  // Update front panel lights
        updateAllRoses(green, red, blue, 255);
        pixels.setBrightness(bright);

        pixels.show();
      }
    } else if (ringChaserSwitchBool == true) {
      if (checkTimer(2) == true) {
        spiralChaser();
        pixels.setBrightness(bright);
        pixels.show();
      }
    } else if (spiralChaserSwitchBool == true) {
      if (checkTimer(3) == true) {
        ringChaser();
        updateFrontPanel(green, red, blue);
        pixels.setBrightness(bright);

        pixels.show();
      }
    } else {
      if (checkTimer(4) == true) {
        updateFrontPanel(green, red, blue);
        updateAllRoses(green, red, blue);
        if (pixels.canShow()) {
          pixels.setBrightness(bright);

          pixels.show();
        }
      }
    }
  }
}
