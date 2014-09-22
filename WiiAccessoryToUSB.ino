//#define DEBUG


// 20140922 Doug Barry
// Portions from many places, acknoweldgements to be added.

#include "UnoJoy.h"
#include <Wire.h>

#include "WiiAccessoryToUSB.h"
#include "WiiAccessories.h"

#define PWR_ANALOGPINS

// settings for Wii HORI Fightstick
#define LS_X_WII_MIN 0
#define LS_X_WII_MAX 63
#define LS_X_WII_INVERT false
#define LS_Y_WII_MIN 0
#define LS_Y_WII_MAX 63
#define LS_Y_WII_INVERT true

#define RS_X_WII_MIN 0
#define RS_X_WII_MAX 31
#define RS_X_WII_INVERT false
#define RS_Y_WII_MIN 0
#define RS_Y_WII_MAX 31
#define RS_Y_WII_INVERT true


WiiClassic wiiClassy = WiiClassic();
WiiNunchuck wiiNunchuck = WiiNunchuck();

dataForController_t controllerData;

void setup()
{
  setupUnoJoy();

#ifdef PWR_ANALOGPINS
  setPowerPins();
#endif

  controllerType = WiiAccessory::detect();

  if ((controllerType == CNTRL_CLASSIC) || (controllerType == CNTRL_ARCADE))
  {
    wiiClassy.begin();
    wiiClassy.update();
  }
  else
  {
    wiiNunchuck.begin();
    wiiNunchuck.update();
  }

#ifdef DEBUG
  Serial.begin(9600);
  delay(5);
  Serial.write(27);       // ESC
  Serial.print("[2J");    // CLS
  Serial.write(27);       // ESC
  Serial.print("[H");     // Cursor home
  delay(10);
#endif
}

