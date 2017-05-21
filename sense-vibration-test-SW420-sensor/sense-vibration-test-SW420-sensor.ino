int EP = 9;

void setup() {
  pinMode(EP, INPUT); // Set EP input for measurement
  Serial.begin(9600); // Init serial 9600
  Serial.println("Vibration demo");
}
void loop() {
  long measurement = TP_init();
  delay(50);
  Serial.print("measurement = ");
  Serial.println(measurement);
  if (measurement > 1000) {
    Serial.println("Sensing vibration");
  }
  else {
    Serial.println("No vibration");
  }
}

long TP_init() {
  delay(10);
  // Wait for the pin to get HIGH and returns measurement
  long measurement = pulseIn (EP, HIGH);
  return measurement;
}
