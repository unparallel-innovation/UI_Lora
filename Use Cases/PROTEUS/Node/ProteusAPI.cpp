//-----------------------------------------------------------------------------
// Copyright (C) UNPARALLEL Innovation, Lda
// All Rights Reserved
// Written by Miguel Costa <miguel.costa@unparallel.pt>, October 2017
//-----------------------------------------------------------------------------

#include "ProteusAPI.h"


// Class Constructor
ProteusAPI::ProteusAPI(byte _nodeID, byte _sensorChipType, int sensorChipNum) :
driver(RFM95_CS, RFM95_INTERRUPT),
manager(driver, _nodeID),
ModbusSerialAndLora(manager),
ina219(0x42)
{
  nodeID = _nodeID;
  sensorChipType = _sensorChipType;
  sensorChipNumber = sensorChipNum;
}

//---------------------------
//         FUNTIONS
//---------------------------

void ProteusAPI::LoRa_enable() {
  pinMode(LORA_NOT_ENABLE, OUTPUT);
  digitalWrite(LORA_NOT_ENABLE, LOW);
  digitalWrite(RFM95_CS, HIGH);

  reset_LoRa();
}

void ProteusAPI::LoRa_disable() {
  digitalWrite(LORA_NOT_ENABLE, HIGH);

  // Pull Chip Select Pin to Low to avoid suplying power trought the pin
  digitalWrite(RFM95_CS, LOW);
}

void ProteusAPI::reset_LoRa() {
  // LoRa radio reset
  P7DIR |= BIT2;    // Set pin P7.2 as output
  P7OUT &= ~BIT2;   // Set pin P7.2 as LOW
  delay(100);
  P7OUT |= BIT2;    // Set pin P7.2 as HIGH
  delay(100);
}

void ProteusAPI::initLora() {
  // Initialize the lora manager
  manager.init();

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  // For higher power levels the launchpad may require an external regulator to power the radio.
  // The MSP launchpad 3.3v regulator cannot supply enought current for the radio and can cause a system reset
  driver.setTxPower(RF95_POWER, false);

  // Configure the radio for long range
  // < Bw = 125 kHz, Cr = 4/5, Sf = 512chips/symbol, CRC on. Slow + long range
  //RH_RF95::ModemConfig RFM95config = { 0x72,   0x94,    0x00};  // Bw125Cr45Sf512
  // < Bw = 125 kHz, Cr = 4/5, Sf = 4096chips/symbol, CRC on. Slower + longer range
  //  RH_RF95::ModemConfig RFM95config = { 0x72,   0xc4,    0x00}; // Bw125Cr45Sf4096
  driver.setModemRegisters(&RFM95config);

  // You can optionally require this module to wait until Channel Activity
  // Detection shows no activity on the channel before transmitting by setting
  // the CAD timeout to non-zero:
  driver.setCADTimeout(10000);

  // Set the frequency of the RFM95
  driver.setFrequency(RF95_FREQ);
}


void ProteusAPI::SendAllSensors_Raw(pnode_sensor  * sensors, byte numbSensor, int to) {
  byte msg[250];
  byte lenLora = 0x00;
  msg[0] = 0x50;
  msg[1] = 0x4E;
  //msg[2] = counterID; not being used ui id msg
  msg[2] = nodeID;
  msg[3] = to;
  //counterID++;
  //msg[4] to be defined later, its the size

  int indexMsg = 5;

  for(int h = 0; h < numbSensor; h++){
    msg[indexMsg] = 0x10;
    indexMsg++;
    msg[indexMsg] = sensors[h].sensor_number+100;
    indexMsg++;

    for(int ff = 0; ff < 4; ff++){
      msg[indexMsg] = sensors[h].raw.bytes[3-ff];
      indexMsg++;
    }
  }

  int lenLora1 = indexMsg; //-4 do inicial + 5 cabecalho
  msg[4]=lenLora1 - 5; // size of header

  word crc = calcCrc(msg[0], msg+1, lenLora1-1);
  msg[indexMsg] = crc >> 8;
  indexMsg++;
  msg[indexMsg] = crc & 0xFF;
  indexMsg++;

  manager.sendtoWait(msg, indexMsg, to);
}

