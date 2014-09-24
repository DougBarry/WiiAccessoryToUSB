/*  WiiAccessoryToUSB
 *   Doug Barry 20140924
 *
 *  WiiAccessoryToUSB.ino
 *   Main routine.
 *
 * This Arduino project allows you to connect a Wii accessory to an Arduino and
 * through use of the UnoJoy project, make it appear as a game controller on PC
 * and PS3. It supports the standard Wii Nunchuck including accelerometers,
 * and the Hori FightStick released for Tatsunoko Vs Capcom.
 *
 * I have used this in conjunction with x360ce on a Windows PC to play games
 * that support the XInput system to play Ultra Street Fighter 4 and other
 * fighting games.
 * See * https://code.google.com/p/unojoy/
 *     * https://code.google.com/p/x360ce/source/checkout
 *
 *
 * What needs adding:
 *  - Calibration routine.
 *  - Input from other console controllers? Plenty of spare D pins.
 *
 * Note: I have only tried this on an Arduino Uno R3, compiled using
 * ArduinoIDE 1.5.7+, so YMMV. I have made some attempts to make this compatible
 * with older versions of the Ardunio IDE, but I do not know if they will work.
 *
 * Acknowledgements.
 * This code draw inspiration and code from the following:
 *  - WiiClassic Test Code by Tim Hirzel: http://playground.arduino.cc/Main/WiiClassicController
 *  - Nunchuck functions by Tod E. Kurt: 2007-11 Tod E. Kurt:  http://todbot.com/blog/
 *  - Wii Extension to USB HID project:
 *      https://code.google.com/p/wii-ext-to-usb/
 *      http://slotcover.wordpress.com/2009/03/12/wii-extension-controller-to-usb-project/
 */


/* Define DEBUG if you wish to see serial output describing the data coming in
 * decoded from the I2C bus. This makes it easier to see which digital buttons
 * map where.
 * If this is not defined, then this will run at full speed, and is intended
 * to be used when the Arduino's ATMEGA8U2 has been flashed with the UnoJoy
 * firmware, to appear as a USB gamepad.
 */
//#define DEBUG

#include "UnoJoy.h"
#include <Wire.h>

#include "WiiAccessoryToUSB.h"
#include "WiiAccessories.h"


/* Define if using a WiiChuck
   Sets pin A3 ~VCC
        pin A2 VDD
*/
#define PWR_ANALOGPINS

// settings for Wii HORI Fightstick
#define FS_LS_X_WII_MIN 0
#define FS_LS_X_WII_MAX 63
#define FS_LS_X_WII_INVERT false
#define FS_LS_Y_WII_MIN 0
#define FS_LS_Y_WII_MAX 63
#define FS_LS_Y_WII_INVERT true

#define FS_RS_X_WII_MIN 0
#define FS_RS_X_WII_MAX 31
#define FS_RS_X_WII_INVERT false
#define FS_RS_Y_WII_MIN 0
#define FS_RS_Y_WII_MAX 31
#define FS_RS_Y_WII_INVERT true


WiiClassic  wiiClassicController;
WiiArcade   wiiArcadeController;
WiiNunchuck wiiNunchuckController;

dataForController_t controllerData;
unsigned char controllerType = CNTRL_NONE;

void setup()
{
  setupUnoJoy();

#ifdef PWR_ANALOGPINS
  setPowerPins();
#endif

  controllerType = WiiAccessory::detect();

  if (controllerType == CNTRL_CLASSIC)
  {
    wiiClassicController = WiiClassic();
    wiiClassicController.begin();
    wiiClassicController.update();
  }
  else if (controllerType == CNTRL_ARCADE)
  {
    wiiArcadeController.begin();
    wiiArcadeController.update();
  }
  else
  {
    wiiNunchuckController.begin();
    wiiNunchuckController.update();
  }

#ifdef DEBUG
  Serial.begin(9600);
  delay(5);
  Serial.write(27);       // ESC
  Serial.print("[H");     // Cursor home
  Serial.write(27);       // ESC
  Serial.print("[2J");    // Clear screen
  delay(10);
#endif
}

