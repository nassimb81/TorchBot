//Mode and OldMode did not speed up CheckModeTransition()
//maximum speed is now limmited by the amount of computations between subsequent motor pulses
//that is why I decided to perform [CeckModeTransition and CalcVel] OR [analogRead] at half the frequency
//this has to be done in both real time situations: Rec and Manual
//
//pin numbers
const int YincPin = 23; //Y+ (pin number)
const int YdecPin = 25; //Y-
const int ZincPin = 27; //Z+
const int ZdecPin = 29; //Z-
const int YminPin = 31; //end switches
const int YmaxPin = 33;
const int ZminPin = 35;
const int ZmaxPin = 37;
const int DirYPin = 8; //direction signal to y motors
const int PulseYPin = 9; //pulse signal to y motors
const int EnableYPin = 10;  //enable
const int DirZPin = 2; //direction signal to z motors
const int PulseZPin = 3; //pulse signal to z motors
const int EnableZPin = 4;  //enable
const int RecPin = 41;  //from dashboard
const int PlayPin = 43; //idem
const int NextPin = 45; //idem
const int HomePin = 47; //idem
const int StartPin = 49; //idem
const int StopPin = 51;  //idem
const int ActionPin = 53;   //idem

const int LedPin = 13;
int LedOnOff = HIGH;
const int VelPotPin = A15; //input pin for analog velocity signal

//variables (and constants)
int Yinc; //these variables are high or low after a digitalRead of the belonging pin
int Ydec;
int Zinc;
int Zdec;
int Ymax;
int Ymin;
int Zmax;
int Zmin;
int YincOld; //used for deceleration when stopping
int YdecOld;
int ZincOld;
int ZdecOld;

const int Requested = LOW;      //value to make the code more readable and easy adaptable
const int NoEndSwitch = HIGH;   //idem: check polarity
const int EndSwitch = !NoEndSwitch;

unsigned long int Last;         //timestamp of the timer: last pulse time

int Ypos; //since we use integers in the Array (SEQ) and with the used resolution
int Zpos; //saturation is not an issue.
long int YposLong;
long int ZposLong;

int VelPotU;  //read from analog input
int CaseNr = 0;
int Tperiod;  //pulse timing
int TperiodOld;
int TperiodWrite;
int TperiodAllowed[] = {
  32000, 4749, 1968, 1511, 1274, 1123, 1016, 935, 871, 819, 775, 738, 706, 678, 653, 631, 611, 593, 576, 561, 547, 534, 522, 511, 501, 491, 482, 473, 465, 457, 450, 443, 436, 430, 424, 418, 413, 408, 403, 398, 393, 389, 385, 381, 377, 373, 369, 366, 363, 360, 357, 354, 351, 348, 345, 342, 339, 336, 334, 332, 330, 328, 326, 324, 322, 320, 318, 316, 314, 312, 310, 308, 306, 304, 302, 300, 299, 298, 297, 296, 295, 294, 293, 292, 291, 290, 289, 288, 287, 286, 285, 284, 283, 282, 281, 280, 279, 278, 277, 276, 275, 274, 273, 272, 271, 270, 269, 268, 267, 266, 265, 264, 263, 262, 261, 260, 259, 258, 257, 256, 255, 254, 253, 252, 251, 250
};
const int Tperiod_indexMax = 125; //nr of elements (first is element 0)
int Tperiod_index = 0;
const int Tmoderate = 417;
const int Tslow = 954;
const int Tfast = 300;

int RecModeRequested; //reading from RecPin
int PlayModeRequested; //reading form PlayPin
int ManualModeRequested; //derived from the upper 2 as an exception
int HomeRequested;
int StartRequested;
int NextRequested;

int ManualMode;  //the system always is in one of these modes as a result of CheckModeTransition()
int PlayMode;  //idem
int RecMode;   //idem
bool StopRecTransition; //the four possible (allowed) mode transitions
bool StopPlayTransition;
bool StartRecTransition;
bool StartPlayTransition;

//byte State = 0; //** int ? waarom rec en manual niet en play wel....
//byte OldState = 15;
//byte NotOldState;
int State = 0; //** int ? waarom rec en manual niet en play wel....
int  OldState = 15;
int  NotOldState;

unsigned long int WaitingTime;
int WaitingTimeInt;               //scale ms by dividing by 2^7: 0.128 s resolution, max waiting 69 min 54 sec
unsigned long int TStartWaiting;
unsigned long int TStartEvent;
bool Waiting4_1stEvent = false; //true;  eerst nog even niet..

const int SEQlength = 1999;     //2000 elements, we see it as 1000 * 2: 1000 step's
int SEQ[SEQlength];
int StepDataC1;
int StepDataC2;
int StepNr;       //step number of the sequence

int ActionButton;
bool ActionButtonPressed;
unsigned long int ActionTime;
int ActionTimeInt;
int ActionNr;

