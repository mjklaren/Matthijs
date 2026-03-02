# Matthijs - Our first Halloween Animatronic

Meet Matthijs, our first animatronic built for Halloween 2025. Matthijs (thanks to my kids for the name) is our first home made animatronic using a plastic skeleton, a couple of actuators and more.

Click on the image below to see Matthijs in action.
[![Matthijs in action during Halloween 2025](https://img.youtube.com/vi/bRoBJLLl_AM/maxresdefault.jpg)](https://www.youtube.com/watch?v=bRoBJLLl_AM)

This animatronic was build using an existing skeleton ordered from Amazon (about 170cm in height), mounted on a custom wooden framework. The motion, sounds and light effects are created using the following components:

- A skeleton (obviously) and cape.
- 4 linear actuators for lifing the arm, stretching the arm, moving the staff forward and letting the whole skeleton lean forward.
- Some nuts and bolts and other parts for the shoulder mechanism.
- Two L298N DC motor controllers for the linear actuators.
- 3 SG90 servo motors (180 degree) and some metal wire to move the head and the jaw.
- A PCA9685 I2C servo controller for the servos.
- One Raspberry Pi Pico.
- Two multi-color LEDs for the eyes.
- A candle shaped lightbulb, a staff made from carton (which unfortunately broke after performing) and a relay board to turn the candle on.
- A strip of WS2812B LEDs and a few capacitors to limit noise on the powerlines.
- A DY-SV17F soundboard to play sound effects and an amplifier.
- ...and plenty of tiewraps, wiring and tape!

For more details on the build, check [these photos in the build folder](https://github.com/mjklaren/Matthijs/tree/main/build). Sound files where generated using Elevenlabs, a bit of imagination and Audacity adding some effects. Sound files can be found in the [sound folder](https://github.com/mjklaren/Matthijs/tree/main/Sounds).

The software for controlling the animatronic was written in C and used [the TISM framework on Github](https://github.com/mjklaren/TISM). TISM is a framework to allow multiple tasks to run on just one Raspberry Pi microcontroller. In this build, TISM's built-in scheduler was pretty usefull in coordinating lights and movements, so check the source code.
