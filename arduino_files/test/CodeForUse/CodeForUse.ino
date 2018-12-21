const int SEQlength = 1999;     //2000 elements, we see it as 1000 * 2: 1000 step's
int SEQ[SEQlength];

//Variables needed for serial communication
String readString;  //string to store characters from serial communication up to end of line (eol)
bool receiveArrayFromPC;
bool sendArrayToPC;
int ReadNr;
int timeOutReceiveFromPC = 0;

bool StartPlayTransition;

void setup() {
  Serial.begin(115200); // start serial
  Serial.println("<Arduino is ready>");
  ReadNr = 1;
  receiveArrayFromPC = true;
  sendArrayToPC = true;
  StartPlayTransition = true;

}

void loop() {
  if (StartPlayTransition == true) {  //code to be executed until transition completed ===========================
    //get array from Java-API
    //go to startpoint (with moderate speed), set speed to value from Array
    // Only goes into receiveArray when a Byte is send
    Serial.print("Receiving_Array");
    while (timeOutReceiveFromPC < 500 && receiveArrayFromPC) {
      delay(100);
      receiveArray();
      timeOutReceiveFromPC = timeOutReceiveFromPC + 1;
    }
    Serial.print("Out of while loop");
    //Serial.println("StartPlayTransition");

    StartPlayTransition = false;


    if (sendArrayToPC) {
      sendArray();
    }
  }
}

//Functions for Serial Communication

//Receive array from java service:
// 1) receives a set of characters from the pc and adds them to String temp until end of line is reached (/n)
// 2) converts String temp to int and adds the int to the SEQ array
// Note if the string is not an integer (+/-) 0 will be added to the array, from the java service this has been taken into consideration
// and only integers will be send to the arduino.
void receiveArray() {
  while (Serial.available() && receiveArrayFromPC) {
    receiveCharacter();
  }
}

void receiveCharacter() {
  char inChar = Serial.read();
  if (inChar != -1) {
    //    timeOutReceiveFromPC = 0;
    String temp = String(inChar);
    readString += temp;

    if (inChar == '\n') {
      SEQ[ReadNr] = readString.toInt();

      if (readString.equals("-32000\n")) { //end of array turning off reading from array
        receiveArrayFromPC = false;
      }
      ReadNr = ReadNr + 1;
      readString = "";
    }
  }
}
// Sends array to java service:
// 1) Loops over SEQ array and prints it to the serial port until -32000 is reached
// 2) Breaks out of loop and sets sendArrayTPC boolean to false
void sendArray() {
  Serial.print("Send_Array");
  for (int i = 0; i <  SEQlength; i++) {
    Serial.print(SEQ[i]);
    if (SEQ[i] == -32000) {
      break;
    }
    Serial.print(',');
  }
  sendArrayToPC = false;
}