void loop()
{

  switch (controllerType)
  {
    case CNTRL_CLASSIC:
      controllerData = getControllerDataClassic();
      break;
    case CNTRL_ARCADE:
      controllerData = getControllerDataArcade();
      break;
    case CNTRL_NUNCHUCK:
      controllerData = getControllerDataNunchuck();
      break;
  }

#ifndef DEBUG
  setControllerData(controllerData);
#endif

#ifdef DEBUG
  Serial.print("TYPE: ");
  Serial.print(controllerType, HEX);
  Serial.println();

  for (int i = 4; i < 6; i++)
  {
    Serial.println(wiiClassicController.data[i], BIN);
  }

  Serial.println();
  Serial.println("XAYB UDLR L12 R12 S S H");
  Serial.print(controllerData.triangleOn ? 1 : 0);
  Serial.print(controllerData.circleOn ? 1 : 0);
  Serial.print(controllerData.squareOn ? 1 : 0);
  Serial.print(controllerData.crossOn ? 1 : 0);
  Serial.print(" ");
  Serial.print(controllerData.dpadUpOn ? 1 : 0);
  Serial.print(controllerData.dpadDownOn ? 1 : 0);
  Serial.print(controllerData.dpadLeftOn ? 1 : 0);
  Serial.print(controllerData.dpadRightOn ? 1 : 0);
  Serial.print(" ");
  Serial.print(" ");
  Serial.print(controllerData.l1On ? 1 : 0);
  Serial.print(controllerData.l2On ? 1 : 0);
  Serial.print(" ");
  Serial.print(" ");
  Serial.print(controllerData.r1On ? 1 : 0);
  Serial.print(controllerData.r2On ? 1 : 0);
  Serial.print(" ");

  Serial.print(controllerData.selectOn ? 1 : 0);
  Serial.print(" ");
  Serial.print(controllerData.startOn ? 1 : 0);
  Serial.print(" ");
  Serial.print(controllerData.homeOn ? 1 : 0);

  Serial.println();

  Serial.print("LSX: ");
  Serial.print(controllerData.leftStickX);
  Serial.print("/");
  if ((controllerType == CNTRL_CLASSIC) || (controllerType == CNTRL_ARCADE)) Serial.println(wiiClassicController.leftStickX());
  if (controllerType == CNTRL_NUNCHUCK) Serial.println(wiiNunchuckController.joystickX());

  Serial.print("LSY: ");
  Serial.print(controllerData.leftStickY);
  Serial.print("/");
  if ((controllerType == CNTRL_CLASSIC) || (controllerType == CNTRL_ARCADE)) Serial.println(wiiClassicController.leftStickY());
  if (controllerType == CNTRL_NUNCHUCK) Serial.println(wiiNunchuckController.joystickY());

  Serial.print("RSX: ");
  Serial.print(controllerData.rightStickX);
  Serial.print("/");
  if ((controllerType == CNTRL_CLASSIC) || (controllerType == CNTRL_ARCADE)) Serial.println(wiiClassicController.rightStickX());
  if (controllerType == CNTRL_NUNCHUCK) Serial.println(wiiNunchuckController.accelX());

  Serial.print("RSY: ");
  Serial.print(controllerData.rightStickY);
  Serial.print("/");
  if ((controllerType == CNTRL_CLASSIC) || (controllerType == CNTRL_ARCADE)) Serial.println(wiiClassicController.rightStickY());
  if (controllerType == CNTRL_NUNCHUCK) Serial.println(wiiNunchuckController.accelY());

  Serial.write(27);       // ESC
  Serial.print("[H");     // Cursor home

#endif

  LoopWait();
}

void inline LoopWait()
{
#ifdef DEBUG
  delay(100);
#else
  delay(10); // ~100Hz
#endif
}

