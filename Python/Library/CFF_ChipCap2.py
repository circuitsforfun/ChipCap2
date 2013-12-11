"""
    This library is for the GE ChipCap2 Humidity & Temperature Sensor

    0 ~ 100% Relative Humidity  (7 Sec. response time)
    -40 C ~ 125 C  Temperature (5 Sec. response time)
    Low and High Alarm settings.

    Various models with analog and digital output @ 3.3V or 5V operation.

    Digi-Key Part Number: 235-1339-ND   (Digital i2c, 5V)
    Breakout Boards available at: www.circuitsforfun.com

    Written by: Richard Wardlow for Circuits for Fun, LLC
    GNU GPL, include above text in redistribution
"""

VERSION = '1.0.0'

import time

CFF_CHIPCAP2_DEFAULT_ADDR = 0x28
CCF_CHIPCAP2_STATUS_VALID = 0
CCF_CHIPCAP2_STATUS_STALE = 1
CCF_CHIPCAP2_STATUS_COMMANDMODE = 2
CCF_CHIPCAP2_COMMAND_BUSY = 0
CCF_CHIPCAP2_COMMAND_SUCCESS = 1
CCF_CHIPCAP2_COMMAND_ERROR = 2
CCF_CHIPCAP2_EEPROM_ERROR_CORRECTED = 2
CCF_CHIPCAP2_EEPROM_ERROR_UNCORRECTED = 4
CCF_CHIPCAP2_EEPROM_ERROR_RAM = 8
CCF_CHIPCAP2_EEPROM_ERROR_CONFIG = 16


class CFF_ChipCap2:
    def __init__(self, mb=None, i2cAddr=CFF_CHIPCAP2_DEFAULT_ADDR, powerPin=1, readyPin=2, alarmLowPin=None, alarmHighPin=None):
        self.i2caddr = i2cAddr
        self.pwrpin = powerPin
        self.readypin = readyPin
        self.alowpin = alarmLowPin
        self.ahighpin = alarmHighPin
        self.mb = mb
        self.temperatureC = 0
        self.temperatureF = 0
        self.humidity = 0
        self.status = 0
        
        if self.mb != None:
            self.mb.digitalState(4,1)
            if self.alowpin != None:
                self.mb.digitalState(self.alowpin, 1)
            if self.ahighpin != None:
                self.mb.digitalState(self.ahighpin, 1)
            self.mb.digitalState(self.readypin, 1)

    def startCommandMode(self):
        """Put ChipCap2 into Command Mode
        Usage: startCommandMode()"""
        if self.mb != None:
            self.mb.pinLow(self.pwrpin)
            self.mb.pausems(50)
            self.mb.pinHigh(self.pwrpin)
            self.mb.pausems(1)
            self.mb.i2cWrite((self.i2caddr << 1), [0xA0, 0, 0])
            self.mb.pausems(5)
            self.readSensor()

    def startNormalMode(self):
        """Put ChipCap2 into Normal Mode if it is in Command Mode
        Usage: startNormalMode()"""
        if self.mb != None:
            self.mb.i2cWrite((self.i2caddr << 1),[0x80, 0, 0])
            self.mb.pausems(5)
            self.readSensor()

    def checkAlarmLow(self):
        """Check if the Low Alarm has been triggered
        Usage: checkAlarmLow()"""
        if self.mb != None:
            if self.mb.digitalRead(self.alowpin) == 1:
                return True
            else:
                return False
        else:
            return False

    def setAlarmLowVal(self, alarmValue):
        """Set The Alarm Low Value in Celsius
        Usage: setAlarmLowVal(10)"""
        if self.mb != None:
            self.readSensor()
            if self.status == CCF_CHIPCAP2_STATUS_COMMANDMODE:
                alowon = (alarmValue + 40) * 99
                lowByte = alowon & 255
                hiByte = alowon >> 8
                self.mb.i2cWrite((self.i2caddr << 1),[0x5A, hiByte, lowByte])
                self.mb.pausems(2)
                alowoff = ((alarmValue + 1) + 40) * 99
                lowByte = alowoff & 255
                hiByte = alowoff >> 8
                self.mb.i2cWrite((self.i2caddr << 1),[0x5B, hiByte, lowByte])
                self.mb.pausems(5)
                

    def getAlarmLowVal(self):
        """Get Current Low Alarm Value Setting and return Value in Celsius
        Usage: getAlarmLowVal()"""
        if self.mb != None:
            self.readSensor()
            if self.status == CCF_CHIPCAP2_STATUS_COMMANDMODE:
                self.mb.i2cWrite((self.i2caddr << 1),[0x1A, 0, 0])
                self.mb.pausems(1)
                edat = self.mb.i2cRead((self.i2caddr << 1) + 1, 3)
                eALow = (((edat[1] << 8) + (edat[2])) / 99) - 40
                return eALow

    def checkAlarmHigh(self):
        """Check if the High Alarm has been triggered
        Usage: checkAlarmHigh()"""
        if self.mb != None:
            if self.mb.digitalRead(self.ahighpin) == 1:
                return True
            else:
                return False
        else:
            return False

    def setAlarmHighVal(self, alarmValue):
        """Set The Alarm High Value in Celsius
        Usage: setAlarmHighVal(30)"""
        if self.mb != None:
            self.readSensor()
            if self.status == CCF_CHIPCAP2_STATUS_COMMANDMODE:
                alowon = (alarmValue + 40) * 99
                lowByte = alowon & 255
                hiByte = alowon >> 8
                self.mb.i2cWrite((self.i2caddr << 1),[0x58, hiByte, lowByte])
                self.mb.pausems(2)
                alowoff = ((alarmValue + 1) + 40) * 99
                lowByte = alowoff & 255
                hiByte = alowoff >> 8
                self.mb.i2cWrite((self.i2caddr << 1),[0x59, hiByte, lowByte])
                self.mb.pausems(5)

    def getAlarmHighVal(self):
        """Get Current High Alarm Value Setting and return Value in Celsius
        Usage: getAlarmHighVal()"""
        if self.mb != None:
            self.readSensor()
            if self.status == CCF_CHIPCAP2_STATUS_COMMANDMODE:
                self.mb.i2cWrite((self.i2caddr << 1),[0x18, 0, 0])
                self.mb.pausems(1)
                edat = self.mb.i2cRead((self.i2caddr << 1) + 1, 3)
                eAHigh = (((edat[1] << 8) + (edat[2])) / 99) - 40
                return eAHigh

    def power(self, onOff):
        """If using a digital pin for power use this function to turn power on and off
        Usage: power(onOff)
        Example Off: power(0)
        Example On: power(1)"""
        if self.mb != None:
            if onOff == 0:
                self.mb.pinLow(self.pwrpin)
            elif onOff == 1:
                self.mb.pinHigh(self.pwrpin)

    def readSensor(self):
        """Read Sensor Data
        Usage: readSensor()"""
        if self.mb != None:
            rht = self.mb.i2cRead((self.i2caddr << 1) + 1, 4)
            self.status = rht[0] >> 6
            rh = ((rht[0] & 63) * 256) + rht[1]
            rh = (rh / 16384.0) * 100.0
            t = (rht[2] * 64) + rht[3] / 4
            t = (t / 16384.0) * 165.0 - 40
            tf = t * 1.8 + 32
            self.humidity = rh
            self.temperatureC = t
            self.temperatureF = tf

    def dataReady(self):
        """Check if data is ready for reading
        Usage: dataReady()"""
        if self.mb != None:
            if self.mb.digitalRead(self.readypin) == 1:
                return True
            else:
                return False
        else:
            return False