bool AllInfoAfailable;        //true after reading end waiting OR position to go to
bool ActionsDuringStep;       //if actions occure during the next step
int NrOfActionsDuringStep;
int ActionPointerWrite;       //write pointer to the action buffer
int ActionPointerRead;        //read pointer from the action buffer
int ActionTimingFifo[24];         //25 Actions possible during one step of the sequence
int ActionDataFifo[24];
bool WaitingEvent;
bool MovingEvent;
bool LastEvent;
long int Ydestination;
long int Zdestination;

//Variables needed for serial communication
String readString;  //string to store characters from serial communication up to end of line (eol)
bool receiveArrayFromPC;
bool sendArrayToPC;
int ReadNr;
int timeOutReceiveFromPC = 0;

int teller;

void setup() {
  //  delay(5000);
  Serial.begin(115200);
  Serial.println("<Arduino is ready>");
  Serial.println("EXIT Setup");
  pinMode(LedPin, OUTPUT);
  pinMode(YincPin, INPUT_PULLUP);
  pinMode(YdecPin, INPUT_PULLUP);
  pinMode(ZincPin, INPUT_PULLUP);
  pinMode(ZdecPin, INPUT_PULLUP);
  pinMode(YmaxPin, INPUT_PULLUP);
  pinMode(YminPin, INPUT_PULLUP);
  pinMode(ZmaxPin, INPUT_PULLUP);
  pinMode(ZminPin, INPUT_PULLUP);
  pinMode(DirYPin, OUTPUT);
  pinMode(PulseYPin, OUTPUT);
  pinMode(EnableYPin, OUTPUT);
  pinMode(DirZPin, OUTPUT);
  pinMode(PulseZPin, OUTPUT);
  pinMode(EnableZPin, OUTPUT);
  pinMode(RecPin, INPUT_PULLUP);
  pinMode(PlayPin, INPUT_PULLUP);
  pinMode(NextPin, INPUT_PULLUP);
  pinMode(HomePin, INPUT_PULLUP);
  pinMode(StartPin, INPUT_PULLUP);
  pinMode(StopPin, INPUT_PULLUP);
  pinMode(ActionPin, INPUT_PULLUP);
  ManualMode = Requested;
  RecMode = !Requested;
  PlayMode = !Requested;
  digitalWrite(EnableYPin, HIGH);     //currently motors are Enabled continuouly
  digitalWrite(EnableZPin, HIGH);     //must be HIGH to prevend decending of the torch
  StopRecTransition = false; //the four mode transitions all false
  StopPlayTransition = false;
  StartRecTransition = false;
  StartPlayTransition = false;
  VelPotU = analogRead(VelPotPin);
  Tperiod = CalcVel(VelPotU);
  //  Tperiod = 534;
  ActionNr = 0;
  ActionsDuringStep = false;
  //  delay(3000);
  InitZeroY();    //move in plus dir until no endswitch, move back until endswitch, move in plus for 1 mm, set position to zero (Home)
  InitZeroZ();

  //initialization for variables serial communication
  receiveArrayFromPC = true;
  sendArrayToPC = true;
}


