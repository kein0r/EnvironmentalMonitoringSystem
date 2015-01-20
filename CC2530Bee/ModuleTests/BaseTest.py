import serial
import struct

ser = serial.Serial(
    port=29,
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
    crc = 0;
    for s in message:
        ser.write( struct.pack('B',s) )
        crc += s;
    crc = 0xff - crc;
    crc = crc & 0xff
    ser.write( struct.pack('B',crc) )

# send some frames 
frameId = 0x00
if (ser.isOpen()):
	# send simple broadcast message 16 bit (16bit addressing)
	sendFrame([0x00, frameId++, 0xff, 0xff, 0x00, 0xaf, 0xfe])
	# send simple message to one recipient (16bit addressing)
	sendFrame([0x00, frameId++, 0xff, 0xff, 0x00, 0xaf, 0xfe])