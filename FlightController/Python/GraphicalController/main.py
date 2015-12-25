'''
Created on 3.8.2015

@author: Anton
'''

import time
import serial
import sys
from gui import Ui_Form
from threading import Thread
from serial.serialutil import writeTimeoutError
from PyQt5.QtWidgets import QApplication, QDialog
from PyQt5 import QtWidgets


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
                    print("{:}".format(self.ser.read(1).decode()), end='')
                except UnicodeDecodeError:
                    print("UnicodeDecodeError: Received bad data")
        print("Stopped listening")


    def echoPing(self):
        self.ser.write(self.ConnectionAnswer.encode())

    def requestStop(self):
        self.stopRequested = True
        print("Requested to stop listening")



class FunctionalGUI(Ui_Form):

    def sendAngles(self):
        string = 'y' + str(self.angles_yaw_spinBox.value())
        string += 'p' + str(self.angles_pitch_spinBox.value())
        string += 'r' + str(self.angles_roll_spinBox.value())
        self.send(string)

    def sendThrottle(self):
        string = 't' + str(self.throttle_spinBox.value())
        self.send(string)

    def send(self, message):
        message = chr(2) + message  #STX
        message += chr(3) #ETX
        try:
            self.ser.write(message.encode())
            print("Send:", message)
        except serial.serialutil.SerialTimeoutException:
            print("WARNING: Write timeout exceeded!")

    def setButtons(self):
        self.throttle_send_btn.released.connect(self.sendThrottle)

    def openConnection(self):
        self.ser = serial.Serial(
        port='\\.\COM3',
        baudrate=38400,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        writeTimeout=2) # Wait maximum 2 seconds on write


app = QApplication(sys.argv)
window = QDialog()
ui = FunctionalGUI()
ui.setupUi(window)
ui.setButtons()
ui.openConnection()
window.show()
sys.exit(app.exec_())
'''
else:
    listener = ListeningThread(ser)
    listener.start()
    message = ''
    print("You can now send commands")
    while(message.strip().lower() != "exit"):
        message = chr(2) #STX
        message += input()
        message += chr(3) #ETX
        try:
            ser.write(message.encode())
        except serial.serialutil.SerialTimeoutException:
            print("WARNING: Write timeout exceeded!")
    print("Exiting now")
    listener.requestStop()
    time.sleep(1) # Wait for listener to stop before we close the serial.
    ser.close()'''

if __name__ == '__main__':
    pass
