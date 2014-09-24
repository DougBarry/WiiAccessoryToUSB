WiiAccessoryToUSB
----------
**Doug Barry 20140924**

**Info:**

This Arduino project allows you to connect a Wii accessory to an Arduino and
through use of the UnoJoy project, make it appear as a game controller on PC
and PS3. It supports the standard Wii Nunchuck including accelerometers,
and the Hori FightStick released for Tatsunoko Vs Capcom.

I have used this in conjunction with x360ce on a Windows PC to play games
that support the XInput system to play Ultra Street Fighter 4 and other
fighting games.

- [UnoJoy][1]
- [x360ce][2]


**What needs adding:**

 - Calibration routine.
 - Input from other console controllers? Plenty of spare D pins.

**Note:**
I have only tried this on an Arduino Uno R3, compiled using
ArduinoIDE 1.5.7+, so YMMV. I have made some attempts to make this compatible
with older versions of the Ardunio IDE, but I do not know if they will work.

**Acknowledgements.**
This code draw inspiration and code from the following:

 - [WiiClassic][3] Test Code by Tim Hirzel
 - [Nunchuck functions][4] by Tod E. Kurt: 2007-11 Tod E. Kurt
 - Wii Extension to USB HID project:

     - https://code.google.com/p/wii-ext-to-usb/
     - http://slotcover.wordpress.com/2009/03/12/wii-extension-controller-to-usb-project/

[1]:https://code.google.com/p/unojoy/
[2]:https://code.google.com/p/x360ce/source/checkout
[3]:http://playground.arduino.cc/Main/WiiClassicController
[4]:http://todbot.com/blog/