""" Created by Anton Debner in 2015 """
from serialcommunicator import SerialCommunicator
from angles import Angles
from consoleinput import ConsoleInput
from controllerinput import ControllerInput

serialCom = SerialCommunicator(38400, "port")
console = ConsoleInput(func1, exit)
controller = ControllerInput(0)
targetAngles = Angles(0.0, 0.0, 0.0)

def exit():
    serialCom.writeNow("exit\n")
    serialCom.close()
    controller.close()