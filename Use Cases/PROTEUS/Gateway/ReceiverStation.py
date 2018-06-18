#!/usb/bin/env python

"""
	Lora Custom Protocol
"""
__author__	= """Unparallel"""
__date__ 		= "01-01-2017"
__version__	= "0.0.4"
__license__ = "x"

#0.0.4 -> now gateway can send timestamp to the pnode in order set time/date 

import sys
import os

import time
import array
import PnodeInfo
import LoraInit
import ModbusServer
import MqttServer
import DataMsg
from datetime import datetime

sys.path.append(
	os.path.join(
		os.path.dirname(__file__),
		'..'
	)
)

try:    	
	LoraObj = LoraInit.LoraModule()
	PNODEmodbusObj = ModbusServer.ModbusServer()
	#PNODEmqttObj = MqttServer.mqttInfo("","","iot.eclipse.org","1883",LoraObj.idGateway)
	PNODEmqttObj = MqttServer.mqttInfo("proteus","proteusTests0517","185.52.32.107","1883",LoraObj.idGateway)
	while True:
		if LoraObj.receiveMsgWithData():
			
			if LoraObj.dataIsUIProtocol():
				data = DataMsg.dataAux.getDataFromLoraMsg(LoraObj.getMsg())	
				if LoraObj.getOriginChr() == LoraObj.nodesTypes.PNODE_TYPE:
					print 'pnode'
					
					PNODEmodbusObj.setValuesModbus(data)
					PNODEmqttObj.SendToMqttPnode(data,LoraObj.getIDOrigin())
				if LoraObj.getOriginChr() == LoraObj.nodesTypes.WS_TYPE:
					print 'weather'
			
			else:
				print 'Please create your type message.' 
		elif LoraObj.loraMsgStatus =='MsgHolding':
			rawData = LoraObj.getMsg()#message which dont belongs to our protocol
			print 'The gateway received other type of message '
			print rawData

		time.sleep(1)
		print
finally:
	print 'ending modbus server'
	PNODEmodbusObj.stop()
