//-----------------------------------------------------------------------------
// Copyright (C) UNPARALLEL Innovation, Lda
// All Rights Reserved
// Written by Miguel Costa <miguel.costa@unparallel.pt>, October 2017
//-----------------------------------------------------------------------------

#ifndef PROTEUSAPI_H
#define PROTEUSAPI_H

#include <Arduino.h>
#include <SPI.h>

#include "RadioHead.h"
#include <RH_RF95.h>

#define MSP_VERSION  209

// LoRa Frequency, Mode and Power
#define RF95_POWER 23

// Arduino Modbus Converter
// #define RF95_FREQ 869.0
// static const RH_RF95::ModemConfig RFM95config = { 0x72,   0x94,    0x00};

// Proteus Gateway
#define RF95_FREQ 868.0
static const RH_RF95::ModemConfig RFM95config = { 0x72,   0x74,    0x00};

// Circular buffer Average Parameters
#define NUM_SAMPLES_AVG 25
#define STABLE_PERCENTAGE 0.05  // 5%


// -------------------------------------
// UI Digital Board Pinouts
#define P5_6    55
#define P5_7    56
#define P7_2    7
#define LORA_NOT_ENABLE                 P6_1
#define RFM95_RST                       P7_2
#define RFM95_CS                        P2_7
#define RFM95_INTERRUPT                 P1_0

//-------------------------------------------
//         TYPEDEF STRUCTS & UNIONS
//-------------------------------------------
typedef union sensor_union
{
  uint32_t  valueU32;
  float     valueFloat;
  word      words[2];
  byte      bytes[4];
} sensor_union;


struct pnode_sensor {
  const char * name;
  byte sensor_number;

  sensor_union raw;
  sensor_union calibrated;

  // Sensor number for each CAPS type
  byte caps_sensor_position[3];
  int time_to_stabilize;

  // Biasing
  afe16_t dac_period;     // period in ms
  afe8_t  dac_dutycycle;  // duty cycle (percentage)
  afe8_t  dac_range;      // 0: up to 32uA - 4: up to 255uA - 8: up to 2mA
  afe8_t  dac_value;      // from 0 til 255
  // Sensing
  afe8_t  sensing_gain;   // 0x00: 1x , 0x01: 2x , 0x02: 4x , 0x03: 8x

  // Modbus register addresses
  int register_value_LSB;
  int register_value_MSB;
  int register_raw_LSB;
  int register_raw_MSB;

  // y = mx + b;   x = ( y - b) / m
  float calibration_M;
  float calibration_B;
};

struct pnode_register {
  const char * name;
  byte sensor_number;

  sensor_union raw;

  // Modbus register addresses
  int register_raw_LSB;
  int register_raw_MSB;
};

struct pmu_register {
  const char * name;
  byte sensor_number;

  sensor_union raw;

  afe_pmu_register pmu_register;

  // Modbus register addresses
  int register_raw_LSB;
  int register_raw_MSB;
};

//-------------------------------------------
//                  CLASS
//-------------------------------------------

class ProteusAPI : public ModbusSerialAndLora {
private:

  // RFM95 Objects
  RH_RF95 driver;              // Singleton instance of the radio driver
  RHReliableDatagram manager;  // Class to manage message delivery and receipt

  // PNode ID
  byte nodeID;

  // Sensor Caps type
  byte sensorChipType;
  int sensorChipNumber;

public:
  ProteusAPI(byte nodeID, byte sensorChipType, int sensorChipNumber);

  //-------------------------------------------
  //            FUNTIONS PROTOTYPES
  //-------------------------------------------
public:
  // LoRa Power Control
  void LoRa_enable();
  void LoRa_disable();
  void reset_LoRa();

  void initLora();

  // Lora Custom Protocol
  void SendAllSensors_Raw(pnode_sensor * sensors, byte numbSensor, int to);
  void SendAllSensors_Calibrated(pnode_sensor * sensors, byte numbSensor, int to);
  void SendAllSensors_Raw_timestamp(pnode_sensor * sensors, byte numbSensor, uint32_t timestamp, int to);
  void SendAllSensors_Calibrated_timestamp(pnode_sensor * sensors, byte numbSensor, uint32_t timestamp, int to);

  void SendOneSensor_Raw(pnode_sensor sensor, int to);
  void SendOneSensor_Calibrated(pnode_sensor sensor, int to);
  void SendOneSensors_Raw_timestamp(pnode_sensor sensor, uint32_t timestamp, int to);
  void SendOneSensors_Calibrated_timestamp(pnode_sensor sensor, uint32_t timestamp, int to);

  void SendAlarm(pnode_sensor sensor, float value, uint32_t timestamp, int to);

  unsigned long SendReqTimestamp();
  void SendAll_PMU_Registers(pnode_register  * pnode_registers, byte number_of_pnode_registers,
    pmu_register  * pmu_registers, byte number_of_pmu_registers, int to);

};

#endif //PROTEUSAPI_H
