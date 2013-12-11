/**************************************************************************
    This library is for the GE ChipCap2 Humidity & Temperature Sensor
    
    0 ~ 100% Relative Humidity  (7 Sec. response time)
    -40 C ~ 125 C  Temperature (5 Sec. response time)
    With Low and High alarm triggers.
    
    Various models with analog and digital output @ 3.3V or 5V operation.
    
    Digi-Key Part Number: 235-1339-ND   (Digital i2c, 5V)
    Breakout Boards available at: www.circuitsforfun.com

    Written by: Richard Wardlow @ Circuits for Fun, LLC
    GNU GPL, include above text in redistribution
***************************************************************************/

#include <CFF_ChipCap2.h>
#include <Wire.h>

////////////////////////////////////////
// ChipCap2 Digital I2C Class Methods
////////////////////////////////////////
CFF_ChipCap2::CFF_ChipCap2(uint8_t addr)
{
    _i2caddr = addr;
}

// Initiate the Wire library and join the I2C bus 
void CFF_ChipCap2::begin(void)
{
    Wire.begin(_i2caddr);
    status = 0;
}

// Configure which pin is used for the ChipCap2 Power pin and set as output
void CFF_ChipCap2::configPowerPin(uint8_t pwrpin)
{
    _pwrpin = pwrpin;
    pinMode(_pwrpin, OUTPUT);
}

// Configure which pin is used for the ChipCap2 Ready pin and set as input
void CFF_ChipCap2::configReadyPin(uint8_t readypin)
{
    _readypin = readypin;
    pinMode(_readypin, INPUT);
}

// Configure which pin is used for the ChipCap2 Alarm Low pin and set as input
void CFF_ChipCap2::configAlarmLowPin(uint8_t alowpin)
{
    _alowpin = alowpin;
    pinMode(_alowpin, INPUT);
}

// Configure which pin is used for the ChipCap2 Alarm High pin and set as input
void CFF_ChipCap2::configAlarmHighPin(uint8_t ahighpin)
{
    _ahighpin = ahighpin;
    pinMode(_ahighpin, INPUT);
}

// If using the pwrpin for powering chipcap use this function to turn it on or off
void CFF_ChipCap2::power(uint8_t onOff)
{
    if(_pwrpin != NULL)
    {
        if(onOff == 0)
            digitalWrite(_pwrpin, LOW);
        
        if(onOff == 1)
            digitalWrite(_pwrpin, HIGH);
    }
}

// Check the ChipCap2 Ready pin to see if conversion is complete
bool CFF_ChipCap2::dataReady(void)
{
    uint8_t val;
    
    val = digitalRead(_readypin);
    if ( val == 0)
        return false;
    if (val == 1)
        return true;

}

// Check for Alarm Low Trigger
bool CFF_ChipCap2::checkAlarmLow(void)
{
    uint8_t val;
    
    val = digitalRead(_alowpin);
    if ( val == 0)
        return false;
    if (val == 1)
        return true;
}

// Set Alarm Low Trigger Value in Celsius
void CFF_ChipCap2::setAlarmLowVal(int16_t alarmlowon)
{
    readSensor();
    if (status == CCF_CHIPCAP2_STATUS_COMMANDMODE)
    {
        uint16_t alowon, alowoff;
        uint8_t lowByte, hiByte;
        int8_t alarmlowoff = alarmlowon + 1;
        alowon = ((alarmlowon + 40) * 99);
        lowByte = alowon & 255;
        hiByte = alowon >> 8;
        Wire.beginTransmission(_i2caddr);
        Wire.write(0x5A);
        Wire.write(hiByte);
        Wire.write(lowByte);
        Wire.endTransmission();
        delay(2);
        alowoff = ((alarmlowoff + 40) * 99);
        lowByte = alowoff & 255;
        hiByte = alowoff >> 8;
        Wire.beginTransmission(_i2caddr);
        Wire.write(0x5B);
        Wire.write(hiByte);
        Wire.write(lowByte);
        Wire.endTransmission();
        delay(2);
    }
}

