#include <mbed.h>
#include "ACS37800_CS.h"

#define I2C_SDA   P0_10   // p28
#define I2C_SCL   P0_11   // p27

#define RELAY1    P0_18
#define RELAY2    P0_17
#define RELAY3    P0_16

/*
  One Gang    -->   0x6A
  Two Gang    -->   0x6B and 0x6A
  Three Gang  -->   ox6E, 0x6B and 0x6A
*/
#define CS1_ADDRESS   0x6E    
#define CS2_ADDRESS   0x6B 
#define CS3_ADDRESS   0x6A   

I2C i2c(I2C_SDA, I2C_SCL);
ACS37800 current_ch_2(&i2c);
ACS37800 current_ch_3(&i2c);

DigitalOut relay1(RELAY1);
DigitalOut relay2(RELAY2);
DigitalOut relay3(RELAY3);

// Global copies of the register contents
// EEPROM registers are 0B-0F. Shadow registers are 1B-1F. Volatile registers are 20-30.
ACS37800_REGISTER_0B_t reg0B_ch2;
ACS37800_REGISTER_0C_t reg0C_ch2;
ACS37800_REGISTER_0D_t reg0D_ch2;
ACS37800_REGISTER_0E_t reg0E_ch2;
ACS37800_REGISTER_0F_t reg0F_ch2;

ACS37800_REGISTER_0B_t reg0B_ch3;
ACS37800_REGISTER_0C_t reg0C_ch3;
ACS37800_REGISTER_0D_t reg0D_ch3;
ACS37800_REGISTER_0E_t reg0E_ch3;
ACS37800_REGISTER_0F_t reg0F_ch3;

ACS37800_REGISTER_0B_t reg1B_ch3;
ACS37800_REGISTER_0C_t reg1C_ch3;
ACS37800_REGISTER_0D_t reg1D_ch3;
ACS37800_REGISTER_0E_t reg1E_ch3;
ACS37800_REGISTER_0F_t reg1F_ch3;

ACS37800_REGISTER_0B_t reg1B;
ACS37800_REGISTER_0C_t reg1C;
ACS37800_REGISTER_0D_t reg1D;
ACS37800_REGISTER_0E_t reg1E;
ACS37800_REGISTER_0F_t reg1F;


ACS37800_REGISTER_2A_t  reg2A_ch3; 
ACS37800_REGISTER_20_t  reg20_ch3; 
ACS37800_REGISTER_26_t  reg26_ch3; 
ACS37800_REGISTER_27_t  reg27_ch3; 


