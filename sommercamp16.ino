
#include <Wire.h>
#include <EEPROM.h>
#include <assert.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

//Pins
#define goLeftPin 7
#define goRightPin 6
#define tasterPin 13

#define leftSonarTrig 11

#define leftSonarEcho 12
#define rightSonarTrig 9

#define rightSonarEcho 10

#define buzzerPin 8

/*************************************************
 * Noten
 *************************************************/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978


//Display
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

static const uint8_t PROGMEM
        menu_0[] = {
            B00011000,
            B00000000,
            B00000000,
            B00000000,
            B00000000,
            B00000000,
            B00000000,
            B00011000 },
        menu_1[] = {
            B00000000,
            B00000000,
            B00011000,
            B00111100,
            B00111100,
            B01111110,
            B00000000,
            B00000000 },
        menu_2[] = {
            B00000000,
            B00000000,
            B01010101,
            B01010101,
            B01010101,
            B01010101,
            B00000000,
            B00000000 },
        menu_3[] = {
            B00000000,
            B00011110,
            B00000010,
            B01000010,
            B11100010,
            B01000010,
            B01111110,
            B00000000 },
        menu_pause[] = {
            B00000000,
            B01111110,
            B01111110,
            B00000000,
            B00000000,
            B01111110,
            B01111110,
            B00000000 };

//Own Song - ImperialMarch
int starwars_notes[] = {
        NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
        NOTE_AS3, NOTE_AS4, 0,
        0,
        NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
        NOTE_AS3, NOTE_AS4, 0,
        0,
        NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
        NOTE_DS3, NOTE_DS4, 0,
        0,
        NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
        NOTE_DS3, NOTE_DS4, 0,
        0, NOTE_DS4, NOTE_CS4, NOTE_D4,
        NOTE_CS4, NOTE_DS4,
        NOTE_DS4, NOTE_GS3,
        NOTE_G3, NOTE_CS4,
        NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
        NOTE_GS4, NOTE_DS4, NOTE_B3,
        NOTE_AS3, NOTE_A3, NOTE_GS3,
        0, 0, 0,
        ////
        ////
        NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5,
        NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4,
        NOTE_E5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_C5,
        NOTE_GS4, NOTE_F4, NOTE_C5, NOTE_A4,
        NOTE_A5, NOTE_A4, NOTE_A4, NOTE_A5, NOTE_GS5, NOTE_GS5,
        NOTE_FS5, NOTE_F5, NOTE_FS5, 0, NOTE_AS4, NOTE_DS4, NOTE_D5, NOTE_CS5,
        //
        NOTE_C5, NOTE_B4, NOTE_C5, 0, NOTE_F5,NOTE_GS4, NOTE_F4, NOTE_A4,
        NOTE_C5, NOTE_A4, NOTE_C5, NOTE_E5,
        NOTE_A5, NOTE_A4, NOTE_A4, NOTE_A5, NOTE_GS5, NOTE_G5,
        NOTE_FS5, NOTE_F5, NOTE_F5, 0, NOTE_AS4, NOTE_DS5, NOTE_D5, NOTE_CS5,
        NOTE_C5, NOTE_B4, 0, NOTE_F4, NOTE_GS4, NOTE_F4, NOTE_C5,
        NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4,
        //
        NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5,
        NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4,
        NOTE_E5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_C5,
        NOTE_GS4, NOTE_F4, NOTE_C5, NOTE_A4,
        NOTE_A5, NOTE_A4, NOTE_A4, NOTE_A5, NOTE_GS5, NOTE_GS5,
        NOTE_FS5, NOTE_F5, NOTE_FS5, 0, NOTE_AS4, NOTE_DS4, NOTE_D5, NOTE_CS5,
        //
        NOTE_C5, NOTE_B4, NOTE_C5, 0, NOTE_F4, NOTE_A4, NOTE_F4, NOTE_C5,
        NOTE_GS4, NOTE_F4, NOTE_C5, NOTE_A4,
        ////
        ////
        NOTE_E7, NOTE_E7, 0, NOTE_E7,
        0, NOTE_C7, NOTE_E7, 0,
        NOTE_G7, 0, 0,  0,
        NOTE_G6, 0, 0, 0,

        NOTE_C7, 0, 0, NOTE_G6,
        0, 0, NOTE_E6, 0,
        0, NOTE_A6, 0, NOTE_B6,
        0, NOTE_AS6, NOTE_A6, 0,

        NOTE_G6, NOTE_E7, NOTE_G7,
        NOTE_A7, 0, NOTE_F7, NOTE_G7,
        0, NOTE_E7, 0, NOTE_C7,
        NOTE_D7, NOTE_B6, 0, 0,

        NOTE_C7, 0, 0, NOTE_G6,
        0, 0, NOTE_E6, 0,
        0, NOTE_A6, 0, NOTE_B6,
        0, NOTE_AS6, NOTE_A6, 0,

        NOTE_G6, NOTE_E7, NOTE_G7,
        NOTE_A7, 0, NOTE_F7, NOTE_G7,
        0, NOTE_E7, 0, NOTE_C7,
        NOTE_D7, NOTE_B6, 0, 0
};

