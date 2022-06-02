#include "Wire.h" // For I2C

#include "LCD.h" // For LCD

#include "LiquidCrystal_I2C.h"

#define LED1 13
#define LED2 11
#define LED3 9

#define BUZZER 8

#define PIN_BUTTON 2
#define PIN_AUTOPLAY 1
#define PIN_READWRITE 10
#define PIN_CONTRAST 12

byte sprite[] = {'.', 1, 2, 3, 4, 5, 6, 7, ' '};

byte heroPositions[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

byte posibileTerenuri[] = {0, 1, 2};

#define HERO_HORIZONTAL_POSITION 1       // Horizontal position of hero on screen


LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7);
static char terrainUpper[17];
static char terrainLower[17];
static bool buttonPushed = false;
static byte heroPos = 1;
static bool playing = false;
static unsigned int timpEfectiv = 0;

int totalScore = 0;
int currentLevel = 1;
static bool isWinning = false;
int lifes = 3;
int dificulty = 0;
int frame;

void initializeGraphics(){
  
  static byte graphics[] = {
    // Run position 1

    B01111,
  B01011,
  B01111,
  B01100,
  B01111,
  B11110,
  B10010,
  B11000,
    // Run position 2
  
    
    B01111,
  B01011,
  B01111,
  B01100,
  B01111,
  B11110,
  B10010,
  B00011,
    
    // Jump
   
    B01111,
  B01011,
  B01111,
  B01100,
  B01111,
  B11110,
  B10010,
  B11011,
    // Jump lower
   
  B01100,
  B01111,
  B11110,
  B10010,
  B11011,
  B00000,
  B00000,
  B00000,
    // Ground
    B00000,
    B00000,
    B00000,
    B11111,
    B11111,
    B10001,
    B10001,
    B10001,
    // Ground right
    B00000,
    B00000,
    B00000,
    B00011,
    B00011,
    B00010,
    B00010,
    B00010,
    // Ground left
    B00000,
    B00000,
    B00000,
    B11000,
    B11000,
    B01000,
    B01000,
    B01000,
  };

  
  for (int i = 0; i < 7; ++i) {
    lcd.createChar(i + 1, &graphics[i * 8]);
  }
  
  for (int i = 0; i < 16; ++i) {
    terrainUpper[i] = sprite[8];
    terrainLower[i] = sprite[8];
  }
}

bool drawHero(byte position, char* terrainUpper, char* terrainLower, unsigned int score) {
  bool collide = false;
  char upperSave = terrainUpper[HERO_HORIZONTAL_POSITION];
  char lowerSave = terrainLower[HERO_HORIZONTAL_POSITION];
  byte upper, lower;

  if (position == heroPositions[0]) {
    upper = sprite[8];
    lower = sprite[8];  
  }

  if (position == heroPositions[1]) {
    upper = sprite[8];
    lower = sprite[1];
  }

  if (position == heroPositions[2]) {
    upper = sprite[8];
    lower = sprite[2];
  }

  if (position == heroPositions[3] || position == heroPositions[10]) {
    upper = sprite[8];
    lower = sprite[3];
  }

  if (position == heroPositions[4] || position == heroPositions[9]) {
    upper = sprite[0];
    lower = sprite[4];
  }

  if (position == heroPositions[3] || position == heroPositions[10]) {
    upper = sprite[8];
    lower = sprite[3];
  }

  if (position == heroPositions[5] || position == heroPositions[6] || position == heroPositions[7] || position == heroPositions[8]) {
    upper = sprite[3];
    lower = sprite[8];
  }

  if (position == heroPositions[11]) {
    upper = sprite[1];
    lower = sprite[8];
  }

  if (position == heroPositions[12]) {
    upper = sprite[2];
    lower = sprite[8];
  }
  
  if (upper != ' ') {
    terrainUpper[HERO_HORIZONTAL_POSITION] = upper;
    collide = (upperSave == sprite[8]) ? false : true;
  }
  if (lower != ' ') {
    terrainLower[HERO_HORIZONTAL_POSITION] = lower;
    collide |= (lowerSave == sprite[8]) ? false : true;
  }
  
  byte digits = (score > 9999) ? 5 : (score > 999) ? 4 : (score > 99) ? 3 : (score > 9) ? 2 : 1;
  
  // Draw the scene
  terrainUpper[16] = '\0';
  terrainLower[16] = '\0';
  char temp = terrainUpper[16-digits];
  terrainUpper[16-digits] = '\0';
  lcd.setCursor(0,0);
  lcd.print(terrainUpper);
  terrainUpper[16-digits] = temp;  
  lcd.setCursor(0,1);
  lcd.print(terrainLower);
  
  lcd.setCursor(16 - digits,0);
  lcd.print(score);

  totalScore = score;
  
  terrainUpper[HERO_HORIZONTAL_POSITION] = upperSave;
  terrainLower[HERO_HORIZONTAL_POSITION] = lowerSave;
  return collide;
}


