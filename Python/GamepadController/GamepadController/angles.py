class Angles(object):
    """Used to store the angles of yaw, pitch and roll axes.
    This is sometimes also used to store the angular rates of those axes.

    Methods toString() and parseString() can be used to transfer the angles to another device as a string.
    """
    MaxDecimals = 3
    
    def __init__(self, yaw, pitch, roll):
        self.yaw = yaw
        self.pitch = pitch
        self.roll = roll

    def toString(self):
        '''This method encodes the angles in to a string, which can then be parsed in to angles with the parseString-method.
        This is really useful if we want to transfer the angles over e.g. Bluetooth.
        '''
        yaw = round(self.yaw,Angles.MaxDecimals)
        pitch = round(self.pitch,Angles.MaxDecimals)
        roll = round(self.roll,Angles.MaxDecimals)
        return "y" + str(yaw) + "p" + str(pitch) + "r" + str(roll)

    def parseString(self, str):
        '''This method attempts to parse a string in to yaw, pitch and roll.
        If successful, returns True and changes the instance angles. Otherwise returns False.
        '''
        # First we try to find the letters y,p,r in the string.
        # ValueError is raised (and False returned) if one of the letters (y,p,r) is not found in the string.
        try:
            y_ind = str.index('y')
            p_ind = str.index('p')
            r_ind = str.index('r')
        except ValueError: 
            return False
        # Let's check if the letters are not in correct order or there is no room for atleast one number in between.
        if not (y_ind == 0 and y_ind+1 < p_ind and p_ind+1 < r_ind and len(str)-1 > r_ind): # Notice the negation
           return False 
        # Finally we attempt to parse the numbers. ValueError is raised (and False returned) if the numbers contain any invalid characters.
        try:
            yaw = float(str[y_ind+1:p_ind])
            pitch = float(str[p_ind+1:r_ind])
            roll = float(str[r_ind+1:])
        except ValueError:
            return False
        # We have now successfully parsed the string and we can safely change the instance variables.
        self.yaw = yaw
        self.pitch = pitch
        self.roll = roll
        return True
            