void read_EEPROM_Config_1G()
{
  printf("Load EEPROM Config - 1G Switch\r\n");
  current_ch_3.readRegister(&reg0B_ch3.data.all, ACS37800_REGISTER_EEPROM_0B);
  current_ch_3.readRegister(&reg0C_ch3.data.all, ACS37800_REGISTER_EEPROM_0C);
  current_ch_3.readRegister(&reg0D_ch3.data.all, ACS37800_REGISTER_EEPROM_0D);
  current_ch_3.readRegister(&reg0E_ch3.data.all, ACS37800_REGISTER_EEPROM_0E);
  current_ch_3.readRegister(&reg0F_ch3.data.all, ACS37800_REGISTER_EEPROM_0F);

  printf("Register 0B : qvo_fine          : %02lX\r\n", reg0B_ch3.data.bits.qvo_fine);
  printf("Register 0B : sns_fine          : %02lX\r\n", reg0B_ch3.data.bits.sns_fine);
  printf("Register 0B : crs_sns           : %02lX\r\n", reg0B_ch3.data.bits.crs_sns);
  printf("Register 0B : iavgselen         : %02lX\r\n", reg0B_ch3.data.bits.iavgselen);
  printf("Register 0B : pavgselen         : %02lX\r\n", reg0B_ch3.data.bits.pavgselen);

  printf("Register 0C : rms_avg_1         : %02lX\r\n", reg0C_ch3.data.bits.rms_avg_1);
  printf("Register 0C : rms_avg_2         : %02lX\r\n", reg0C_ch3.data.bits.rms_avg_2);
  printf("Register 0C : vchan_offset      : %02lX\r\n", reg0C_ch3.data.bits.vchan_offset_code);

  printf("Register 0D : ichan_del_en      : %02lX\r\n", reg0D_ch3.data.bits.ichan_del_en);
  printf("Register 0D : chan_del_sel      : %02lX\r\n", reg0D_ch3.data.bits.chan_del_sel);
  printf("Register 0D : fault             : %02lX\r\n", reg0D_ch3.data.bits.fault);
  printf("Register 0D : fltdly            : %02lX\r\n", reg0D_ch3.data.bits.fltdly);

  printf("Register 0E : vevent_cycs       : %02lX\r\n", reg0E_ch3.data.bits.vevent_cycs);
  printf("Register 0E : overvreg          : %02lX\r\n", reg0E_ch3.data.bits.overvreg);
  printf("Register 0E : undervreg         : %02lX\r\n", reg0E_ch3.data.bits.undervreg);
  printf("Register 0E : delaycnt_sel      : %02lX\r\n", reg0E_ch3.data.bits.delaycnt_sel);
  printf("Register 0E : halfcycle_en      : %02lX\r\n", reg0E_ch3.data.bits.halfcycle_en);
  printf("Register 0E : squarewave_en     : %02lX\r\n", reg0E_ch3.data.bits.squarewave_en);
  printf("Register 0E : zerocrosschansel  : %02lX\r\n", reg0E_ch3.data.bits.zerocrosschansel);
  printf("Register 0E : zerocrossedgesel  : %02lX\r\n", reg0E_ch3.data.bits.zerocrossedgesel);

  printf("Register 0F : i2c_slv_addr      : %02lX\r\n", reg0F_ch3.data.bits.i2c_slv_addr);
  printf("Register 0F : i2c_dis_slv_addr  : %02lX\r\n", reg0F_ch3.data.bits.i2c_dis_slv_addr);
  printf("Register 0F : dio_0_sel         : %02lX\r\n", reg0F_ch3.data.bits.dio_0_sel);
  printf("Register 0F : dio_1_sel         : %02lX\r\n", reg0F_ch3.data.bits.dio_1_sel);
  printf("Register 0F : n                 : %02lX\r\n", reg0F_ch3.data.bits.n);
  printf("Register 0F : bypass_n_en       : %02lX\r\n\r\n", reg0F_ch3.data.bits.bypass_n_en);
}

