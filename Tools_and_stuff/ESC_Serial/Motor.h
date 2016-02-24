struct motor
{  
  int pin;
  char description[10];
  Servo servo;
};

typedef struct motor Motor;
