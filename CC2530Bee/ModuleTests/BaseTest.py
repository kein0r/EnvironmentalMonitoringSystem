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
    print("Frame:    ", end=" ")
    print(testFrame, end=" ")
    if (testFrame == rxFrame):
        print(": OK")
    else:
        print(": NOK")
        print("Received: ", end=" ")
        print(rxFrame)
    return

def checkFrame(testFrame, expectedResult):
    sendFrame(testFrame)
    rxFrame = receiveFrame()
    print("Frame:    ", end=" ")
    print(expectedResult, end=" ")
    if (expectedResult == rxFrame):
        print(": OK")
    else:
        print(": NOK")
        print("Received: ", end=" ")
        print(rxFrame)
    return

def checkRxFrame(testFrame, rxFrame):
    print("Frame:    ", end=" ")
    print(testFrame, end=" ")
    # ignore RSSI value
    testFrame[3] = rxFrame[3]
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
    # Some simple frame tests
    simpleUARTEchoTest([0x44, frameId, 0xff])
    simpleUARTEchoTest([0x44, frameId, 0xff, 0xff])
    simpleUARTEchoTest([0x44, frameId, 0xff, 0xff, 0x00])
    simpleUARTEchoTest([0x44, frameId, 0xff, 0xff, 0x00, 0xaf])
    simpleUARTEchoTest([0x44, frameId, 0xff, 0xff, 0x00, 0xaf, 0xfe])
    # Test message size bigger than USART buffer
    simpleUARTEchoTest([0x44, frameId, 0xff, 0xff, 0x00, 0xaf, 0xfe, 0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0, 0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0])
    frameId += 1
    # Character escape test
    simpleUARTEchoTest([0x44, frameId, 0xff, 0xff, 0x7e, 0xaf, 0xfe])
    frameId += 1
    simpleUARTEchoTest([0x44, frameId, 0xff, 0xff, 0x7d, 0xaf, 0xfe])
    frameId += 1
    simpleUARTEchoTest([0x44, frameId, 0xff, 0xff, 0x11, 0xaf, 0xfe])
    frameId += 1
    simpleUARTEchoTest([0x44, frameId, 0xff, 0xff, 0x13, 0xaf, 0xfe])
    frameId += 1
    
    # Read parameter tests
    # Check channel (CN = 0x4348) equal to 0x19
    checkFrame([0x08, frameId, 0x43, 0x48],[0x88, frameId, 0x43, 0x48, 0, 0x19])
    # Test PAN ID (ID = 0x4944) equal to 0x3233
    checkFrame([0x08, frameId, 0x49, 0x44],[0x88, frameId, 0x49, 0x44, 0, 0x32, 0x33])
    # Test Destination Address High (DH = 0x4448) equal to 0x0
    checkFrame([0x08, frameId, 0x44, 0x48],[0x88, frameId, 0x44, 0x48, 0x0, 0x0, 0x0, 0x0, 0x0])
    # Test Destination Address Low (DL = 0x444c) equal to 0x0
    checkFrame([0x08, frameId, 0x44, 0x4c],[0x88, frameId, 0x44, 0x4c, 0x0, 0x0, 0x0, 0x0, 0x0])
    # Check ShortAddress (MY = 0x4d59) equal to 0xaffe
    checkFrame([0x08, frameId, 0x4d, 0x59],[0x88, frameId, 0x4d, 0x59, 0, 0xaf, 0xfe])
    # Check ExtendedAddress (SH = 0x5348) equal to serialno.
    checkFrame([0x08, frameId, 0x53, 0x48],[0x88, frameId, 0x53, 0x48, 0, 0x0, 0x0, 0x0, 0x0])
    # Check ExtendedAddress (SH = 0x534c) equal to serialno.
    checkFrame([0x08, frameId, 0x53, 0x4c],[0x88, frameId, 0x53, 0x4c, 0, 0x0, 0x0, 0x0, 0x0])

    # Tx tests
    # send simple broadcast message 16 bit (16bit addressing). No ACK expected
    sendFrame([0x01, frameId, 0xff, 0xff, 0x00, 0xaf, 0xfe])
    frameId+=1
    # send simple message 16 bit (64bit addressing). No ACK expected
    sendFrame([0x00, frameId, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xaf, 0xfe])
    frameId+=1
    # Check auto ACK. Set SRCSHORTEN0:SRCSHORTEN1 0xeeee, FRMCTRL0.AUTOACK = 1
    sendFrame([0x01, frameId, 0xee, 0xee, 0x00, 0xaf, 0xfe])

    # Rx tests
    # Make sure that "Add seq. number is disabled"
    # All tests use source address 0xeeee and sequnce no. starting from 0x41
    ser.timeout = 8
    # Rx test 16bit broadcast: Send: 61 88 41 33 32 ff ff ee ee aa bb cc dd
    checkRxFrame([0x81, 0xee, 0xee,0x0, 0x02, 0xaa, 0xbb, 0xcc, 0xdd], receiveFrame())
    # Rx test 16bit default address 0xaffe: Send: 61 88 41 33 32 af fe ee ee aa bb cc dd
    checkRxFrame([0x81, 0xee, 0xee,0x0, 0x00, 0xaa, 0xbb, 0xcc, 0xdd], receiveFrame())
    # Rx test 16bit broadcasr PAN ID: Send: 61 88 41 ff ff af fe ee ee aa bb cc dd
    checkRxFrame([0x81, 0xee, 0xee,0x0, 0x04, 0xaa, 0xbb, 0xcc, 0xdd], receiveFrame())

    # Rx test 64bit broadcast: Send: 61 8c 77 33 32 00 00 00 00 00 00 ff ff af fe aa bb cc dd
    # There is no 64bit broadcast
    # Rx test 64bit broadcast: Send: 61 8c 77 33 32 00 00 00 00 00 00 ff ff af fe aa bb cc dd
    
    # Change source address to 0xfffe check if source address is 64bit now
