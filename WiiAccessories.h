/*  WiiAccessoryToUSB
 *   Doug Barry 20140924
 *
 *  WiiAccessory.h
 *   Accessories library.
 *
 * The classes in this library ease the receipt and decoding of
 * Wii accessory data over the I2C bus. The base class
 * WiiAccessory is designed to be extended, with methods to
 * return button, axes and accelerometer information.
 */

#ifndef WIIACCESSORY_H
#define WIIACCESSORY_H

#include <Wire.h>

#define CNTRL_NONE 0
#define CNTRL_NUNCHUCK 0xFE
#define CNTRL_CLASSIC 0xFD
#define CNTRL_ARCADE 0x5F

/* Base class, contains code for communication with accessory
 * and decoding of information.
 */
class WiiAccessory
{
  public:
    byte cnt;
    uint8_t data[6];

    // Initialise communication with accessory
    void begin()
    {
      Wire.begin();
      cnt = 0;
      Wire.beginTransmission (0x52);
#if (ARDUINO >= 100)
      Wire.write (0x40);
      Wire.write (0x00);
#else
      Wire.send (0x40);
      Wire.send (0x00);
#endif
      Wire.endTransmission ();
    }

    // Get current accessory status registers and buffer
    bool update() {
      cnt = 0;
      Wire.requestFrom (0x52, 6);// request data from nunchuck
      while (Wire.available ()) {
#if (ARDUINO >= 100)
        data[cnt] = decodeByte(Wire.read());
#else
        data[cnt] = decodeByte(Wire.receive());
#endif
        cnt++;
      }
      requestNext();
      if (cnt >= 5) {
        return 1;   // success
      }
      return 0; //failure
    }

    // Static method which returns the type of accessory connected.
    static int detect()
    {

      Wire.begin();                // join i2c bus as master
      Wire.beginTransmission(0x52);// transmit to device 0x52
#if (ARDUINO >= 100)
      Wire.write((uint8_t)0x40);// sends memory address
      Wire.write((uint8_t)0x00);// sends sent a zero.
#else
      Wire.send((uint8_t)0x40);// sends memory address
      Wire.send((uint8_t)0x00);// sends sent a zero.
#endif
      Wire.endTransmission();// stop transmitting

      delay(5);

      Wire.beginTransmission(0x52);// transmit to device 0x52
#if (ARDUINO >= 100)
      Wire.write((uint8_t)0xFE);
#else
      Wire.send((uint8_t)0xFE);
#endif
      Wire.endTransmission();// stop transmitting

      delay(10);

      unsigned char typeBuffer[7];

      Wire.requestFrom(0x52, 2);

      int i = 0;
      while (Wire.available())
      {
#if (ARDUINO >= 100)
        typeBuffer[i] = Wire.read();
#else
        typeBuffer[i] = Wire.receive();
#endif
        i++;
        if (i > 7)
        {
          // bad things
          break;
        }
      }

      if ((typeBuffer[0] == 0xFE) && (typeBuffer[1] == 0xFE))
      {
        return CNTRL_NUNCHUCK;
      }
      else if ((typeBuffer[0] == 0xFD) && (typeBuffer[1] == 0xFD))
      {
        return CNTRL_CLASSIC;
      }
      else// if ((typeBuffer[0] == 0x5F) && (typeBuffer[1] == 0x5F))
      {
        return CNTRL_ARCADE;
      }
    }

  private:
  
    // Method to decode bytes received.
    byte decodeByte(byte x)
    {
      x = (x ^ 0x17) + 0x17;
      return x;
    }

    // Request the next chunk of the registers.
    void requestNext()
    {
      Wire.beginTransmission (0x52);	// transmit to device 0x52
#if (ARDUINO >= 100)
      Wire.write(0x00);		// sends one byte
#else
      Wire.send(0x00);		// sends one byte
#endif
      Wire.endTransmission ();	// stop transmitting
    }
};


/* Subclass which analyses the registers buffer as if it were a Wii Classic
 * controller.
 * This function is likely quite wrong, as I do not have a classic
 * controller against which to correct it!
 */
class WiiClassic : public WiiAccessory {
  public:

    bool lIsPressed() {
      return data[5] & 0b10000000;
    }

    bool rIsPressed() {
      return data[4] & 0b00000010;
    }

