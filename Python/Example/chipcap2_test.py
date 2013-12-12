"""
    Simple pyMCU example using the ChipCap2 Temperature & Humidity Sensor
    Gets current Low and High Alarm values from EEPROM and prints them
    Sets New Low and High Alarm Values
    Then goes into an endless loop to display the Humidity and Temperature every 5 seconds
"""

import sys

#sys.path.append('H:/GitHub/ChipCap2/Python/Library') # set your path to the ChipCap2 lib if you didn't copy it to your site-packages folder
import pymcu # import pyMCU library
import CFF_ChipCap2  # import ChipCap2 library

mb = pymcu.mcuModule() # init pyMCU class

ChipCap2_i2cAddr = 0x28
ChipCap2_PowerPin = 1
ChipCap2_ReadyPin = 2
ChipCap2_AlarmLowPin = 3
ChipCap2_AlarmHighPin = 5

cc2 = CFF_ChipCap2.CFF_ChipCap2(mb, ChipCap2_i2cAddr, ChipCap2_PowerPin, ChipCap2_ReadyPin, ChipCap2_AlarmLowPin, ChipCap2_AlarmHighPin) # init ChipCap2 class and set all init variables
# Alternatively you could just do: cc2 = CFF_ChipCap2.CFF_ChipCap2(mb)  if you are using all the default settings

cc2.power(1)                        # turn ChipCap2 power on
mb.pausems(500)

cc2.startCommandMode() # put ChipCap2 in Command Mode
if cc2.status == CFF_ChipCap2.CCF_CHIPCAP2_STATUS_COMMANDMODE:  # check to make sure it went into command mode
    print "ChipCap2 is now in command mode"
    print "Get Current Alarm Values"
    alow = cc2.getAlarmLowVal()
    print "Alarm Low: ", alow
    ahigh = cc2.getAlarmHighVal()
    print "Alarm High: ", ahigh
    mb.pausems(100)
    print "Setting New Alarm Values"
    cc2.setAlarmHighVal(29) # Set High Alarm to 29 C
    mb.pausems(100)
    cc2.setAlarmLowVal(18) # Set Low Alarm to 18 C
    mb.pausems(100)

    print "Going back to Normal Mode"
    cc2.startNormalMode() # Go back into Normal Mode
    mb.pausems(100)
else:
    print "ChipCap2 is in Normal Mode"



while 1:
    if cc2.dataReady() == True: # if data is ready for reading
        cc2.readSensor()     # read sensor data
        print "Humidity: ", cc2.humidity
        print "Temperature: ", cc2.temperatureC, "C"
        print "Temperature: ", cc2.temperatureF, "F"
    mb.pausems(5000)

