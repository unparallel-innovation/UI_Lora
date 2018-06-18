from datetime import datetime
import PnodeInfo

class dataAux:
    
    	@staticmethod
	def getDataFromLoraMsg(data):
		headerLora=data[0:4]
	    	header=data[4:9] # 10 is we use ui id msg
        	msg=data[9:] #same
        
		mesu = dataAux.measures(datetime.utcnow(),header[2]) # 3 is we use ui id msg
		#print headerLora,header,msg
		indeData=0
		while indeData < header[4]: #5 is we use ui id msg
			mesu1 = dataAux.readings() 
			if chr(header[0])+chr(header[1]) == 'PN':
				print 'Operation: ' + PnodeInfo.functionC[hex(msg[indeData])]+ ' Sensor:  ',
			indeData+=1
			if chr(header[0])+chr(header[1]) == 'PN':
				print PnodeInfo.getNameByNumber(msg[indeData]) , "\t SensorNumber: " , str(msg[indeData]) + '   data: ',
			mesu1.sensortype = msg[indeData]
			mesu1.datatype = "float" # hardcoded, it can be diferent for others systems
			indeData+=1
			indeFor=indeData
			fina=[]
			finatime = []
			test=""
			
			for y in range(indeFor,PnodeInfo.findSize(hex(msg[indeFor-2]))+indeFor):
				test+=hex(msg[y])[2:].zfill(2)
				print(hex(msg[y])[:].zfill(2)),
				indeData+=1
			print
			#print test
			fina.append(int("0x" + str(test[0:4]),16))
			fina.append(int("0x" + str(test[4:8]),16))
			mesu1.value = fina
			if PnodeInfo.findSize(hex(msg[indeFor-2])) > 4:
				finatime.append(int("0x" + str(test[8:12]),16))
				finatime.append(int("0x" + str(test[12:16]),16))
				mesu1.timestamp = finatime
				
			mesu.readingList.append(mesu1)
				
		return mesu


    
	class readings:
    		datatype = ""
    		value = ''
    		timestamp = 0
    		sensortype = 0

    	class measures:
    
    		def __init__(self, timestamp,id_device):
    			self.readingList = []
			self.timestamp = timestamp
			self.id_device = id_device
