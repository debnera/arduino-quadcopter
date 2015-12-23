'''
Created on 3.8.2015

@author: Anton
'''

import time
import serial
from threading import Thread
from serial.serialutil import writeTimeoutError

class ListeningThread(Thread):
    
    def __init__(self, ser):
        Thread.__init__(self)
        self.stopRequested = False
        self.ser = ser
        self.ConnectionQuery = "ping" # This is the message Quad sends when it wants to check connection status.
        self.ConnectionAnswer = "pingok\n" # This is the message we send back to Quad
        
    def run(self):
        print("Starting to listen")
        minDelay = 1/1000
        maxDelay = 1/5
        while(self.stopRequested == False):
            if (self.ser.inWaiting() > 0):
                try:
                    print(self.ser.read(1).decode())
                except UnicodeDecodeError:
                    print("UnicodeDecodeError: Received bad data")
        print("Stopped listening")
        
            
    def echoPing(self):
        self.ser.write(self.ConnectionAnswer.encode())
    
    def requestStop(self):
        self.stopRequested = True
        print("Requested to stop listening")

def openConnection():
    ser = serial.Serial(
    port='\\.\COM6',
    baudrate=38400,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    writeTimeout=2) # Wait maximum 2 seconds on write
    return ser

try:
    ser = openConnection()
except:
    print("Unable to open connection, aborting")
else:
    listener = ListeningThread(ser)
    listener.start()
    message = ''
    print("You can now send commands")
    while(message.strip().lower() != "exit"):
        message = input()
        message += char(3); #ETX
        try:
            ser.write(message.encode())
        except serial.serialutil.SerialTimeoutException:
            print("WARNING: Write timeout exceeded!")
    print("Exiting now")
    listener.requestStop()
    time.sleep(1) # Wait for listener to stop before we close the serial.
    ser.close()

if __name__ == '__main__':
    pass
