#import Global
import sys
import os
import time
import array
import json
sys.path.append(
	os.path.join(
		os.path.dirname(__file__),
		'..'
	)
)
import libLora as pyrfm



class LoraModule:
    idGateway=254
    class nodesTypes:
        PNODE_TYPE = 'PN'
        WS_TYPE = 'WT'

    loraMsg = []
    loraCheckRepeat = []
    loraMsgStatus=''
    
    def __init__(self):
        conf={
        	'll':{
        		'type':'rfm95'
        	},
        	'pl':{
        		'type':	'serial_seed',
        		'port':	'/dev/ttyO2'
        	}
        }
        self.ll=pyrfm.getLL(conf)
        #print('HW-Version: ', self.ll.getVersion())
        if self.ll.setOpModeSleep(True,True):
        	self.ll.setFiFo()
        	self.ll.setOpModeIdle()
        	self.ll.setModemConfig('Bw125Cr45Sf128');
        	self.ll.setPreambleLength(8)
        	self.ll.setFrequency(850)
        	self.ll.setTxPower(13)

    def receiveMsgWithData(self):
    	if self.ll.waitRX(timeout=3):
		LoraModule.loraMsg=self.ll.recv()
		#print LoraModule.loraMsg
		header1=LoraModule.loraMsg[0:4]
		header=LoraModule.loraMsg[4:9] #10 if we are using ui id message
		#LoraModule.sendAck(self)
		#print header, header1
		LoraModule.cleanHistoryRepeater(self)
		if self.checkCRC(LoraModule.loraMsg):
		        LoraModule.sendAck(self)
    	    		print 'Checksum is ok!'
    			if self.checkDestinationGateway(header):#check here if the message is to repeat or belongs to this gateway
    				if LoraModule.loraMsg[9] == 0x40:#request to sent timestamp
    				    LoraModule.SendTimeStamp(self)
    				    return False
    				else:
    				    return True
    			else:
    		    	#check the message if it was already retransmited. Not done
    				LoraModule.retransmissionData(self)

    				return False
		else:
			print 'Error in Checksum'
			
		LoraModule.loraMsgStatus = "MsgHolding"
			
			
    def sendAck(self):
        if LoraModule.loraMsg[0] != 0xFF:
            msgAck="?"
            data=list(array.array('B',(msgAck.encode('iso8859-1'))))
            self.ll.sendWithHeader(data,LoraModule.getIDOrigin(self),LoraModule.getIDGatewayFromLora(self),0x80,LoraModule.loraMsg[2])

            
    def sendRetransmit(self,data):
        print data
        self.ll.sendWithHeader(data,LoraModule.getIDGatewayFromLora(self),LoraModule.getIDOrigin(self),0,LoraModule.loraMsg[2])
    
    def SendTimeStamp(self):
        timehex = hex(int(time.time()))

        msg = [int("0x50",16),int("0x4E",16),LoraModule.getIDGatewayFromLora(self),LoraModule.getIDOrigin(self),9,int("A0",16),int("0x"+timehex[2:4],16),int("0x"+timehex[4:6],16),int("0x"+timehex[6:8],16),int("0x"+timehex[8:10],16)]
        crc = LoraModule.calc_crc(msg[:])
        high, low = divmod(crc, 0x100)
        #print 'crc',hex(crc) ,'high',hex(high),'low',hex(low)
        msg.append(low)
        msg.append(high)
        self.ll.sendWithHeader(msg,LoraModule.getIDOrigin(self),LoraModule.getIDGatewayFromLora(self),0,LoraModule.loraMsg[2])
        print 'Sent response with Timestamp.'
       
    
    			
    def getMsg(self):
        LoraModule.loraMsgStatus=''
	return LoraModule.loraMsg
	    
    def dataIsUIProtocol(self):
        for key, value in LoraModule.nodesTypes.__dict__.items():
            if value == (chr(LoraModule.loraMsg[4])+chr(LoraModule.loraMsg[5])):
                return True
                

        return False

        
    def getOriginChr(self):
        return chr(LoraModule.loraMsg[4])+chr(LoraModule.loraMsg[5])        

    def getIDOrigin(self):
        return LoraModule.loraMsg[6]
        #return LoraModule.loraMsg[7] # with the UI id message
        
    def getIDGatewayFromLora(self):
        return LoraModule.loraMsg[0] #used to retransmit

    @staticmethod
    def retransmissionData(self):
        #if LoraModule.loraCheckRepeat.count(LoraModule.loraMsg[6:9]) > 5:
        if LoraModule.loraMsg[2:3] + LoraModule.loraMsg[6:8] in [row[1] for row in LoraModule.loraCheckRepeat]:
            #print LoraModule.loraCheckRepeat.count(LoraModule.loraMsg[6:9])
            print 'The message was ignored. The message was already repeated.'
        else:

            LoraModule.sendRetransmit(self,LoraModule.loraMsg[4:])
            self.ll.waitPacketSent()
            LoraModule.loraCheckRepeat.append((round(time.time(),2) , LoraModule.loraMsg[2:3] + LoraModule.loraMsg[6:8]))
            #LoraModule.loraCheckRepeat.append(LoraModule.loraMsg[6:9])
            print 'The message was retransmitted'
        print LoraModule.loraCheckRepeat
    
 
        
    @staticmethod
    def calc_crc(data):
        crc = 0xFFFF
        for pos in data:
            crc ^= pos 
            for i in range(8):
                if ((crc & 1) != 0):
                    crc >>= 1
                    crc ^= 0xA001
                else:
                    crc >>= 1
        return crc
    
    @staticmethod
    def checkCRC(datatoCheck):
        crcCalculated =LoraModule.calc_crc(datatoCheck[4:-2])
        crcfromMessage = datatoCheck[-2:]
        if hex(crcCalculated)[2:].zfill(4) == hex(crcfromMessage[1])[2:].zfill(2)+hex(crcfromMessage[0])[2:].zfill(2):
            return True
        else:
            return False
            
    @staticmethod
    def checkDestinationGateway(header):
        if header[3] == LoraModule.idGateway or header[3] == 0xFF: # 4 if we use ui id msg
            return True
        else:
            return False

    def cleanHistoryRepeater(self):
        
        timenow = time.time()
        gettimes =  [row[0] for row in LoraModule.loraCheckRepeat] 
        a = LoraModule.loraCheckRepeat
        for times in gettimes:
            if (timenow - times > 60):
                print 'clean'
                for row in a:
                    for elem in row:
                        #print row
                        if elem == times:
                            del self.loraCheckRepeat[LoraModule.loraCheckRepeat.index(row)]
                    print ''
            else:
                break
	return