int16_t CFF_ChipCap2::getAlarmLowVal()
{
    readSensor();
    if (status == CCF_CHIPCAP2_STATUS_COMMANDMODE)
    {
        uint8_t edat[3];
        int16_t eALow;
        Wire.beginTransmission(_i2caddr);
        Wire.write(0x1A);
        Wire.write(0);
        Wire.write(0);
        Wire.endTransmission();
        delay(1);
        Wire.beginTransmission(_i2caddr);
        Wire.requestFrom((uint8_t)_i2caddr, (uint8_t)3, (uint8_t)true);
        edat[0] = Wire.read();
        edat[1] = Wire.read();
        edat[2] = Wire.read();
        Wire.endTransmission();
        eALow = (((edat[1] << 8) + (edat[2])) / 99) - 40; 
        return eALow;
    }
}

// Check for Alarm High Trigger
bool CFF_ChipCap2::checkAlarmHigh(void)
{
    uint8_t val;
    
    val = digitalRead(_ahighpin);
    if ( val == 0)
        return false;
    if (val == 1)
        return true;
}

// Set Alarm High Trigger Value in Celsius
void CFF_ChipCap2::setAlarmHighVal(int16_t alarmhighon)
{
    readSensor();
    if (status == CCF_CHIPCAP2_STATUS_COMMANDMODE)
    {
        uint16_t ahion, ahioff;
        uint8_t lowByte, hiByte;
        int8_t alarmhighoff = alarmhighon - 1;
        ahion = ((alarmhighon + 40) * 99);
        lowByte = ahion & 255;
        hiByte = ahion >> 8;
        Wire.beginTransmission(_i2caddr);
        Wire.write(0x58);
        Wire.write(hiByte);
        Wire.write(lowByte);
        Wire.endTransmission();
        delay(2);
        ahioff = ((alarmhighoff + 40) * 99);
        lowByte = ahioff & 255;
        hiByte = ahioff >> 8;
        Wire.beginTransmission(_i2caddr);
        Wire.write(0x59);
        Wire.write(hiByte);
        Wire.write(lowByte);
        Wire.endTransmission();
        delay(2);
    }
}

int16_t CFF_ChipCap2::getAlarmHighVal()
{
    readSensor();
    if (status == CCF_CHIPCAP2_STATUS_COMMANDMODE)
    {
        uint8_t edat[3];
        int16_t eAHigh;
        Wire.beginTransmission(_i2caddr);
        Wire.write(0x18);
        Wire.write(0);
        Wire.write(0);
        Wire.endTransmission();
        delay(1);
        Wire.beginTransmission(_i2caddr);
        Wire.requestFrom((uint8_t)_i2caddr, (uint8_t)3, (uint8_t)true);
        edat[0] = Wire.read();
        edat[1] = Wire.read();
        edat[2] = Wire.read();
        Wire.endTransmission();
        eAHigh = (((edat[1] << 8) + (edat[2])) / 99.29) - 40; 
        return eAHigh;
    }
}

// Enters command mode (Only valid during first 10ms after power-on)
void CFF_ChipCap2::startCommandMode(void)
{
    if (_pwrpin != NULL)
    {
        power(0);
        delay(50);
        power(1);
        delay(2);
    }
    Wire.beginTransmission(_i2caddr);
    Wire.write(0xA0);
    Wire.write(0);
    Wire.write(0);
    Wire.endTransmission();
    delay(5);
    readSensor();
}

// Enters normal mode of operation
void CFF_ChipCap2::startNormalMode(void)
{
    Wire.beginTransmission(_i2caddr);
    Wire.write(0x80);
    Wire.write(0);
    Wire.write(0);
    Wire.endTransmission();
    delay(5);
    readSensor();
}

// Read Humidity and Temperature Sensor Data
void CFF_ChipCap2::readSensor(void)
{
    uint8_t rht[4];
    
    Wire.beginTransmission(_i2caddr);
    Wire.requestFrom((uint8_t)_i2caddr, (uint8_t)4, (uint8_t)true);
    rht[0] = Wire.read();
    rht[1] = Wire.read();
    rht[2] = Wire.read();
    rht[3] = Wire.read();	
    Wire.endTransmission();
    status = rht[0] >> 6;
    humidity = (((rht[0] & 63) << 8) + rht[1]) / 163.84;
    temperatureC = (((rht[2] << 6) + (rht[3] / 4)) / 99.29) - 40; 
    temperatureF = temperatureC * 1.8 + 32;
}



//////////////////////////////////////////////////////
// ChipCap2 Analog (Pulse Density Modulation) Class
//////////////////////////////////////////////////////

// TODO....
