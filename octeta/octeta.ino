//constantele motoarelor:
int MOTOR1_PIN1 = 3;
int MOTOR1_PIN2 = 5;
int MOTOR2_PIN1 = 6;
int MOTOR2_PIN2 = 9;

struct movement {
  int mode;
  int ticks;
  int priority;
};

void setup()
{
  //setam slot-urile motoarelor pe OUTPUT
  pinMode(MOTOR1_PIN1, OUTPUT); //
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);

  // setam toate slot-urile cu senzori pe INPUT pt a primi informatie
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

movement m[10];
int
  moves = 0,
  colorThresholdFata = 600,
  colorThresholdSpate = 750,
  distThreshold = 350,
  ignoreWhiteTicks = 0,
  defaultModeTicks = 0;

const int
  MOVE_MODE_FATA = 0,
  MOVE_MODE_STANGA = 1,
  MOVE_MODE_DREAPTA = 2,
  MOVE_MODE_SPATE = 3,
  MOVE_MODE_STOP = 4,
  MOVE_MODE_SPATE_STANGA = 5,
  MOVE_MODE_SPATE_DREAPTA = 6;
  
const int
  ROTATE_TICKS = 200,
  DISTANCE_M = 2;

int getCurPriority() {
  if (moves == 0) {
    return 9999;
  }
  return m[0].priority;
}

void addMove(int mode, int ticks, int priority) {
  // Mutari cu prioritate mai mica? Screw them.
  while (
    moves > 0 &&
    m[moves - 1].priority > priority
  ) {
    moves--;
  }
  
  // Adauga miscare
  m[moves].mode = mode;
  m[moves].ticks = ticks;
  m[moves].priority = priority;
  moves++;
}