int starwars_tempo[] = {
        12, 12, 12, 12,
        12, 12, 6,
        3,
        12, 12, 12, 12,
        12, 12, 6,
        3,
        12, 12, 12, 12,
        12, 12, 6,
        3,
        12, 12, 12, 12,
        12, 12, 6,
        6, 18, 18, 18,
        6, 6,
        6, 6,
        6, 6,
        18, 18, 18, 18, 18, 18,
        10, 10, 10,
        10, 10, 10,
        3, 3, 3,
        ////
        ////
        4, 4, 4, 6, 16,
        4, 6, 16, 2,
        4, 4, 4, 6, 16,
        4, 6, 16, 2,
        4, 6, 16, 4, 6, 16,
        16, 16, 8, 8, 8, 4, 6, 16,
        //
        16,16,8,8,8,4,6,16,
        4,6,16,2,
        4,6,16,4,6,16,
        16,16,8,8,8,4,6,16,
        16,16,8,8,8,4,6,16,
        4,6,16,2,
        //
        4, 4, 4, 6, 16,
        4, 6, 16, 2,
        4, 4, 4, 6, 16,
        4, 6, 16, 2,
        4, 6, 16, 4, 6, 16,
        16, 16, 8, 8, 8, 4, 6, 16,
        //
        16,16,8,8,8,4,6,16,
        4,6,16,2,
        ////
        ////
        12, 12, 12, 12,
        12, 12, 12, 12,
        12, 12, 12, 12,
        12, 12, 12, 12,

        12, 12, 12, 12,
        12, 12, 12, 12,
        12, 12, 12, 12,
        12, 12, 12, 12,

        9, 9, 9,
        12, 12, 12, 12,
        12, 12, 12, 12,
        12, 12, 12, 12,

        12, 12, 12, 12,
        12, 12, 12, 12,
        12, 12, 12, 12,
        12, 12, 12, 12,

        9, 9, 9,
        12, 12, 12, 12,
        12, 12, 12, 12,
        12, 12, 12, 12,
};

static uint8_t mapArray[7] ={B10000001};
byte mapbuffer[7];

enum displayableModes {M_PLAY, M_CREDITS, M_RESTART};
enum modes {MENU, GAME, CREDITS, BREAK, START};
enum sides {LEFT, RIGHT};
enum move {TOLEFT, STAY, TORIGHT};

int currModeSel = M_PLAY;
int leftBorderPosition = 2;
int rightBorderPosition = 6;
int bottomLeftBorderPosition = 0;
int bottomRightBorderPosition = 7;
int playerPosition = 4;
int currMode = START;
long buttonPressedTime[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool buttonPressed[] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false};
long lastStepTime = 0;
long lastMapStepTime = 0;
int barLength = 1;

long lastStepPlayedTime = 0;
long currentDelay = 0;
long currentNote = 0;

long timeGameStart = 0;
bool gameTasterMode = false;

long sonarLeftData = 0, sonarLeftCount = 0;
long sonarRightData = 0, sonarRightCount = 0;

long microsecondsToCentimeters(long microseconds) {
    // The speed of sound is 340 m/s or 29 microseconds per centimeter.
    // The ping travels out and back, so to find the distance of the
    // object we take half of the distance travelled.
    return microseconds / 29 / 2;
}

long getSonarData(int side) {
    int trig, echo;
    if(side == LEFT) {
        trig = leftSonarTrig;
        echo = leftSonarEcho;
    } else if(side == RIGHT) {
        trig = rightSonarTrig;
        echo = rightSonarEcho;
    } else return -1;

    pinMode(trig, OUTPUT);
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(5);
    digitalWrite(trig, LOW);

    pinMode(echo,INPUT);
    long duration = pulseIn(echo, HIGH);
    long distance = microsecondsToCentimeters(duration);

    /*Serial.print("Sonardata: "); Serial.println(side);
    Serial.print("Duration:  "); Serial.println(duration);
    Serial.print("Distance:  "); Serial.println(distance);
    Serial.println("-------------------");
    Serial.println();*/
    return distance;
}

