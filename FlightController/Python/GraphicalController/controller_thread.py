import time
import sys
from threading import Thread
import pygame


class ControllerThread(Thread):

    def __init__(self, id, call_function):
        Thread.__init__(self)
        self.delay = 1; # Updates every 1 second
        self.buttonFunctions = dict() # Dict which connects buttons to functions
        self.controller = None
        self.call_function = call_function
        pygame.init() # Scans system for joysticks, listens to events
        print("Number of joysticks found: " + str(pygame.joystick.get_count()))
        if (pygame.joystick.get_count() > id):
            self.controller = pygame.joystick.Joystick(id)
            print("Connected to " + self.controller.get_name())
            self.controller.init()
            print("Number of axes: " + str(self.controller.get_numaxes())
                  + ". Number of buttons: " + str(self.controller.get_numbuttons()))
        else:
            print("Given joystick id not found.")


    def run(self):
        print("Controllering")
        while(True):
            self.checkPressedButtons()
            yaw = self.apply_deadzone(self.getAxis(3)) # 3 = Right stick (left -1, right 1)
            pitch = self.apply_deadzone(-self.getAxis(1)) # 1 = Left stick (up -1, down 1)
            roll = self.apply_deadzone(self.getAxis(0)) # 0 = Left stick (left -1, right 1)
            self.call_function(yaw, pitch, roll)
            time.sleep(self.delay)

    def apply_deadzone(self, value):
        d = 0.2
        if (abs(value) < d):
            return 0
        elif (value > 0): #positive
            value = (value - d) / (1.0 - d) # scales it back to [0,1]
        else: #negative
            value = (value + d) / (1.0 - d) # scales it back to [0,1]
        return value

    def printAllAxis(self):
        # 0 = Left stick (left -1, right 1)
        # 1 = Left stick (up -1, down 1)
        # 2 = Left trigger [-1,1]
        # 3 = Right stick (left -1, right 1)
        # 4 = Right stick (up -1, down 1)
        # 5 = Right trigger [-1,1]
        # Buttons:
        # 0 = A
        # 1 = B
        # 2 = X
        # 3 = Y
        # 4 = Left shoulder
        # 5 = Right shoulder
        # 6 = back
        # 7 = start
        # 8 = xbox
        # 9 = Left stick
        # 10 = Right stick
        for i in range(self.controller.get_numaxes()):
            print(i, "=", self.controller.get_axis(i), end='   ')
        print("")

    def getAxis(self, number):
        """Returns the current value of given axis"""
        if (not self.controller):
            print('no controller')
            return
        if (self.controller.get_numaxes() <= number):
            print("Warning: ControllerInput: Joystick axis " + number + " not found.")
            return 0
        return self.controller.get_axis(number)

    def connectButton(self, number, function):
        """Connects a joystick button to given function.

        NOTE: The connected functions will only be called if checkPressedButtons
        is called. Therefore you have to periodically call checkPressedButtons.
        """
        if (not self.controller):
            return
        if (self.controller.get_numbuttons() < number):
            print("Warning: ControllerInput: Attempted to connect to invalid button.")
        if (number in self.buttonFunctions.keys()):
            print("Warning: ControllerInput: Overriding previously connected function.")
        self.buttonFunctions[number] = function

    def checkPressedButtons(self):
        """Checks for pressed buttons and calls their corresponding functions,
        if a function has been connected to the button.
        """
        for event in pygame.event.get():
            if event.type == pygame.JOYBUTTONDOWN:
                print("Pressed:" + str(event.button))
                if (event.button in self.buttonFunctions.keys()):
                    self.buttonFunctions[event.button]()

    def close(self):
        pygame.quit()


if __name__ == "__main__":
    """For quick debugging"""
    def test(yaw, pitch, roll):
        print(yaw, pitch, roll)
    controller = ControllerThread(0, test)
    controller.start()
    controller.connectButton(0, controller.printAllAxis)
    try:
        while(1):
            pass
            #controller.checkPressedButtons()
    except KeyboardInterrupt:
        pygame.quit()
