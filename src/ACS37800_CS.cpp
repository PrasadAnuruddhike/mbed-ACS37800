#include "ACS37800_CS.h"

ACS37800::ACS37800(I2C *i2c)
    :_i2c(i2c)
{
    
}

//Start I2C communication using the specified port
//Returns true if successful or false if no sensor detected
// bool ACS37800::begin(uint8_t address, I2C *i2c)
// {
//     _ACS37800Address = address;
//     _i2c = i2c;

// }

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