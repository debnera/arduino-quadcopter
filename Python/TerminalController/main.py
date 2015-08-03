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
        self.ConnectionQuery = "PING" # This is the message Quad sends when it wants to check connection status.
        self.ConnectionAnswer = "PingOK" # This is the message we send back to Quad
        
    def run(self):
        print("Starting to listen")
        minDelay = 1/1000
        maxDelay = 1/5
        while(self.stopRequested == False):
            if (self.ser.inWaiting() > 0):
                delay = 0
                incoming = ''
                # We assume that every incoming message ends in newline ('\n').
                while(incoming == '' or incoming[-1] != '\n'):
                    if (self.ser.inWaiting() > 0):
                        incoming += self.ser.read(1).decode()
                    elif (maxDelay > delay):
                        # Lets sleep for a moment in case there is still incoming data
                        time.sleep(minDelay)
                        delay += minDelay
                    else:
                        # MaxDelay reached, but the incoming string doesn't end with '\n'
                        print("Warning: maxDelay exceeded when waiting for incoming data. (Does the data end in '\\n'?) \n")
                        incoming += '\n' # Add '\n' for consistency
                if (incoming != '' and incoming[0] != '\n'):
                    self.parseCommand(incoming)
        print("Stopped listening")
        
    def parseCommand(self, command):
        stripped = command.strip().lower()
        if stripped == self.ConnectionQuery.lower():
            self.ser.write(self.ConnectionAnswer.encode())
            print(stripped + " -> " + self.ConnectionAnswer + "\n")
        else:
            print(command)
    
    def requestStop(self):
        self.stopRequested = True
        print("Requested to stop listening")

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
    time.sleep(1)
    ser.write("Hello Quad!\n".encode())
    time.sleep(2)
    ser.write("\nPING\n".encode())
    time.sleep(5)
    listener.requestStop()

if __name__ == '__main__':
    pass