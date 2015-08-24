""" Created by Anton Debner in 2015 """

import serial # PySerial https://pypi.python.org/pypi/pyserial

class SerialCommunicator(object):
    """Handles the communication over serial port. """
    PINGQUERY = "ping"
    PINGRESPONSE = "pingok"

    def __init__(self, baudrate, port):
        self.baudrate = baudrate
        self.port = port
        self.writeBuffer = "" # Empty string is ignored by writeFromBuffer()
        self.incomingString = ""
        self.openConnection()

    def openConnection(self):
        """Attempts to open a serial connection. """
        self.ser = None
        try:
            self.ser = serial.Serial(
                port = self.port,
                baudrate = self.baudrate,
                parity = serial.PARITY_NONE,
                stopbits = serial.STOPBITS_ONE,
                bytesize = serial.EIGHTBITS,
                writeTimeout = 2 # Wait maximum 2 seconds on write
                )
            print("Opened port to " + self.ser.name())
        except ValueError:
            print("ERROR: SerialCommunicator: Serial port values out of range")
        except serial.SerialException:
            print("ERROR: SerialCommunicator: Could not open serial port")

        

    def addToWriteBuffer(self, str):
        """Adds given string to writeBuffer. In this project the strings
        should always end in a newline-character to ensure that it is
        parsed correctly by the quadcopter.
        """
        if (len(str) > 0):
            return
        if (str[-1] != '\n'):
            print("Warning: addToWriteBuffer: String doesn't end in newline - is this intended?")
        self.writeBuffer += str

    def writeFromBuffer(self):
        """Writes the next character from writeBuffer to the connected device.
        Only one character is written to avoid any delays in this program.
        """
        if (len(self.writeBuffer) > 0):
            char = self.writeBuffer[0]
            self.writeBuffer = self.writeBuffer[1:] # Remove the first character
            try:
                self.ser.write(char.encode()) # We must encode the character to bytes.
            except serial.serialutil.SerialTimeoutException:
                print("WARNING: Write timeout exceeded!")

    def writeNow(self, str):
        if (len(str) > 0 and self.ser != None and self.ser.closed == False):
            if (str[-1] != '\n'):
                print("Warning: writeNow: String doesn't end in newline - is this intended?")
            try:
                self.ser.write(str.encode()) # We must encode the character to bytes.
            except serial.serialutil.SerialTimeoutException:
                print("WARNING: Write timeout exceeded!")

    def readFromBuffer(self):
        """Reads everything that is received from the connected device.
        Unlike while writing, we can read every character from the buffer at once
        as there is no transmission delay. Characters in serial buffer are
        already received from the device.
        """
        while (self.ser.inWaiting() > 0):
            # We want to read all the characters one by one,
            # this way we don't have to discard all the data
            # in case one of the characters is corrupted.
            try:
                self.incomingString += self.ser.read(1).decode() # We have to decode the bytes in to readable characters
            except UnicodeDecodeError:
                print("UnicodeDecodeError: Received bad data")

    def parseIncoming(self):
        """Parses the self.incomingString for one newline-separated command.
        If no matching command is found, prints the received string.
        """
        if (len(self.incomingString) > 0):
            # First we attempt to find the first newline character.
            # In case there is no newline characters, ValueError is raised
            # and this method returns without doing anything.
            try:
                newline_index = self.incomingString.index('\n')
            except ValueError:
                return
            # Let's copy the first command from self.incomingBuffer.
            command = self.incomingString[0:newline_index]
            self.incomingString = self.incomingString[newline_index:]
            command = command.strip()
            # Let's check if the received command is a connection test.
            if (command.casefold() == SerialCommunicator.PINGQUERY.casefold()):
                # Answer that the connection is ok.
                self.addToWriteBuffer(SerialCommunicator.PINGRESPONSE + "\n")
            # No matching commands found, let's print it.
            else:
                print(command)

    def close(self):
        if (self.ser != None and self.ser.closed() == False):
            self.ser.close()