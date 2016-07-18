#include <HardwareSerial.h>
#include <Arduino.h>
#include <LiquidCrystal/LiquidCrystal.h>

String text[4] = {
        "Hallo",
        "Welt!",
        "Es ist",
        "%temp °C."
};
int currLine = -1;
boolean changable = true;

boolean startsWith(String pre, String str) {
    boolean startsWith = true;
    if(pre.length() <= str.length() && str.length() != 0)
        for(int i = 0; i < pre.length(); i++) {
            if(pre[i] != str[i]) {
                startsWith = false;
                break;
            }
        }
    return startsWith;
}

void printToLCD(LiquidCrystal lcd, String text) {
    if(startsWith("%temp", text))  {
        int wert = (int) ((float) analogRead(A5) * (float) (5.0/1024.0) * 100.0) + .5;
        lcd.print(wert);
        lcd.print(" ");
        lcd.print((char)223);
        lcd.print("C");
    } else {
        lcd.print(text);
    }
}

void setup() {
    // put your setup code here, to run once:
    LiquidCrystal lcd(2, 3, 7, 6, 5, 4);
    lcd.begin(16,2);
    lcd.print("Start....");
    Serial.begin(9600);
    delay(2000);
    pinMode(13, INPUT);
    digitalWrite(13, HIGH);
    pinMode(A5, INPUT);
}

void loop() {
    delay(20);
    if(digitalRead(13) == LOW) {
        if(changable) {
            currLine = (currLine+1) % (text->length()-1);
            LiquidCrystal lcd(2, 3, 7, 6, 5, 4);
            lcd.begin(16,2);
            lcd.clear();
            changable = false;
            printToLCD(lcd, text[currLine]);
            Serial.println(text->length()-1);
            if(currLine+1 < text->length()-1) {
                lcd.setCursor(0,1);
                printToLCD(lcd, text[currLine+1]);
            } else {
                printToLCD(lcd, " ");
            }
        }
    } else {
        changable = true;
    }
}

int precentToRate(int percent) {
    return percent * 255 / 100;
}