void doSensorData(int side) {
    if(side == LEFT) {
        sonarLeftCount++;
        sonarLeftData += getSonarData(side);
    } else if(side == RIGHT) {
        sonarRightCount++;
        sonarRightData += getSonarData(side);
    }
}

int getSonarBetterData(int side) {
    if(side == LEFT) {
        int data = (int) (sonarLeftData / sonarLeftCount);
        sonarLeftCount = 0;
        sonarLeftData = 0;
        return data;
    } else if(side == RIGHT) {
        int data = (int) (sonarRightData / sonarRightCount);
        sonarRightCount = 0;
        sonarRightData = 0;
        return data;
    }
}

bool buttonShortPressed(int taster){
    bool pressed = digitalRead(taster) == LOW;

    if(pressed && !buttonPressed[taster]) {
        buttonPressed[taster] = true;
        buttonPressedTime[taster] = millis();
        return false;
    } else if(!pressed && buttonPressed[taster]) {
        Serial.println((millis() - buttonPressedTime[taster]));
        if((millis() - buttonPressedTime[taster]) <= 500)  {
            buttonPressed[taster] = false;
            return true;
        } else return false;
    }
    return false;
}

bool buttonLongPressed(int taster){
    bool pressed = digitalRead(taster) == LOW;

    if(pressed && !buttonPressed[taster]) {
        buttonPressed[taster] = true;
        buttonPressedTime[taster] = millis();
        return false;
    } else if(!pressed && buttonPressed[taster]) {
        Serial.println((millis() - buttonPressedTime[taster]));
        if((millis() - buttonPressedTime[taster]) > 500)  {
            buttonPressed[taster] = false;
            return true;
        } else return false;
    }
    return false;
}

/* Returns an integer in the range [0, n).
 *
 * Uses rand(), and so is affected-by/affects the same seed.
 */
int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Chop off all of the values that would cause skew...
    long end = RAND_MAX / n; // truncate skew
    assert (end > 0L);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}

void displayText(String text, int pixelLength) {
    //Zeigt einen Text auf dem Display
    matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix.setTextSize(1);
    matrix.setTextColor(LED_GREEN);
    matrix.setRotation(3);
    int length = pixelLength * -1;
    for (int8_t x = 7; x >= length; x--) {
        matrix.clear();
        matrix.setCursor(x, 0);
        matrix.print(text);
        matrix.writeDisplay();
        delay(100);
        if(buttonShortPressed(tasterPin)) break;
    }
    matrix.setRotation(0);
}

void loading() {
    matrix.clear();

    /*Serial.print("Zeit: ");
    Serial.print(millis() - lastStepTime);
    Serial.print("Length: ");
    Serial.println(barLength);*/
    if((millis() - lastStepTime) >= 100) {
        barLength = (barLength + 1) % 8;
        lastStepTime = millis();
    }
    matrix.setRotation(3);
    matrix.drawLine(0, 6, barLength, 6, LED_GREEN);
    matrix.writeDisplay();
    matrix.setRotation(0);
}

byte generateNewMapLine() {
    int side = randint(3);
    int leftBorder = leftBorderPosition;
    int rightBorder = rightBorderPosition;
    switch(side) {
        case TOLEFT:
            if(leftBorder-1 >= 0) {
                leftBorder--;
                rightBorder--;
            }
            break;
        case STAY:
            break;
        case TORIGHT:
            if(rightBorder+1 <= 7) {
                leftBorder++;
                rightBorder++;
            }
            break;
    }
    byte thisline = B00000000;
    thisline |= 1 << leftBorder;
    thisline |= 1 << rightBorder;

    leftBorderPosition = leftBorder;
    rightBorderPosition = rightBorder;
    Serial.print("Mapgen: "); Serial.println(thisline, BIN);
    return thisline;
}

void preGenerateMap(){
    //generiert den Anfang der Map
    Serial.println("Startgen");
    for(int i=0;i<7;i++) {
        loading();
        mapbuffer[i] = B10000001;
        delay(100);
    }
    Serial.println("Endgen.");
}

int limitInt(int limit, int value) {
    if(value >= limit) return limit;
    else return value;
}

void updateDistances(int left, int right){
    if(gameTasterMode) {
        matrix.setRotation(3);
        matrix.drawLine(0,0,8,0, LED_RED);
        matrix.setRotation(0);
    } else {
        matrix.setRotation(3);
        int lineLengthLeft = limitInt(4, (int) ((left / 10.0) + .5));
        int lineLengthRight = limitInt(4, (int) ((right / 10.0) + .5));
        //Serial.println("updateSensor: ");
        //Serial.print("Distance: "); Serial.print(left); Serial.print(" | "); Serial.println(right);
        //Serial.print("Length:   "); Serial.print(lineLengthLeft); Serial.print(" | "); Serial.println(lineLengthRight);
        matrix.drawLine(-1, 0, lineLengthLeft, 0, LED_YELLOW);
        matrix.drawLine(7 - lineLengthRight, 0, 7, 0, LED_YELLOW);
        matrix.setRotation(0);
    }
}

