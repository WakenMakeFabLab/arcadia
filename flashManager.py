import serial
import struct
import sys
from time import sleep

def writeFlash():
    fileName = sys.argv[3]
    samples = open(fileName, 'rb').read()
    print len(samples)
    ser.write(b'w')
    print 'written'
    print ser.readline() # Wait write ACK
    size = len(samples)
    ser.write(struct.pack('>I', size))
    while True:
        data = ser.readline();
        if len(data) > 0:
            print data
        if data[0:15] == 'Expecting bytes':
            for x in range(0, len(samples)):
                ser.write(samples[x])
                print str(x) + ' ' + str(samples[x]) + ' '+ ser.readline()

def readFlash():
    ser.write(b'd')
    print 'dump'
    while True:
        data = ser.readline();
        if len(data) > 0:
            print data

def eraseFlash():
    ser.write(b'e')
    print 'Erase'
    print ser.readline();
    print ser.readline();

if len(sys.argv) < 3:
    print 'Usage: python flashManager.py port cmd [binaryFileName]'
    sys.exit(1)

port = sys.argv[1]
cmd = sys.argv[2]
ser = serial.Serial(port, 115200)

try:
    data = ser.readline()
    print data
    if data[0:5] == 'Ready':
        print 'Arduino is ready'
        if cmd == 'read':
            readFlash()
        elif cmd == 'write':
            writeFlash()
        elif cmd == 'erase':
            eraseFlash()
except KeyboardInterrupt:
    print "Closing serial..."
    ser.close()

ser.close()
