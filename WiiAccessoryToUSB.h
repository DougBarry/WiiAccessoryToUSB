/*  WiiAccessoryToUSB
 *   Doug Barry 20140924
 *
 *  WiiAccessoryToUSB.h
 *   Header.
 */
 
#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <Wire.h>
#include "WiiAccessories.h"

#ifndef WIICHUCKTOUSB_H
#define WIICHUCKTOUSB_H

// Pin definitions for WiiChuck power.
#define WIICHUCK_PWRPIN PORTC3
#define WIICHUCK_GNDPIN PORTC2
// Uses port C (analog in) pins as power & ground for Nunchuck
static void setPowerPins()
{
  DDRC |= _BV(WIICHUCK_PWRPIN) | _BV(WIICHUCK_GNDPIN);
  PORTC &= ~ _BV(WIICHUCK_GNDPIN);
  PORTC |=  _BV(WIICHUCK_PWRPIN);
  delay(100);  // Wait for things to stabilize
}

#endif
