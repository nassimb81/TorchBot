//pin numbers currently randomly chosen
const int YincPin = 22; //Y+ (pin number)
const int YdecPin = 23; //Y-
const int ZincPin = 24; //Z+
const int ZdecPin = 25; //Z-
const int YmaxPin = 26; //end switches
const int YminPin = 27;
const int ZmaxPin = 28;
const int ZminPin = 29;
const int DirYPin = 30; //direction signal to y motors
const int PulseYPin = 31; //pulse signal to y motors
const int EnableY = 32;  //enable
const int DirZPin = 33; //direction signal to z motors
const int PulseZPin = 34; //pulse signal to z motors
const int EnableZ = 35;  //enable
const int RecPin = 36;  //from dashboard
const int PlayPin = 37; //idem
const int NextPin = 38; //idem
const int HomePin = 39; //idem
const int StartPin = 40; //idem
const int StopPin = 41;  //idem
const int Action = 42;   //idem

const int VelPotPin = A0; //input pin for analog velocity signal

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

unsigned long int Last;//timestamp of the timer: last pulse time

long int Ypos; //-2,147,483,648 to 2,147,483,647, should be enough UNSIGNED INT: 0 to 65,535 might be a better idea?????
long int Zpos;
int YposInt;
int ZposInt;

int VelPotU;  //read from analog input
int Tperiod;  //pulse timing

int RecModeRequested; //reading from RecPin
int PlayModeRequested; //reading form PlayPin
int NullModeRequested; //derived from the upper 2 as an exception

int HomeRequested;
int StartRequested;
int NextRequested;

bool NullMode;  //the system always is in one of these modes as a result of CheckModeTransition()
bool PlayMode;  //idem
bool RecMode;   //idem
bool StopRecTransition; //the four possible (allowed) mode transitions
bool StopPlayTransition;
bool StartRecTransition;
bool StartPlayTransition;
bool Waiting4T; //for timing of the motor pulses
  
void setup() {
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
  pinMode(EnableYpin,OUTPUT);
  pinMode(DirZPin,OUTPUT);
  pinMode(PulseZPin,OUTPUT);
  pinMode(EnableZpin,OUTPUT);
  pinMode(RecPin,INPUT_PULLUP);
  pinMode(PlayPin,INPUT_PULLUP);
  pinMode(NextPin,INPUT_PULLUP);
  pinMode(HomePin,INPUT_PULLUP);
  pinMode(StartPin,INPUT_PULLUP);
  pinMode(StopPin,INPUT_PULLUP);
  pinMode(ActionPin,INPUT_PULLUP);
  NullMode = true;
  RecMode = false;
  PlayMode = false;
  //move in plus dir until no endswitch, move back until endswitch, move in plus for 0.5 mm, set position to zero (Home)
  Serial.begin(9600);
}

//{}

void loop() {
  //allways start testing if a transition between Modes has to be carried out
  if(StartPlayTransition == true){
      //code to be executed until transition completed
      //get array from RespPi (ALLWAYS?: no stand alone operation possible....,but how to solve this elegantly? (timeout?)
	  //go to startpoint (with moderate speed), set speed to value from Array
      StartPlayTransition = false;
  }else if(StartRecTransition == true){
      //code to be executed until transition completed
      //clear Array, write speed and initial position
      StartRecTransition = false;
  }else if(StopPlayTransition == true){
      //code to be executed until transition completed
      StopPlayTransition = false;
  }else if(StopRecTransition == true){
      //code to be executed until transition completed
      //write array to RespPi
      StopRecTransition = false;
  }else{                                 //NO TRANSITION BETWEEN MODES
      Last = millis();                   //as if the forelast pulse was given at this moment
      
      if(NullMode == true){              //This if else if construction results in performing a transition 
                                         //before entering a while loop for the current Mode (Play, Rec, Null)
          while(NullMode == true){              //while Rec / Play switch in Null position
              ReadDCState();                   //Read Direction Control input pins
              CondPulseAndUpdatePos();         //give pulses if no EndSwitch is active And update position
              VelPotU = analogRead(VelPotPin); //in the Null mode the speed can be controlled
              Tperiod = CalcVel(VelPotU);
              CheckModeTransition();           //End one Mode and Start an other Mode if requested
                 //possibly add some delay if pulse high time is to short for the motordriver....
              digitalWrite(PulseYPin,LOW);
              digitalWrite(PulseZPin,LOW);
              Waiting4T = true;
              while(Waiting4T == true){       //keep pulsing with Tperiod
                  if((millis() - Last) >= Tperiod){
                      Waiting4T = false;
                      Last = Last + Tperiod;
                  }
                  //perform tests or whatever needs to be done continuesly
              }
              if(digitalRead(HomePin)==Requested){
                  Yinc = !Requested; Ydec = Requested; Zinc = !Requested; Zdec = Requested; //home direction
                  for(int i = max(Ypos,Zpos);i>0;i--){
                        if(Ypos==0){Ydec = !Requested;}   //stop moving when home position reached
                        if(Zpos==0){Zdec = !Requested;}   //idem
                        CondPulseAndUpdatePos();         //give pulses if no EndSwitch is active And update position
                  }
              }
          } 
          
      }else if(PlayMode == true){           //This if else if construction results in performing a transition 
                                            //before entering a while loop for the current Mode (Play, Rec, Null)
			//now wait for the start button to be pressed
          while(PlayMode == true){
            //code for the PlayMode
            //a CheckModeTransition should be included
            //a test on the Next button
			//after the last step from the Array, wait for Start button while also performing CheckModeTransition
          }
           
      }else if(RecMode == true){            //This if else if construction results in performing a transition 
                                            //before entering a while loop for the current Mode (Play, Rec, Null)       
          while(RecMode == true){
            //code for the RecMode
			//het kan zijn dat het begin van een beweging tegelijk het eind van een beweging is (bv start z tijdens y beweging)
			//dan moet er dus naar het Array geschreven (v? en y en z)
			//of het is het eind van een pauze en dan moet t-tstart worden weggeschreven
			//bij einde beweging: noteer de tijd en schrijf v(?) en y en z naar Array
			//het eerste event moet zijn als start beweging tijdens een beweging (we willen geen pauze aan het begin)
		    //check the ActionButton, write action number to Array, no data
            //a CheckModeTransition should be included
          }
      } 
  }
}

