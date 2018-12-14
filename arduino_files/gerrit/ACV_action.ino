//pin numbers currently randomly chosen
const int YincPin = 23; //Y+ (pin number)
const int YdecPin = 25; //Y-
const int ZincPin = 27; //Z+
const int ZdecPin = 29; //Z-
const int YmaxPin = 31; //end switches
const int YminPin = 33;
const int ZmaxPin = 35;
const int ZminPin = 37;
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

const int Requested = LOW;      //value to make the code more readable and easy adaptable
const int NoEndSwitch = HIGH;   //idem: check polarity

unsigned long int Last;         //timestamp of the timer: last pulse time

int Ypos; //since we use integers in the Array (SEQ) and with the used resolution
int Zpos; //saturation is not an issue.

int VelPotU;  //read from analog input
int Tperiod;  //pulse timing
int TperiodOld;
bool Tperiod_changed;
int Tmoderate = 1000;

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
bool Waiting4T; //for timing of the motor pulses

int State = 0;
int OldState = 15;
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

int Button;
int ButtonOld;
bool ButtonPressed;
unsigned long int ActionTime;
int ActionTimeInt;
int ActionNr;

bool AllInfoAfailable;        //true after reading end waiting OR position to go to
bool ActionsDuringStep;       //if actions occure during the next step
int ActionPointerWrite;       //write pointer to the action buffer
int ActionPointerRead;        //read pointer from the action buffer
int ActionBuffer[24];         //25 Actions possible during one step of the sequence
bool WaitingEvent;
bool MovingEvent;
bool LastEvent;

void setup() {
  pinMode(LedPin,OUTPUT);						 
  pinMode(YincPin,INPUT_PULLUP);
  pinMode(YdecPin,INPUT_PULLUP);
  pinMode(ZincPin,INPUT_PULLUP);
  pinMode(ZdecPin,INPUT_PULLUP);
  pinMode(YmaxPin,INPUT_PULLUP);
  pinMode(YminPin,INPUT_PULLUP);
  pinMode(ZmaxPin,INPUT_PULLUP);
  pinMode(ZminPin,INPUT_PULLUP);
  pinMode(DirYPin,OUTPUT);
  pinMode(PulseYPin,OUTPUT);
  pinMode(EnableYPin,OUTPUT);
  pinMode(DirZPin,OUTPUT);
  pinMode(PulseZPin,OUTPUT);
  pinMode(EnableZPin,OUTPUT);
  pinMode(RecPin,INPUT_PULLUP);
  pinMode(PlayPin,INPUT_PULLUP);
  pinMode(NextPin,INPUT_PULLUP);
  pinMode(HomePin,INPUT_PULLUP);
  pinMode(StartPin,INPUT_PULLUP);
  pinMode(StopPin,INPUT_PULLUP);
  pinMode(ActionPin,INPUT_PULLUP);
  ManualMode = Requested;
  RecMode = !Requested;
  PlayMode = !Requested;
  digitalWrite(EnableYPin,HIGH);      //currently motors are Enabled continuous, maybee unnessasary, maybee no problem
  digitalWrite(EnableZPin,HIGH);
  StopRecTransition = false; //the four mode transitions all false
  StopPlayTransition = false;
  StartRecTransition = false;
  StartPlayTransition = false;
  VelPotU = analogRead(VelPotPin);
  Tperiod = CalcVel(VelPotU);
  Tperiod_changed = true;
  ActionNr = 0;
  ActionsDuringStep = false;
  //move in plus dir until no endswitch, move back until endswitch, move in plus for 0.5 mm, set position to zero (Home)
  Serial.begin(9600);
}


