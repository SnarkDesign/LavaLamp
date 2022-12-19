# LavaLamp
Arduino Lava Lamp

It's about creating a lava lamp setup with dual LED Strips to make some cool 1.5D color designs.
The base controller is an Arduino board and 2 WS2812B LED strips
A 5V 20Watt PS to drive 2X 288 LEDs + arduino board

The controller design sports a remote control to change parametters and an LCD display to display what is going on or do different options like change modes, start-stop, etc. 

The LED strip display was inspired by this Reddit post

https://www.reddit.com/r/generative/comments/zipyft/simplex_noise_landscape_rendered_one_line_at_a/

I went Arduino C (vs Raspeberry Python) and kind of came to something similar.

Using the famous Fastled.h library, Elegoo Arduino Uno 3