void loop() {
  //allways start testing if a transition between Modes has to be carried out
  if (StartPlayTransition == true) {  //code to be executed until transition completed ===========================
    //get array from RespPi (ALLWAYS?: no stand alone operation possible....,but how to solve this elegantly? (timeout?)
    //go to startpoint (with moderate speed), set speed to value from Array
    // Only goes into receiveArray when a Byte is send
    Serial.print("Request_Array_From_PC");
    while (receiveArrayFromPC) {
      //      receiveArray();
      receiveCharacter();
//      delay(100);
//      timeOutReceiveFromPC = timeOutReceiveFromPC + 1;
    }
    //Serial.println("StartPlayTransition");

    StartPlayTransition = false;

    teller = 1;
    do {
      ReadStepData(teller);
      Serial.print("C1 and C2: ");
      Serial.print(StepDataC1);
      Serial.print("  ");
      Serial.println(StepDataC2);
      teller = teller + 1;
    } while (StepDataC1 != -32000 & teller < 400);
    StepNr = 1;     //start from the 'top'
    ReadStepData(StepNr);
    if (StepDataC1 == -31000) {
      ReadStepData(StepNr);           //Second row should contain the starting point of the recording
      Ydestination = 4UL * StepDataC1;
      Zdestination = 4UL * StepDataC2;
      Move2YZ(Ydestination, Zdestination, Tmoderate); //go to initial position with moderate speed
    }
    else {
      while (PlayMode == Requested) {   //if in PlayMode
        CheckModeTransition();        //wait until Manual Mode
      }
    }
    //Serial.println("StartPlayTransition ended");
  }
  else if (StartRecTransition == true) { //code to be executed until transition completed========================
    //start writing from StepNr 1
    StartRecTransition = false;
    StepNr = 1;
    ActionButtonPressed = false;
    ActionNr = 0;
    WriteArray(-31000, 0);         //-31000: begin of Array mark
    Ypos = (YposLong + 2) >> 2;
    Zpos = (ZposLong + 2) >> 2;
    WriteArray(Ypos, Zpos);       //initial position
    Waiting4_1stEvent = true;
    //Serial.println("StartRecTransition ended");
  }
  else if (StopPlayTransition == true) { //code to be executed until transition completed========================
    StopPlayTransition = false;
    ActionsDuringStep = false;          //only actions while in PlayMode
    //Serial.println("StopPlayTransition ended");
  }
  else if (StopRecTransition == true) {  //code to be executed until transition completed========================
    StopRecTransition = false;
    if (State == 15) {
      WaitingTime = millis() - TStartWaiting;
      WaitingTimeInt = WaitingTime >> 7;
      if (WaitingTimeInt < 0) {
        WaitingTimeInt = 32767; //no negative values, max wait 69 min 54 sec,
      }
      WriteArray(-30008, WaitingTimeInt);             //Error when trying to wait longer than 2h33m

    }
    else {
      Ypos = (YposLong + 2) >> 2;
      Zpos = (ZposLong + 2) >> 2;
      WriteArray(Ypos, Zpos);
    }
    WriteArray(-32000, 0); //marking the end of a Sequence (Array)
    //write Array to PC (if connected)
    while (sendArrayToPC) {
      sendArray();
    }
    //just stay where you are!
    //Serial.println("StopRecTransition ended");
  }
  else {                                //NO TRANSITION BETWEEN MODES
    //=================================================================================================================
    if (ManualMode == Requested) {            //This if else if construction results in performing a transition
      //before entering a while loop for the current Mode (Play, Rec, Manual)
      State = 15;   //**is dit nodig  ja?                  //enter manual mode while waiting
      CaseNr = 0;   //always do a analogRead before CalcVel
      //Serial.println("entering ManualMode");
      while (ManualMode == Requested) {         //while Rec / Play switch in Manual position
        ReadDCState();                   //Read Direction Control input pins and calculate State
        StateChangeExeptions();		//was SetLastIfStart, nu wordt daar ook het stoppen geregeld
        CondPulseAndUpdatePos();         //give pulses if no EndSwitch is active And update position
        CaseNr = CaseNr + 1;
        switch (CaseNr) {
          case 1:
            VelPotU = analogRead(VelPotPin); //in the Manual mode the speed can be controlled
            break;
          case 2:
            Tperiod = CalcVel(VelPotU);
            CheckModeTransition();           //End one Mode and Start an other Mode if requested
            if (digitalRead(ActionPin) == LOW) {
              digitalWrite(EnableYPin, LOW);     //Disable motors for manual corrections of Y and Z position
              digitalWrite(EnableZPin, LOW);     //
            }
            else {
              digitalWrite(EnableYPin, HIGH);     //motors are Enabled
              digitalWrite(EnableZPin, HIGH);
            }
            if (digitalRead(HomePin) == Requested) {
              Move2YZ(800, 800, Tmoderate);
              InitZeroY();
              InitZeroZ();
            }
            CaseNr = 0;                     //next time do an analogRead
            break;
        }
        digitalWrite(PulseYPin, LOW);
        digitalWrite(PulseZPin, LOW);
        Wait4Tperiod(Tperiod);
      }
      //==================================================================================================================
    }
    else if (PlayMode == Requested) {         //This if else if construction results in performing a transition
      //before entering a while loop for the current Mode (Play, Rec, Manual)
      //Serial.println("PlayMode waiting for start");
      while (digitalRead(StartPin) != Requested & PlayMode == Requested) { //now wait for the start button to be pressed
        CheckModeTransition();           //keep an eye on these switches
      }
      LastEvent = false;
      //Serial.println("Sequence start");
      while (PlayMode == Requested) {
        DecodeNextSteps();          //read from Array until all actions and waitingTime OR movement are available
        TStartEvent = millis();     //timings with respect to this time
        if (WaitingEvent == true) {
          while ((millis() - TStartEvent) < WaitingTime) {
            if (digitalRead(NextPin) == LOW) {
              break;          //exit loop if next button pushed
            }
            if (ActionsDuringStep == true) {
              ActionCheck();
            }
            CheckModeTransition();    //check this also within this waiting loop...
            if (StopRecTransition == true || StartRecTransition == true || StopPlayTransition == true || StartPlayTransition == true) {
              break;
            }

            //hier zou een test kunnen komen die een break veroorzaakt als gevolg van Action Data.....
          }
        }
        if (MovingEvent == true) {
          Move2YZ(Ydestination, Zdestination, Tperiod);
        }
        if (LastEvent == true) {
          StepNr = 2;
          ReadStepData(StepNr);    //read initial position of the sequence
          Ydestination = 4UL * StepDataC1;
          Zdestination = 4UL * StepDataC2;
          Move2YZ(YposLong, Zdestination, Tmoderate);    //move to inital position
          Move2YZ(Ydestination, Zdestination, Tmoderate); //a Tperiod reading could also be a good idea
          LastEvent = false;
          while (digitalRead(StartPin) != Requested & PlayMode == Requested) { //now wait for the start button to be pressed
            CheckModeTransition();           //keep an eye on these switches
          }
        }
        CheckModeTransition();  //** deze lijkt niet nodig
      }
      //==================================================================================================================
    }
    else if (RecMode == Requested) {          //This if else if construction results in performing a transition
      //before entering a while loop for the current Mode (Play, Rec, Manual)
      State = 15;
      CaseNr = 0;
      //Serial.println("entering Rec mode");
      while (RecMode == Requested) {         //while Rec / Play switch in Manual position
        ReadDCState();                   //Read Direction Control input pins and calculate State
        EventHandling();                 //also sets Last in case of starting a movement
        CondPulseAndUpdatePos();         //give pulses if no EndSwitch is active And update position
        CaseNr = CaseNr + 1;
        switch (CaseNr) {               //less computation by slowing down the rates off:
          case 1:
            VelPotU = analogRead(VelPotPin); //reading of analog speed setpoint
            break;
          case 2:
            Tperiod = CalcVel(VelPotU);     //calculation of Tperiod and
            CheckModeTransition();          //End one Mode and Start an other Mode if requested
            CaseNr = 0;                     //next time do an analogRead
            break;
        }
        digitalWrite(PulseYPin, LOW);
        digitalWrite(PulseZPin, LOW);
        Wait4Tperiod(Tperiod);
      }
    }
  }
}


