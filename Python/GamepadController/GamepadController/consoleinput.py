""" Created by Anton Debner in 2015 """
from threading import Thread
class ConsoleInput(Thread):
    """Used for handling user console input.

    This is a separate thread, because we don't want to block the main
    thread while waiting for input.
    """

    def __init__(self, writeFunction, exitFunction):
        """As the names suggest, writeFunction and exitFunction must be
        callable functions.
        """
        Thread.__init__(self)
        self.writeFunction = writeFunction
        self.exitFunction = exitFunction

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
                try:
                    self.exitFunction()
                except:
                    print("ERROR: Invalid exitFunction.")
            elif (len(s) > 1):
                try:
                    self.writeFunction(s)
                except:
                    print("ERROR: Invalid writeFunction.")