void read_SHADOW_Config_1G()
{
  printf("Load Shadow Config - 1G Switch\r\n");
  current_ch_3.readRegister(&reg1B_ch3.data.all, ACS37800_REGISTER_SHADOW_1B);
  current_ch_3.readRegister(&reg1C_ch3.data.all, ACS37800_REGISTER_SHADOW_1C);
  current_ch_3.readRegister(&reg1D_ch3.data.all, ACS37800_REGISTER_SHADOW_1D);
  current_ch_3.readRegister(&reg1E_ch3.data.all, ACS37800_REGISTER_SHADOW_1E);
  current_ch_3.readRegister(&reg1F_ch3.data.all, ACS37800_REGISTER_SHADOW_1F);

  printf("Register 1B : qvo_fine          : %02lX\r\n", reg1B_ch3.data.bits.qvo_fine);
  printf("Register 1B : sns_fine          : %02lX\r\n", reg1B_ch3.data.bits.sns_fine);
  printf("Register 1B : crs_sns           : %02lX\r\n", reg1B_ch3.data.bits.crs_sns);
  printf("Register 1B : iavgselen         : %02lX\r\n", reg1B_ch3.data.bits.iavgselen);
  printf("Register 1B : pavgselen         : %02lX\r\n", reg1B_ch3.data.bits.pavgselen);

  printf("Register 1C : rms_avg_1         : %02lX\r\n", reg1C_ch3.data.bits.rms_avg_1);
  printf("Register 1C : rms_avg_2         : %02lX\r\n", reg1C_ch3.data.bits.rms_avg_2);
  printf("Register 1C : vchan_offset      : %02lX\r\n", reg1C_ch3.data.bits.vchan_offset_code);

  printf("Register 1D : ichan_del_en      : %02lX\r\n", reg1D_ch3.data.bits.ichan_del_en);
  printf("Register 1D : chan_del_sel      : %02lX\r\n", reg1D_ch3.data.bits.chan_del_sel);
  printf("Register 1D : fault             : %02lX\r\n", reg1D_ch3.data.bits.fault);
  printf("Register 1D : fltdly            : %02lX\r\n", reg1D_ch3.data.bits.fltdly);

  printf("Register 1E : vevent_cycs       : %02lX\r\n", reg1E_ch3.data.bits.vevent_cycs);
  printf("Register 1E : overvreg          : %02lX\r\n", reg1E_ch3.data.bits.overvreg);
  printf("Register 1E : undervreg         : %02lX\r\n", reg1E_ch3.data.bits.undervreg);
  printf("Register 1E : delaycnt_sel      : %02lX\r\n", reg1E_ch3.data.bits.delaycnt_sel);
  printf("Register 1E : halfcycle_en      : %02lX\r\n", reg1E_ch3.data.bits.halfcycle_en);
  printf("Register 1E : squarewave_en     : %02lX\r\n", reg1E_ch3.data.bits.squarewave_en);
  printf("Register 1E : zerocrosschansel  : %02lX\r\n", reg1E_ch3.data.bits.zerocrosschansel);
  printf("Register 1E : zerocrossedgesel  : %02lX\r\n", reg1E_ch3.data.bits.zerocrossedgesel);

  printf("Register 1F : i2c_slv_addr      : %02lX\r\n", reg1F_ch3.data.bits.i2c_slv_addr);
  printf("Register 1F : i2c_dis_slv_addr  : %02lX\r\n", reg1F_ch3.data.bits.i2c_dis_slv_addr);
  printf("Register 1F : dio_0_sel         : %02lX\r\n", reg1F_ch3.data.bits.dio_0_sel);
  printf("Register 1F : dio_1_sel         : %02lX\r\n", reg1F_ch3.data.bits.dio_1_sel);
  printf("Register 1F : n                 : %02lX\r\n", reg1F_ch3.data.bits.n);
  printf("Register 1F : bypass_n_en       : %02lX\r\n\r\n", reg1F_ch3.data.bits.bypass_n_en);
}