void DecodeNextSteps() {
  AllInfoAfailable = false;
  NrOfActionsDuringStep = 0;
  ActionPointerWrite = -1;
  ActionPointerRead = 0;              //start at 0 each next step
  ActionsDuringStep = false;          //step is this moving or waiting event of the sequence
  MovingEvent = false;
  WaitingEvent = false;
  LastEvent = false;
  do {
    ReadStepData(StepNr);
    if (StepDataC1 == -30008) {       //waiting event
      WaitingTimeInt = StepDataC2;
      WaitingTime = 128UL * WaitingTimeInt;
      WaitingEvent = true;
      AllInfoAfailable = true;
    }
    if (StepDataC1 == -30004) {       //new Tperiod
      Tperiod = StepDataC2;
    }
    if (StepDataC1 == -32000) {       //end of sequence
      LastEvent = true;            //LastEvent == true results in reading inital pos from Array....
      AllInfoAfailable = true;
    }
    if (StepDataC1 < 0 & StepDataC1 > -501) { //Action,(500 maximum)
      ActionNr = StepDataC1;
      ActionsDuringStep = true;
      NrOfActionsDuringStep = NrOfActionsDuringStep + 1;
      ActionPointerWrite = ActionPointerWrite + 1;  //use fifo buffer for events,
      //if ActionPointerWrite > 24: Error....
      ActionTimingFifo[ActionPointerWrite] = StepDataC2;
      ReadStepData(StepNr);     //read next data from Array in order to read Action Data
      //if StepDataC1 != ActionNr : Error HOEWEL deze int kan ook andere info bevatten (ActionKind ?)
      ActionDataFifo[ActionPointerWrite] = StepDataC2;
    }
    if (StepDataC1 >= 0) {            //position
      Ydestination = 4UL * StepDataC1;
      Zdestination = 4UL * StepDataC2;
      MovingEvent = true;
      AllInfoAfailable = true;
    }

  }
  while (AllInfoAfailable == false);
}

//this function changes the values of Last, and calls: CondPulseAndUpdate, Wait4Tperiod
//it uses YposLong en ZposLong
//move to (Y,Z) with 'speed' T
void Move2YZ(long int Y, long int Z, int T) {
  long int DeltaY;
  long int DeltaZ;
  long int i;
  int n;
  int IndexMax;
  bool OneExtraStep;
  DeltaY = Y - YposLong;
  DeltaZ = Z - ZposLong;
  if (DeltaY < 0) {
    Yinc = !Requested;
    Ydec = Requested;
  }
  else {
    Yinc = Requested;
    Ydec = !Requested;
  }
  if (DeltaZ < 0) {
    Zinc = !Requested;
    Zdec = Requested;
  }
  else {
    Zinc = Requested;
    Zdec = !Requested;
  }
  i = max((abs(DeltaY)), (abs(DeltaZ))); //
  //  if(abs(DeltaY) != abs(DeltaZ)){Serial.println("YZ movement not scale 1:1");} //** TESTING
  n = i >> 1; //hallf the number of steps rouded downward
  //hier moet je de maximaal gebruikte index weten en die is afhankelijk van de gewenste T
  IndexMax = findIndMax(T);
  // Serial.print("IndexMax ");
  // Serial.println(IndexMax);
  if (n > (IndexMax)) {
    n = IndexMax;   //never more steps than ..
  }
  else {
    if (2 * n < i) {
      OneExtraStep = false;    //stopping takes n+1....
    }
    else {
      OneExtraStep = true;
    }
  }
  Last = micros() + 150;
  //  //Serial.print("moving");
  for (i; i > n + 1; i--) { //** **n+1 **eerder stoppen met deze for loop om naar Stop te springen!
    if (YposLong == Y) {
      Yinc = !Requested;
      Ydec = !Requested;
    }
    if (ZposLong == Z) {
      Zinc = !Requested;
      Zdec = !Requested;
    }
    CondPulseAndUpdatePos();
    delayMicroseconds(100);         //delay equal to AnalogRead (used in other modes after CondPu...)
    digitalWrite(PulseYPin, LOW);
    digitalWrite(PulseZPin, LOW);
    if (ActionsDuringStep == true) {
      ActionCheck();
    }
    CheckModeTransition();
    if (StopRecTransition == true || StartRecTransition == true || StopPlayTransition == true || StartPlayTransition == true) {
      Wait4Tperiod(32100);
      Stop();
      break;
    }
    if (digitalRead(StopPin) == LOW) {    //stop pressed
      Wait4Tperiod(32100);
      Stop();
      while (PlayMode == Requested) {   //if in PlayMode
        CheckModeTransition();        //wait until Manual Mode
      }
      break;  //exit the for loop, (in manual mode this happens right away)
    }
    Wait4Tperiod(T);
  }
  if (OneExtraStep == true) {
    CondPulseAndUpdatePos();
    delayMicroseconds(100);         //delay equal to AnalogRead (used in other modes after CondPu...)
    digitalWrite(PulseYPin, LOW);
    digitalWrite(PulseZPin, LOW);
    Wait4Tperiod((TperiodAllowed[Tperiod_index + 1])); //just in to the next interval
  }
  //  Serial.println("stoppen start hier");
  //  Serial.print("Tperiod_index: ");
  //  Serial.println(Tperiod_index);
  Stop();   //** van deze stop weet ik dat die werkt
  //delay(100);   //possibly unnessasary
  // Serial.print("moved to: YposLong ");
  // Serial.println(YposLong);
  // Serial.print("  ZposLong ");
  // Serial.println(ZposLong);
}


