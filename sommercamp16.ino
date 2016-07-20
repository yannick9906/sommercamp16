#include <Adafruit_HMC5883_U.h>

#include <Adafruit_Sensor.h>

#include <Wire.h>
#include <assert.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

//Display
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(80);

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

enum displayableModes {M_PLAY, M_CREDITS, M_RESTART};
enum modes {MENU, GAME, CREDITS, BREAK, START};

int currModeSel = M_PLAY;
int leftBorderPosition = 0;
int rightBorderPosition = 7;
int playerPosition = 4;
int currMode = START;
long buttonPressedTime = 0;
bool buttonPressed = false;
long lastStepTime = 0;
int barLength = 1;

const char lightSensor = A3;
const int tasterPin = 13;

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

float getSensorData() {
  /* Get a new sensor event */ 
  sensors_event_t event;
  mag.getEvent(&event);
  // Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  float heading = atan2(event.magnetic.y, event.magnetic.x);

  float declinationAngle = 0.0593412;
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180/M_PI; 

  return headingDegrees;
}

void displayText(String text) {
    //Zeigt einen Text auf dem Display
    matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix.setTextSize(1);
    matrix.setTextColor(LED_GREEN);
    matrix.setRotation(3);
    for (int8_t x = 7; x >= -36; x--) {
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
    if(newLBorder < 0 || newRBorder > 7){
        Serial.println("---> Recursion!");
        return generateNewMapLine();
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

void updateMap(){

}

void displayMap(){
    matrix.clear();
    matrix.drawBitmap(0, 0, mapArray, 8, 8, LED_GREEN);
    matrix.writeDisplay();
    delay(500);
}

void startUp() {
    //Startet ein neues Spiel
    displayText("Heyho!");
    preGenerateMap();
    currMode = MENU;
}

bool isLight(){
  /*//Gibt zurueck ob Helligkeitssensor aktiv
  int light = analogRead(lightSensor);
  Serial.println(light);
  if(light > 0){
    return true;
  }*/
  return false;
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

void displaySensorDetails(){
    sensor_t sensor;
    mag.getSensor(&sensor);
    // Get a new sensor event
    sensors_event_t event;
    mag.getEvent(&event);
    Serial.println("------------------------------------");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");
    Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");
    Serial.println("------------------------------------");
    Serial.println("");
    delay(500);
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
    updateMap();
    displayMap();
    if(isLight()) currMode = BREAK;
    if(leftBorderPosition >= playerPosition >= rightBorderPosition)
        currMode = MENU;
}

void loopBreak() {
    matrix.clear();
    matrix.drawBitmap(0, 0, menu_pause, 8, 8, LED_YELLOW);
    matrix.writeDisplay();
    if(!isLight()) currMode = GAME;
}


void setup() {
    //Init
    Serial.begin(9600);
    //Tasterinitialisierung
    pinMode(tasterPin, INPUT);
    digitalWrite(tasterPin, HIGH);

    Serial.println("Our awesome Racinggame"); Serial.println("");
    /* Initialise the sensor */
    /*if(!mag.begin()) {
        // There was a problem detecting the HMC5883 ... check your connections
        Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
        while(1);
    }*/
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
    displaySensorDetails();
}


