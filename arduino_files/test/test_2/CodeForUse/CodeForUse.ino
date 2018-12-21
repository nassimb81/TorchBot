const int SEQlength = 1999;     //2000 elements, we see it as 1000 * 2: 1000 step's
int SEQ[SEQlength];
int ReadNr;
String readString;
bool exportArray = false;
bool receiveSend = true;
bool receiving = true;
int timeOutReceiveFromPC;

void setup() {
  Serial.begin(115200); // start serial
  Serial.println("<Arduino is ready>");
  ReadNr = 1;
  receiving = true;
  exportArray = true;
}

void loop() {
  delay(20);
  while (Serial.available() && receiveSend) {
    receiveArray();
  }

  if (exportArray) {
    sendArray();
  }
}

void receiveArray() {
  while (receiving) {
    receiveCharacter();
  }
}

void receiveCharacter() {

  timeOutReceiveFromPC = 0;
  char inChar = Serial.read();
  if (inChar != -1) {
    String temp = String(inChar);
    readString += temp;

    if (inChar == '\n') {
      Serial.println("In Char");
      if (readString.equals("-32000\n")) {
        receiving = false;
        exportArray = true;
      }
      SEQ[ReadNr] = readString.toInt();
      Serial.print("Print in SEQ: ");
      Serial.println(SEQ[ReadNr]);
      ReadNr = ReadNr + 1;
      readString = "";
    }
  }
}


void sendArray() {
  Serial.println("Send Array");
  for (int i = 1; i <  SEQlength; i++) {
    Serial.println(SEQ[i]);
    if (SEQ[i] == -32000) {
      break;
    }
    Serial.print(',');
  }
  exportArray = false;
}
