""" Created by Anton Debner in 2015 """
import pygame # Pygame is the easiest way I could figure out to use joysticks
class ControllerInput(object):
    """Used for accessing joystick axes and buttons."""
    def __init__(self, id):
        self.buttonFunctions = dict() # Dict which connects buttons to functions
        self.controller = None
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

    def getAxis(self, number):
        """Returns the current value of given axis"""
        if (not self.controller):
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
    controller = ControllerInput(0)

    def test():
        print(controller.getAxis(0))
    controller.connectButton(0, test)
    try:
        while(1):
            controller.checkPressedButtons()
    except KeyboardInterrupt:
        pygame.quit()
