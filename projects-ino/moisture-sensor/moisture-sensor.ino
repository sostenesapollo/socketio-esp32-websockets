void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorVal = analogRead(2);
  
  int percentage = map(sensorVal, 352, 637, 100, 0);

  Serial.println(String(percentage)+"%");
  delay(500);
}
