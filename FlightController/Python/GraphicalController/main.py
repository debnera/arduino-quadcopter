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
import PyQt5
from gamepad import Gamepad

class FunctionalGUI(Ui_Form):

    '''
    -----------------INITIALIZATION--------------------------------------------
    '''

    def __init__(self, window):
        super().__init__()
        self.ser = None
        self.read_timer = None
        self.gamepad_timer = None
        self.gamepad = None
        self.bytes = 0 # Used to track the amount of bytes sent
        self.packets = 0 # Used to track the amount of packets sent
        self.setupUi(window)
        self.setButtons()
        self.createGamepad()

    def setButtons(self):
        self.throttle_send_btn.clicked.connect(self.sendThrottle)
        self.throttle_reset_btn.clicked.connect(self.resetThrottle)
        self.angles_send_btn.clicked.connect(self.sendAngles)
        self.angles_reset_btn.clicked.connect(self.resetAngles)

        self.mode_raw_btn.clicked.connect(self.mode_raw)
        self.mode_rate_btn.clicked.connect(self.mode_rate)
        self.mode_angle_btn.clicked.connect(self.mode_angle)
        self.start_btn.clicked.connect(self.start)
        self.stop_btn.clicked.connect(self.stop)
        self.serial_open_btn.clicked.connect(self.openConnection)

        self.pid_send_btn.clicked.connect(self.sendPID)
        self.yaw_reset_btn.clicked.connect(self.resetYaw)

        self.gamepad_connect_btn.clicked.connect(self.createGamepad)
        self.gamepad_apply_btn.clicked.connect(self.setGamepadFrequency)



    '''
    -----------------GAMEPAD---------------------------------------------------
    '''

    def createGamepad(self):
        if self.gamepad == None:
            print("Connecting gamepad...")
            self.gamepad = Gamepad(0)
            if self.gamepad.enabled():
                self.gamepad.connectButton(0, self.addMinThrottle)
                self.gamepad.connectButton(1, self.removeMinThrottle)
                self.gamepad.connectButton(6, self.stop)
                self.gamepad.connectButton(7, self.start)
                self.gamepad.connectButton(3, self.disableGamepad)
                self.gamepad_timer = PyQt5.QtCore.QTimer()
                self.gamepad_timer.timeout.connect(self.checkGamepad)
                self.setGamepadFrequency() # This method starts gamepad_timer
            else:
                self.gamepad.close()
                self.gamepad = None
        else:
            print("Gamepad already exists!")

    def setGamepadFrequency(self):
        if self.gamepad != None:
            self.gamepad_timer.stop()
            freq = self.gamepad_frequency_spinBox.value()
            interval = 1000 / freq # Milliseconds
            self.gamepad_timer.start(interval)
            print("Gamepad updating rate is now", freq, "({:f} ms)".format(interval))
        else:
            print("Gamepad doesn't exist!")

    def checkGamepad(self):
        if self.gamepad != None:
            self.gamepad.update()
            y, p, r = self.gamepad.getYPR()
            throttle = self.gamepad.getThrottle()
            self.changeYPR(y, p, r)
            self.changeThrottle(throttle)
            if self.angles_sendOnChange_bool.isChecked():
                self.sendAngles()
            if self.throttle_sendOnChange_bool.isChecked():
                self.sendThrottle()

    def disableGamepad(self):
        print("Disabling gamepad")
        self.resetThrottle()
        if self.gamepad != None:
            self.gamepad.close()
            self.gamepad = None

    '''
    -----------------BUTTONS---------------------------------------------------
    '''

    # --------------Primary control--------------------------------------------

    def changeYPR(self, yaw, pitch, roll):
        self.yaw_doubleSpinBox.setValue(yaw * self.yaw_max_spinbox.value())
        self.pitch_doubleSpinBox.setValue(pitch * self.pitch_max_spinbox.value())
        self.roll_doubleSpinBox.setValue(roll * self.roll_max_spinbox.value())

    def changeThrottle(self, throttle_input):
        # Input goes from 0 to 1
        self.throttle_spinBox.setValue(throttle_input * self.throttle_max_spinBox.value())

    def addMinThrottle(self):
        prev = self.throttle_offset_spinBox.value()
        increment = self.throttle_offset_step_spinBox.value()
        self.throttle_offset_spinBox.setValue(prev + increment)
        print("Setting default throttle to:", self.throttle_offset_spinBox.value())

    def removeMinThrottle(self):
        prev = self.throttle_offset_spinBox.value()
        increment = self.throttle_offset_step_spinBox.value()
        self.throttle_offset_spinBox.setValue(prev - increment)
        if self.throttle_offset_spinBox.value() < 0:
            self.throttle_offset_spinBox.setValue(0)
        print("Setting default throttle to:", self.throttle_offset_spinBox.value())

    def sendAngles(self):
        y = self.yaw_doubleSpinBox.value() + self.yaw_offset_doubleSpinBox.value()
        p = self.pitch_doubleSpinBox.value() + self.pitch_offset_doubleSpinBox.value()
        r = self.roll_doubleSpinBox.value() + self.roll_offset_doubleSpinBox.value()
        string = 'y' + str(y)
        string += 'p' + str(p)
        string += 'r' + str(r)
        self.send(string)

    def sendThrottle(self):
        thr = self.throttle_spinBox.value() + self.throttle_offset_spinBox.value()
        string = 't' + str(thr)
        self.send(string)

    # --------------Secondary control------------------------------------------

    def start(self):
        self.send(chr(17))
        print("Activating engines")

    def stop(self):
        #self.resetAngles()
        self.resetThrottle()
        self.send(chr(20)) #DC4
        print("Disabling engines")

    def resetYaw(self):
        '''
        Resets the yaw axis of copter to zero.
        '''
        self.send('r')
        print("Resetting yaw axis")

    def mode_raw(self):
        '''
        Starts the raw controlling mode.
        Angles correspond to motor powers without PID.
        '''
        self.send('m2')
        print("Switching to raw control")

    def mode_rate(self):
        '''
        Starts the rate controlling mode.
        Angles correspond to angular rates.
        '''
        self.send('m1')
        print("Switching to rate control")

    def mode_angle(self):
        '''
        Starts the angle controlling mode.
        Angles correspond to actual orientation of the copter.
        '''
        self.send('m0')
        print("Switching to angle control")

    def resetAngles(self):
        self.yaw_doubleSpinBox.setValue(0)
        self.pitch_doubleSpinBox.setValue(0)
        self.roll_doubleSpinBox.setValue(0)
        self.yaw_offset_doubleSpinBox.setValue(0)
        self.pitch_offset_doubleSpinBox.setValue(0)
        self.roll_offset_doubleSpinBox.setValue(0)
        self.sendAngles()

    def resetThrottle(self):
        self.throttle_offset_spinBox.setValue(0)
        self.throttle_spinBox.setValue(0)
        self.sendThrottle()


    # --------------Other commands---------------------------------------------
    def sendPID(self):
        self.send('p' + str(self.pid_rollpitch_p.value()))
        self.send('i' + str(self.pid_rollpitch_i.value()))
        self.send('w' + str(self.pid_yaw_p.value()))
        print("All pid values sent!")


    # ----------------GUI------------------------------------------------------



    '''
    -----------------SERIAL STUFF----------------------------------------------
    '''


    def openConnection(self):
        print("Opening bluetooth connection...")
        self.ser = serial.Serial(
        port=None,
        baudrate=115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        writeTimeout=2) # Wait maximum 2 seconds on write
        if (self.read_timer == None):
            self.read_timer = PyQt5.QtCore.QTimer()
        self.read_timer.timeout.connect(self.readSerial)
        self.read_timer.start(100)
        for i in range(2):
            try:
                print("Trying port", i)
                self.ser.port = '/dev/rfcomm' + str(i)
                self.ser.open()
                print("Bluetooth open!")
                return # Success!
            except serial.serialutil.SerialException:
                print("Cannot open bluetooth at port", i)
        self.ser = None # Opening failed


    def send(self, message):
        if (self.ser != None):
            message = chr(2) + message  #STX
            message += chr(3) #ETX
            self.packets += 1
            self.bytes += len(message)
            self.data_label.setText(str(self.bytes))
            self.packet_label.setText(str(self.packets))
            try:
                self.ser.write(message.encode())
            except serial.serialutil.SerialTimeoutException:
                print("WARNING: Write timeout exceeded!")
                print("WARNING: Bluetooth disconnected")
                self.ser = None

    def readSerial(self):
        if (self.ser != None):
            while (self.ser.inWaiting() > 0):
                try:
                    c = self.ser.read(1).decode()
                    print("{:}".format(c), end='')
                    if (c == chr(5)):
                        self.send(chr(6))
                except UnicodeDecodeError:
                    print("UnicodeDecodeError: Received bad data")
        else:
            self.read_timer.stop()

    '''
    -----------------END OF CLASS----------------------------------------------
    '''


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = QDialog()
    ui = FunctionalGUI(window)
    window.show()
    sys.exit(app.exec_())
