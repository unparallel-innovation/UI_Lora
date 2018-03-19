# UI LoRa Protocol

The LoRa communication protocol is being developed with the purpose of having an easy way to communicate bi-directionally between sensor nodes and gateways. The gateway can request the sensor nodes to send sensor measurements but this protocol also allows the nodes to initiate communications to send sensor values without receiving a request from the gateway. The values can be sent with or without a timestamp per measurement. This makes it very easy to do data logging in the node since it can store several readings and timestamps of one sensor and then send all the data in a single message. The protocol also supports bundling several sensors values from different sensors and send a single message with a single timestamp for all the measurements. Additionally, the protocol supports alarms sent from the sensor node that can be handled differently in the gateway to trigger actions or alarm response procedures.

There are also other miscellaneous features that add functionality like sending calibration values to the node, for wireless calibration. The gateway can also read and write variables in the node that can be used for control, status or other features. Read the node’s date and time to check if it is correct and in case the node loses the correct time the gateway can send a command to set the date and time. Additionally, the gateway can activate or deactivate sensors in the node and send a reboot command to restart a node.

The protocol supports any number of operations in a single message, limited only by the maximum LoRa maximum message size. This allows doing more with a smaller number of messages which is very important for saving energy, since wireless communication is one of the more energy intensive tasks. This way several different operations can be grouped in a single LoRa packet reducing by a large factor the number of communications between sensor nodes and Gateway. Each operation has a fixed number of data bytes (fixed length by operation) making it easy to parse several operations in a single message. The complete message can have any size, from a single operation to several operations, being only limited by the LoRa maximum payload.

## **Protocol Structure**

< Protocol Identifier bytes > < ID > < length byte >
< operation byte > < data bytes >
< operation byte > < data bytes >
…  …
< operation byte > < data bytes >
< CRC bytes >

Protocol Identifier – 2 bytes
 - This is used to identify messages that belong to this protocol.
 - These two bytes can also be used to handle several sensor networks
   using the same gateway as long as they use different identifiers and
   the gateway is prepared to handle them.


ID – 1 byte
 - 0 – Broadcast
 - 1 - 200 – Sensor node ID
 - 201 - 255 – Repeater/Gateway ID

The number of bytes in the ID field can easily be extended to support more Nodes and Gateways.
	
Length – 1 byte
 - Number of bytes succeeding the length byte and preceding the CRC bytes. Total number of bytes in the operations and data parameters.

Operation - 1 byte
 - 256 possible operations 

Sensor Number - 1 byte
 - 256 possible sensors

CRC – 2 bytes
 - Cyclic Redundancy Check (CRC) for error-detecting.

**Operations: Node -&gt; Gateway**

| Function Code | Function Name | Number of Data Bytes | Data bytes |
| --- | --- | --- | --- |
| 0x00 | ACK / Confirm | 0 | - |
| 0x10 | Send Value | 4 | Float value read |
| 0x20 | Timestamp | 4 | Date and Time |
| 0x30 | Send Value with Timestamp | 8 | Float value + Timestamp |
| 0x40 | Alarm | 8 | Value that triggered the alarm + Timestamp |

**Operations:**  **Gateway -&gt; Node**

| Function Code | Function Name | Number of Data Bytes | Data bytes |
| --- | --- | --- | --- |
| 0x50 | Read Sensor | 0 | - |
| 0x60 | Calibrate | 8 | Float values (m and b) for the linear equation used in the sensor calibration. y = m\*x + b |
| 0x70 | Read Variable | 2 | Variable address |
| 0x80 | Set variable | 6 | Variable address and Value to Set |
| 0x90 | Restart Node | 0 | - |
| 0xA0 | Set Time and Date | 4 | Time and Date to set |
| 0xB0 | Read Time and Date | 0 | - |
| 0xC0 | Activate Sensor | 0 | - |
| 0xD0 | Deactivate Sensor | 0 | - |
| 0xE0 | - | - | - |
| 0xF0 | Error | 1 | Error Code |
