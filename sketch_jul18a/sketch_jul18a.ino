void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("Hallo Welt");

    int x,y;
    float z;

    x = 1;
    y = x / 2;
    z = x / 2;
    z = (float) x / 2.0;
}

void loop() {
  // put your main code here, to run repeatedly:
    //Serial.println("Hallo Welt in Loop");
}