    bool leftShoulderIsPressed() {
      return isPressedRowBit(0, 5);
    }
    bool rightShoulderIsPressed() {
      return isPressedRowBit(0, 1);
    }
    bool lzIsPressed() {
      return data[4] & 0b00100000;
    }
    bool rzIsPressed() {
      return data[5] & 0b00000100;
    }
    bool leftDIsPressed() {
      return data[5] & 0b00000010;
    }
    bool rightDIsPressed() {
      return data[4] & 0b10000000;
    }
    bool upDIsPressed() {
      return data[5] & 0b00000001;
    }
    bool downDIsPressed() {
      return data[4] & 0b01000000;
    }
    bool selectIsPressed() {
      return data[4] & 0b00010000;
    }
    bool homeIsPressed() {
      return data[4] & 0b00001000;
    }
    bool startIsPressed() {
      return data[4] & 0b00000100;
    }
    bool xIsPressed() {
      return data[5] & 0b00100000;
    }
    bool yIsPressed() {
      return data[5] & 0b00001000;
    }
    bool aIsPressed() {
      return data[5] & 0b01000000;
    }
    bool bIsPressed() {
      return data[5] & 0b00010000;
    }
    int rightShouldPressure() {
      return data[3] & 0x1f;
    }
    int leftShouldPressure() {
      return ((data[2] & 0x60) >> 2) + ((data[3] & 0xe0) >> 5);
    }
    int leftStickX() {
      return  ( (data[0] & 0x3f));
    }
    int leftStickY() {
      return  ((data[1] & 0x3f));
    }
    int rightStickX() {
      return ((data[0] & 0xc0) >> 3) + ((data[1] & 0xc0) >> 5) +  ((data[2] & 0x80) >> 7);
    }
    int rightStickY() {
      return data[2] & 0x1f;
    }


  protected:
    bool isPressedRowBit(byte row, byte bit)
    {
      byte mask = (1 << bit);
      return (!(data[row + 5] & mask));
    }

};

class WiiArcade : public WiiAccessory {
  public:

    bool lIsPressed() {
      return data[5] & 0b10000000;
    }

    bool rIsPressed() {
      return data[4] & 0b00000010;
    }

    bool leftShoulderIsPressed() {
      return isPressedRowBit(0, 5);
    }
    bool rightShoulderIsPressed() {
      return isPressedRowBit(0, 1);
    }
    bool lzIsPressed() {
      return data[4] & 0b00100000;
    }
    bool rzIsPressed() {
      return data[5] & 0b00000100;
    }
    bool leftDIsPressed() {
      return data[5] & 0b00000010;
    }
    bool rightDIsPressed() {
      return data[4] & 0b10000000;
    }
    bool upDIsPressed() {
      return data[5] & 0b00000001;
    }
    bool downDIsPressed() {
      return data[4] & 0b01000000;
    }
    bool selectIsPressed() {
      return data[4] & 0b00010000;
    }
    bool homeIsPressed() {
      return data[4] & 0b00001000;
    }
    bool startIsPressed() {
      return data[4] & 0b00000100;
    }
    bool xIsPressed() {
      return data[5] & 0b00100000;
    }
    bool yIsPressed() {
      return data[5] & 0b00001000;
    }
    bool aIsPressed() {
      return data[5] & 0b01000000;
    }
    bool bIsPressed() {
      return data[5] & 0b00010000;
    }
    int rightShouldPressure() {
      return data[3] & 0x1f;
    }
    int leftShouldPressure() {
      return ((data[2] & 0x60) >> 2) + ((data[3] & 0xe0) >> 5);
    }
    int leftStickX() {
      return  ( (data[0] & 0x3f));
    }
    int leftStickY() {
      return  ((data[1] & 0x3f));
    }
    int rightStickX() {
      return ((data[0] & 0xc0) >> 3) + ((data[1] & 0xc0) >> 5) +  ((data[2] & 0x80) >> 7);
    }
    int rightStickY() {
      return data[2] & 0x1f;
    }


  protected:
    bool isPressedRowBit(byte row, byte bit)
    {
      byte mask = (1 << bit);
      return (!(data[row + 5] & mask));
    }

};


/* Subclass which analyses the registers buffer as if it were a Wii Nunchuck
 * controller.
 */
class WiiNunchuck : public WiiAccessory
{
  public:

    // returns zbutton state: 1=pressed, 0=notpressed
    bool zIsPressed() {
      return ((data[5] >> 0) & 1) ? 0 : 1;
    }

    // returns zbutton state: 1=pressed, 0=notpressed
    bool cIsPressed() {
      return ((data[5] >> 1) & 1) ? 0 : 1;
    }

    // returns value of x-axis joystick
    int joystickX() {
      return data[0];
    }

    // returns value of y-axis joystick
    int joystickY() {
      return data[1];
    }

    // returns value of x-axis accelerometer
    int accelX() {
      return ((data[2] << 2) + ((data[5] & (3 << 2 ) >> 2)) >> 2);
    }

    // returns value of y-axis accelerometer
    int accelY() {
      return ((data[3] << 2) + ((data[5] & (3 << 4 ) >> 4)) >> 2);
    }

    // returns value of z-axis accelerometer
    int accelZ() {
      return ((data[4] << 2) + ((data[5] & (3 << 6 ) >> 6)) >> 2);
    }

};

#endif


