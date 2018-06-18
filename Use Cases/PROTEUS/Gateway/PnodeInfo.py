import os

from datetime import datetime
import struct
from collections import namedtuple    
from aenum import Enum, NoAlias


class EnumBase(type):
    def __init__(self, name, base, fields):
        super(EnumBase, self).__init__(name, base, fields)
        self.__mapping = dict((v, k) for k, v in fields.iteritems())
    def __getitem__(self, val):
        return self.__mapping[val]

def enum(*seq, **named):
    enums = dict(zip(seq, range(len(seq))), **named)
    return EnumBase('Enum', (), enums)    

Sensor = namedtuple('Sensor', 'number name RawValueLow RawValueHi CaliValueLow CaliValueHi AlarmValueLow AlarmValueHi')
Sensor1 = Sensor(1, "CNT1-R1", 0x50 , 0x51,0x100,0x101,0x300,0x301)
Sensor2 = Sensor(2, "CNT1-R2", 0x52 , 0x53,0x102,0x103,0x302,0x303)
Sensor3 = Sensor(3, "CNT1-R3", 0x54 , 0x55,0x104,0x105,0x304,0x305)
Sensor4 = Sensor(4, "CNT2-R1", 0x56 , 0x57,0x106,0x107,0x306,0x307)
Sensor5 = Sensor(5, "CNT2-R2", 0x58 , 0x59,0x108,0x109,0x308,0x309)
Sensor6 = Sensor(6, "CNT2-R3", 0x5A , 0x5B,0x10A,0x10B,0x30A,0x30B)
Sensor7 = Sensor(7, "CNT3-R1", 0x5C , 0x5D,0x10C,0x10D,0x30C,0x30D)
Sensor8 = Sensor(8, "CNT3-R2", 0x5E , 0x5F,0x10E,0x10F,0x30E,0x30F)
Sensor9 = Sensor(9, "CNT3-R3", 0x60 , 0x61,0x110,0x111,0x310,0x311)
Sensor10 = Sensor(10, "Temperature1", 0x62 , 0x63,0x112,0x113,0x312,0x313)
Sensor11 = Sensor(11, "Temperature2", 0x64 , 0x65,0x114,0x115,0x314,0x315)
Sensor12 = Sensor(12, "Pressure1", 0x66 , 0x67,0x116,0x117,0x316,0x317)
Sensor13 = Sensor(13, "Pressure2", 0x68 , 0x69,0x118,0x119,0x318,0x319)
Sensor14 = Sensor(14, "Conductivity1", 0x6E , 0x6F,0x11E,0x11F,0x31E,0x31F)
Sensor15 = Sensor(15, "Conductivity2", 0x70 , 0x71,0x120,0x121,0x320,0x321)
Sensor16 = Sensor(16, "FlowRate1", 0x6A , 0x6B,0x11A,0x11B,0x31A,0x31B)
Sensor17 = Sensor(17, "FlowRate2", 0x6C , 0x6D,0x11C,0x11D,0x31C,0x31D)
#energy
Sensor18 = Sensor(18, "Pnode Battery", 0x72 , 0x73,0x122,0x123,0x322,0x323)
Sensor19 = Sensor(19, "Pnode Bus Voltage", 0x74 , 0x75,0x124,0x125,0x324,0x325)
Sensor20 = Sensor(20, "Pnode Current", 0x76 , 0x77,0x126,0x127,0x326,0x327)
#pmu

Sensor21 = Sensor(21, "PMU_HARVESTER_PVG", 0x78 , 0x79,0x128,0x129,0x328,0x329)
Sensor22 = Sensor(22, "PMU_HARVESTER_TVG", 0x7A , 0x7B,0x12A,0x12B,0x32A,0x32B)
Sensor23 = Sensor(23, "PMU_LT_EH_ON", 0x7C , 0x7D,0x12C,0x12D,0x32C,0x32D)
Sensor24 = Sensor(24, "PMU_LT_BATT", 0x7E , 0x7F,0x12E,0x12F,0x32E,0x32F)
Sensor25 = Sensor(25, "PMU_LT_BATT_CHARGING", 0x80 , 0x81,0x130,0x131,0x330,0x331)
Sensor26 = Sensor(26, "PMU_LT_VOUT", 0x82 , 0x83,0x132,0x133,0x331,0x333)
Sensor27 = Sensor(27, "PMU_LT_IOUT", 0x84 , 0x85,0x134,0x135,0x334,0x335)
Sensor28 = Sensor(28, "PMU_CH1_SCAP", 0x86 , 0x87,0x136,0x137,0x336,0x337)
Sensor29 = Sensor(29, "PMU_CH1_VOUT", 0x88 , 0x89,0x138,0x139,0x338,0x339)
Sensor30 = Sensor(30, "PMU_CH1_IOUT", 0x8A , 0x8B,0x13A,0x13B,0x33A,0x33B)
Sensor31 = Sensor(31, "PMU_CH1_ENERGY_AVAILABLE", 0x8C , 0x8D,0x13C,0x13D,0x33C,0x33D)
Sensor32 = Sensor(32, "PMU_CH2_IIN", 0x8E , 0x8F,0x13E,0x13F,0x33E,0x33F)
Sensor33 = Sensor(33, "PMU_CH2_SCAP", 0x90 , 0x91,0x140,0x141,0x340,0x341)
Sensor34 = Sensor(34, "PMU_CH2_VOUT", 0x92 , 0x93,0x142,0x143,0x342,0x343)
Sensor35 = Sensor(35, "PMU_CH2_IOUT", 0x94 , 0x95,0x144,0x145,0x344,0x345)
Sensor36 = Sensor(36, "PMU_CH2_ENERGY_AVAILABLE", 0x96 , 0x97,0x146,0x147,0x346,0x347)


Sensores = [Sensor1,Sensor2,Sensor3,Sensor4,Sensor5,Sensor6,Sensor7,Sensor8,Sensor9,Sensor10,Sensor11,Sensor12,Sensor13,Sensor14,Sensor15,Sensor16,Sensor17,Sensor18,Sensor19,Sensor20,Sensor21,Sensor22,Sensor23,Sensor24,Sensor25,Sensor26,Sensor27,Sensor28,Sensor29,Sensor30,Sensor31,Sensor32,Sensor33,Sensor34,Sensor35,Sensor36]
   
functionC = enum(sendValueF="0x10",timeStampF ="0x20", valueWtimeF= "0x21", alarmF ="0x30", reqTimeStampF = "0x40" , gateSendTimestampF = "0xA0")
class lengData(Enum):
    _settings_ = NoAlias
    sendValue= 4
    timeStamp =4
    valueWtime= 8
    alarm= 8
    reqTimeStamp = 4
    gateSendTimestampF = 4#actually its used 0
 
def findSize(code):
    x = functionC[code]
    return lengData[x[:-1]].value

        
def getblocknamebynumber(number):
    if 0<=number<=99:
        return 'Alarm'
    elif 100<=number<=199:
        return 'Raw'
    elif 200<=number<=300:
        return 'Cali'
        
def getaddressbynumber(number):
    if 0<=number<=99:
        return Sensores[number-1].AlarmValueLow
    elif 100<=number<=199:
        return Sensores[number-101].RawValueLow
    elif 200<=number<=300:
        return Sensores[number-201].CaliValueLow
        
def getNameByNumber(number):
    if 0<=number<=99:
        return Sensores[number-1].name
    elif 100<=number<=199:
        return Sensores[number-101].name
    elif 200<=number<=300:
        return Sensores[number-201].name
        