void ProteusAPI::SendAllSensors_Calibrated(pnode_sensor  * sensors, byte numbSensor, int to) {
  byte msg[250];
  byte lenLora = 0x00;
  msg[0] = 0x50;
  msg[1] = 0x4E;
  msg[2] = nodeID;
  msg[3] = to;
  //msg[4] to be defined later, its the size

  int indexMsg=5;

  for(int h = 0; h < numbSensor; h++){
    msg[indexMsg] = 0x10;
    indexMsg++;
    msg[indexMsg] = sensors[h].sensor_number+200;
    indexMsg++;

    for(int ff = 0; ff < 4; ff++){
      msg[indexMsg] = sensors[h].calibrated.bytes[3-ff];
      indexMsg++;
    }
  }

  int lenLora1 = indexMsg;//-4 do inicial + 5 cabecalho
  msg[4]=lenLora1 - 5;

  word crc = calcCrc(msg[0], msg+1, lenLora1-1);
  msg[indexMsg] = crc >> 8;
  indexMsg++;
  msg[indexMsg] = crc & 0xFF;
  indexMsg++;

  manager.sendtoWait(msg, indexMsg, to);
}

void ProteusAPI::SendAllSensors_Raw_timestamp(pnode_sensor  * sensors, byte numbSensor, uint32_t timestamp, int to) {

  byte msg[250];
  byte lenLora = 0x00;
  msg[0] = 0x50;
  msg[1] = 0x4E;
  msg[2] = nodeID;
  msg[3] = to;
  //msg[4] to be defined later, its the size

  sensor_union aux;
  aux.valueU32 = timestamp;

  int indexMsg = 5;

  for(int h = 0; h < numbSensor; h++){
    msg[indexMsg] = 0x21;
    indexMsg++;
    msg[indexMsg] = sensors[h].sensor_number+100;
    indexMsg++;

    for(int ff = 0; ff < 4; ff++){
      msg[indexMsg] = sensors[h].raw.bytes[3-ff];
      indexMsg++;
    }
    for(int ff = 0; ff < 4; ff++){
      msg[indexMsg] = aux.bytes[4-ff-1];
      indexMsg++;
    }
  }

  int lenLora1 = indexMsg;//-4 do inicial + 5 cabecalho
  msg[4] = lenLora1 - 5;

  word crc = calcCrc(msg[0], msg+1, lenLora1-1);
  msg[indexMsg] = crc >> 8;
  indexMsg++;
  msg[indexMsg] = crc & 0xFF;
  indexMsg++;

  manager.sendtoWait(msg, indexMsg, to);
}

void ProteusAPI::SendAllSensors_Calibrated_timestamp(pnode_sensor  * sensors, byte numbSensor, uint32_t timestamp, int to) {

  byte msg[250];
  byte lenLora = 0x00;
  msg[0] = 0x50;
  msg[1] = 0x4E;
  msg[2] = nodeID;
  msg[3] = to;
  //msg[4] to be defined later, its the size

  int indexMsg = 5;

  sensor_union aux;
  aux.valueU32 = timestamp;

  for(int h = 0; h < numbSensor; h++){
    msg[indexMsg] = 0x21;
    indexMsg++;
    msg[indexMsg] = sensors[h].sensor_number+200;
    indexMsg++;

    for(int ff = 0; ff < 4; ff++){
      msg[indexMsg]=sensors[h].calibrated.bytes[3-ff];
      indexMsg++;
    }
    for(int ff = 0; ff < 4; ff++){
      msg[indexMsg] = aux.bytes[3-ff];
      indexMsg++;
    }
  }

  int lenLora1 = indexMsg;//-4 do inicial + 5 cabecalho
  msg[4] = lenLora1 - 5;

  word crc = calcCrc(msg[0], msg+1, lenLora1-1);
  msg[indexMsg] = crc >> 8;
  indexMsg++;
  msg[indexMsg] = crc & 0xFF;
  indexMsg++;

  manager.sendtoWait(msg, indexMsg, to);
}

void ProteusAPI::SendOneSensor_Raw(pnode_sensor sensor, int to) {
  byte msg[250];
  byte lenLora = 0x00;
  msg[0] = 0x50;
  msg[1] = 0x4E;
  msg[2] = nodeID;
  msg[3] = to;
  //msg[4] to be defined later, its the size

  int indexMsg = 5;

  msg[indexMsg] = 0x10;
  indexMsg++;
  msg[indexMsg] = sensor.sensor_number+100;
  indexMsg++;

  for(int ff = 0; ff < 4; ff++){
    msg[indexMsg] = sensor.raw.bytes[3-ff];
    indexMsg++;
  }

  int lenLora1 = indexMsg;//-4 do inicial + 5 cabecalho
  msg[4] = lenLora1 - 5;

  word crc = calcCrc(msg[0], msg+1, lenLora1-1);
  msg[indexMsg] = crc >> 8;
  indexMsg++;
  msg[indexMsg] = crc & 0xFF;
  indexMsg++;

  manager.sendtoWait(msg, indexMsg, to);
}

