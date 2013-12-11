/**************************************************************************
    This is a test for the CFF_ChipCap2 Library
    
    This test will put the ChipCap2 Sensor into Command Mode,
    Set a High and Low Alarm Value,
    Then go back into Normal Mode and then print Temperature and Humidity
    every 5 seconds.
***************************************************************************/

#include <Wire.h>
#include <CFF_ChipCap2.h>

CFF_ChipCap2 cc2 = CFF_ChipCap2();
  
void setup()
{
  
  Serial.begin(9600);
  cc2.begin();
  
  cc2.configPowerPin(5);
  cc2.startCommandMode();
  delay(100);
  if (cc2.status == CCF_CHIPCAP2_STATUS_COMMANDMODE)
  {
    Serial.print("ChipCap2 is now in command mode!\n"); 
    Serial.print("Setting New Alarm Values\n"); 
    // Set Alarm Registers
    cc2.setAlarmHighVal(29);
    delay(100);
    cc2.setAlarmLowVal(18);
    delay(100);
    
    Serial.print("Going back into Normal mode\n"); 
    // Go back to normal mode
    cc2.startNormalMode();
    delay(100);
  }
  else
  {
     Serial.print("ChipCap2 is in normal mode.\n"); 
  }
  delay(2);
  cc2.configReadyPin(2);
  cc2.configAlarmLowPin(3);
  cc2.configAlarmHighPin(4);
}

void loop()
{
  
  while (1)
  {
    if (cc2.dataReady() == true)
    {
      cc2.readSensor();
      
      Serial.print("Humidity: ");
      Serial.print(cc2.humidity);
      Serial.print("\n");
  
      Serial.print("Temperature C: ");
      Serial.print(cc2.temperatureC);
      Serial.print("\n");
      
      Serial.print("Temperature F: ");
      Serial.print(cc2.temperatureF);
      Serial.print("\n");
    }
    delay(5000);
  }
}