void buttonPush() {
  buttonPushed = true;
}

int count1 = 0;
byte teren1[] = {5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5,
                5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

int count2 = 0;
byte teren2[] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5,
                5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

int count3 = 0;
byte teren3[] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 5, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

void level1(int frame) {
  if (count1 == 119) count1 = 0;

  for (int i = 0; i < 16; ++i) {
    char current = terrainLower[i];
    char next = (i == 15) ? teren1[count1++] : terrainLower[i+1];
    
    if (current == sprite[8])
      if (next == sprite[5])
        terrainLower[i] = sprite[6];
       else terrainLower[i] = sprite[8];

    if (current == sprite[5])
      if (next == sprite[8])
        terrainLower[i] = sprite[7];
       else terrainLower[i] = sprite[5];

    if (current == sprite[6])
        terrainLower[i] = sprite[5];

    if (current == sprite[7])
        terrainLower[i] = sprite[8];
  }
   
  if (buttonPushed) {
    if (heroPos <= heroPositions[2]) heroPos = heroPositions[3];
    buttonPushed = false;
  }  
  if (drawHero(heroPos, terrainUpper, terrainLower, timpEfectiv >> 3)) {
    if (lifes == 1) playing = false; // The hero collided with something. Too bad.
    heroPos = heroPositions[6];
    lifes--;
    int t = 0;
    do {
      t++;
      tone(BUZZER, 450);
      delay(200);
      noTone(BUZZER);
      delay(200);  
    } while (t < 2);
  } else {
    if (heroPos == heroPositions[2] || heroPos == heroPositions[10]) {
      heroPos = heroPositions[1];
    } else if ((heroPos >= heroPositions[5] && heroPos <= heroPositions[7]) && terrainLower[HERO_HORIZONTAL_POSITION] != sprite[8]) {
      heroPos = heroPositions[11];
    } else if (heroPos >= heroPositions[11] && terrainLower[HERO_HORIZONTAL_POSITION] == sprite[8]) {
      heroPos = heroPositions[7];
    } else if (heroPos == heroPositions[12]) {
      heroPos = heroPositions[11];
    } else {
      ++heroPos;
    }
    ++timpEfectiv;
    
    digitalWrite(PIN_AUTOPLAY, terrainLower[HERO_HORIZONTAL_POSITION + 2] == sprite[8] ? HIGH : LOW);
  }
  delay(frame);
}



void level2(int frame) {
  if (count2 == 119) count2 = 0;

  for (int i = 0; i < 16; ++i) {
    char current = terrainLower[i];
    char next = (i == 15) ? teren2[count2++] : terrainLower[i+1];
    
    if (current == sprite[8])
      if (next == sprite[5])
        terrainLower[i] = sprite[6];
       else terrainLower[i] = sprite[8];

    if (current == sprite[5])
      if (next == sprite[8])
        terrainLower[i] = sprite[7];
       else terrainLower[i] = sprite[5];

    if (current == sprite[6])
        terrainLower[i] = sprite[5];

    if (current == sprite[7])
        terrainLower[i] = sprite[8];
  }
  
  if (buttonPushed) {
    if (heroPos <= heroPositions[2]) heroPos = heroPositions[3];
    buttonPushed = false;
  }  
  if (drawHero(heroPos, terrainUpper, terrainLower, timpEfectiv >> 3)) {
    //playing = false; // The hero collided with something. Too bad.
    heroPos = heroPositions[6];
    lifes--;
    int t = 0;
    do {
      t++;
      tone(BUZZER, 450);
      delay(200);
      noTone(BUZZER);
      delay(200);  
    } while (t < 2);
  } else {
    if (heroPos == heroPositions[2] || heroPos == heroPositions[10]) {
      heroPos = heroPositions[1];
    } else if ((heroPos >= heroPositions[5] && heroPos <= heroPositions[7]) && terrainLower[HERO_HORIZONTAL_POSITION] != sprite[8]) {
      heroPos = heroPositions[11];
    } else if (heroPos >= heroPositions[11] && terrainLower[HERO_HORIZONTAL_POSITION] == sprite[8]) {
      heroPos = heroPositions[7];
    } else if (heroPos == heroPositions[12]) {
      heroPos = heroPositions[11];
    } else {
      ++heroPos;
    }
    ++timpEfectiv;
    
    digitalWrite(PIN_AUTOPLAY, terrainLower[HERO_HORIZONTAL_POSITION + 2] == sprite[8] ? HIGH : LOW);
  }
  delay(frame);
}



void level3(int frame) {
  if (count3 == 119) count3 = 0;

  for (int i = 0; i < 16; ++i) {
    char current = terrainLower[i];
    char next = (i == 15) ? teren3[count1++] : terrainLower[i+1];
    
    if (current == sprite[8])
      if (next == sprite[5])
        terrainLower[i] = sprite[6];
       else terrainLower[i] = sprite[8];

    if (current == sprite[5])
      if (next == sprite[8])
        terrainLower[i] = sprite[7];
       else terrainLower[i] = sprite[5];

    if (current == sprite[6])
        terrainLower[i] = sprite[5];

    if (current == sprite[7])
        terrainLower[i] = sprite[8];
  }
  
  if (buttonPushed) {
    if (heroPos <= heroPositions[2]) heroPos = heroPositions[3];
    buttonPushed = false;
  }  
  if (drawHero(heroPos, terrainUpper, terrainLower, timpEfectiv >> 3)) {
    //playing = false; // The hero collided with something. Too bad.
    heroPos = heroPositions[6];
    lifes--;
    int t = 0;
    do {
      t++;
      tone(BUZZER, 450);
      delay(200);
      noTone(BUZZER);
      delay(200);  
    } while (t < 2);
  } else {
    if (heroPos == heroPositions[2] || heroPos == heroPositions[10]) {
      heroPos = heroPositions[1];
    } else if ((heroPos >= heroPositions[5] && heroPos <= heroPositions[7]) && terrainLower[HERO_HORIZONTAL_POSITION] != sprite[8]) {
      heroPos = heroPositions[11];
    } else if (heroPos >= heroPositions[11] && terrainLower[HERO_HORIZONTAL_POSITION] == sprite[8]) {
      heroPos = heroPositions[7];
    } else if (heroPos == heroPositions[12]) {
      heroPos = heroPositions[11];
    } else {
      ++heroPos;
    }
    ++timpEfectiv;
    
    digitalWrite(PIN_AUTOPLAY, terrainLower[HERO_HORIZONTAL_POSITION + 2] == sprite[8] ? HIGH : LOW);
  }
  delay(frame);
}


void setup(){

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  
  pinMode(PIN_READWRITE, OUTPUT);
  digitalWrite(PIN_READWRITE, LOW);
  pinMode(PIN_CONTRAST, OUTPUT);
  digitalWrite(PIN_CONTRAST, LOW);
  pinMode(PIN_BUTTON, INPUT);
  digitalWrite(PIN_BUTTON, HIGH);
  pinMode(PIN_AUTOPLAY, OUTPUT);
  digitalWrite(PIN_AUTOPLAY, HIGH);
  
  // Digital pin 2 maps to interrupt 0
  attachInterrupt(0/*PIN_BUTTON*/, buttonPush, FALLING);
  
  initializeGraphics();
  
  lcd.begin(16, 2);
  lcd.setBacklightPin(3,POSITIVE); // BL, BL_POL
  lcd.setBacklight(HIGH);
}

//int dificulty = 0;

int intro() {

  int dificulty = 0;
  int time = 0;
  
  lcd.setCursor(0,0);
  lcd.print("   Welcome to   ");
  lcd.setCursor(0,1);
  lcd.print("     Fences     ");

  delay(5000);


  while (dificulty == 0) {
  lcd.setCursor(0,0);
  lcd.print("Chose dificulty");
  lcd.setCursor(0,1);
  lcd.print("      Easy      ");
  //dificulty = 1;

  time = 0;
  while (time < 2000) {
    delay(1);
    if (buttonPushed) {
      dificulty = 1;
      break;
    }
    time++;  
  }

  if (dificulty == 1)
    break;

  lcd.setCursor(0,0);
  lcd.print("Chose dificulty");
  lcd.setCursor(0,1);
  lcd.print("     Medium     ");
  //dificulty = 2;
  
  time = 0;
  while (time < 2000) {
    delay(1);
    if (buttonPushed) {
      dificulty = 2;
      break;
    }
    time++;  
  }

  if (dificulty == 2)
    break;

  lcd.setCursor(0,0);
  lcd.print("Chose dificulty");
  lcd.setCursor(0,1);
  lcd.print("      Hard      ");
  //dificulty = 3;
  
  time = 0;
  while (time < 2000) {
    delay(1);
    if (buttonPushed) {
      dificulty = 3;
      break;
    }
    time++;  
  }

  if (dificulty == 3)
    break;
  
  }
  if (dificulty == 1) frame = 400;
  if (dificulty == 2) frame = 200;
  if (dificulty == 3) frame = 100;
  
  return dificulty;
}

void loop(){

  static bool blink = false;

  if (dificulty == 0)
    dificulty = intro();
  if (lifes == 3) {
    digitalWrite(LED1, HIGH);    // turn on LED1
    digitalWrite(LED2, HIGH);    // turn on LED2     
    digitalWrite(LED3, HIGH);    // turn on LED3
  }

  if (lifes == 2) {
    digitalWrite(LED1, LOW);    // turn on LED1
    digitalWrite(LED2, HIGH);    // turn on LED2     
    digitalWrite(LED3, HIGH);    // turn on LED3
  }

  if (lifes == 1) {
    digitalWrite(LED1, LOW);    // turn on LED1
    digitalWrite(LED2, LOW);    // turn on LED2     
    digitalWrite(LED3, HIGH);    // turn on LED3
  }

  if (lifes == 0) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);   
    digitalWrite(LED3, LOW);
    if(!buttonPushed) {
      lcd.setCursor(0,0);
      lcd.print("GAME OVER       ");
      lcd.setCursor(0,1);
      lcd.print("                ");
    }
    else {
      initializeGraphics();
      heroPos = heroPositions[1];
      playing = true;
      buttonPushed = true;
      timpEfectiv = 0;
    }
    return;
  }
  

  if (currentLevel == 1) {
    level1(frame);
    if (totalScore == 20) {
      //totalScore = 0;
      currentLevel = 2;
      lcd.setCursor(0,0);
      lcd.print("Ready for the   ");
      lcd.setCursor(0,1);
      lcd.print("Second Level... ");
      delay(5000);  
    }
  }

  if (currentLevel == 2) {
    level2(frame);
    if (totalScore == 10) {
      //totalScore = 0;
      currentLevel = 3;
      lcd.setCursor(0,0);
      lcd.print("Ready for the   ");
      lcd.setCursor(0,1);
      lcd.print("Third Level...  ");
      delay(5000);
    }
  }

  if (currentLevel == 3) {
    level3(frame);
    if (totalScore == 15) {
      //totalScore = 0;
      currentLevel = 1;
      lcd.setCursor(0,0);
      lcd.print("You Win!!!");
      lcd.setCursor(0,1);
      lcd.print("                ");
      isWinning = true;
      playing = false;
      delay(1000);
      return;
    }
  }

  if (isWinning == true)
    return;
}
