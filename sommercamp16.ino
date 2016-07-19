#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

//Display
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);


char mode = "START";
enum displayableModes {"GAME", "CREDITS", "RESTART"};
int displayedMode = 0;
int leftBorderPosition = 0;
int rightBorderPosition = 7;
int playerPosition = 4;
bool inMenu = false;
bool inGame = false;
bool inCredits = false;
long buttonPressedTime = 0;
bool buttonPressed = false;

const char lightSensor = "A3"
const int tasterPin = 13;

//Map
static const uint8_t PROGMEM mapArray[7] ={B10000001};

boolean buttonShortPressed(){
  //Gibt zurueck, ob der Knopf kurz gedrueckt wurde
  bool pressed = digitalRead(tasterPin) == LOW;
  if(pressed && !buttonPressed){
    buttonPressedTime = millis();
    buttonPressed = true;
    return false;
  }else if(!pressed && buttonPressed){
    buttonPressed = false;
    if((buttonPressedTime - millis())<100){
      return true;
    }
  }
}

boolean buttonLongPressed(){
  //Gibt zurueck, ob der Knopf lang gedrueckt wurde
  bool pressed = digitalRead(tasterPin) == LOW;
  if(pressed && !buttonPressed){
    buttonPressedTime = millis();
    buttonPressed = true;
    return false;
  }else if(!pressed && buttonPressed){
    buttonPressed = false;
    if((buttonPressedTime - millis())>100){
      return true;
    }
  }
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

int[] getSensorData(){
  /* Get a new sensor event */ 
  sensors_event_t event; 
  mag.getEvent(&event);
  // Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  
  // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
  // Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
  // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
  float declinationAngle = 0.22;
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180/M_PI; 
  
  return headingDegrees
}

void displayText(char text){
  //Zeigt einen Text auf dem Display  
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextSize(1);
  matrix.setTextColor(LED_GREEN);
  for (int8_t x=7; x>=-36; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print(text);
    matrix.writeDisplay();
    delay(2000);
}

int generateNewMapLine(){
  chance = randint(2);
  int newLBorder, newRBorder;
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
    newRBorder = newLBorder + 4;
    if(newLBorder < 0 || newRBorder > 7){
      return generateNewMapLine()
    }
    static const uint8_t PROGMEM temp = {B00000000}
    for ( i=0; i<8; i++ ){
      if(i == newLBorder || i == newRBorder){
        temp ^= (-x ^ temp) & (1 << n);
      }
  }
}

void preGenerateMap(){
  //generiert den Anfang der Map
  static const uint8_t PROGMEM 
  temp ={B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001};
  for ( i=0; i<7; i++ ){
    temp[i] = generateNewMapLine()
  }
  mapArray = temp
}

void updateMap(){
  
}

void displayMap(){
  matrix.clear();
  matrix.drawBitmap(0, 0, mapArray, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
}

void startUp(){
  //Startet ein neues Spiel
  displayText("Willkommen!");
  inMenu = true;
  mode = "MENU";
  preGenerateMap();
}

void reset(){
  //Resettet alle Variablen auf die Startwerte
  mode = "START";
  leftBorderPosition = 0;
  rightBorderPosition = 7;
  playerPosition = 4;
  inMenu = false;
  inGame = false;
  startUp();
}

boolean isLight(){
  //Gibt zurueck ob Helligkeitssensor aktiv
  int light = analogRead(lightSensor);
  Serial.println(light);
  if(light > 0){
    return true;
  }
  return false;
}

void displaySensorDetails(void){
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}


void setup() {
  //Init
  Serial.begin(9600);
  //Tasterinitialisierung
  pinMode(tasterPin, INPUT);
  digitalWrite(tasterPin, HIGH);
  //Magnetometerinitialisierung
  Serial.println("HMC5883 Magnetometer Test"); Serial.println("");
  /* Initialise the sensor */
  if(!mag.begin()){
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }
  //Displayinitialisierung
  matrix.begin(0x70);
  // Yannick macht hier die Init rein
  startUp();
}

void loop() {
  //Unterbricht das Spiel, falls der Helligkeitssensor nicht abgedeckt ist
  if(inGame){
    if(isLight()){
      mode = "BREAK";
    }else{
      mode = "GAME";
    }
  }
  //Spielt die einzelnen Modi ab
  switch(mode){
    case "MENU":
      //Zeigt das Menu
      displayText(displayableModes[displayedMode]);
      break;
    case "BREAK":
      //Zeigt Pause
      displayText("PAUSE");
      break;
    case "GAME":
      //Gameschleife
      displayMap();
      updateMap();
      if(leftBorderPosition>=playerPosition>=rightBorderPosition){
        currMode = MENU;
      }
      break;
    case "CREDITS":
      //Abspielen der Credits
      inCredits = true;
      displayText("Credits");
      inCredits = false;
      mode = "MENU";
      break;
    //Startet das System neu
    case "RESTART":
      reset()
      break;
    default:
      //Falls unbekannter Modus ausgewaehlt ist, zurueck ins Menu
      mode = "MENU";
      break;
    }
   //Spiel abbrechen, falls der Knopf lange gedrueckt wurde.
   if(inGame && buttonLongPressed()){
    mode = "MENU";
    inGame = false;
    inMenu = true;
   }
   //Angezeigten Modus durchwechseln
   if(inMenu && buttonShortPressed()){
    if(displayedMode++ < displayableModes.length()-1){
      displayedMode++;
    }else{
      displayedMode = 0;
    }
   }
   //Angezeigten Modus auswaehlen
   if(inMenu && butttonLongPressed()){
    mode = displayableModes[displayedMode]
   }
}