void updatePlayer() {
    if(gameTasterMode) {
        if(buttonShortPressed(goLeftPin) && playerPosition-1 >= 0) playerPosition--;
        if(buttonShortPressed(goRightPin) && playerPosition+1 <= 7) playerPosition++;
    } else if((millis() - lastStepTime) >= 250) {
        int lineLengthLeft = limitInt(4, (int) ((getSonarBetterData(LEFT)/10.0)+.5));
        int lineLengthRight = limitInt(4, (int) ((getSonarBetterData(RIGHT)/10.0)+.5));
        int heightDifference = lineLengthRight - lineLengthLeft;
        //Serial.print("Difference: "); Serial.println(heightDifference);

        playerPosition = 4-heightDifference;
        lastStepTime = millis();
    }
    matrix.setRotation(3);
    matrix.drawPixel(playerPosition, 7, LED_RED);
    matrix.setRotation(0);
}

void updateMap(){
    int time = 500;
    if(gameTasterMode) time = 100;
    if((millis() - lastMapStepTime) >= time) {
        //Shift map down
        byte temp[7];
        for (int i = 0; i < 6; i++) {
            temp[i] = mapbuffer[i + 1];
        }
        temp[6] = (byte) generateNewMapLine();

        //Write to map buffer
        for (int i = 0; i < 7; i++) {
            mapbuffer[i] = temp[i];
        }
        lastMapStepTime = millis();

        bool first = false;
        for(int i=0;i<8;i++) {
            if(CHECK_BIT(mapbuffer[0], i))
                if(!first) {
                    first = true;
                    bottomLeftBorderPosition = i;
                } else {
                    bottomRightBorderPosition = i;
                    return;
                }
        }
        bottomLeftBorderPosition = 0;
        bottomRightBorderPosition = 7;
    }
}

void displayMap(){
    matrix.setRotation(1);
    uint8_t PROGMEM bitmap[7];
    for (int i = 0; i < 7; i++) {
        bitmap[i] = mapbuffer[i];
    }
    int score = (int) ((millis() - timeGameStart) / 1000 * 5);
    int highscore = 0;
    EEPROM.get(0x10, highscore);
    int color = LED_GREEN;
    if(score > highscore) {
        color = LED_YELLOW;
    }
    matrix.drawBitmap(0,0,bitmap, 8,7,color);
    matrix.setRotation(0);
}

void startUp() {
    //Startet ein neues Spiel
    displayText("Heyho!", 36);
    preGenerateMap();
    currMode = MENU;
    for(int i=0; i<7; i++) {
        mapbuffer[i] = mapArray[i];
    }
}

void reset(){
    //Resettet alle Variablen auf die Startwerte
    matrix.clear();
    matrix.drawRect(0,0,8,8, LED_RED);
    matrix.drawRect(1,1,6,6, LED_RED);
    matrix.drawRect(2,2,4,4, LED_RED);
    matrix.drawRect(3,3,2,2, LED_RED);
    matrix.writeDisplay();
    delay(500);

    barLength = 0;
    currMode = START;
    currModeSel = M_PLAY;
    leftBorderPosition = 2;
    rightBorderPosition = 6;
    bottomRightBorderPosition = 8;
    bottomLeftBorderPosition = 0;
    playerPosition = 4;
    startUp();
}

void buzz(int targetPin, long frequency, long length) {
    digitalWrite(buzzerPin, HIGH);
    long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
    long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
    for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
        digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
        delayMicroseconds(delayValue); // wait for the calculated delay value
        digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
        delayMicroseconds(delayValue); // wait again or the calculated delay value
    }
    digitalWrite(buzzerPin, LOW);

}

void playMusicStep() {
    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

    if ((millis() - lastStepPlayedTime) >= currentDelay) {
        int size = sizeof(starwars_notes) / sizeof(int);
        if (currentNote < size) {
            //digitalWrite(buzzerPin, LOW);
            currentNote++;
            int noteDuration = 1000 / starwars_tempo[currentNote];
            currentDelay = 1000000 / starwars_notes[currentNote] / 2;
            //tone(buzzerPin, starwars_notes[currentNote], noteDuration);
            buzz(buzzerPin, starwars_notes[currentNote], noteDuration);
            currentDelay = (long) (noteDuration * 1.30);
            lastStepPlayedTime = millis();
        } else {
            currentNote = 0;
        }
    } else {

    }
}