void ProteusAPI::SendOneSensor_Calibrated(pnode_sensor sensor, int to) {
  byte msg[250];
  byte lenLora = 0x00;
  msg[0] = 0x50;
  msg[1] = 0x4E;
  msg[2] = nodeID;
  msg[3] = to;
  //msg[4] to be defined later, its the size

  int indexMsg = 5;

  msg[indexMsg] = 0x10;
  indexMsg++;
  msg[indexMsg] = sensor.sensor_number+200;
  indexMsg++;

  for(int ff = 0; ff < 4; ff++){
    msg[indexMsg] = sensor.calibrated.bytes[3-ff];
    indexMsg++;
  }

  int lenLora1=indexMsg;//-4 do inicial + 5 cabecalho
  msg[4]=lenLora1 - 5;

  word crc = calcCrc(msg[0], msg+1, lenLora1-1);
  msg[indexMsg] = crc >> 8;
  indexMsg++;
  msg[indexMsg] = crc & 0xFF;
  indexMsg++;

  manager.sendtoWait(msg, indexMsg, to);
}

void ProteusAPI::SendOneSensors_Raw_timestamp(pnode_sensor sensor, uint32_t timestamp, int to) {
  byte msg[250];
  byte lenLora = 0x00;
  msg[0] = 0x50;
  msg[1] = 0x4E;
  msg[2] = nodeID;
  msg[3] = to;
  //msg[4] to be defined later, its the size

  int indexMsg = 5;

  msg[indexMsg] = 0x21;
  indexMsg++;
  msg[indexMsg] = sensor.sensor_number+100;
  indexMsg++;

  for(int ff = 0; ff < 4; ff++){
    msg[indexMsg] = sensor.raw.bytes[3-ff];
    indexMsg++;
  }

  sensor_union aux;
  aux.valueU32 = timestamp;

  for(int ff = 0; ff < 4; ff++){
    msg[indexMsg] = aux.bytes[3-ff];
    indexMsg++;
  }

  int lenLora1 = indexMsg;//-4 do inicial + 5 cabecalho
  msg[4] = lenLora1 - 5;

  word crc = calcCrc(msg[0], msg+1, lenLora1-1);
  msg[indexMsg] = crc >> 8;
  indexMsg++;
  msg[indexMsg] = crc & 0xFF;
  indexMsg++;

  manager.sendtoWait(msg, indexMsg, to);
}

void ProteusAPI::SendOneSensors_Calibrated_timestamp(pnode_sensor sensor, uint32_t timestamp, int to) {

  byte msg[250];
  byte lenLora = 0x00;
  msg[0] = 0x50;
  msg[1] = 0x4E;
  msg[2] = nodeID;
  msg[3] = to;
  //msg[4] to be defined later, its the size

  int indexMsg = 5;

  msg[indexMsg] = 0x21;
  indexMsg++;
  msg[indexMsg] = sensor.sensor_number+200;
  indexMsg++;

  for(int ff = 0; ff < 4; ff++){
    msg[indexMsg] = sensor.calibrated.bytes[3-ff];
    indexMsg++;
  }

  sensor_union aux;
  aux.valueU32 = timestamp;

  for(int ff = 0; ff < 4; ff++){
    msg[indexMsg] = aux.bytes[3-ff];
    indexMsg++;
  }

  int lenLora1 = indexMsg;//-4 do inicial + 5 cabecalho
  msg[4] = lenLora1 - 5;

  word crc = calcCrc(msg[0], msg+1, lenLora1-1);
  msg[indexMsg] = crc >> 8;
  indexMsg++;
  msg[indexMsg] = crc & 0xFF;
  indexMsg++;

  manager.sendtoWait(msg, indexMsg, to);
}

void ProteusAPI::SendAlarm(pnode_sensor sensor, float value, uint32_t timestamp, int to) {

  byte msg[250];
  byte lenLora = 0x00;
  msg[0] = 0x50;
  msg[1] = 0x4E;
  msg[2] = nodeID;
  msg[3] = to;
  //msg[4] to be defined later, its the size

  int indexMsg = 5;

  msg[indexMsg] = 0x30;//code for alarm
  indexMsg++;
  msg[indexMsg] = sensor.sensor_number;
  indexMsg++;

  sensor_union aux;
  aux.valueFloat = value;

  for(int ff=0;ff<4;ff++){
    msg[indexMsg]=aux.bytes[3-ff];
    indexMsg++;
  }

  aux.valueU32 = timestamp;

  for(int ff = 0; ff < 4; ff++){
    msg[indexMsg] = aux.bytes[3-ff];
    indexMsg++;
  }

  int lenLora1 = indexMsg;//-4 do inicial + 5 cabecalho
  msg[4] = lenLora1 - 5;

  word crc = calcCrc(msg[0], msg+1, lenLora1-1);
  msg[indexMsg] = crc >> 8;
  indexMsg++;
  msg[indexMsg] = crc & 0xFF;
  indexMsg++;

  manager.sendtoWait(msg, indexMsg, to);
}