void loop() {
  //allways start testing if a transition between Modes has to be carried out
  if(StartPlayTransition == true){    //code to be executed until transition completed ===========================
      //get array from RespPi (ALLWAYS?: no stand alone operation possible....,but how to solve this elegantly? (timeout?)
	  //go to startpoint (with moderate speed), set speed to value from Array
      StepNr = 1;     //start from the 'top'
      ReadStepData(StepNr);
      //StepDataC1 should be -31000 test: write ERROR to screen?
      ReadStepData(StepNr);           //Second row should contain the starting point of the recording					  
      Move2YZ(StepDataC1,StepDataC2,Tmoderate);  //go to initial position with moderate speed
      StartPlayTransition = false;
  }else if(StartRecTransition == true){   //code to be executed until transition completed========================
      //start writing from StepNr 1
      StepNr = 1; 
      WriteArray(-31000,0);          //-31000: begin of Array mark   
      WriteArray(Ypos,Zpos);        //initial position
      Waiting4_1stEvent = true;
      StartRecTransition = false;
  }else if(StopPlayTransition == true){   //code to be executed until transition completed========================
//      Last = micros();
//      Move2YZ(0,0,Tmoderate);
//OR  Stay where you are: no pulses: manual mode is set already, in Manual use Home to go to 0,0
//NO CODE YET
      StopPlayTransition = false;
  }else if(StopRecTransition == true){    //code to be executed until transition completed========================
//hoe ga je handig om met stoppen omdat dit zowel tijdens wachten als tijdens bewegen kan(??) meestal wachten....      
      //write array to RespPi
      if(State == 15){
            WaitingTime = millis() - TStartWaiting;
            WaitingTimeInt = WaitingTime >> 7;    
            if(WaitingTimeInt < 0){WaitingTimeInt = 32767;} //no negative values, max wait 69 min 54 sec, 
            WriteArray(-30008,WaitingTimeInt);              //Error when trying to wait longer than 2h33m

      }else{
            WriteArray(Ypos,Ypos);
      }
      WriteArray(-32000,0); //marking the end of a Sequence (Array)
//why go to initial position?
//      StepNr = 2;                                 //step where the initial position is written
//      ReadStepData(StepNr);      					       //read initial position
//      Move2YZ(StepDataC1,StepDataC2,Tmoderate);  //go to initial position
      StopRecTransition = false;
  }else{                                 //NO TRANSITION BETWEEN MODES
//=================================================================================================================      
      if(ManualMode == Requested){              //This if else if construction results in performing a transition 
                                         //before entering a while loop for the current Mode (Play, Rec, Manual)
          State = 15;                         //enter manual mode while waiting										
          while(ManualMode == Requested){           //while Rec / Play switch in Manual position
              ReadDCState();                   //Read Direction Control input pins and calculate State
              SetLastIfStart();							   
              CondPulseAndUpdatePos();         //give pulses if no EndSwitch is active And update position
              VelPotU = analogRead(VelPotPin); //in the Manual mode the speed can be controlled
              Tperiod = CalcVel(VelPotU);
              digitalWrite(PulseYPin,LOW);
              digitalWrite(PulseZPin,LOW);
              Wait4Tperiod(Tperiod);            
              CheckModeTransition();           //End one Mode and Start an other Mode if requested
              if(digitalRead(HomePin)==Requested){
                  Move2YZ(0,0,Tmoderate);
              }
          } 
//==================================================================================================================          
      }else if(PlayMode == Requested){           //This if else if construction results in performing a transition 
                                            //before entering a while loop for the current Mode (Play, Rec, Manual)
			    while(digitalRead(StartPin) != Requested & PlayMode == Requested){  //now wait for the start button to be pressed
			       CheckModeTransition();           //keep an eye on these switches     
			    }
          while(PlayMode == Requested){
              DecodeNextSteps();          //read from Array until all actions and waitingTime OR movement are available
              if(WaitingEvent == true){
                
              }
              if(MovingEvent == true){
                
              }

            
/*a CheckModeTransition should be included, perhaps in several loops
CheckModeTransition();   
a test on the Next button
after the last step from the Array, go to starting position 
the next line must be entered after finishing a sequence and after a moveYZ to the initial position of the sequence */

              if(LastEvent == true){
                  StepNr = 2;
                  ReadStepData(StepNr);    //read initial position of the sequence
                  Move2YZ(StepDataC1,StepDataC2,1500);      //move to inital position
					        break;    //exit the while loop: from main loop: no Transition, still PlayMode! 
					                  //(resulting in waiting for start)
              }
        }
//==================================================================================================================        
      }else if(RecMode == Requested){            //This if else if construction results in performing a transition 
                                            //before entering a while loop for the current Mode (Play, Rec, Manual)       
          OldState = 15;  									 
          while(RecMode == Requested){           //while Rec / Play switch in Manual position
              ReadDCState();                   //Read Direction Control input pins and calculate State
              EventHandling();
              CondPulseAndUpdatePos();         //give pulses if no EndSwitch is active And update position
              VelPotU = analogRead(VelPotPin); //in the Manual mode the speed can be controlled
              Tperiod = CalcVel(VelPotU);
              digitalWrite(PulseYPin,LOW);
              digitalWrite(PulseZPin,LOW);
              Wait4Tperiod(Tperiod);            
              CheckModeTransition();           //End one Mode and Start an other Mode if requested
          }
      } 
  }
}