void read_EEPROM()
{
  // Read the eeprom registers
  current_ch_2.readRegister(&reg0B_ch2.data.all, ACS37800_REGISTER_EEPROM_0B);
  current_ch_2.readRegister(&reg0C_ch2.data.all, ACS37800_REGISTER_EEPROM_0C);
  current_ch_2.readRegister(&reg0D_ch2.data.all, ACS37800_REGISTER_EEPROM_0D);
  current_ch_2.readRegister(&reg0E_ch2.data.all, ACS37800_REGISTER_EEPROM_0E);
  current_ch_2.readRegister(&reg0F_ch2.data.all, ACS37800_REGISTER_EEPROM_0F);

  current_ch_3.readRegister(&reg0B_ch3.data.all, ACS37800_REGISTER_EEPROM_0B);
  current_ch_3.readRegister(&reg0C_ch3.data.all, ACS37800_REGISTER_EEPROM_0C);
  current_ch_3.readRegister(&reg0D_ch3.data.all, ACS37800_REGISTER_EEPROM_0D);
  current_ch_3.readRegister(&reg0E_ch3.data.all, ACS37800_REGISTER_EEPROM_0E);
  current_ch_3.readRegister(&reg0F_ch3.data.all, ACS37800_REGISTER_EEPROM_0F);

  // current_ch_2.readRegister(&reg1B.data.all, ACS37800_REGISTER_SHADOW_1B);

  printf("Register 0B : qvo_fine          : %02lX   |   %02lX\r\n", reg0B_ch2.data.bits.qvo_fine, reg0B_ch3.data.bits.qvo_fine);
  printf("Register 0B : sns_fine          : %02lX   |   %02lX\r\n", reg0B_ch2.data.bits.sns_fine, reg0B_ch3.data.bits.sns_fine);
  // printf("Register 1B : sns_fine          : %ld\r\n", reg1B.data.bits.sns_fine);
  printf("Register 0B : crs_sns           : %02lX   |   %02lX\r\n", reg0B_ch2.data.bits.crs_sns,  reg0B_ch3.data.bits.crs_sns);
  printf("Register 0B : iavgselen         : %02lX   |   %02lX\r\n", reg0B_ch2.data.bits.iavgselen, reg0B_ch3.data.bits.iavgselen);
  printf("Register 0B : pavgselen         : %02lX   |   %02lX\r\n", reg0B_ch2.data.bits.pavgselen, reg0B_ch3.data.bits.pavgselen);

  printf("Register 0C : rms_avg_1         : %02lX   |   %02lX\r\n", reg0C_ch2.data.bits.rms_avg_1, reg0C_ch3.data.bits.rms_avg_1);
  printf("Register 0C : rms_avg_2         : %02lX   |   %02lX\r\n", reg0C_ch2.data.bits.rms_avg_2, reg0C_ch3.data.bits.rms_avg_2);
  printf("Register 0C : vchan_offset      : %02lX   |   %02lX\r\n", reg0C_ch2.data.bits.vchan_offset_code, reg0C_ch3.data.bits.vchan_offset_code);

  printf("Register 0D : ichan_del_en      : %02lX   |   %02lX\r\n", reg0D_ch2.data.bits.ichan_del_en, reg0D_ch3.data.bits.ichan_del_en);
  printf("Register 0D : chan_del_sel      : %02lX   |   %02lX\r\n", reg0D_ch2.data.bits.chan_del_sel, reg0D_ch3.data.bits.chan_del_sel);
  printf("Register 0D : fault             : %02lX   |   %02lX\r\n", reg0D_ch2.data.bits.fault, reg0D_ch3.data.bits.fault);
  printf("Register 0D : fltdly            : %02lX   |   %02lX\r\n", reg0D_ch2.data.bits.fltdly, reg0D_ch3.data.bits.fltdly);

  printf("Register 0E : vevent_cycs       : %02lX   |   %02lX\r\n", reg0E_ch2.data.bits.vevent_cycs, reg0E_ch3.data.bits.vevent_cycs);
  printf("Register 0E : overvreg          : %02lX   |   %02lX\r\n", reg0E_ch2.data.bits.overvreg, reg0E_ch3.data.bits.overvreg);
  printf("Register 0E : undervreg         : %02lX   |   %02lX\r\n", reg0E_ch2.data.bits.undervreg, reg0E_ch3.data.bits.undervreg);
  printf("Register 0E : delaycnt_sel      : %02lX   |   %02lX\r\n", reg0E_ch2.data.bits.delaycnt_sel, reg0E_ch3.data.bits.delaycnt_sel);
  printf("Register 0E : halfcycle_en      : %02lX   |   %02lX\r\n", reg0E_ch2.data.bits.halfcycle_en, reg0E_ch3.data.bits.halfcycle_en);
  printf("Register 0E : squarewave_en     : %02lX   |   %02lX\r\n", reg0E_ch2.data.bits.squarewave_en, reg0E_ch3.data.bits.squarewave_en);
  printf("Register 0E : zerocrosschansel  : %02lX   |   %02lX\r\n", reg0E_ch2.data.bits.zerocrosschansel, reg0E_ch3.data.bits.zerocrosschansel);
  printf("Register 0E : zerocrossedgesel  : %02lX   |   %02lX\r\n", reg0E_ch2.data.bits.zerocrossedgesel, reg0E_ch3.data.bits.zerocrossedgesel);

  printf("Register 0F : i2c_slv_addr      : %02lX   |   %02lX\r\n", reg0F_ch2.data.bits.i2c_slv_addr, reg0F_ch3.data.bits.i2c_slv_addr);
  printf("Register 0F : i2c_dis_slv_addr  : %02lX   |   %02lX\r\n", reg0F_ch2.data.bits.i2c_dis_slv_addr, reg0F_ch3.data.bits.i2c_dis_slv_addr);
  printf("Register 0F : dio_0_sel         : %02lX   |   %02lX\r\n", reg0F_ch2.data.bits.dio_0_sel, reg0F_ch3.data.bits.dio_0_sel);
  printf("Register 0F : dio_1_sel         : %02lX   |   %02lX\r\n", reg0F_ch2.data.bits.dio_1_sel, reg0F_ch3.data.bits.dio_1_sel);
  printf("Register 0F : n                 : %02lX   |   %02lX\r\n", reg0F_ch2.data.bits.n, reg0F_ch3.data.bits.n);
  printf("Register 0F : bypass_n_en       : %02lX   |   %02lX\r\n\r\n", reg0F_ch2.data.bits.bypass_n_en, reg0F_ch3.data.bits.bypass_n_en);
}