void loop()
{
  int S_FS = analogRead(0); // fata stanga // culoare stanga
  int S_FD = analogRead(1); // fata dreapta // culoare dreapta
  int S_SS = analogRead(3); // spate stanga // lat dr
  int S_SD = analogRead(2); // spate dreapta // lat stg
  int dist = analogRead(4); // distanta
  
  if (getCurPriority() > 2) {
    if (
      S_SS < colorThresholdSpate &&
      S_SD < colorThresholdSpate
    ) {
      addMove(MOVE_MODE_FATA    , ROTATE_TICKS * 4 * DISTANCE_M, 2);
      addMove(MOVE_MODE_DREAPTA , ROTATE_TICKS * 2, 2);
      Serial.println("Spate doua;");
    }   
    else if (
      S_FS < colorThresholdFata &&
      S_FD < colorThresholdFata
    ) {
      addMove(MOVE_MODE_SPATE , ROTATE_TICKS * 4 * DISTANCE_M,  2);
      addMove(MOVE_MODE_STANGA , ROTATE_TICKS * 2, 2);
      Serial.println("Fata doua;");    
    }
    else if (
      S_FS < colorThresholdFata &&
      S_SS < colorThresholdSpate
    ) {
      addMove(MOVE_MODE_DREAPTA , ROTATE_TICKS    , 2);
      addMove(MOVE_MODE_FATA    , ROTATE_TICKS * 4 * DISTANCE_M, 2);
      Serial.println("Stanga doua;"); 
    }
    else if (
      S_FD < colorThresholdFata &&
      S_SD < colorThresholdSpate
    ) {
      addMove(MOVE_MODE_STANGA  , ROTATE_TICKS    , 2);
      addMove(MOVE_MODE_FATA    , ROTATE_TICKS * 4 * DISTANCE_M, 2);
      Serial.println("Dreapta doua;"); 
    }
  }
  
  if (getCurPriority() > 5) {
    if (S_FS < colorThresholdFata) { // E alb
      if (dist > distThreshold) { // Are ceva in fata
        addMove(MOVE_MODE_STANGA  , ROTATE_TICKS    , 5);
        addMove(MOVE_MODE_SPATE   , ROTATE_TICKS * 2 * DISTANCE_M, 5);
        Serial.println("Fata stanga; il vad in fata;");
      }
      else {
        addMove(MOVE_MODE_SPATE  , ROTATE_TICKS * 2 * DISTANCE_M   , 5);
        addMove(MOVE_MODE_DREAPTA, ROTATE_TICKS       , 5);
        Serial.println("Fata stanga;");
      }
    }
    else if (S_FD < colorThresholdFata) { // E alb
      if (dist > distThreshold) { // Are ceva in fata
        addMove(MOVE_MODE_DREAPTA  , ROTATE_TICKS     , 5);
        addMove(MOVE_MODE_SPATE,     ROTATE_TICKS * 2 * DISTANCE_M , 5);
        Serial.println("Fata dreapta; il vad in fata;");
      }
      else {
        addMove(MOVE_MODE_SPATE  , ROTATE_TICKS * 2   , 5);
        addMove(MOVE_MODE_STANGA , ROTATE_TICKS       , 5);
        Serial.println("Fata dreapta;");
      }
    }
    else if (S_SS < colorThresholdSpate) { // E alb
      if (dist > distThreshold) { // Are ceva in fata
        addMove(MOVE_MODE_STANGA  , ROTATE_TICKS    , 5);
        addMove(MOVE_MODE_FATA ,    ROTATE_TICKS * 2 * DISTANCE_M, 5);
        Serial.println("Spate stanga; il vad in fata;");
      }
      else {
        addMove(MOVE_MODE_DREAPTA  , ROTATE_TICKS    , 5);
        addMove(MOVE_MODE_FATA     , ROTATE_TICKS * 2 * DISTANCE_M, 5);
        Serial.println("Spate stanga;");
      }
    }
    else if (S_SD < colorThresholdSpate) { // E alb
      if (dist > distThreshold) { // Are ceva in fata
        addMove(MOVE_MODE_DREAPTA  , ROTATE_TICKS    , 5);
        addMove(MOVE_MODE_FATA     , ROTATE_TICKS * 2 * DISTANCE_M, 5);
        Serial.println("Spate dreapta; il vad in fata;");
      }
      else {
        addMove(MOVE_MODE_STANGA  , ROTATE_TICKS    , 5);
        addMove(MOVE_MODE_FATA ,    ROTATE_TICKS * 2 * DISTANCE_M, 5);
        Serial.println("Spate dreapta;");
      }
    }
  }
  
  if (moves > 0) {
    if (getCurPriority() < 10) {
      defaultModeTicks = 0;
    }
    m[moves - 1].ticks--;
         
    // Cum ne miscam astazi?
    switch(m[moves - 1].mode) {
      case MOVE_MODE_FATA:
        go(255, 240);
        break;
        
      case MOVE_MODE_STANGA:
        go(-255, 255);
        break;
        
      case MOVE_MODE_DREAPTA:
        go(255, -255);
        break;
        
      case MOVE_MODE_SPATE:
        go(-255, -255);
        break;
        
      case MOVE_MODE_STOP:
        go(0, 0);
        break;
        
      case MOVE_MODE_SPATE_STANGA:
        go(-255, 150);
        break;
        
      case MOVE_MODE_SPATE_DREAPTA:
        go(150, -255);
        break;
    } 
    
    // Termina move-ul
    if (m[moves - 1].ticks == 0) {
      Serial.println("Am terminat un move.");
      moves--;
    }
  }
  else {    
    // Increase ticks for default mode.
    defaultModeTicks++;
    
    // Too many default ticks? Suntem lipiti de el.
    if (defaultModeTicks > 6000) {
      int r = random(0, 2);
      if (0 == 0) {          
        addMove(MOVE_MODE_STOP, ROTATE_TICKS * 2, 7);
        addMove(MOVE_MODE_FATA, ROTATE_TICKS * 2, 7);
        addMove(MOVE_MODE_STOP, ROTATE_TICKS * 2, 7);
        addMove(MOVE_MODE_FATA, ROTATE_TICKS * 2, 7);
        addMove(MOVE_MODE_STOP, ROTATE_TICKS * 2, 7);
        addMove(MOVE_MODE_FATA, ROTATE_TICKS * 2, 7);
        addMove(MOVE_MODE_STOP, ROTATE_TICKS * 2, 7);
        addMove(MOVE_MODE_FATA, ROTATE_TICKS * 2, 7);
      } 
      else if (r == 1) {     
        addMove(MOVE_MODE_STANGA, ROTATE_TICKS * 8, 7);
        addMove(MOVE_MODE_FATA, ROTATE_TICKS * 8, 7);
      }      
      else if (r == 2) {     
        addMove(MOVE_MODE_DREAPTA, ROTATE_TICKS * 8, 7);
        addMove(MOVE_MODE_FATA, ROTATE_TICKS * 8, 7);
      }      
    }
    
    // Default move -> attack!
    go(255, 255); 
    
    // Random rotation
    int r = random(0, 8000);
    if (r == 0) {
      addMove(MOVE_MODE_STANGA  , ROTATE_TICKS * 4, 10);
      Serial.println("Random stanga.");
    } 
    else if (r == 1) {
      addMove(MOVE_MODE_DREAPTA , ROTATE_TICKS * 4, 10);
      Serial.println("Random dreapta.");
    }
  }  
}

void go(int speedLeft, int speedRight) {
  if (speedLeft > 0) {
    analogWrite(MOTOR1_PIN1, speedLeft);
    analogWrite(MOTOR1_PIN2, 0);
  }
  else {
    analogWrite(MOTOR1_PIN1, 0);
    analogWrite(MOTOR1_PIN2, -speedLeft);
  }
  
  if (speedRight > 0) {
    analogWrite(MOTOR2_PIN1, speedRight);
    analogWrite(MOTOR2_PIN2, 0);
  }
  else {
    analogWrite(MOTOR2_PIN1, 0);
    analogWrite(MOTOR2_PIN2, -speedRight);
  }
}
