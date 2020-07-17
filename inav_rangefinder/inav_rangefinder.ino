
/*
   Set I2C Slave address
*/
#define I2C_ADDRESS 0x14

// #define DEBUG


#define STATUS_OK 0
#define STATUS_OUT_OF_RANGE 1

#include <Wire.h>


uint8_t i2c_regs[] =
{
  STATUS_OUT_OF_RANGE, // status
  0, // high byte of distance
  0, // low byte of distance
};

const byte reg_size = sizeof(i2c_regs);

void requestEvent()
{
  Wire.write(i2c_regs, 3);
}

void receiveEvent(int howMany)
{
  if (howMany < 1) {
    // Sanity-check
    return;
  }

  while ( (howMany--) > 0) {
    Wire.read();
  }
}

void setup()
{
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

#ifdef DEBUG
  Serial.begin(115200);
#endif
}


void loop()
{
  static unsigned long nextUpdate = 0;

  unsigned long now = millis();
  if ( now > nextUpdate) {
    nextUpdate = now + 100;

    long duration = 22 + (now % 5000) / 1000; // TODO FIX

    uint8_t stat = STATUS_OUT_OF_RANGE;

    if (duration > 0) {
      stat = STATUS_OK;
    }

    uint16_t cm = duration;

    noInterrupts();
    i2c_regs[0] = stat;
    i2c_regs[1] = cm >> 8;
    i2c_regs[2] = cm & 0xFF;
    interrupts();

#ifdef DEBUG
    Serial.print("Regs: ");
    Serial.print(i2c_regs[0]);
    Serial.print(" ");
    Serial.print(i2c_regs[1]);
    Serial.print(" ");
    Serial.println(i2c_regs[2]);
#endif
  }
}
