""" Created by Anton Debner in 2015 """
from threading import Thread
class ConsoleInput(Thread):
    """Used for handling user console input.

    This is a separate thread, because we don't want to block the main
    thread while waiting for input.
    """

    def __init__(self, writeFunction, exitFunction, restartFunction):
        """As the names suggest, writeFunction, exitFunction and restartFunction
        must be callable functions.
        """
        Thread.__init__(self)
        self.writeFunction = writeFunction
        self.exitFunction = exitFunction
        self.restartFunction = restartFunction

    def run(self):
        """Sends any console input to given writeFunction.
        Stops listening for input when "exit" is received
        and calls exitFunction.

        NOTE:   This method is called by the threading start()-method.
                This method should not be called directly.
        """
        self.running = True
        print("Console ready for input")
        while(self.running == True):
            s = input(">> ") + "\n"
            if (s.casefold().strip() == "exit"):
                self.running = False
                if (self.exitFunction != None):
                    self.exitFunction()
            elif (s.casefold().strip() == "restart"):
                if (self.restartFunction != None):
                    self.restartFunction()
            elif (len(s) > 1):
                if (self.writeFunction != None):
                    self.writeFunction(s)


