'''
Created on 3.8.2015

@author: Anton
'''

import time
import serial
from threading import Thread

class ListeningThread(Thread):
    
    def __init__(self, ser):
        Thread.__init__(self)
        self.stopRequested = False
        self.ser = ser
        
    def run(self):
        print("Starting to listen")
        while(self.stopRequested == False):
            if (self.ser.inWaiting() > 0):
                time.sleep(1/10)
                print(self.ser.read(self.ser.inWaiting()).decode())
                #print(self.ser.read().decode(encoding='utf_8', errors='strict'))
        print("Stopped listening")
        
    def requestStop(self):
        self.stopRequested = True
        print("Requested to stop listening")
        
    def echoConnectionCheck(self):
        pass

def openConnection():
    ser = serial.Serial(
    port='\\.\COM3',
    baudrate=38400,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
    )
    return ser

try:
    ser = openConnection()
except:
    print("Unable to open connection, aborting")
else:
    listener = ListeningThread(ser)
    listener.start()
    time.sleep(2)
    ser.write("Hello Quad!\n".encode(encoding='utf_8', errors='strict'))
    time.sleep(2)
    listener.requestStop()

if __name__ == '__main__':
    pass