#include <mbed.h>
#include "ACS37800_CS.h"

#define I2C_SDA   P0_10   // p28
#define I2C_SCL   P0_11   // p27

I2C i2c(I2C_SDA, I2C_SCL);
ACS37800 mySensor(&i2c);


int main() {
  wait_us(2000000);  
  // put your setup code here, to run once:
  printf("ACS37800 TEST \n");
  
  int count = 0;
  for (int addr = 0; addr < 127; addr++)
  {
    // 0 return is OK
    if(!i2c.write(addr << 1, NULL, 0))
    {
      printf("I2C device found at address 0x%02X (0x%02X in 8-bit)\n", addr, addr << 1);
      count ++;
    }
    wait_us(20000);
  }

  if(count)
  {
    printf("%d", count);
  }
  else
  {
    printf("No");
  }
  printf(" device(s) found\r\n");


  // From the ACS37800 datasheet:
  // CONFIGURING THE DEVICE FOR AC APPLICATIONS : DYNAMIC CALCULATION OF N
  // Set bypass_n_en = 0 (default). This setting enables the device to
  // dynamically calculate N based off the voltage zero crossings.
  // Disable bypass_n in shadow memory and eeprom
  mySensor.setBypassNenable(false, true);

  // We need to connect the LO pin to the 'low' side of the AC source.
  // So we need to set the divider resistance to 4M Ohms (instead of 2M).
  // Comment this line if you are using GND to measure the 'low' side of the AC voltage
  mySensor.setDividerRes(4000000); 

  while(1) {
    // put your main code here, to run repeatedly:
    float volts = 0.0;
    float amps = 0.0;

    mySensor.readRMS(&volts, &amps);

    printf("Volts: %2f  Amps: %2f\r\n", volts, amps);

    wait_us(250000);
  }
}