void DecodeNextSteps(){
    AllInfoAfailable = false;
    ActionPointerWrite = -1;
    ActionPointerRead = 0;
    ActionsDuringStep = false;
    MovingEvent = false;
    WaitingEvent = false;
    LastEvent = false;
    do{
        ReadStepData(StepNr);
        if(StepDataC1 == -30008){         //waiting event
            
            WaitingEvent = true;
            AllInfoAfailable = true;
        }
        if(StepDataC1 == -30004){         //new Tperiod
          
        }
        if(StepDataC1 == -32000){         //end of sequence
            //read initial position
            LastEvent == true;
            AllInfoAfailable = true;  
        }
        if(StepDataC1 < 0 & StepDataC1 > -501){   //Action,(500 maximum)
            ActionsDuringStep = true; 
            ActionPointerWrite = ActionPointerWrite + 1;
            
        }
        if(StepDataC1 >= 0){              //position

            MovingEvent = true;
            AllInfoAfailable = true;
        }
        
    }while(AllInfoAfailable == false); 
}

//this function changes the values of Last, and calls: CondPulseAndUpdate, Wait4Tperiod
//it uses Ypos en Zpos
//move to (Y,Z) with 'speed' T
void Move2YZ(int Y,int Z, int T){
  int DeltaY;
  int DeltaZ;
  int dY = 1;
  int dZ = 1;
  int i;
  DeltaY = Y - Ypos;
  DeltaZ = Z - Zpos;
  if(DeltaY <0){dY = -1; Yinc = !Requested; Ydec = Requested;}
  if(DeltaZ <0){dY = -1; Zinc = !Requested; Zdec = Requested;}
//  Tperiod = T;
  i = max((abs(DeltaY)),(abs(DeltaZ)));
  Last = micros()+100;					  
  for(i;i>0;i--){
      if(Ypos == Y){Yinc = !Requested; Ydec = !Requested;} 
      if(Zpos == Z){Zinc = !Requested; Zdec = !Requested;} 
      CondPulseAndUpdatePos();
      delayMicroseconds(100);
      digitalWrite(PulseYPin,LOW);
      digitalWrite(PulseZPin,LOW);
      if(ActionsDuringStep == true){
                                          //CheckActionTime()......        
      }
      Wait4Tperiod(T);         
  } 
}

