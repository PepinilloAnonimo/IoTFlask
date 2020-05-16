void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  int opcion = Serial.read();
  Serial.print(opcion);
  if(opcion == 0){
      digitalWrite(LED_BUILTIN, LOW);
  } else if(opcion == 1){
      digitalWrite(LED_BUILTIN, HIGH);
  }
}
