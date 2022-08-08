#include "ACS37800_CS.h"

ACS37800::ACS37800(I2C *i2c)
    :_i2c(i2c)
{
    
}

//Start I2C communication using the specified port
//Returns true if successful or false if no sensor detected
bool ACS37800::begin(int address)
{
    _ACS37800Address = address << 1;

    return true;
}

//Read a register's contents. Contents are returned in data.
ACS37800ERR ACS37800::readRegister(uint32_t *value, uint8_t address)
{   
    char data[4] = {0x00, 0x00, 0x00, 0x00};
    const char addr = address;
    _i2c->write(_ACS37800Address, &addr, 1);

    // Read 4 bytes (32 bits)
    _i2c->read(_ACS37800Address, data, 4);

    //Data is returned LSB first (little endian)
    uint32_t readData = data[3];
    readData = readData << 8;
    readData = readData | data[2];
    readData = readData << 8;
    readData = readData | data[1];
    readData = readData << 8;
    readData = readData | data[0];

    *value = readData;

    // value = data[3];
    // value = value << 8;
    // value = value | data[2];
    // value = value << 8;
    // value = value | data[1];
    // value = value << 8;
    // value = value | data[0];

    return(ACS37800_SUCCESS);
}

//Write data to the selected register
ACS37800ERR ACS37800::writeRegister(uint32_t value, uint8_t address)
{
    const char addr = address;
    char data[5];
    data[0] = address;
    data[1] = value;
    value = value >> 8;
    data[2] = value;
    value = value >> 8;
    data[3] = value;
    value = value >> 8;
    data[4] = value;

    _i2c->write(_ACS37800Address, data, 5);

    return(ACS37800_SUCCESS);
}

//Set the number of samples for RMS calculations. Bypass_N_Enable must be set/true for this to have effect.
ACS37800ERR ACS37800::setNumberOfSamples(uint32_t numberOfSamples, bool _eeprom)
{
    // Set the customer access code
    ACS37800ERR error = writeRegister(ACS37800_CUSTOMER_ACCESS_CODE, ACS37800_REGISTER_VOLATILE_2F);

    ACS37800_REGISTER_0F_t store;
    error = readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1F);

    //Adjust the number of samples (limit to 10 bits)
    store.data.bits.n = numberOfSamples & 0x3FF;

    // Write register 1F
    error = writeRegister(store.data.all, ACS37800_REGISTER_SHADOW_1F);


    // Check if user wants to set eeprom too
    if(_eeprom)
    {
        // Read register 0F
        error = readRegister(&store.data.all, ACS37800_REGISTER_EEPROM_0F); 
        
        //Adjust the number of samples (limit to 10 bits)
        store.data.bits.n = numberOfSamples & 0x3FF; 

        // Write register 0F
        error = writeRegister(store.data.all, ACS37800_REGISTER_EEPROM_0F); 
    }

    // Clear the customer access code
    error = writeRegister(0, ACS37800_REGISTER_VOLATILE_2F); 
    ThisThread::sleep_for(100ms);

    return (error);
}

//Read and return the number of samples from shadow memory
ACS37800ERR ACS37800::getNumberOfSamples(uint32_t *numberOfSamples)
{
    ACS37800_REGISTER_0F_t store;

    // Read register 1F
    ACS37800ERR error = readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1F);

    //Return the number of sample
    *numberOfSamples = store.data.bits.n; 
    printf("getNumberOfSamples: number of samples is currently: %d\r\n", *numberOfSamples);

    return (error);
}

//Set/Clear the Bypass_N_Enable flag
ACS37800ERR ACS37800::setBypassNenable(bool bypass, bool _eeprom)
{
    // Set the customer access code
    ACS37800ERR error = writeRegister(ACS37800_CUSTOMER_ACCESS_CODE, ACS37800_REGISTER_VOLATILE_2F);

    ACS37800_REGISTER_0F_t store;

    // Read register 1F
    error = readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1F); 

    if (bypass) //Adjust bypass_n_en
    {
        store.data.bits.bypass_n_en = 1;
    }
    else
    {
        store.data.bits.bypass_n_en = 0;
    }

    // Write register 1F
    error = writeRegister(store.data.all, ACS37800_REGISTER_SHADOW_1F); 

    // Check if user wants to set eeprom too
    if(_eeprom)
    {
        // Read register 0F
        error = readRegister(&store.data.all, ACS37800_REGISTER_EEPROM_0F); 

        if (bypass) //Adjust bypass_n_en
        {
        store.data.bits.bypass_n_en = 1;
        }
        else
        {
        store.data.bits.bypass_n_en = 0;
        }

        // Write register 0F
        error = writeRegister(store.data.all, ACS37800_REGISTER_EEPROM_0F); 
    }

    // Clear the customer access code
    error = writeRegister(0, ACS37800_REGISTER_VOLATILE_2F);
    ThisThread::sleep_for(100ms);

    return (error);
}

