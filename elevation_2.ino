//ARDUINO 1.0+ ONLY
//ARDUINO 1.0+ ONLY
#include <Ethernet.h>
#include <EthernetClient.h>
#include <SPI.h>


////////////////////////////////////////////////////////////////////////
//CONFIGURE
////////////////////////////////////////////////////////////////////////
//byte server[] = {5, 135, 188, 103}; 

byte server[] = {213, 186, 33, 87}; 
// 213.186.33.87 / we love the net ip address
// 88, 190, 253, 247 / albertine meunier ip address

//The location to go to on the server
//make sure to keep HTTP/1.0 at the end, this is telling it what type of file it is
String location = "/elevation/altitude.php?user=2 HTTP/1.0";

// if need to change the MAC address (Very Rare)
byte mac[] = {0xAA, 0xBB, 0xCC, 0xDE, 0x02,0x01};
////////////////////////////////////////////////////////////////////////

EthernetClient client;

char inString[32]; // string for incoming serial data
int stringPos = 0; // string index counter
boolean startRead = false; // is reading?
int led = 5;
int inPin = 7;
int val = 0;
int initialisation = 1;
int firstTime = 1;
int cur_alt = 0;
int delta_alt;
long mv;



#define DIR_PIN 2
#define STEP_PIN 3


void setup()
{
  Serial.begin(9600);
  pinMode(DIR_PIN, OUTPUT); 
  pinMode(STEP_PIN, OUTPUT); 
  pinMode(led, OUTPUT);  
  pinMode(inPin, INPUT); 
  Ethernet.begin(mac);
  
 
}

void loop(){
 Serial.println("get data from API");
 String pageValue = connectAndRead(); //connect to the server and read the output
 delay(5000); //wait 10 seconds before connecting again
}

 
String connectAndRead(){

  Serial.println("connecting...");
    if (firstTime) {
      location = "/elevation/altitude.php?user=2&init=1 HTTP/1.0";
    }else {
       location = "/elevation/altitude.php?user=2&init=0 HTTP/1.0";
    }
    Serial.println(location);

  //port 80 is typical of a www page
  if (client.connect(server, 80)) {
    Serial.println("connected");
    client.print("GET ");
    client.println(location);
    client.println("Host: we-love-the.net");
    client.println();
    return readPage(); //go and read the output
   }else{
    return "connection failed";
  }

}

String readPage(){
  //read the page, and capture & return everything between '<' and '>'

  stringPos = 0;
  memset( &inString, 0, 32 ); //clear inString memory

  while(true){

    if (client.available()) {
      char c = client.read();

      if (c == '<' ) { //'<' is our begining character
        startRead = true; //Ready to start reading the part 
      }else if(startRead){

        if(c != '>'){ //'>' is our ending character
          inString[stringPos] = c;
          stringPos ++;

        }else{
       
          startRead = false;
          client.stop();
          client.flush();
          float ret = atof(inString);
          
          if (!firstTime) {
             Serial.print(ret,0);
             Serial.println(" return value");
             delta_alt = ret;
             cur_alt = cur_alt + delta_alt;
             mv = delta_alt;
             Serial.print(cur_alt);
             Serial.println(" valeur courante ");
           }else {
              Serial.print(ret,0);
              Serial.println(" metres altitude albertine");
              cur_alt = ret;
              mv = ret;
           }
         rotate(mv*600, .5); 
         Serial.print(mv*600);
         Serial.println(" mv ");
         delay(10000);   
         Serial.print("et (deconnecting) voila ");
         firstTime = 0;
         return inString;
         
        }

      }
    }

  }

}

void rotate(int steps, float speed){ 
  //rotate a specific number of microsteps (8 microsteps per step) - (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (steps > 0)? HIGH:LOW;
  steps = abs(steps);

  digitalWrite(DIR_PIN,dir); 

  float usDelay = (1/speed) * 70;

  for(int i=0; i < steps; i++){ 
    digitalWrite(STEP_PIN, HIGH); 
    delayMicroseconds(usDelay); 

    digitalWrite(STEP_PIN, LOW); 
    delayMicroseconds(usDelay); 
  } 
} 

void rotateDeg(float deg, float speed){ 
  //rotate a specific number of degrees (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (deg > 0)? HIGH:LOW;
  digitalWrite(DIR_PIN,dir); 

  int steps = abs(deg)*(1/0.225);
  float usDelay = (1/speed) * 70;

  for(int i=0; i < steps; i++){ 
    digitalWrite(STEP_PIN, HIGH); 
    delayMicroseconds(usDelay); 

    digitalWrite(STEP_PIN, LOW); 
    delayMicroseconds(usDelay); 
  } 
}