void read_data()
{
  union
  {
      int16_t Signed;
      uint16_t unSigned;
  } signedUnsigned; // Avoid any ambiguity when casting to signed int

  current_ch_3.readRegister(&reg2A_ch3.data.all, ACS37800_REGISTER_VOLATILE_2A);

  //Extract irms as signed int
  signedUnsigned.unSigned = reg2A_ch3.data.bits.icodes; 
  float amps_inst = (float)signedUnsigned.Signed;

  //Convert from codes to the fraction of ADC Full Scale (16-bit)
  amps_inst /= 55000.0; 

  //Convert to amps
  amps_inst *= ACS37800_DEFAULT_CURRENT_RANGE; 

  printf("Register 2A : Instantaneous     : %ld,  Inst Amps: %2f\r\n", signedUnsigned.unSigned, amps_inst);


  current_ch_3.readRegister(&reg20_ch3.data.all, ACS37800_REGISTER_VOLATILE_20);

  //Extract irms as signed int
  signedUnsigned.unSigned = reg20_ch3.data.bits.irms; 
  float amps_rms = (float)signedUnsigned.Signed;

  //Convert from codes to the fraction of ADC Full Scale (16-bit)
  amps_rms /= 55000.0; 

  //Convert to amps
  amps_rms *= ACS37800_DEFAULT_CURRENT_RANGE; 

  printf("Register 20 : RMS               : %ld,  RMS Amps: %2f\r\n", signedUnsigned.unSigned, amps_rms);


  current_ch_3.readRegister(&reg26_ch3.data.all, ACS37800_REGISTER_VOLATILE_26);

  //Extract irms as signed int
  signedUnsigned.unSigned = reg26_ch3.data.bits.irmsavgonesec; 
  float amps_avg_s = (float)signedUnsigned.Signed;

  //Convert from codes to the fraction of ADC Full Scale (16-bit)
  amps_avg_s /= 55000.0; 

  //Convert to amps
  amps_avg_s *= ACS37800_DEFAULT_CURRENT_RANGE; 

  printf("Register 20 : AVG Seconds       : %ld,  AVG Seconds Amps: %2f    Power:   %2f\r\n", signedUnsigned.unSigned, amps_avg_s, (amps_avg_s*(float(230.0))));
  // printf("%2f\r\n", amps_avg_s);

  // current_ch_3.readRegister(&reg27_ch3.data.all, ACS37800_REGISTER_VOLATILE_27);

  // //Extract irms as signed int
  // signedUnsigned.unSigned = reg27_ch3.data.bits.irmsavgonemin; 
  // float amps_avg_m = (float)signedUnsigned.Signed;

  // //Convert from codes to the fraction of ADC Full Scale (16-bit)
  // amps_avg_m /= 55000.0; 

  // //Convert to amps
  // amps_avg_m *= ACS37800_DEFAULT_CURRENT_RANGE; 

  // printf("Register 20 : AVG Minutes      : %ld,  AVG Minutes Amps: %2f\r\n", signedUnsigned.unSigned, amps_avg_m);
}