//find the maximal index used with the requested T
int findIndMax(int T) {
  int i;
  int T_A;
  i = 0;
  T = constrain(T, TperiodAllowed[Tperiod_indexMax], TperiodAllowed[0]); //T within maximum bounds
  do {
    T_A = constrain(T, TperiodAllowed[(i + 1)], TperiodAllowed[i]);
    i = i + 1;
  }
  while (T_A != T);
  i = i - 1;
  return i;
}

//(only)During Play ActionsDuringStep can be true
//if so check if ActionTime has arrived and perform nessesary actions
void ActionCheck() {
  ActionTimeInt = ActionTimingFifo[ActionPointerRead];
  ActionTime = 128UL * ActionTimeInt;
  if ((millis() - TStartEvent) >= ActionTime) {
    //Serial.print("Action detected   ");
    //write data form ActionDataFifo[ActionPointerRead]....
    //a third Fifo ActionKindFifo could be added if nessesary
    if ((NrOfActionsDuringStep - 1) > ActionPointerRead) {
      ActionPointerRead = ActionPointerRead + 1;
      //Serial.println(ActionPointerRead);
    }
    else {
      ActionsDuringStep = false;
    }
  }
}

//An event occourse when OldState is unequal to State, no event: nothing happens
//changes the values of: TStartEvent, Last, possibly Waiting4_1stEvent
//possibly TStartWaiting.
//writes values to the Array, called in RecMode
void EventHandling() {
  if (Waiting4_1stEvent == false) {       //action is never the first event, this is prohibeted
    if (ActionButtonPressed == true) {        //Button pressed event, (tested in Waiting4Tperiod)
      ActionTime = ActionTime - TStartEvent;  //timing in millis
      ActionTimeInt = ActionTime >> 7;        //scaling like waiting timeing
      if (ActionTimeInt < 0) {
        ActionTimeInt = 32767; //Error....
      }
      WriteArray(ActionNr, ActionTimeInt);
      WriteArray(ActionNr, 0);              //ActionData to be added offline
      ActionButtonPressed = false;
    }
  }
  if (OldState != State) {            //only action when the State changes
    TStartEvent = millis();         //necessary to know in case of the action button
    if (OldState == 15) {           //START, old state was waiting but possibly for the 1st event
      if (Waiting4_1stEvent == true) {
        WriteArray(-30004, Tperiod);
        TperiodWrite = Tperiod; //remember in order to check if a new value has to be written to Array
        Tperiod_index = 0;
        Last = micros() + 100;    //see comment SetLastIfStart
        Waiting4_1stEvent = false;
      }
      else {                     //end of waiting
        WaitingTime = millis() - TStartWaiting;
        WaitingTimeInt = WaitingTime >> 7;
        if (WaitingTimeInt < 0) {
          WaitingTimeInt = 32767; //no negative values, max wait 69 min 54 sec,
        }
        //Error: waiting time saturated at max value
        WriteArray(-30008, WaitingTimeInt);             //Error when trying to wait longer than 2h33m
        if (TperiodWrite != Tperiod) {  //only write T if changed
          WriteArray(-30004, Tperiod);
          TperiodWrite = Tperiod;
        }
        Tperiod_index = 0;
        Last = micros() + 100;    //see comment SetLastIfStart
      }
    }
    else if (State == 15) {        //STOP and start waiting, write position
      RealTimeStop();                     //** eerst gecontroleerd stoppen
      if (TperiodWrite != Tperiod) {  //only write T if changed
        WriteArray(-30004, Tperiod); //de snelheid wordt hier niet meer veranderd
        TperiodWrite = Tperiod;     //we stoppen altijd tussendoor
      }
      TStartWaiting = millis();
      Ypos = (YposLong + 2) >> 2;
      Zpos = (ZposLong + 2) >> 2;
      WriteArray(Ypos, Zpos);
    }
    else {                         //change of direction, stop, write position (and speed?)
      RealTimeStop();
      if (TperiodWrite != Tperiod) {  //only write T if changed
        WriteArray(-30004, Tperiod); //de snelheid wordt hier niet meer veranderd
        TperiodWrite = Tperiod;     //we stoppen altijd tussendoor
      }
      Ypos = (YposLong + 2) >> 2;
      Zpos = (ZposLong + 2) >> 2;
      WriteArray(Ypos, Zpos);
    }
  }
}


