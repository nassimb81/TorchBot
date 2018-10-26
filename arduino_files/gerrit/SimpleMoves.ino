//pin numbers currently randomly chosen
const int YincPin = 22; //Y+ (pin number)
const int YdecPin = 23; //Y-
//const int YincPin = 4; //Y+ (pin number) pullup werkt niet op de usb voeding van de PC
//const int YdecPin = 5; //Y-
const int ZincPin = 24; //Z+
const int ZdecPin = 25; //Z-
const int YmaxPin = 26; //end switches
const int YminPin = 27;
const int ZmaxPin = 28;
const int ZminPin = 29;
const int DirYPin = 30; //direction signal to y motors
const int PulseYPin = 31; //pulse signal to y motors
const int EnableYpin = 32;  //enable
const int DirZPin = 33; //direction signal to z motors
const int PulseZPin = 34; //pulse signal to z motors
const int EnableZpin = 35;  //enable
const int RecPin = 36;  //from dashboard
const int PlayPin = 37; //idem
const int NextPin = 38; //idem
const int HomePin = 39; //idem
const int StartPin = 40; //idem
const int StopPin = 41;  //idem
const int ActionPin = 42;   //idem

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
  digitalWrite(EnableYpin,HIGH);
  Serial.begin(9600);
}

//{}

void loop() {
  ReadDCState();      //read de direction control state (operation- and end-switches)
//  Serial.print("Yinc: ");
//  Serial.print(Yinc);
//  Serial.print("  Ydec: ");
//  Serial.println(Ydec);
  CondPulseAndUpdatePos();
  delayMicroseconds(1000);
//  delay(50);
  digitalWrite(PulseYPin,LOW);
  delayMicroseconds(1000);
//  delay(50);
}


void ReadDCState(){                 //Read Direction Control input pins 
  Yinc = digitalRead(YincPin);      //2 times switch positions
  Ydec = digitalRead(YdecPin);
  Ymax = digitalRead(YmaxPin);      //2 times EndSwitches
  Ymin = digitalRead(YminPin);
}

int CalcVel(int U){
  int T;
  T = 500 + U/2;   //bijvoorbeeld 
  return T;  
}

////We ronden voor het Array gewoon af zonder poespas op 20ste van mm (na optellen 40ste): kleine onnauwkeurigheid, lekker overzichtelijk
//void CondPulseAndUpdatePos(){                       //motor pulses if EndSwitches NOT operated
//  if(Yinc == Requested && Ymax == NoEndSwitch){       //AND update the position
//    digitalWrite(DirYPin,HIGH);
//    digitalWrite(PulseYPin,HIGH); 
//    Ypos = Ypos + 1; 
//  }  
//  if(Ydec == Requested && Ymin == NoEndSwitch){
//    digitalWrite(DirYPin,LOW);
//    digitalWrite(PulseYPin,HIGH); 
//    Ypos = Ypos - 1; 
//  }  
//}

//We ronden voor het Array gewoon af zonder poespas op 20ste van mm (na optellen 40ste): kleine onnauwkeurigheid, lekker overzichtelijk
void CondPulseAndUpdatePos(){                       //motor pulses if EndSwitches NOT operated
  if(Yinc == Requested){       //AND update the position
    digitalWrite(DirYPin,HIGH);
    digitalWrite(PulseYPin,HIGH); 
    Ypos = Ypos + 1; 
//        Serial.print("Yinc requested");
//        Serial.println(Ypos);
  }  
  if(Ydec == Requested){
    digitalWrite(DirYPin,LOW);
    digitalWrite(PulseYPin,HIGH); 
    Ypos = Ypos - 1; 
//        Serial.print("Ydec requested");
//        Serial.println(Ypos);
  }  
}