// Read and return the bypass_n_en flag from shadow memory
ACS37800ERR ACS37800::getBypassNenable(bool *bypass)
{
    ACS37800_REGISTER_0F_t store;

    // Read register 1F
    ACS37800ERR error = readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1F); 

    //Return bypass_n_en
    *bypass = (bool)store.data.bits.bypass_n_en; 
    printf("getBypassNenable: bypass_n_en is currently: %d\r\n", *bypass);

    return (error);
}

//Get the coarse current gain from shadow memory
ACS37800ERR ACS37800::getCurrentCoarseGain(float *currentCoarseGain)
{
    ACS37800_REGISTER_0B_t store;

    // Read register 1B
    ACS37800ERR error = readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1B); 

    float gain = ACS37800_CRS_SNS_GAINS[store.data.bits.crs_sns];

    uint32_t iavgselen = store.data.bits.iavgselen;
    printf("i avg selection bit: %d\r\n", iavgselen);

    //Return the gain
    *currentCoarseGain = gain; 

    printf("getCurrentCoarseGain: shadow gain setting is currently: %f\r\n", *currentCoarseGain);

    return(error);
}

// Read volatile register 0x20. Return the vInst (Volts) and iInst (Amps).
ACS37800ERR ACS37800::readRMS(float *vRMS, float *iRMS)
{
    ACS37800_REGISTER_20_t store;

    // Read register 20
    ACS37800ERR error = readRegister(&store.data.all, ACS37800_REGISTER_VOLATILE_20); 

    //Extract vrms. Convert to voltage in Volts.
    // Note: datasheet says "RMS voltage output. This field is an unsigned 16-bit fixed point number with 16 fractional bits"
    // Datasheet also says "Voltage Channel ADC Sensitivity: 110 LSB/mV"
    float volts = (float)store.data.bits.vrms;

    //Convert from codes to the fraction of ADC Full Scale (16-bit)
    volts /= 55000.0; 

    //Convert to mV (Differential Input Range is +/- 250mV)
    volts *= 250; 

    //Convert to Volts
    volts /= 1000; 

    //Correct for the voltage divider: (RISO1 + RISO2 + RSENSE) / RSENSE
    //Or:  (RISO1 + RISO2 + RISO3 + RISO4 + RSENSE) / RSENSE
    float resistorMultiplier = (_dividerResistance + _senseResistance) / _senseResistance;
    volts *= resistorMultiplier;

    *vRMS = volts;
    printf("readRMS: volts (V, after correction) is %f\r\n", *vRMS);

    //Extract the irms. Convert to current in Amps.
    //Datasheet says: "RMS current output. This field is a signed 16-bit fixed point number with 15 fractional bits"

    union
    {
        int16_t Signed;
        uint16_t unSigned;
    } signedUnsigned; // Avoid any ambiguity when casting to signed int

    //Extract irms as signed int
    signedUnsigned.unSigned = store.data.bits.irms; 
    float amps = (float)signedUnsigned.Signed;

    //Convert from codes to the fraction of ADC Full Scale (16-bit)
    amps /= 55000.0; 

    //Convert to Amps
    amps *= _currentSensingRange; 

    *iRMS = amps;
    printf("readRMS: amps (A, after correction) is %f\r\n", *iRMS);

    return(error);
}

// Read volatile registers 0x2A and 0x2C. Return the vInst (Volts), iInst (Amps) and pInst (VAR).
// ACS37800ERR ACS37800::readInstantaneous(float *vInst, float *iInst, float *pInst)
// {
//     ACS37800_REGISTER_2A_t store;
    
//     // Read register 2A
//     ACS37800ERR error = readRegister(&store.data.all, ACS37800_REGISTER_VOLATILE_2A); 


// }

//Change the value of the sense resistor (Ohms)
void ACS37800::setSenseRes(float newRes)
{
    _senseResistance = newRes;
}

//Change the value of the voltage divider resistance (Ohms)
void ACS37800::setDividerRes(float newRes)
{
    _dividerResistance = newRes;
}