//Last is the time the last pulse was started (begin of a Tperiod interval)
//Since Last is incremented with Tperiod each period it needs to be set
//at the beginning of a movement. By adding 150 the first step will be somewhat
//slower (instead of quicker)!
void StateChangeExeptions() {   //** was: void SetLastIfStart(){
  if (OldState != State) {            //only action when the State changes
    if (OldState == 15) {
      Tperiod_index = 0;
      Last = micros() + 110;  //how much longer should this be best?
    }
    else {
      RealTimeStop();
    }
  }
}


//Sets Tperiod_index + 1 steps and stops: longest time between pulses = second value of TperiodAllowed
void Stop() {
  for (int i = Tperiod_index; i >= 0; i--) { //** één stap meer door >= ???
    CondPulseAndUpdatePos();   //** i en Tperiod_index gaan zo gelijk op, hoe lang is tijd tussen laatste 2 pulsen
    delayMicroseconds(100);     //
    digitalWrite(PulseYPin, LOW);
    digitalWrite(PulseZPin, LOW);
    Wait4Tperiod(32100);
  }
}


//identical to Stop() exept is uses CondPulseEtc_RT which uses the former DC State
void RealTimeStop() {
  for (int i = Tperiod_index; i >= 0; i--) { //** één stap meer door >= ??? (zie Stop()   )
    CondPulseEtc_RT();   //** i en Tperiod_index gaan zo gelijk op, hoe lang is tijd tussen laatste 2 pulsen
    delayMicroseconds(100);     //
    digitalWrite(PulseYPin, LOW);
    digitalWrite(PulseZPin, LOW);
    Wait4Tperiod(32100);
  }
}

//it seems it is not possible to pass an Array from a function
//I keep using all these global variables...
//StepDataC1 and StepDataC2 are updated
//StepNr is incremented each time data is read
void ReadStepData(int SeqStepNr) {
  int index;
  index = (SeqStepNr - 1) * 2;
  index = constrain(index, 0, SEQlength); //just to be shure nothing go's (really) wrong
  StepDataC1 = SEQ[index];
  StepDataC2 = SEQ[index + 1];
  //Serial.print("Read Step nr: ");
  //Serial.print(SeqStepNr);
  //Serial.print("  StepDataC1: ");
  //Serial.print(StepDataC1);
  //Serial.print("  StepDataC2: ");
  //Serial.println(StepDataC2);
  StepNr = StepNr + 1;
}


//column 1 and column 2 of the Array
//StepNr is incremented each time data is written
void WriteArray(int C1, int C2) {
  int index;
  index = (StepNr - 1) * 2;
  index = constrain(index, 0, SEQlength); //just to be shure nothing go's (really) wrong
  SEQ[index] = C1;
  SEQ[index + 1] = C2;
  StepNr = StepNr + 1;
}

//Reading RecMode and PlayMode, deriving ManualMode
//checking mode transitions
//setting the boolians for the distinction between modes (Rec, Play, Manual) AND
//booleans for the 4 allowed mode transitions
void CheckModeTransition() {
  RecModeRequested = digitalRead(RecPin);                   //which Mode requested?
  PlayModeRequested = digitalRead(PlayPin);
  if (RecModeRequested == !Requested && PlayModeRequested == !Requested) {
    ManualModeRequested = Requested;
  }
  else {
    ManualModeRequested = !Requested;         //now requested Mode is known =================================
  }
  if (RecModeRequested == Requested && PlayMode == Requested) {  //first test if the user tries
    ManualMode = Requested;                                      //a transition from Play to Rec
    PlayMode = !Requested;                                       //at once, this will be handled
    RecMode = !Requested;
    StopPlayTransition = true;
  }
  else if (PlayModeRequested == Requested && RecMode == Requested) {  //idem for Rec to Play
    ManualMode = Requested;
    RecMode = !Requested;
    PlayMode = !Requested;
    StopRecTransition = true;
  }
  else {                                   //now transitions to or from ManualMode ========================
    if (RecMode != RecModeRequested) {      //Rec mode change requested ====================================
      if (RecModeRequested == Requested) {       //Rec mode requested
        StartRecTransition = true;
        RecMode = Requested;
        ManualMode = !Requested;
        PlayMode = !Requested;
      }
      else {
        StopRecTransition = true;           //Stop Rec: Manual mode requested
        RecMode = !Requested;
        ManualMode = Requested;
        PlayMode = !Requested;
      }
    }
    if (PlayMode != PlayModeRequested) {    //Play mode change requested ====================================
      if (PlayModeRequested == Requested) {      //Play mode requested
        StartPlayTransition = true;
        PlayMode = Requested;
        ManualMode = !Requested;
        RecMode = !Requested;
      }
      else {
        StopPlayTransition = true;          //Manual mode requested
        PlayMode = !Requested;
        ManualMode = Requested;
        RecMode = !Requested;
      }
    }
  }
}