//An event occurse when OldState is unequal to State, no event: nothing happens
//changes the values of: TStartEvent, Tperiod_changed set to false, Last, possibly Waiting4_1stEvent
//possibly TStartWaiting. Depends also on Tperiod_changed
//writes values to the Array
void EventHandling(){
    if(Waiting4_1stEvent == false){         //action is never the first event, this is prohibeted
        if(ButtonPressed == true){          //Button pressed event, (tested in Waiting4Tperiod)
            ActionTime = ActionTime - TStartEvent;  //timing in millis
            ActionTimeInt = ActionTime >> 7;        //scaling like waiting timeing
            if(ActionTimeInt < 0){ActionTimeInt = 32767;} //Error....
            WriteArray(ActionNr,ActionTimeInt);
        }
    }
    if(OldState != State){              //only action when the State changes
        TStartEvent = millis();         //necessary to know in case of the action button
        if(OldState == 15){             //START, old state was waiting but possibly for the 1st event
            if(Waiting4_1stEvent == true){
                WriteArray(-30004,Tperiod);
                Tperiod_changed = false;
                Last = micros()+100;      //see comment SetLastIfStart										  																	  
                Waiting4_1stEvent = false;
            }else{                      //end of waiting
                WaitingTime = millis() - TStartWaiting;
                WaitingTimeInt = WaitingTime >> 7;    
                if(WaitingTimeInt < 0){WaitingTimeInt = 32767;} //no negative values, max wait 69 min 54 sec, 
                                                                //Error: waiting time saturated at max value
                WriteArray(-30008,WaitingTimeInt);              //Error when trying to wait longer than 2h33m
                if(Tperiod_changed == true){    //only write T if changed
                    WriteArray(-30004,Tperiod);
                    Tperiod_changed = false;
                }
                Last = micros()+100;      //see comment SetLastIfStart																	  
            }
        }else if(State == 15){          //STOP and start waiting, write position 
            TStartWaiting = millis();
            WriteArray(Ypos,Zpos);
        }else{                          //change of direction write position (and speed?)
            if(Tperiod_changed == true){    //only write T if changed
                WriteArray(-30004,Tperiod);
                Tperiod_changed = false;
            }
            WriteArray(Ypos,Zpos);
        }
    }
}


//Last is the time the last pulse was started (begin of a Tperiod interval)
//Since Last is incremented with Tperiod each period it needs to be set
//at the beginning of a movement. By adding 100 the first step will be somewhat
//slower (instead of quicker)!
void SetLastIfStart(){
    if(OldState != State){              //only action when the State changes
          if(OldState == 15){
              Last = micros()+100;    //how much longer should this be best?
          }
    }
}


//it seems it is not possible to pass an Array from a function
//I keep using all these global variables...
//StepDataC1 and StepDataC2 are updated
//StepNr is incremented each time data is read
void ReadStepData(int SeqStepNr){
     int index;
     index = (SeqStepNr - 1) * 2;
     index = constrain(index,0,SEQlength);  //just to be shure nothing go's (really) wrong
     StepDataC1 = SEQ[index];
     StepDataC2 = SEQ[index + 1];
     StepNr = StepNr + 1;
}


//column 1 and column 2 of the Array
//StepNr is incremented each time data is written
void WriteArray(int C1, int C2){
    int index;
    index = (StepNr - 1) * 2;
    index = constrain(index,0,SEQlength);   //just to be shure nothing go's (really) wrong
    SEQ[index] = C1;
    SEQ[index+1] = C2;
    StepNr = StepNr + 1;
}

//write a function ExecuteStep(step) to be called in PlayMode. 
//Action: output data
//set speed, wait or move
//end: move to starting point, initialize step, set speed 

//Reading RecMode and PlayMode, deriving ManualMode
//checking mode transitions
//setting the boolians for distinction of modes (Rec, Play, Manual) AND
//booleans for the 4 allowed mode transitions
void CheckModeTransition(){
  RecModeRequested = digitalRead(RecPin);                   //which Mode requested?
  PlayModeRequested = digitalRead(PlayPin);
  if(RecModeRequested == !Requested && PlayModeRequested == !Requested){
    ManualModeRequested = Requested;
  }else{
    ManualModeRequested = !Requested;                        //now requested Mode is known
  }
  if(RecModeRequested == Requested && PlayMode == Requested){    //first test if the user tries
    ManualMode = Requested;                                         //a transition from Play to Rec
    PlayMode = !Requested;                                       //at once, this will be handled
    RecMode = !Requested;                                        //unnassasary but more readable code
    StopPlayTransition = true;                              //in two transitions
  }else if(PlayModeRequested == Requested && RecMode == Requested){    //idem for Rec to Play
    ManualMode = Requested;
    RecMode = !Requested;
    PlayMode = !Requested;
    StopRecTransition = true;
  }else{                                    //now transitions to or from ManualMode
    if(RecMode != RecModeRequested){        //Rec mode change requested
      if(RecModeRequested == Requested){         //Rec mode requested
        StartRecTransition = true;
        RecMode = Requested;
        ManualMode = !Requested;
        PlayMode = !Requested;
      }else{
        StopRecTransition = true;           //Stop Rec: Manual mode requested
        RecMode = !Requested;
        ManualMode = Requested;
        PlayMode = !Requested;
      }
    }
    if(PlayMode != PlayModeRequested){      //Play mode change requested
      if(PlayModeRequested == Requested){        //Play mode requested
        StartPlayTransition = true;
        PlayMode = Requested;
        ManualMode = !Requested;
        RecMode = !Requested;
      }else{
        StopPlayTransition = true;          //Manual mode requested
        PlayMode = !Requested;
        ManualMode = Requested;
        RecMode = !Requested;
      }
    }    
  }
}