unsigned long ProteusAPI::SendReqTimestamp()
{
  int retry = 0;
  while (retry < 5 ) {
    byte msg[RH_RF95_MAX_MESSAGE_LEN];
    byte lenLora = 0x00;
    msg[0] = 0x50;
    msg[1] = 0x4E;
    msg[2] = nodeID;
    msg[3] = 0xFF; //broadcast
    //msg[4] to be defined later, its the size

    int indexMsg = 5;

    msg[indexMsg] = 0x40;//code for request timestamp
    indexMsg++;

    int lenLora1 = indexMsg;//-4 do inicial + 5 cabecalho
    msg[4] = lenLora1 - 5;

    word crc = calcCrc(msg[0], msg+1, lenLora1-1);
    msg[indexMsg] = crc >> 8;
    indexMsg++;
    msg[indexMsg] = crc & 0xFF;
    indexMsg++;

    manager.sendtoWait(msg, indexMsg, 0xFF);

    unsigned long previousMillis = millis();
    unsigned long currentMillis = millis();

    while (currentMillis - previousMillis < 1500) {
      currentMillis = millis();

      if (driver.available())
      {
        // Should be a message for us now
        uint8_t lenMsg = RH_RF95_MAX_MESSAGE_LEN;
        uint8_t from = 0, to, id, flags;

        if(manager.recvfromAck(msg, &lenMsg, &from, &to, &id, &flags))
        {
          word crcM = calcCrc(msg[0], msg+1, 10-1);
          word crcAux = crcM & 0xFF;
          if ((nodeID == to) &&  (msg[5] == 0xA0) && (msg[10] == crcM >> 8) && (crcAux) )
          {
            sensor_union aux1;
            aux1.bytes[0] = msg[9];
            aux1.bytes[1] = msg[8];
            aux1.bytes[2] = msg[7];
            aux1.bytes[3] = msg[6];

            Serial.print("Got a timestamp. Setting RTC to Unixtime: ");
            Serial.println(aux1.valueU32);

            return aux1.valueU32;
          }
        }
      }
    }
    if( retry < 5)
    Serial.println("No reply from Gateway. Retrying...");
    retry++;
  }
  Serial.println("No reply from Gateway. Settinng time to 1/1/2017");
  return 1483228800; // 1/1/2017
}

void ProteusAPI::SendAll_PMU_Registers(pnode_register  * pnode_registers, byte number_of_pnode_registers, pmu_register  * pmu_registers, byte number_of_pmu_registers, int to)
{
  byte msg[250];
  byte lenLora = 0x00;
  msg[0] = 0x50;
  msg[1] = 0x4E;
  //msg[2] = counterID; not being used ui id msg
  msg[2] = nodeID;
  msg[3] = to;
  //counterID++;
  //msg[4] to be defined later, its the size

  int indexMsg = 5;

  for(int h = 0; h < number_of_pnode_registers; h++){
    msg[indexMsg] = 0x10;
    indexMsg++;
    msg[indexMsg] = pnode_registers[h].sensor_number+100;
    indexMsg++;

    for(int ff = 0; ff < 4; ff++){
      msg[indexMsg] = pnode_registers[h].raw.bytes[3-ff];
      indexMsg++;
    }
  }

  for(int h = 0; h < number_of_pmu_registers; h++){
    msg[indexMsg] = 0x10;
    indexMsg++;
    msg[indexMsg] = pmu_registers[h].sensor_number+100;
    indexMsg++;

    for(int ff = 0; ff < 4; ff++){
      msg[indexMsg] = pmu_registers[h].raw.bytes[3-ff];
      indexMsg++;
    }
  }

  int lenLora1 = indexMsg; //-4 do inicial + 5 cabecalho
  msg[4]=lenLora1 - 5; // size of header

  word crc = calcCrc(msg[0], msg+1, lenLora1-1);
  msg[indexMsg] = crc >> 8;
  indexMsg++;
  msg[indexMsg] = crc & 0xFF;
  indexMsg++;

  manager.sendtoWait(msg, indexMsg, to);
}