//write a function stepdata=ReadArray(step), stepdata being an array with 2 elements (int)

//write a function ExecuteStep(step) to be called in PlayMode. 
//Action: output data
//set speed, wait or move
//end: move to starting point, initialize step, set speed 

//there are 4 transitions allowed: Null to Play or Rec and vice versa
//first the requested Mode are read or derived
//transitions from Rec to Play and vice versa are degraded to transition to Null
//the next CheckModeTransition will perform the rest of the transition
//THIS FUNCTION SHOULD NOT BE CALLED DURING A TRANSITION
//ASK NASSIM IF A TEST SHOULD BE ADDED TE PREVEND FUTURE PROBLEMS
//{}
void CheckModeTransition(){
  RecModeRequested = digitalRead(RecPin);                   //which Mode requested?
  PlayModeRequested = digitalRead(PlayPin);
  if(RecModeRequested == LOW && PlayModeRequested == LOW){
    NullModeRequested = HIGH;
  }else{
    NullModeRequested = LOW;                                //now requested Mode is known
  }
  if(RecModeRequested == true && PlayMode == true){          //first test if the user tries
    NullMode = true;                                         //a transition from Play to Rec
    PlayMode = false;                                       //at once, this will be handled
    RecMode = false;                                        //unnassasary but more readable code
    StopPlayTransition = true;                              //in two transitions
  }else if(PlayModeRequested == true && RecMode == true){    //idem for Rec to Play
    NullMode = true;
    RecMode = false;
    PlayMode = false;
    StopRecTransition = true;
  }else{                                    //now transitions to or from NullMode
    if(RecMode != RecModeRequested){        //Rec mode change requested
      if(RecModeRequested == true){         //Rec mode requested
        StartRecTransition = true;
        RecMode = true;
        NullMode = false;
        PlayMode = false;
      }else{
        StopRecTransition = true;           //Null mode requested
        RecMode = false;
        NullMode = true;
        PlayMode = false;
      }
    }
    if(PlayMode != PlayModeRequested){      //Play mode change requested
      if(PlayModeRequested == true){        //Play mode requested
        StartPlayTransition = true;
        PlayMode = true;
        NullMode = false;
        RecMode = false;
      }else{
        StopPlayTransition = true;          //Null mode requested
        PlayMode = false;
        NullMode = true;
        RecMode = false;
      }
    }    
  }
}

void ReadDCState(){                 //Read Direction Control input pins 
  Yinc = digitalRead(YincPin);      //4 times switch positions
  Ydec = digitalRead(YdecPin);
  Zinc = digitalRead(ZincPin);
  Zdec = digitalRead(ZdecPin); 
  Ymax = digitalRead(YmaxPin);      //4 times EndSwitches
  Ymin = digitalRead(YminPin);
  Zmax = digitalRead(ZmaxPin);
  Zmin = digitalRead(ZminPin); 
}

int CalcVel(int U){
  int T;
  T = 500 + U/2;   //bijvoorbeeld 
  return T;  
}

//We ronden voor het Array gewoon af zonder poespas op 20ste van mm (na optellen 40ste): kleine onnauwkeurigheid, lekker overzichtelijk
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
  if(Zdec == Requested && Ydec == NoEndSwitch){
    digitalWrite(DirZPin,LOW);
    digitalWrite(PulseYPin,HIGH);  
    Zpos = Zpos - 1;
  }
}