//Change the current-sensing range (Amps)
//ACS37800KMACTR-030B3-I2C is a 30.0 Amp part - as used on the SparkFun Qwiic Power Meter
//ACS37800KMACTR-090B3-I2C is a 90.0 Amp part
void ACS37800::setCurrentRange(float newCurrent)
{
    _currentSensingRange = newCurrent;
}



ACS37800ERR ACS37800::setCurrentCoarseGain(uint32_t currentCoarseGain, bool _eeprom)
{
    // Set the customer access code
    ACS37800ERR error = writeRegister(ACS37800_CUSTOMER_ACCESS_CODE, ACS37800_REGISTER_VOLATILE_2F);

    ACS37800_REGISTER_0B_t store;

    // Read register 1B
    error = readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1B); 

    store.data.bits.crs_sns = currentCoarseGain;

    printf("readGain: %d actual gain: %f\r\n", store.data.bits.crs_sns, ACS37800_CRS_SNS_GAINS[currentCoarseGain]);

    // Write register 1B
    error = writeRegister(store.data.all, ACS37800_REGISTER_SHADOW_1B); 

    // Check if user wants to set eeprom too
    if(_eeprom)
    {
        // Read register 0B
        error = readRegister(&store.data.all, ACS37800_REGISTER_EEPROM_0B); 

        store.data.bits.crs_sns = ACS37800_CRS_SNS_GAINS[currentCoarseGain];

        // Write register 0B
        error = writeRegister(store.data.all, ACS37800_REGISTER_EEPROM_0B); 
    }

    // Clear the customer access code
    error = writeRegister(0, ACS37800_REGISTER_VOLATILE_2F);
    ThisThread::sleep_for(100ms);

    return (error);
}


ACS37800ERR ACS37800::setCurrentAvgSelection(bool currentAvgSelection, bool _eeprom)
{
    // Set the customer access code
    ACS37800ERR error = writeRegister(ACS37800_CUSTOMER_ACCESS_CODE, ACS37800_REGISTER_VOLATILE_2F);

    ACS37800_REGISTER_0B_t store;

    // Read register 1B
    error = readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1B); 

    //set current avg selection
    if (currentAvgSelection) 
    {
        store.data.bits.iavgselen = 1;
    }
    else
    {
        store.data.bits.iavgselen = 0;
    }

    // Write register 1B
    error = writeRegister(store.data.all, ACS37800_REGISTER_SHADOW_1B); 
    printf("Error: %d set iavg sel: %d \r\n", error, store.data.bits.iavgselen);

    // Check if user wants to set eeprom too
    if(_eeprom)
    {
        // Read register 0B
        error = readRegister(&store.data.all, ACS37800_REGISTER_EEPROM_0B); 

        //set current avg selection
        if (currentAvgSelection) 
        {
            store.data.bits.iavgselen = 1;
        }
        else
        {
            store.data.bits.iavgselen = 0;
        }

        // Write register 0B
        error = writeRegister(store.data.all, ACS37800_REGISTER_EEPROM_0B); 
    }

    // Clear the customer access code
    error = writeRegister(0, ACS37800_REGISTER_VOLATILE_2F);
    ThisThread::sleep_for(100ms);

    return (error);
}


ACS37800ERR ACS37800::setRMSavg(uint32_t rms_avg_1, uint32_t rms_avg_2, bool _eeprom)
{
    // Set the customer access code
    ACS37800ERR error = writeRegister(ACS37800_CUSTOMER_ACCESS_CODE, ACS37800_REGISTER_VOLATILE_2F);

    ACS37800_REGISTER_0C_t store;

    // Read register 1C
    error = readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1C); 

    store.data.bits.rms_avg_1 = rms_avg_1;
    store.data.bits.rms_avg_2 = rms_avg_2;

    // Write register 1C
    error = writeRegister(store.data.all, ACS37800_REGISTER_SHADOW_1C); 

    // Check if user wants to set eeprom too
    if(_eeprom)
    {
        // Read register 0C
        error = readRegister(&store.data.all, ACS37800_REGISTER_EEPROM_0C); 

        store.data.bits.rms_avg_1 = rms_avg_1;
        store.data.bits.rms_avg_2 = rms_avg_2;

        // Write register 0C
        error = writeRegister(store.data.all, ACS37800_REGISTER_EEPROM_0C); 
    }

    // Clear the customer access code
    error = writeRegister(0, ACS37800_REGISTER_VOLATILE_2F);
    ThisThread::sleep_for(100ms);

    return (error);
}


