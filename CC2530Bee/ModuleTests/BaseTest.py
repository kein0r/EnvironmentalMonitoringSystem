import serial
import struct

ser = serial.Serial(
    port=30,
    baudrate=57600,
    # parity=serial.PARITY_ODD,
    # stopbits=serial.STOPBITS_TWO,
    # bytesize=serial.SEVENBITS
    timeout=1
)


def sendFrame(message):
    ser.write( struct.pack('B',0x7e) )
    # must be set to >H later to use big-endian
    ser.write( struct.pack('H',len(message)) )
    crc = 0
    for s in message:
        if (( s == 0x7e) || ( s == 0x7d) || ( s == 0x11) || ( s == 0x13) ):
            ser.write( struct.pack('B', 0x7d) )
            ser.write( struct.pack('B',s ^ 0x20) )
        else:    
            ser.write( struct.pack('B',s) )
        crc += s
    crc = 0xff - crc
    crc = crc & 0xff
    ser.write( struct.pack('B',crc) )
    return

def receiveFrame():
    delimiter = struct.unpack('B', ser.read(1))
    length = struct.unpack('H', ser.read(2))
    message = []
    crc = 0
    for i in range(0,length[0]):
        a = struct.unpack('B', ser.read(1))
        if (a[0] == 0x7d):
            a = struct.unpack('B', ser.read(1))
            a = [a[0]^0x20]
        message = message + [a[0]]
        crc += a[0]
    rcrc = ser.read(1)
    crc += rcrc[0]
    if (crc & 0xff == 0xff):
        return list(message)
    else:
        print ("CRC Error! Calculated:", (crc-rcrc[0]) & 0xff, "Received:", rcrc[0])
        return ''

def simpleUARTEchoTest(testFrame):
    # sends different frames with id 0x44
    sendFrame(testFrame)
    rxFrame = receiveFrame()
    print("Frame: ", end=" ")
    print(testFrame, end=" ")
    if (testFrame == rxFrame):
        print(": OK")
    else:
        print(": NOK")
        print("Received: ", end=" ")
        print(rxFrame)
    return

# send some frames 
frameId = 0x00
if (ser.isOpen()):
    # empty serial buffer
    ser.read(100)
    simpleUARTEchoTest([0x44, frameId, 0xff, 0xff, 0x00, 0xaf, 0xfe])
    # send simple broadcast message 16 bit (16bit addressing)
    sendFrame([0x01, frameId, 0xff, 0xff, 0x00, 0xaf, 0xfe])
    frameId+=1
    # send simple message to one recipient (16bit addressing)
    sendFrame([0x01, frameId, 0xff, 0xff, 0x00, 0xaf, 0xfe])
    frameId+=1
