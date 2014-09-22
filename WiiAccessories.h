#ifndef WIIACCESSORY_H
#define WIIACCESSORY_H

#include <Wire.h>

#define CNTRL_NONE 0
#define CNTRL_NUNCHUCK 0xFE
#define CNTRL_CLASSIC 0xFD
#define CNTRL_ARCADE 0x5F


class WiiAccessory
{
  public:
    byte cnt;
    uint8_t data[6];

    void begin()
    {
      Wire.begin();
      cnt = 0;
      Wire.beginTransmission (0x52);
      Wire.write (0x40);
      Wire.write (0x00);
      Wire.endTransmission ();
    }

    bool update() {
      cnt = 0;
      Wire.requestFrom (0x52, 6);// request data from nunchuck
      while (Wire.available ()) {
        data[cnt] = decodeByte( Wire.read());
        cnt++;
      }
      requestNext();
      if (cnt >= 5) {
        return 1;   // success
      }
      return 0; //failure
    }

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
      Wire.write((uint8_t)0xFE);
      Wire.endTransmission();// stop transmitting

      delay(10);

      unsigned char typeBuffer[7];

      Wire.requestFrom(0x52, 2);

      int i = 0;
      while (Wire.available())
      {
        typeBuffer[i] = Wire.read();
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
    byte decodeByte(byte x)
    {
      x = (x ^ 0x17) + 0x17;
      return x;
    }

    void requestNext()
    {
      Wire.beginTransmission (0x52);	// transmit to device 0x52
      Wire.write (0x00);		// sends one byte
      Wire.endTransmission ();	// stop transmitting
    }
};

class WiiClassic : public WiiAccessory {
  public:

    /*
      controllerData.triangleOn =  !(wiiClassy.getRawButtons()[1] & 0b00001000);//   Y            X
      controllerData.circleOn =    !(wiiClassy.getRawButtons()[1] & 0b00010000);//   B            A
      controllerData.squareOn =    !(wiiClassy.getRawButtons()[1] & 0b00100000);//   X            Y
      controllerData.crossOn =     !(wiiClassy.getRawButtons()[1] & 0b01000000);//   A            B

      controllerData.dpadUpOn =    !(wiiClassy.getRawButtons()[1] & 0b00000001);
      controllerData.dpadDownOn =  !(wiiClassy.getRawButtons()[0] & 0b01000000);
      controllerData.dpadLeftOn =  !(wiiClassy.getRawButtons()[1] & 0b00000010);
      controllerData.dpadRightOn = !(wiiClassy.getRawButtons()[0] & 0b10000000);

      controllerData.l1On =        !(wiiClassy.getRawButtons()[0] & 0b00100000);//   LB           zL
      controllerData.l2On =        !(wiiClassy.getRawButtons()[0] & 0b00000010);//   RT           R
      controllerData.r1On =        !(wiiClassy.getRawButtons()[1] & 0b10000000);//   RB           L
      controllerData.r2On =        !(wiiClassy.getRawButtons()[1] & 0b00000100);//   LT           zR
    */

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