//Wait for the end of a Period T_lapse as passed to the function (if allowed due to acceleration constrains)
//depends on Last, updates Last
void Wait4Tperiod(int T_lapse) {
  int T_lapseSmooth;
  bool Waiting4T; //for timing of the motor pulses
  Waiting4T = true;
  T_lapseSmooth = SmoothAcc(T_lapse); //geeft stack overflow in de compiler!!!
  while (Waiting4T == true) {     //keep pulsing with Tperiod
    if ((micros() - Last) >= T_lapseSmooth) {
      Waiting4T = false;
      Last = Last + T_lapseSmooth; //If Last is set long time ago: waiting time almost zero
    }                           //this is a risk! But setting Last together with the pulse
    //leads to jitter and differences between Play, Rec and Manual
    if (ActionButtonPressed == false) { //if tested true once stop testing during this waiting4Tperiod
      CheckActionButton();        //tested here to prevend slow checking frequency
    }
  }
}

//constrain Treq to an Allowed Tperiod called T_Allowed in this function
//keep shifting the index so next request will approche the requested period with the best value possible
int SmoothAcc(int Treq) {
  int T_Allowed;
  T_Allowed = constrain(Treq, TperiodAllowed[(Tperiod_index + 1)], TperiodAllowed[Tperiod_index]); //This T is allowed
  if (Treq > T_Allowed) {
    Tperiod_index = Tperiod_index - 1; //if requested outside current interval ...
  }
  if (Treq < T_Allowed) {
    Tperiod_index = Tperiod_index + 1; //... move to adjecent interval.
  }
  Tperiod_index = constrain(Tperiod_index, 0, (Tperiod_indexMax - 1)); //constrain index between 0 and max index - 1
  //Serial.println(Tperiod_index);
  return T_Allowed;
}


void CheckActionButton() {
  int ActionButtonOld;
  ActionButtonOld = ActionButton;
  ActionButton = digitalRead(ActionPin);
  ActionButtonPressed = false;
  if (ActionButtonOld != ActionButton & ActionButton == LOW) { //react on downslope
    ActionButtonPressed = true;
    ActionTime = millis();    //remember the timing
    ActionNr = ActionNr - 1;  //counted in negative numbers
  }
}

//** dit moet veranderen: Y en Z afzonderljk uitvoeren: InitZeroY en InitZeroZ....,
// later zelfs nog verder aanpassen omdat links en rechts appart moeten!
void InitZeroY() {
  Tperiod_index = 0;
  Last = micros();
  do {
    Ydec = Requested;
    Yinc = !Requested;      //request the propper direction
    Zdec = !Requested;
    Zinc = !Requested;
    CondPulseAndUpdatePos();
    delayMicroseconds(100);
    digitalWrite(PulseYPin, LOW);
    digitalWrite(PulseZPin, LOW);
    Wait4Tperiod(Tslow);
  }
  while (Ymin == NoEndSwitch);
  Stop();
  do {
    Ydec = !Requested;
    Yinc = Requested;
    CondPulseAndUpdatePos();                  //increase until EndSwitches released
    delayMicroseconds(100);
    digitalWrite(PulseYPin, LOW);
    digitalWrite(PulseZPin, LOW);
    Wait4Tperiod(Tslow);
  }
  while (Ymin == EndSwitch);
  Stop();
  YposLong = 0;                         //temporary origin
  delay(100);
  Move2YZ(100, 0, Tslow);               //1mm margin
  YposLong = 0;                         //Y ORIGIN
  //Serial.println("EXIT InitZero");
}


//** dit moet veranderen: Y en Z afzonderljk uitvoeren: InitZeroY en InitZeroZ....,
// later zelfs nog verder aanpassen omdat links en rechts appart moeten!
void InitZeroZ() {
  Tperiod_index = 0;
  Last = micros();
  do {
    Ydec = !Requested;
    Yinc = !Requested;      //request the propper direction
    Zdec = Requested;
    Zinc = !Requested;      //towards smaller Y and Z
    CondPulseAndUpdatePos();
    delayMicroseconds(100);
    digitalWrite(PulseYPin, LOW);
    digitalWrite(PulseZPin, LOW);
    Wait4Tperiod(Tslow);
  }
  while (Zmin == NoEndSwitch);
  Stop();
  do {
    Zdec = !Requested;
    Zinc = Requested;
    CondPulseAndUpdatePos();                  //increase until EndSwitches released
    delayMicroseconds(100);
    digitalWrite(PulseYPin, LOW);
    digitalWrite(PulseZPin, LOW);
    Wait4Tperiod(Tslow);
  }
  while (Zmin == EndSwitch);
  Stop();
  ZposLong = 0;                   //temporary origin
  Move2YZ(0, 100, Tslow);                       //1mm margin
  ZposLong = 0;                   //Z ORIGIN
  //Serial.println("EXIT InitZero");
}


