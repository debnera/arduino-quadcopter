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
-----------------INITIALIZATION------------------------------------------------
'''

    def __init__(self, window):
        super().__init__()
        self.ser = None
        self.read_timer = None
        self.gamepad_timer = None
        self.bytes = 0 # Used to track the amount of bytes sent
        self.packets = 0 # Used to track the amount of packets sent
        self.minThrottle = 0
        self.setupUi(window)
        self.setButtons()
        self.createGamepad()

    def setButtons(self):
        self.throttle_send_btn.released.connect(self.sendThrottle)
        self.angles_send_btn.released.connect(self.sendAngles)
        self.angles_reset_btn.released.connect(self.resetAngles)
        self.stop_btn.released.connect(self.stop)
        self.start_btn.released.connect(self.start)

        #self.throttle_slider.valueChanged.connect(self.maybeSendThrottle)
        #self.throttle_spinBox.valueChanged.connect(self.maybeSendThrottle)
        self.angles_roll_doubleSpinBox.valueChanged.connect(self.syncAngleSliders)
        self.angles_roll_slider.valueChanged.connect(self.syncAngleBoxes)
        #self.angles_roll_slider.valueChanged.connect(self.maybeSendAngles)
        self.angles_pitch_doubleSpinBox.valueChanged.connect(self.syncAngleSliders)
        self.angles_pitch_slider.valueChanged.connect(self.syncAngleBoxes)
        #self.angles_pitch_slider.valueChanged.connect(self.maybeSendAngles)
        self.angles_yaw_doubleSpinBox.valueChanged.connect(self.syncAngleSliders)
        self.angles_yaw_slider.valueChanged.connect(self.syncAngleBoxes)
        #self.angles_yaw_slider.valueChanged.connect(self.maybeSendAngles)
        self.max_apply.clicked.connect(self.setSliderRanges)

        self.send_PID_p.clicked.connect(self.sendP)
        self.send_PID_i.clicked.connect(self.sendI)

'''
-----------------GAMEPAD-------------------------------------------------------
'''

    def createGamepad(self):
        self.gamepad = Gamepad(0)
        self.gamepad.connectButton(0, self.addMinThrottle)
        self.gamepad.connectButton(1, self.removeMinThrottle)
        self.gamepad.connectButton(6, self.stop)
        self.gamepad.connectButton(7, self.start)
        self.gamepad.connectButton(3, self.disableGamepad)
        self.gamepad_timer = PyQt5.QtCore.QTimer()
        self.gamepad_timer.timeout.connect(self.checkGamepad)
        self.setSliderRanges() # This method also starts gamepad_timer

    def checkGamepad(self):
        if self.use_gamepad_bool.isChecked():
            self.gamepad.update()
            y, p, r = self.gamepad.getYPR()
            throttle = self.gamepad.getThrottle()
            self.changeYPR(y, p, r)
            self.changeThrottle(throttle)

    def disableGamepad(self):
        self.resetThrottle()
        self.use_gamepad_bool.setChecked(False)

'''
-----------------BUTTONS-------------------------------------------------------
'''

# --------------Primary control------------------------------------------------

    def changeYPR(self, yaw, pitch, roll):
        # Only send angles after all three values have been changed.
        #temp = self.angles_sendOnChange_bool.isChecked()
        #self.angles_sendOnChange_bool.setChecked(False)
        self.angles_yaw_doubleSpinBox.setValue(yaw * self.yaw_max_spinbox.value())
        self.angles_pitch_doubleSpinBox.setValue(pitch * self.pitch_max_spinbox.value())
        self.angles_roll_doubleSpinBox.setValue(roll * self.roll_max_spinbox.value())
        #self.angles_sendOnChange_bool.setChecked(temp)
        if self.angles_sendOnChange_bool.isChecked():
            self.sendAngles()

    def changeThrottle(self, throttle_input):
        # Input goes from 0 to 1
        self.throttle_spinBox.setValue(self.minThrottle + throttle_input * self.throttle_max_spinBox.value())
        if (self.throttle_sendOnChange_bool.isChecked()):
            self.sendThrottle()

    def addMinThrottle(self):
        self.minThrottle += 71 # 71 is the limit for activating stabilizer
        print("Setting default throttle to:", self.minThrottle)

    def removeMinThrottle(self):
        self.minThrottle -= 71
        if self.minThrottle < 0:
            self.minThrottle = 0
        print("Setting default throttle to:", self.minThrottle)

    def sendAngles(self):
        string = 'y' + str(self.angles_yaw_doubleSpinBox.value())
        string += 'p' + str(self.angles_pitch_doubleSpinBox.value())
        string += 'r' + str(self.angles_roll_doubleSpinBox.value())
        self.send(string)


    def sendThrottle(self):
        string = 't' + str(self.throttle_spinBox.value())
        self.send(string)

# --------------Secondary control------------------------------------------------

    def start(self):
        self.send(chr(17))

    def stop(self):
        #self.resetAngles()
        self.resetThrottle()
        self.send(chr(20)) #DC4

    def resetAngles(self):
        self.angles_yaw_doubleSpinBox.setValue(0)
        self.angles_pitch_doubleSpinBox.setValue(0)
        self.angles_roll_doubleSpinBox.setValue(0)

    def resetThrottle(self):
        self.minThrottle = 0
        self.throttle_spinBox.setValue(0)


# --------------Other commands-------------------------------------------------

    def sendP(self):
        self.send('p' + str(self.PID_p.value()))
        print("Sent p-value")

    def sendI(self):
        self.send('i' + str(self.PID_i.value()))
        print("Sent i-value")

# ----------------GUI----------------------------------------------------------
    def syncAngleSliders(self):
        mult = 100
        self.angles_yaw_slider.setValue(int(self.angles_yaw_doubleSpinBox.value() * mult))
        self.angles_pitch_slider.setValue(int(self.angles_pitch_doubleSpinBox.value() * mult))
        self.angles_roll_slider.setValue(int(self.angles_roll_doubleSpinBox.value() * mult))

    def syncAngleBoxes(self):
        div = 100
        self.angles_yaw_doubleSpinBox.setValue(self.angles_yaw_slider.value() / div)
        self.angles_pitch_doubleSpinBox.setValue(self.angles_pitch_slider.value() / div)
        self.angles_roll_doubleSpinBox.setValue(self.angles_roll_slider.value() / div)

    def maybeSendAngles(self):
        self.syncAngleBoxes()
        if(self.angles_sendOnChange_bool.isChecked() == True):
            self.sendAngles()

    def maybeSendThrottle(self):
        if(self.throttle_sendOnChange_bool.isChecked() == True):
            self.sendThrottle()

    def setMinMax(self, obj, value):
        obj.setMinimum(-value)
        obj.setMaximum(value)

    def setSliderRanges(self):
        mult = 100
        self.setMinMax(self.angles_yaw_slider, self.yaw_max_slider.value() * mult)
        self.setMinMax(self.angles_yaw_doubleSpinBox, self.yaw_max_slider.value())
        self.setMinMax(self.angles_pitch_slider, self.pitch_max_slider.value()* mult)
        self.setMinMax(self.angles_pitch_doubleSpinBox, self.pitch_max_slider.value())
        self.setMinMax(self.angles_roll_slider, self.roll_max_slider.value()* mult)
        self.setMinMax(self.angles_roll_doubleSpinBox, self.roll_max_slider.value())
        self.throttle_slider.setMaximum(self.throttle_max_spinBox.value())
        self.throttle_spinBox.setMaximum(self.throttle_max_spinBox.value())
        if (self.gamepad_timer != None):
            freq = self.gamepad_frequency_spinBox.value()
            if freq < 1:
                freq = 1
            self.gamepad_timer.stop()
            self.gamepad_timer.start(1/freq * 1000)
        if self.ser == None:
            self.openConnection()



'''
-----------------SERIAL STUFF--------------------------------------------------
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
                #print("Send:", message)
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
-----------------END OF CLASS--------------------------------------------------
'''






app = QApplication(sys.argv)
window = QDialog()
ui = FunctionalGUI(window)
window.show()
sys.exit(app.exec_())


if __name__ == '__main__':
    pass
