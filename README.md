# LED Rose Bush

This project is a 3D printed rose bush that uses RGB LEDs to create a beautiful lighting effect. It was originally created as a unique Valentine's Day gift but can be enjoyed all year round.

## Parts Required

Here's a list of the parts you'll need to build this LED rose bush:

- 13 RGB LEDs (either addressable or normal)
- ESP8266 microcontroller (I used a Wemos D1 Mini)
- Optional: PCA9685 PWM Servo driver
- About 2 meters of wire
- Solder and heatshrink
- Hot glue
- About 200g of filament/resin

## Building the Rose Bush

To build the rose bush, you'll need to 3D print the rose models, which can be found on Thingiverse (link provided below). I used Jerrill's rose model, as it had a sturdy base that was easy to drill into after printing. I then designed the base and tree using CAD software and printed them.

After printing, you'll need to wire the LEDs to the microcontroller and power source. If you have an over supply of PCA9685 PWM Servo drivers like I did, you can use those instead of addressable LEDs. I used an ESP8266 Wemos D1 Mini as the microcontroller and programmed it with a simple sketch that cycles through three different lighting effects.

## Video Demonstrations

To see the LED rose bush in action, check out the video demonstrations linked below:

- [Video 1: Cycling through RGB colors](https://youtu.be/JwHcCJ-tbWU)
- [Video 2: Staggered RGB locations for interesting effects](https://youtu.be/E2YGMtkr8SA)

## Credits

- Jerrill's rose model on Thingiverse: https://www.thingiverse.com/thing:283738