void ReadDCState() {                //Read Direction Control input pins (this is the only reading of these pins)
  OldState = State;
  YincOld = Yinc;
  YdecOld = Ydec;
  ZincOld = Zinc;
  ZdecOld = Zdec;
  Yinc = digitalRead(YincPin);      //4 times switch positions
  Ydec = digitalRead(YdecPin);
  Zinc = digitalRead(ZincPin);
  Zdec = digitalRead(ZdecPin);
  State = Yinc + 2 * Ydec + 4 * Zinc + 8 * Zdec;
  //  //Serial.print("DC state read: ");
  //  //Serial.println(State);
}

//
int CalcVel(int U) {    //gebruiken we nu eerst even niet, wellicht kan dit gewoon (beina)wel eigenlijk
  int T;
  TperiodOld = Tperiod;
  //  T = 1277913UL / (U+39);   //T max = 32767, T min = 1203
  //  T = 2949456UL / (U+1429);   //T max = 2064, T min = 1202
  //  T = 751752UL / (U+788); 	//T max = 954, T min = 417
  //  T = 246000 / (U+205); //1200 tot 200
  T = 341000UL / (U + 341); //T max = 1000, T min = 250
  //  T = 439000UL / (U+439); //T max = 1000. T min = 300

  return T;
}

//depending on Yinc, Ydec, Zinc and Zdec (read in ReadDCstate) and the endswitches Ymax, Ymin, Zmax and Zmin
//motor pulses are given and the position counter is updated
void CondPulseAndUpdatePos() {                      //motor pulses if EndSwitches NOT operated
  Ymax = digitalRead(YmaxPin);                      //read endswitch
  if (Yinc == Requested && Ymax == NoEndSwitch) {   //conditional pulse
    digitalWrite(DirYPin, HIGH);
    digitalWrite(PulseYPin, HIGH);
    YposLong = YposLong + 1;                                //and: update position
  }
  Ymin = digitalRead(YminPin);
  if (Ydec == Requested && Ymin == NoEndSwitch) {
    digitalWrite(DirYPin, LOW);
    digitalWrite(PulseYPin, HIGH);
    YposLong = YposLong - 1;
  }
  Zmax = digitalRead(ZmaxPin);
  if (Zinc == Requested && Zmax == NoEndSwitch) {
    digitalWrite(DirZPin, LOW);
    digitalWrite(PulseZPin, HIGH);
    ZposLong = ZposLong + 1;
  }
  Zmin = digitalRead(ZminPin);
  if (Zdec == Requested && Zmin == NoEndSwitch) {
    digitalWrite(DirZPin, HIGH);
    digitalWrite(PulseZPin, HIGH);
    ZposLong = ZposLong - 1;
  }
}

//depending on Yinc, Ydec, Zinc and Zdec (read in ReadDCstate) and the endswitches Ymax, Ymin, Zmax and Zmin
//motor pulses are given and the position counter is updated
void CondPulseEtc_RT() {                      //motor pulses if EndSwitches NOT operated
  Ymax = digitalRead(YmaxPin);                      //read endswitch
  if (YincOld == Requested && Ymax == NoEndSwitch) {   //conditional pulse
    digitalWrite(DirYPin, HIGH);
    digitalWrite(PulseYPin, HIGH);
    YposLong = YposLong + 1;                                //and: update position
  }
  Ymin = digitalRead(YminPin);
  if (YdecOld == Requested && Ymin == NoEndSwitch) {
    digitalWrite(DirYPin, LOW);
    digitalWrite(PulseYPin, HIGH);
    YposLong = YposLong - 1;
  }
  Zmax = digitalRead(ZmaxPin);
  if (ZincOld == Requested && Zmax == NoEndSwitch) {
    digitalWrite(DirZPin, LOW);
    digitalWrite(PulseZPin, HIGH);
    ZposLong = ZposLong + 1;
  }
  Zmin = digitalRead(ZminPin);
  if (ZdecOld == Requested && Zmin == NoEndSwitch) {
    digitalWrite(DirZPin, HIGH);
    digitalWrite(PulseZPin, HIGH);
    ZposLong = ZposLong - 1;
  }
}


//Functions for Serial Communication

//Receive array from java service:
// 1) receives a set of characters from the pc and adds them to String temp until end of line is reached (/n)
// 2) converts String temp to int and adds the int to the SEQ array
// Note if the string is not an integer (+/-) 0 will be added to the array, from the java service this has been taken into consideration
// and only integers will be send to the arduino.
//void receiveArray() {
//  while (Serial.available() && receiveArrayFromPC) {
//    receiveCharacter();
//  }
//}

void receiveCharacter() {
  char inChar = Serial.read();
  if (inChar != -1) {
    timeOutReceiveFromPC = 0;
    String temp = String(inChar);
    readString += temp;
   
    if (inChar == '\n') {
      Serial.println("ReadString: ");
      Serial.println(readString);
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