dataForController_t getControllerDataArcade(void)
{
  dataForController_t controllerData = getBlankDataForController();

  wiiArcadeController.update();

  // Arcade stick mappings
  //              PS3                                     XBOX360      Wii
  controllerData.triangleOn =  !wiiArcadeController.xIsPressed();//   Y            X
  controllerData.circleOn =    !wiiArcadeController.aIsPressed();//   B            A
  controllerData.squareOn =    !wiiArcadeController.yIsPressed();//   X            Y
  controllerData.crossOn =     !wiiArcadeController.bIsPressed();//   A            B

  controllerData.dpadUpOn =    !wiiArcadeController.upDIsPressed();
  controllerData.dpadDownOn =  !wiiArcadeController.downDIsPressed();
  controllerData.dpadLeftOn =  !wiiArcadeController.leftDIsPressed();
  controllerData.dpadRightOn = !wiiArcadeController.rightDIsPressed();

  controllerData.l1On =        !wiiArcadeController.lzIsPressed();//   LB           zL
  controllerData.l2On =        !wiiArcadeController.rIsPressed();//   RT           R
  controllerData.r1On =        !wiiArcadeController.lIsPressed();//   RB           L
  controllerData.r2On =        !wiiArcadeController.rzIsPressed();//   LT           zR

  controllerData.selectOn =    !wiiArcadeController.selectIsPressed();
  controllerData.startOn =     !wiiArcadeController.startIsPressed();
  controllerData.homeOn =      !wiiArcadeController.homeIsPressed();

  // this could do with some improvement, specifically, the MIN value is not taken into account currently.
#if defined FS_LS_X_WII_INVERT
#if FS_LS_X_WII_INVERT == true
  controllerData.leftStickX = 255 - (wiiArcadeController.leftStickX() * (256 / (FS_LS_X_WII_MAX + 1)));
#else
  controllerData.leftStickX = wiiArcadeController.leftStickX() * (256 / (FS_LS_X_WII_MAX + 1));
#endif
#endif

#if defined FS_LS_Y_WII_INVERT
#if FS_LS_Y_WII_INVERT == true
  controllerData.leftStickY = 255 - (wiiArcadeController.leftStickY() * (256 / (FS_LS_Y_WII_MAX + 1)));
#else
  controllerData.leftStickY = wiiArcadeController.leftStickY() * (256 / (FS_LS_Y_WII_MAX + 1));
#endif
#endif

#if defined FS_RS_X_WII_INVERT
#if FS_RS_X_WII_INVERT == true
  controllerData.rightStickX = 255 - (wiiArcadeController.rightStickX() * (256 / (FS_RS_X_WII_MAX + 1)));
#else
  controllerData.rightStickX = wiiArcadeController.rightStickX() * (256 / (FS_RS_X_WII_MAX + 1));
#endif
#endif

#if defined FS_RS_Y_WII_INVERT
#if FS_RS_Y_WII_INVERT == true
  controllerData.rightStickY = 255 - (wiiArcadeController.rightStickY() * (256 / (FS_RS_Y_WII_MAX + 1)));
#else
  controllerData.rightStickY = wiiArcadeController.rightStickY() * (256 / (FS_RS_Y_WII_MAX + 1));
#endif
#endif

  return controllerData;
}

dataForController_t getControllerDataClassic(void)
{
  // I do not have a classic controller to use in order to make these mappings correct!
  
  dataForController_t controllerData = getBlankDataForController();

  wiiClassicController.update();

  // Arcade stick mappings
  //              PS3                                      XBOX360      Wii
  controllerData.triangleOn =  !wiiClassicController.xIsPressed();//   Y            X
  controllerData.circleOn =    !wiiClassicController.aIsPressed();//   B            A
  controllerData.squareOn =    !wiiClassicController.yIsPressed();//   X            Y
  controllerData.crossOn =     !wiiClassicController.bIsPressed();//   A            B

  controllerData.dpadUpOn =    !wiiClassicController.upDIsPressed();
  controllerData.dpadDownOn =  !wiiClassicController.downDIsPressed();
  controllerData.dpadLeftOn =  !wiiClassicController.leftDIsPressed();
  controllerData.dpadRightOn = !wiiClassicController.rightDIsPressed();

  controllerData.l1On =        !wiiClassicController.lzIsPressed();//   LB           zL
  controllerData.l2On =        !wiiClassicController.rIsPressed();//   RT           R
  controllerData.r1On =        !wiiClassicController.lIsPressed();//   RB           L
  controllerData.r2On =        !wiiClassicController.rzIsPressed();//   LT           zR

  controllerData.selectOn =    !wiiClassicController.selectIsPressed();
  controllerData.startOn =     !wiiClassicController.startIsPressed();
  controllerData.homeOn =      !wiiClassicController.homeIsPressed();

  controllerData.leftStickX = wiiClassicController.leftStickX();
  controllerData.leftStickY = wiiClassicController.leftStickY();

  controllerData.rightStickX = wiiClassicController.rightStickX();
  controllerData.rightStickY = wiiClassicController.rightStickY();

  return controllerData;
}

dataForController_t getControllerDataNunchuck(void)
{
  dataForController_t controllerData = getBlankDataForController();

  wiiNunchuckController.update();

  controllerData.leftStickX = wiiNunchuckController.joystickX();
  controllerData.leftStickY = wiiNunchuckController.joystickY();
  controllerData.rightStickX = wiiNunchuckController.accelX();
  controllerData.rightStickY = wiiNunchuckController.accelY();
  controllerData.crossOn = wiiNunchuckController.zIsPressed();
  controllerData.circleOn = wiiNunchuckController.cIsPressed();

  return controllerData;
}


