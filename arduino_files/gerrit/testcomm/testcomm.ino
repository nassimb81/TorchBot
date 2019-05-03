bool wait4Reply;
bool connection;
String antwoord;
char stukje;
unsigned long timeW4Reply;
const int ledPin = 13;
int LedOnOff;


void setup(){
  LedOnOff = HIGH;
  connection = false;
  Serial.begin(115200);
}


void loop() {
  wait4Reply = true;
  timeW4Reply = millis();
  Serial.println("hello");
  antwoord = "";
  while(wait4Reply){
    LedOnOff = !LedOnOff;
//    digitalWrite(ledPin,LedOnOff);
//    delay(300);
    if(Serial.available() > 0){
      stukje = Serial.read();
      antwoord += stukje;
      if(stukje == '\n'){
        wait4Reply = false;
      }
    }
    if((millis()-timeW4Reply) > 6000 & wait4Reply == true){
      connection = false;
      wait4Reply = false;
    }
  }
  if(antwoord.equals("hello\n")){
    connection = true;
  }
  Serial.print("communication is: ");
  Serial.println(connection);
  Serial.println(antwoord);
}