ACS37800ERR ACS37800::setQvoFine(uint32_t qvo_fine, bool _eeprom)
{
    // Set the customer access code
    ACS37800ERR error = writeRegister(ACS37800_CUSTOMER_ACCESS_CODE, ACS37800_REGISTER_VOLATILE_2F);

    ACS37800_REGISTER_0B_t store;

    // Read register 1B
    error = readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1B); 

    store.data.bits.qvo_fine = qvo_fine;

    // Write register 1B
    error = writeRegister(store.data.all, ACS37800_REGISTER_SHADOW_1B); 

    // Check if user wants to set eeprom too
    if(_eeprom)
    {
        // Read register 0B
        error = readRegister(&store.data.all, ACS37800_REGISTER_EEPROM_0B); 

        store.data.bits.qvo_fine = qvo_fine;

        // Write register 0B
        error = writeRegister(store.data.all, ACS37800_REGISTER_EEPROM_0B); 
    }

    // Clear the customer access code
    error = writeRegister(0, ACS37800_REGISTER_VOLATILE_2F);
    ThisThread::sleep_for(100ms);

    return (error);
}

ACS37800ERR ACS37800::setSnsFine(uint32_t sns_fine, bool _eeprom)
{
    // Set the customer access code
    ACS37800ERR error = writeRegister(ACS37800_CUSTOMER_ACCESS_CODE, ACS37800_REGISTER_VOLATILE_2F);

    ACS37800_REGISTER_0B_t store;

    // Read register 1B
    error = readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1B); 

    store.data.bits.sns_fine = sns_fine;

    // Write register 1B
    error = writeRegister(store.data.all, ACS37800_REGISTER_SHADOW_1B); 

    // Check if user wants to set eeprom too
    if(_eeprom)
    {
        // Read register 0B
        error = readRegister(&store.data.all, ACS37800_REGISTER_EEPROM_0B); 

        store.data.bits.sns_fine = sns_fine;

        // Write register 0B
        error = writeRegister(store.data.all, ACS37800_REGISTER_EEPROM_0B); 
    }

    // Clear the customer access code
    error = writeRegister(0, ACS37800_REGISTER_VOLATILE_2F);
    ThisThread::sleep_for(100ms);

    return (error);
}

void ACS37800::getEEPROMSettings()
{
    ACS37800_REGISTER_0B_t reg0B_store;
    ACS37800_REGISTER_0C_t reg0C_store;
    ACS37800_REGISTER_0D_t reg0D_store;
    ACS37800_REGISTER_0E_t reg0E_store;
    ACS37800_REGISTER_0F_t reg0F_store;

    readRegister(&reg0B_store.data.all, ACS37800_REGISTER_EEPROM_0B);
    readRegister(&reg0C_store.data.all, ACS37800_REGISTER_EEPROM_0C);
    readRegister(&reg0D_store.data.all, ACS37800_REGISTER_EEPROM_0D);
    readRegister(&reg0E_store.data.all, ACS37800_REGISTER_EEPROM_0E);
    readRegister(&reg0F_store.data.all, ACS37800_REGISTER_EEPROM_0F);

    printf("================ Current Sensor EEPROM ================\r\n");
    printf("Register 0B : qvo_fine          : %02lX\r\n", reg0B_store.data.bits.qvo_fine);
    printf("Register 0B : sns_fine          : %02lX\r\n", reg0B_store.data.bits.sns_fine);
    printf("Register 0B : crs_sns           : %02lX\r\n", reg0B_store.data.bits.crs_sns);
    printf("Register 0B : iavgselen         : %02lX\r\n", reg0B_store.data.bits.iavgselen);
    printf("Register 0B : pavgselen         : %02lX\r\n", reg0B_store.data.bits.pavgselen);

    printf("Register 0C : rms_avg_1         : %02lX\r\n", reg0C_store.data.bits.rms_avg_1);
    printf("Register 0C : rms_avg_2         : %02lX\r\n", reg0C_store.data.bits.rms_avg_2);
    printf("Register 0C : vchan_offset      : %02lX\r\n", reg0C_store.data.bits.vchan_offset_code);

    printf("Register 0D : ichan_del_en      : %02lX\r\n", reg0D_store.data.bits.ichan_del_en);
    printf("Register 0D : chan_del_sel      : %02lX\r\n", reg0D_store.data.bits.chan_del_sel);
    printf("Register 0D : fault             : %02lX\r\n", reg0D_store.data.bits.fault);
    printf("Register 0D : fltdly            : %02lX\r\n", reg0D_store.data.bits.fltdly);

    printf("Register 0E : vevent_cycs       : %02lX\r\n", reg0E_store.data.bits.vevent_cycs);
    printf("Register 0E : overvreg          : %02lX\r\n", reg0E_store.data.bits.overvreg);
    printf("Register 0E : undervreg         : %02lX\r\n", reg0E_store.data.bits.undervreg);
    printf("Register 0E : delaycnt_sel      : %02lX\r\n", reg0E_store.data.bits.delaycnt_sel);
    printf("Register 0E : halfcycle_en      : %02lX\r\n", reg0E_store.data.bits.halfcycle_en);
    printf("Register 0E : squarewave_en     : %02lX\r\n", reg0E_store.data.bits.squarewave_en);
    printf("Register 0E : zerocrosschansel  : %02lX\r\n", reg0E_store.data.bits.zerocrosschansel);
    printf("Register 0E : zerocrossedgesel  : %02lX\r\n", reg0E_store.data.bits.zerocrossedgesel);

    printf("Register 0F : i2c_slv_addr      : %02lX\r\n", reg0F_store.data.bits.i2c_slv_addr);
    printf("Register 0F : i2c_dis_slv_addr  : %02lX\r\n", reg0F_store.data.bits.i2c_dis_slv_addr);
    printf("Register 0F : dio_0_sel         : %02lX\r\n", reg0F_store.data.bits.dio_0_sel);
    printf("Register 0F : dio_1_sel         : %02lX\r\n", reg0F_store.data.bits.dio_1_sel);
    printf("Register 0F : n                 : %02lX\r\n", reg0F_store.data.bits.n);
    printf("Register 0F : bypass_n_en       : %02lX\r\n\r\n", reg0F_store.data.bits.bypass_n_en);
}