//Wait for the end of a Period T_lapse as passes to the function
//depends on Last, updates Last										  
void Wait4Tperiod(int T_lapse){
    Waiting4T = true;
    while(Waiting4T == true){       //keep pulsing with Tperiod
        if((micros() - Last) >= T_lapse){
             Waiting4T = false;
             Last = Last + T_lapse; //If Last is set long time ago: waiting time almost zero
        }                           //this is a risk! But setting Last together with the pulse
                                    //leads to jitter and differences between Play, Rec and Manual
    CheckActionButton();            //tested here to prevend slow checking frequency
    }                                    
}

void CheckActionButton(){
    ButtonOld = Button;
    Button = digitalRead(ActionPin);
    ButtonPressed = false;
    if(ButtonOld != Button & Button == LOW){  //react on downslope
        ButtonPressed = true;
        ActionTime = millis();    //remember the timing
        ActionNr = ActionNr - 1;  //counted in negative numbers
    }
}


void ReadDCState(){                 //Read Direction Control input pins 
  OldState = State;
  Yinc = digitalRead(YincPin);      //4 times switch positions
  Ydec = digitalRead(YdecPin);
  Zinc = digitalRead(ZincPin);
  Zdec = digitalRead(ZdecPin); 
  Ymax = digitalRead(YmaxPin);      //4 times EndSwitches
  Ymin = digitalRead(YminPin);
  Zmax = digitalRead(ZmaxPin);
  Zmin = digitalRead(ZminPin);
  State = Yinc + 2 * Ydec + 4 * Zinc + 8 * Zdec;
}

//
int CalcVel(int U){
  int T;
  TperiodOld = Tperiod;
  T = 1277913UL / (U+39);   //T max = 32767, T min = 1203
  T = 1203;//TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  if(Tperiod_changed == false){                   //reset in event handeling
    if(T != TperiodOld){Tperiod_changed = true;}  //set a flag when Tperiod changes
  }			                                          //check if noise contantly changes T																			  
  return T;  
}

//depending on Yinc, Ydec, Zinc and Zdec and the endswitches Ymax, Ymin, Zmax and Zmin
//all of these read in ReadDCstate
void CondPulseAndUpdatePos(){                       //motor pulses if EndSwitches NOT operated
  if(Yinc == Requested && Ymax == NoEndSwitch){       //AND update the position
    digitalWrite(DirYPin,HIGH);
    digitalWrite(PulseYPin,HIGH); 
    Ypos = Ypos + 1; 
  }  
  if(Ydec == Requested && Ymin == NoEndSwitch){
    digitalWrite(DirYPin,LOW);
    digitalWrite(PulseYPin,HIGH); 
    Ypos = Ypos - 1; 
  }  
  if(Zinc == Requested && Zmax == NoEndSwitch){
    digitalWrite(DirZPin,HIGH);
    digitalWrite(PulseZPin,HIGH);
    Zpos = Zpos + 1;  
  }  
  if(Zdec == Requested && Zmin == NoEndSwitch){
    digitalWrite(DirZPin,LOW);
    digitalWrite(PulseZPin,HIGH);  
    Zpos = Zpos - 1;
  }			 
}