void print_data()
{
  union
  {
      int16_t Signed;
      uint16_t unSigned;
  } signedUnsigned; // Avoid any ambiguity when casting to signed int

  current_ch_3.readRegister(&reg26_ch3.data.all, ACS37800_REGISTER_VOLATILE_26);

  //Extract avg-seconds as signed int
  signedUnsigned.unSigned = reg26_ch3.data.bits.irmsavgonesec; 
  float amps_avg_s = (float)signedUnsigned.Signed;

  //Convert from codes to the fraction of ADC Full Scale (16-bit)
  amps_avg_s /= 55000.0; 

  //Convert to amps
  amps_avg_s *= ACS37800_DEFAULT_CURRENT_RANGE; 

  printf("Amps: %2f    Power:   %2f\r\n", amps_avg_s, (amps_avg_s*(float(230.0))));

}

void qvo_chart(uint32_t gain)
{
  uint32_t qvo_fine = -250;
  current_ch_3.setCurrentCoarseGain(gain, false);
  current_ch_3.setQvoFine(qvo_fine, false);

  printf("**************** Variable        :QVO Fine  Fixed:  Gain x%2f****************\r\n\r\n", ACS37800_CRS_SNS_GAINS[gain]);

  for (int a = 0; a < 51; a++)
  {
    printf("Test Case : %d     QVO_fine   :   %02X\r\n", a, qvo_fine);
    for(int i = 0; i < 10; i ++)
    {
      print_data();
      wait_us(500000);
    }

    printf("End of Test Case  : %d \r\n\r\n", a);
    // update sns fine
    qvo_fine = qvo_fine + 10;
    current_ch_3.setQvoFine(qvo_fine, false);
    wait_us(500000);
  }
  
}

void sns_chart(uint32_t gain)
{
  uint32_t sns_fine = -250;
  current_ch_3.setCurrentCoarseGain(gain, false);
  current_ch_3.setSnsFine(sns_fine, false);

  printf("**************** Variable        :SNS Fine  Fixed:  Gain x%2f****************\r\n\r\n", ACS37800_CRS_SNS_GAINS[gain]);

  for (int a = 0; a < 51; a++)
  {
    printf("Test Case : %d     SNS_fine   :   %02X\r\n", a, sns_fine);
    for(int i = 0; i < 10; i ++)
    {
      print_data();
      wait_us(500000);
    }

    printf("End of Test Case  : %d \r\n\r\n", a);
    // update sns fine
    sns_fine = sns_fine + 10;
    current_ch_3.setSnsFine(sns_fine, false);
    wait_us(500000);
  }
  
}

