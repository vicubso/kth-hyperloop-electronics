const int transistor = 2;

void setup()
{
  pinMode (transistor, OUTPUT);
}

void loop()
{
  digitalWrite (transistor, HIGH);
  delay(200);
  digitalWrite (transistor, LOW);
  delay(200);
}