void loop()
{

  switch (controllerType)
  {
    case CNTRL_CLASSIC:
    case CNTRL_ARCADE:
      controllerData = getControllerDataClassic();
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
    Serial.println(wiiClassy.data[i], BIN);
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
  if ((controllerType == CNTRL_CLASSIC) || (controllerType == CNTRL_ARCADE)) Serial.println(wiiClassy.leftStickX());
  if (controllerType == CNTRL_NUNCHUCK) Serial.println(wiiNunchuck.joystickX());

  Serial.print("LSY: ");
  Serial.print(controllerData.leftStickY);
  Serial.print("/");
  if ((controllerType == CNTRL_CLASSIC) || (controllerType == CNTRL_ARCADE)) Serial.println(wiiClassy.leftStickY());
  if (controllerType == CNTRL_NUNCHUCK) Serial.println(wiiNunchuck.joystickY());

  Serial.print("RSX: ");
  Serial.print(controllerData.rightStickX);
  Serial.print("/");
  if ((controllerType == CNTRL_CLASSIC) || (controllerType == CNTRL_ARCADE)) Serial.println(wiiClassy.rightStickX());
  if (controllerType == CNTRL_NUNCHUCK) Serial.println(wiiNunchuck.accelX());

  Serial.print("RSY: ");
  Serial.print(controllerData.rightStickY);
  Serial.print("/");
  if ((controllerType == CNTRL_CLASSIC) || (controllerType == CNTRL_ARCADE)) Serial.println(wiiClassy.rightStickY());
  if (controllerType == CNTRL_NUNCHUCK) Serial.println(wiiNunchuck.accelY());

  Serial.write(27);       // ESC
  Serial.print("[H");     // Cursor home

#endif

  LoopWait();
}

void LoopWait()
{
#ifdef DEBUG
  delay(100);
#else
  delay(10);
#endif
}

dataForController_t getControllerDataClassic(void)
{
  // Set up a place for our controller data
  //  Use the getBlankDataForController() function, since
  //  just declaring a fresh dataForController_t tends
  //  to get you one filled with junk from other, random
  //  values that were in those memory locations before
  dataForController_t controllerData = getBlankDataForController();
  // Since our buttons are all held high and
  //  pulled low when pressed, we use the "!"
  //  operator to invert the readings from the pins

  wiiClassy.update();

  // Arcade stick mappings
  //              PS3                                                            XBOX360      Wii
  controllerData.triangleOn =  !wiiClassy.xIsPressed();//   Y            X
  controllerData.circleOn =    !wiiClassy.aIsPressed();//   B            A
  controllerData.squareOn =    !wiiClassy.yIsPressed();//   X            Y
  controllerData.crossOn =     !wiiClassy.bIsPressed();//   A            B

  controllerData.dpadUpOn =    !wiiClassy.upDIsPressed();
  controllerData.dpadDownOn =  !wiiClassy.downDIsPressed();
  controllerData.dpadLeftOn =  !wiiClassy.leftDIsPressed();
  controllerData.dpadRightOn = !wiiClassy.rightDIsPressed();

  controllerData.l1On =        !wiiClassy.lzIsPressed();//   LB           zL
  controllerData.l2On =        !wiiClassy.rIsPressed();//   RT           R
  controllerData.r1On =        !wiiClassy.lIsPressed();//   RB           L
  controllerData.r2On =        !wiiClassy.rzIsPressed();//   LT           zR

  controllerData.selectOn =    !wiiClassy.selectIsPressed();
  controllerData.startOn =     !wiiClassy.startIsPressed();
  controllerData.homeOn =      !wiiClassy.homeIsPressed();

#if defined LS_X_WII_INVERT
#if LS_X_WII_INVERT == true
  controllerData.leftStickX = 255 - (wiiClassy.leftStickX() * (256 / (LS_X_WII_MAX + 1)));
#else
  controllerData.leftStickX = wiiClassy.leftStickX() * (256 / (LS_X_WII_MAX + 1));
#endif
#endif

#if defined LS_Y_WII_INVERT
#if LS_Y_WII_INVERT == true
  controllerData.leftStickY = 255 - (wiiClassy.leftStickY() * (256 / (LS_Y_WII_MAX + 1)));
#else
  controllerData.leftStickY = wiiClassy.leftStickY() * (256 / (LS_Y_WII_MAX + 1));
#endif
#endif

#if defined RS_X_WII_INVERT
#if RS_X_WII_INVERT == true
  controllerData.rightStickX = 255 - (wiiClassy.rightStickX() * (256 / (RS_X_WII_MAX + 1)));
#else
  controllerData.rightStickX = wiiClassy.rightStickX() * (256 / (RS_X_WII_MAX + 1));
#endif
#endif

#if defined RS_Y_WII_INVERT
#if RS_Y_WII_INVERT == true
  controllerData.rightStickY = 255 - (wiiClassy.rightStickY() * (256 / (RS_Y_WII_MAX + 1)));
#else
  controllerData.rightStickY = wiiClassy.rightStickY() * (256 / (RS_Y_WII_MAX + 1));
#endif
#endif

  return controllerData;
}


dataForController_t getControllerDataNunchuck(void)
{
  // Set up a place for our controller data
  //  Use the getBlankDataForController() function, since
  //  just declaring a fresh dataForController_t tends
  //  to get you one filled with junk from other, random
  //  values that were in those memory locations before
  dataForController_t controllerData = getBlankDataForController();
  // Since our buttons are all held high and
  //  pulled low when pressed, we use the "!"
  //  operator to invert the readings from the pins

  wiiNunchuck.update();

  controllerData.leftStickX = wiiNunchuck.joystickX();
  controllerData.leftStickY = wiiNunchuck.joystickY();
  controllerData.rightStickX = wiiNunchuck.accelX();
  controllerData.rightStickY = wiiNunchuck.accelY();
  controllerData.crossOn = wiiNunchuck.zIsPressed();
  controllerData.circleOn = wiiNunchuck.cIsPressed();

  return controllerData;
}


