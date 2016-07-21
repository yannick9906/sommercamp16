
#include <Wire.h>
#include <assert.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

//Display
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

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

static uint8_t mapArray[7] ={B10000001};
byte mapbuffer[7];

enum displayableModes {M_PLAY, M_CREDITS, M_RESTART};
enum modes {MENU, GAME, CREDITS, BREAK, START};
enum sides {LEFT, RIGHT};

int currModeSel = M_PLAY;
int leftBorderPosition = 0;
int rightBorderPosition = 7;
int playerPosition = 4;
int currMode = START;
long buttonPressedTime = 0;
bool buttonPressed = false;
long lastStepTime = 0;
long lastMapStepTime = 0;
int barLength = 1;

const int tasterPin = 13;
const int leftSonarTrig = 11, leftSonarEcho = 12;
const int rightSonarTrig = 9, rightSonarEcho = 10;

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

bool buttonShortPressed(){
    bool pressed = digitalRead(tasterPin) == LOW;

    if(pressed && !buttonPressed) {
        buttonPressed = true;
        buttonPressedTime = millis();
        return false;
    } else if(!pressed && buttonPressed) {
        Serial.println((millis() - buttonPressedTime));
        if((millis() - buttonPressedTime) <= 500)  {
            buttonPressed = false;
            return true;
        } else return false;
    }
    return false;
}

bool buttonLongPressed(){
    bool pressed = digitalRead(tasterPin) == LOW;

    if(pressed && !buttonPressed) {
        buttonPressed = true;
        buttonPressedTime = millis();
        return false;
    } else if(!pressed && buttonPressed) {
        Serial.println((millis() - buttonPressedTime));
        if((millis() - buttonPressedTime) > 500)  {
            buttonPressed = false;
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

int generateNewMapLine() {
    int chance = randint(3);
    int newLBorder, newRBorder;
    Serial.print("Randint: "); Serial.println(chance);
    switch(chance){
        case 0:
            newLBorder = leftBorderPosition--;
            break;
        case 1:
            newLBorder = leftBorderPosition;
            break;
        case 2:
            newLBorder = leftBorderPosition++;
            break;
    }
    Serial.println("---> Endswitch");
    newRBorder = newLBorder + 4;
    if (newRBorder > 7) {
        newLBorder--;
        newRBorder--;
    } else if (newLBorder < 0) {
        newLBorder++;
        newRBorder++;
    }
    static uint8_t temp = {B00000000};
    Serial.println("---> Startfor");
    for (int i=0; i<8; i++ ){
        Serial.print("---> Forstep i=");
        Serial.println(i);
        if(currMode == START) loading();
        if(i == newLBorder || i == newRBorder){
            temp ^= 1 << i;
        }
    }
    return temp;
}

void preGenerateMap(){
    //generiert den Anfang der Map
    Serial.println("Startgen");
    for (int i=0; i<7; i++){
        mapArray[i] = (uint8_t) generateNewMapLine();
    }
    Serial.println("Endgen.");
}

int limitInt(int limit, int value) {
    if(value >= limit) return limit;
    else return value;
}

void updateDistances(int left, int right){
    matrix.setRotation(3);
    int lineLengthLeft = limitInt(4, (int) ((left/10.0)+.5));
    int lineLengthRight = limitInt(4, (int) ((right/10.0)+.5));
    //Serial.println("updateSensor: ");
    //Serial.print("Distance: "); Serial.print(left); Serial.print(" | "); Serial.println(right);
    //Serial.print("Length:   "); Serial.print(lineLengthLeft); Serial.print(" | "); Serial.println(lineLengthRight);
    matrix.drawLine(-1,0,lineLengthLeft,0, LED_YELLOW);
    matrix.drawLine(7-lineLengthRight,0,7,0, LED_YELLOW);
    matrix.writeDisplay();
    matrix.setRotation(0);
}

void updatePlayer(int left, int right) {
    if((millis() - lastStepTime) >= 250) {
        int lineLengthLeft = limitInt(4, (int) ((left/10.0)+.5));
        int lineLengthRight = limitInt(4, (int) ((right/10.0)+.5));
        int heightDifference = lineLengthRight - lineLengthLeft;
        Serial.print("Difference: ");
        Serial.println(heightDifference);

        playerPosition = 4-heightDifference;
        lastStepTime = millis();
    }
    matrix.setRotation(3);
    matrix.drawPixel(playerPosition, 7, LED_RED);
    matrix.writeDisplay();
    matrix.setRotation(0);
}

void updateMap(){
    if((millis() - lastMapStepTime) >= 250) {
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
    }
}

void displayMap(){
    matrix.setRotation(3);
    /*for(int i=0; i<7; i++)
        for(int j=0; j<8; j++)
            if(CHECK_BIT(mapbuffer[i],j))
                matrix.drawPixel(j,i,LED_GREEN);*/
    uint8_t PROGMEM bitmap[7];
    for (int i = 0; i < 7; i++) {
        bitmap[i] = mapbuffer[i];
    }
    matrix.drawBitmap(0,1,bitmap, 8,7,LED_GREEN);
    matrix.writeDisplay();
    matrix.setRotation(0);
}

void startUp() {
    //Startet ein neues Spiel
    //displayText("Heyho!", 36);
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
    leftBorderPosition = 0;
    rightBorderPosition = 7;
    playerPosition = 4;
    startUp();
}

void loopMenu() {
    if(buttonShortPressed()) {
        currModeSel = (currModeSel+1) % 3;
    }
    if(buttonLongPressed()) {
        switch(currModeSel) {
            case M_PLAY:
                currMode = GAME;
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
}

void loopGame() {
    matrix.clear();
    int leftDistance  = (int) getSonarData(LEFT);
    int rightDistance = (int) getSonarData(RIGHT);
    updateDistances(leftDistance, rightDistance);
    updatePlayer(leftDistance, rightDistance);
    updateMap();
    displayMap();
    if(buttonShortPressed()) currMode = BREAK;
    if(buttonLongPressed()) currMode = MENU;
    //if(leftBorderPosition >= playerPosition >= rightBorderPosition)
    //    currMode = MENU;
}

void loopBreak() {
    if(buttonShortPressed()) currMode = GAME;
    if(buttonLongPressed()) currMode = MENU;
    matrix.clear();
    matrix.drawBitmap(0, 0, menu_pause, 8, 8, LED_YELLOW);
    matrix.writeDisplay();
}


void setup() {
    //Init
    Serial.begin(9600);
    //Tasterinitialisierung
    pinMode(tasterPin, INPUT);
    digitalWrite(tasterPin, HIGH);

    Serial.println("Our awesome Racinggame"); Serial.println("");
    //Displayinitialisierung
    matrix.begin(0x70);
    startUp();
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