int main() {
  wait_us(2000000);  
  // put your setup code here, to run once:
  printf("ACS37800 TEST \n");
  current_ch_2.begin(CS2_ADDRESS);
  current_ch_3.begin(CS3_ADDRESS);

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


  relay1 = 1;
  relay2 = 1;
  relay3 = 1;

  // From the ACS37800 datasheet:
  // CONFIGURING THE DEVICE FOR AC APPLICATIONS : DYNAMIC CALCULATION OF N
  // Set bypass_n_en = 0 (default). This setting enables the device to
  // dynamically calculate N based off the voltage zero crossings.
  // Disable bypass_n in shadow memory and eeprom
  // current_ch_2.setBypassNenable(false, true);
  current_ch_3.setBypassNenable(false, true);

  // We need to connect the LO pin to the 'low' side of the AC source.
  // So we need to set the divider resistance to 4M Ohms (instead of 2M).
  // Comment this line if you are using GND to measure the 'low' side of the AC voltage
  // current_ch_2.setDividerRes(4000000); 

  // uint32_t gain = 7;

  // current_ch_2.setCurrentCoarseGain(gain, false);
  // // wait_us(100000);
  // current_ch_2.setCurrentAvgSelection(true, false);
  // // wait_us(100000);

  // float coarse_gain;
  // current_ch_2.getCurrentCoarseGain(&coarse_gain);
  // read_EEPROM();

  read_EEPROM_Config_1G();
  read_SHADOW_Config_1G();
  
  uint32_t rms_avg_1    = 124;
  uint32_t rms_avg_2    = 512;
  uint32_t course_gain  = 0;   // 3 --> x3.5
  uint32_t qvo_fine     = 0;   // 25 --> 0x19
  uint32_t sns_fine     = 0;  //  182 --> 0xB6
  uint32_t n_samples    = 512;

  current_ch_3.setNumberOfSamples(n_samples, false);
  current_ch_3.setBypassNenable(true, false);

  current_ch_3.setCurrentAvgSelection(true, false);
  current_ch_3.setRMSavg(rms_avg_1, rms_avg_2, false);
  current_ch_3.setCurrentCoarseGain(course_gain, false);
  current_ch_3.setQvoFine(qvo_fine, false);
  current_ch_3.setSnsFine(sns_fine, false);

  read_SHADOW_Config_1G();

  printf("----------------  Settings  ----------------\r\n");
  printf("N Samples       :   %ld\n", n_samples);
  printf("Course Gain     :   %ld\n", course_gain);
  printf("QVO FINE        :   %ld\n", qvo_fine);
  printf("SNS FINE        :   %ld\n", sns_fine);
  printf("RMS AVG 1       :   %ld\n", rms_avg_1);
  printf("--------------------------------------------\r\n");
  printf("When rms avg 1 : %ld\r\n", rms_avg_1);

  // qvo_chart(course_gain);
  // sns_chart(course_gain);

  // read_SHADOW_Config_1G();
  /*  
  printf("**************** Variable        :SNS Fine ****************\r\n\r\n");

  for (int a = 0; a < 51; a++)
  {
    printf("Test Case : %d     Sns_fine   :   %02X\r\n", a, sns_fine);
    for(int i = 0; i < 10; i ++)
    {
      print_data();
      wait_us(1000000);
    }

    printf("End of Test Case  : %d \r\n\r\n", a);
    // update sns fine
    sns_fine = sns_fine + 10;
    current_ch_3.setSnsFine(sns_fine, false);
  }

  wait_us(500000);
  */
  /*
  sns_fine = 0;
  current_ch_3.setSnsFine(sns_fine, false);

  for (int a = 0; a < 26; a++)
  {
    printf("Test Case : %d     Sns_fine   :   %02X\r\n", a + 25, sns_fine);
    for(int i = 0; i < 10; i ++)
    {
      print_data();
      wait_us(1000000);
    }

    printf("End of Test Case  : %d \r\n\r\n", a + 25);
    // update sns fine
    sns_fine = sns_fine + 10;
    current_ch_3.setSnsFine(sns_fine, false);
  }
  */

  while(1) {
    // put your main code here, to run repeatedly:
    // float volts = 0.0;
    // float amps = 0.0;

    // current_ch_2.readRMS(&volts, &amps);

    // printf("Volts: %2f  Amps: %2f\r\n", volts, amps);

    // wait_us(250000);

    read_data();
    // for (int a = 0; a < 5; a++)
    // {
    //   printf("Test Case : %d     Qvo_fine   :   %ld", a, qvo_fine);
    //   for(int i = 0; i < 20; i ++)
    //   {
    //     print_data();
    //     wait_us(1000000);
    //   }

    //   // update qvo fine
    //   qvo_fine = qvo_fine + 10;
    //   current_ch_3.setQvoFine(qvo_fine, false);
    // }

    wait_us(500000);
  }
}

