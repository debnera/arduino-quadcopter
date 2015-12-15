""" Created by Anton Debner in 2015 """
from serialcommunicator import SerialCommunicator
from angles import Angles
from consoleinput import ConsoleInput
from controllerinput import ControllerInput

class Main(object):

    def __init__(self):
        self.baudrate = 38400
        self.port = "port"
        self.serialCom = SerialCommunicator(self.baudrate, self.port)
        self.controller = ControllerInput(0)
        self.console = ConsoleInput(self.write, self.exit, self.restart)
        self.console.start()
        self.targetAngles = Angles(0.0, 0.0, 0.0)

    def write(self,str):
        self.serialCom.addToWriteBuffer(str)

    def restart(self):
        print("Restarting")
        self.serialCom.close()
        self.controller.close()
        self.serialCom = SerialCommunicator(self.baudrate, self.port)
        self.controller = ControllerInput(0)

    def exit(self):
        print("Exiting")
        self.serialCom.writeNow("exit\n")
        self.serialCom.close()
        self.controller.close()


if __name__ == "__main__":
    main = Main()
    