void loopMenu() {
    if(buttonShortPressed(tasterPin)) {
        currModeSel = (currModeSel+1) % 3;
    }
    if(buttonLongPressed(tasterPin)) {
        switch(currModeSel) {
            case M_PLAY:
                preGenerateMap();
                currMode = GAME;
                timeGameStart = millis();
                break;
            case M_CREDITS:
                currMode = CREDITS;
                displayText("Copyright", 52);
                displayText("Janik&Yannick", 75);
                displayText("Sommercamp 2016", 96);
                currMode = MENU;
                break;
            case M_RESTART:
                reset();
                break;
        }
    }

    switch(currModeSel) {
        case M_PLAY:
            matrix.clear();
            matrix.drawBitmap(0, 0, menu_0, 8, 8, LED_GREEN);
            matrix.drawBitmap(0, 0, menu_1, 8, 8, LED_RED);
            matrix.writeDisplay();
            break;
        case M_CREDITS:
            matrix.clear();
            matrix.drawBitmap(0, 0, menu_0, 8, 8, LED_GREEN);
            matrix.drawBitmap(0, 0, menu_2, 8, 8, LED_RED);
            matrix.writeDisplay();
            break;
        case M_RESTART:
            matrix.clear();
            matrix.drawBitmap(0, 0, menu_0, 8, 8, LED_GREEN);
            matrix.drawBitmap(0, 0, menu_3, 8, 8, LED_RED);
            matrix.writeDisplay();
            break;
    }
    playMusicStep();
}

void doDeath() {
    Serial.println("Death!");
    int score = (int) ((millis() - timeGameStart) / 1000 * 5);
    int highscore = 0;
    EEPROM.get(0x10, highscore);
    int color = LED_RED;
    if(score > highscore) {
        EEPROM.put(0x10, score);
        color = LED_YELLOW;
    }
    matrix.drawRect(3,3,2,2, color);
    matrix.writeDisplay();
    delay(500);
    matrix.drawRect(2,2,4,4, color);
    matrix.writeDisplay();
    delay(500);
    matrix.drawRect(1,1,6,6, color);
    matrix.writeDisplay();
    delay(500);
    matrix.drawRect(0,0,8,8, color);
    matrix.writeDisplay();
    delay(500);
    displayText("Score: ", 30);
    displayText(String(score), 45);
    currMode = MENU;
}

void loopGame() {
    matrix.clear();
    doSensorData(LEFT);
    doSensorData(RIGHT);
    int leftDistance  = (int) getSonarData(LEFT);
    int rightDistance = (int) getSonarData(RIGHT);
    updateDistances(leftDistance, rightDistance);
    updatePlayer();
    updateMap();
    displayMap();
    matrix.writeDisplay();
    if(buttonShortPressed(tasterPin)) currMode = BREAK;
    if(buttonLongPressed(tasterPin)) currMode = MENU;
    Serial.print("Pos: "); Serial.print(bottomLeftBorderPosition); Serial.print("|"); Serial.print(playerPosition); Serial.print("|");Serial.println(bottomRightBorderPosition);
    if(bottomLeftBorderPosition >= playerPosition || playerPosition >= bottomRightBorderPosition)
        doDeath();
}

void loopBreak() {
    if(buttonShortPressed(tasterPin)) currMode = GAME;
    if(buttonLongPressed(tasterPin)) {
        gameTasterMode = !gameTasterMode;
        EEPROM.put(0x20, gameTasterMode);
    }
    matrix.clear();
    matrix.drawBitmap(0, 0, menu_pause, 8, 8, LED_YELLOW);
    matrix.writeDisplay();
}


void setup() {
    //Init
    Serial.begin(9600);
    //Tasterinitialisierung
    pinMode(tasterPin, INPUT);
    pinMode(goLeftPin, INPUT);
    pinMode(goRightPin, INPUT);
    digitalWrite(tasterPin, HIGH);
    digitalWrite(goLeftPin, HIGH);
    digitalWrite(goRightPin, HIGH);
    pinMode(buzzerPin, OUTPUT);//buzzer

    Serial.println("Our awesome Racinggame"); Serial.println("");
    //Displayinitialisierung
    matrix.begin(0x70);
    startUp();
    EEPROM.get(0x20, gameTasterMode);
}

void loop() {
    switch(currMode) {
        case MENU:
            loopMenu();
            break;
        case GAME:
            loopGame();
            break;
        case BREAK:
            loopBreak();
            break;
    }
}