void ACS37800::getShadowSettings()
{
    ACS37800_REGISTER_0B_t reg1B_store;
    ACS37800_REGISTER_0C_t reg1C_store;
    ACS37800_REGISTER_0D_t reg1D_store;
    ACS37800_REGISTER_0E_t reg1E_store;
    ACS37800_REGISTER_0F_t reg1F_store;

    readRegister(&reg1B_store.data.all, ACS37800_REGISTER_SHADOW_1B);
    readRegister(&reg1C_store.data.all, ACS37800_REGISTER_SHADOW_1C);
    readRegister(&reg1D_store.data.all, ACS37800_REGISTER_SHADOW_1D);
    readRegister(&reg1E_store.data.all, ACS37800_REGISTER_SHADOW_1E);
    readRegister(&reg1F_store.data.all, ACS37800_REGISTER_SHADOW_1F);

    printf("================ Current Sensor Shadow ================\r\n");
    printf("Register 1B : qvo_fine          : %02lX\r\n", reg1B_store.data.bits.qvo_fine);
    printf("Register 1B : sns_fine          : %02lX\r\n", reg1B_store.data.bits.sns_fine);
    printf("Register 1B : crs_sns           : %02lX\r\n", reg1B_store.data.bits.crs_sns);
    printf("Register 1B : iavgselen         : %02lX\r\n", reg1B_store.data.bits.iavgselen);
    printf("Register 1B : pavgselen         : %02lX\r\n", reg1B_store.data.bits.pavgselen);

    printf("Register 1C : rms_avg_1         : %02lX\r\n", reg1C_store.data.bits.rms_avg_1);
    printf("Register 1C : rms_avg_2         : %02lX\r\n", reg1C_store.data.bits.rms_avg_2);
    printf("Register 1C : vchan_offset      : %02lX\r\n", reg1C_store.data.bits.vchan_offset_code);

    printf("Register 1D : ichan_del_en      : %02lX\r\n", reg1D_store.data.bits.ichan_del_en);
    printf("Register 1D : chan_del_sel      : %02lX\r\n", reg1D_store.data.bits.chan_del_sel);
    printf("Register 1D : fault             : %02lX\r\n", reg1D_store.data.bits.fault);
    printf("Register 1D : fltdly            : %02lX\r\n", reg1D_store.data.bits.fltdly);

    printf("Register 1E : vevent_cycs       : %02lX\r\n", reg1E_store.data.bits.vevent_cycs);
    printf("Register 1E : overvreg          : %02lX\r\n", reg1E_store.data.bits.overvreg);
    printf("Register 1E : undervreg         : %02lX\r\n", reg1E_store.data.bits.undervreg);
    printf("Register 1E : delaycnt_sel      : %02lX\r\n", reg1E_store.data.bits.delaycnt_sel);
    printf("Register 1E : halfcycle_en      : %02lX\r\n", reg1E_store.data.bits.halfcycle_en);
    printf("Register 1E : squarewave_en     : %02lX\r\n", reg1E_store.data.bits.squarewave_en);
    printf("Register 1E : zerocrosschansel  : %02lX\r\n", reg1E_store.data.bits.zerocrosschansel);
    printf("Register 1E : zerocrossedgesel  : %02lX\r\n", reg1E_store.data.bits.zerocrossedgesel);

    printf("Register 1F : i2c_slv_addr      : %02lX\r\n", reg1F_store.data.bits.i2c_slv_addr);
    printf("Register 1F : i2c_dis_slv_addr  : %02lX\r\n", reg1F_store.data.bits.i2c_dis_slv_addr);
    printf("Register 1F : dio_0_sel         : %02lX\r\n", reg1F_store.data.bits.dio_0_sel);
    printf("Register 1F : dio_1_sel         : %02lX\r\n", reg1F_store.data.bits.dio_1_sel);
    printf("Register 1F : n                 : %02lX\r\n", reg1F_store.data.bits.n);
    printf("Register 1F : bypass_n_en       : %02lX\r\n\r\n", reg1F_store.data.bits.bypass_n_en);
}

ACS37800ERR ACS37800::setPreDefineValue()
{
    ACS37800_REGISTER_0B_t reg1B_store;

    uint32_t rms_avg_1      =   124;
    uint32_t rms_avg_2      =   512;
    uint32_t course_gain    =   3;          // 3 --> x3.5
    uint32_t n_samples      =   512;
    uint32_t qvo_fine       =   0;          
    uint32_t sns_fine       =   0;

    // @Shadow: Set number of samples for RMS Calculation 
    ACS37800ERR error = setNumberOfSamples(n_samples, false);

    // @Shadow: Enable RMS calculation based on 'n' instead of zero cross 
    error = setBypassNenable(true, false);

    // @Shadow: Enable Current average selection 'iavgselen'
    error = setCurrentAvgSelection(true, false);

    // @Shadow: Set RMS average value
    error = setRMSavg(rms_avg_1, rms_avg_2, false);

    // @Shadow: Set current Coarse Gain
    error = setCurrentCoarseGain(course_gain, false);

    // Read EEPROM 0x1B to get default qvo_fine and sns_fine data
    error = readRegister(&reg1B_store.data.all, ACS37800_REGISTER_EEPROM_0B);

    qvo_fine = reg1B_store.data.bits.qvo_fine;
    sns_fine = reg1B_store.data.bits.sns_fine;

    // @Shadow: Set QVO Fine which get from EEPROM
    error = setQvoFine(qvo_fine, false);

    // @Shadow: Set SNS Fine which get from EEPROM
    error = setSnsFine(sns_fine, false);

    return error;
}

void ACS37800::calibrateSensor()
{
    uint32_t qvo_fine = 0x19;
    union
    {
        int16_t Signed;
        uint16_t unSigned;
    } signedUnsigned; // Avoid any ambiguity when casting to signed int
    
    bool is_calibration_PCS = true;
    int ocounter_PCS = 0;
    ACS37800_REGISTER_26_t reg26_store;

    ThisThread::sleep_for(5s);
    while(is_calibration_PCS)
    {
        if(ocounter_PCS > 0 && ocounter_PCS % 5 == 0)
        {
            qvo_fine = qvo_fine - 1;
            setQvoFine(qvo_fine, false);
        }
        readRegister(&reg26_store.data.all, ACS37800_REGISTER_VOLATILE_26);
        
        // Extract irmsavgsec as signed int
        signedUnsigned.unSigned = reg26_store.data.bits.irmsavgonesec;
        float amps_avg_s = (float)signedUnsigned.Signed;

        //Convert from codes to the fraction of ADC Full Scale (16-bit)
        amps_avg_s /= 55000.0; 

        //Convert to amps
        amps_avg_s *= ACS37800_DEFAULT_CURRENT_RANGE; 

        printf("%d - Register 26 : AVG Seconds       : 0x%02X,  AVG Seconds Amps: %2f    Power:   %2f, QVO: 0x%02X\r\n",ocounter_PCS, signedUnsigned.unSigned, amps_avg_s, (amps_avg_s*(float(230.0))), qvo_fine);


        ThisThread::sleep_for(2s);
        ocounter_PCS++;
    }
}