#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

int in1 = 14;
int in2 = 12;
int in3 = 13;
int in4 = 15;

const int ProxVoor= 16; // verander de pin nog
const int ProxLinks= 10; // verander de pin nog
const int ProxRechts= A0; // verander de pin nog
const int pinSwitch = 1;
const int trigPin = 5;
const int echoPin = 4;

int irArray [3];
long duration;
int distance;

int peoplefinder = 0;
int autonoom = 0;
int StatoSwitch = 0;

char lijnkant = 'n';

MDNSResponder mdns;
 
ESP8266WebServer server(80);
//hier vullen we de gegevens in van het wifi waar mee we willen verbinden
String webPage;
const char* ssid     = "Tesla IoT";      //wifi name
const char* password = "fsL6HgjN";  //wifi password
 
void setup() {

pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
pinMode(pinSwitch, INPUT_PULLUP);
pinMode(ProxLinks,INPUT);
pinMode(ProxVoor,INPUT);
pinMode(ProxRechts,INPUT);

 pinMode(in1, OUTPUT);
 pinMode(in2, OUTPUT);
 pinMode(in3, OUTPUT);
 pinMode(in4, OUTPUT);

 //hier staat wat op de pagina te voor schijn moet komen om te zorgen dat er knoppen komen om de auto te besturen
  webPage += "<h1>ESP8266 Web Server</h1><p>Afstandsbediening</p>";
  webPage += "<a href=\"socket1W\">&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp<button>w</button></a></p>";
  webPage += "<a href=\"socket1A\"><button>a</button></a>&nbsp&nbsp&nbsp&nbsp";
  webPage += "<a href=\"socket1S\"><button>s</button></a>&nbsp&nbsp&nbsp&nbsp";
  webPage += "<a href=\"socket1D\"><button>d</button></a></p>";
  webPage += "<a href=\"socket1STOP\">&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp<button>STOP</button></a></p>";
  webPage += "<h1></h1><p>Turns</p>";
  webPage += "<a href=\"socket1TURNL\"><button>links</button></a>&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
  webPage += "<a href=\"socket1TURNR\"><button>rechts</button></a></p>";
  webPage += "<h1></h1><p>Autonoom</p>";
  webPage += "<a href=\"socket1AAN\"><button>aan</button></a>&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp";
  webPage += "<a href=\"socket1UIT\"><button>uit</button></a></p>";
  
  Serial.begin(115200);
  delay(100);
 
 
  Serial.println();
  Serial.println();
  Serial.print("Verbinding maken met ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 //hier wordt gecontroleerd of het wifi is aangesloten en wordt er een ip adress weergegeven om verbinding mee te maken
  Serial.println("");
  Serial.println("WiFi aangesloten");  
  Serial.println("IP addres: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) 
    Serial.println("MDNS responder gestart");
 //hier stuur je de informatie naar de server om het erop te zetten
 //en wat er gedaan moet worden als erop de knop geklikt is
  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  server.on("/socket1W", [](){
    server.send(200, "text/html", webPage);
    
    forward();
  });
  
  server.on("/socket1A", [](){
    server.send(200, "text/html", webPage);
    
    left();
  });
    
  server.on("/socket1S", [](){
    server.send(200, "text/html", webPage);
   
    backwards();
  });
   
  server.on("/socket1D", [](){
    server.send(200, "text/html", webPage);
    
    right();

  });

  server.on("/socket1STOP", [](){
    server.send(200, "text/html", webPage);
    
    stoppen();

  });
    
    server.on("/socket1TURNL", [](){
    server.send(200, "text/html", webPage);
    
    turn90L();
  });

    server.on("/socket1TURNR", [](){
    server.send(200, "text/html", webPage);
    
    turn90r();
  });
    
    server.on("/socket1AAN", [](){
    server.send(200, "text/html", webPage);
    
    autonoom = 1;
  });

    server.on("/socket1UIT", [](){
    server.send(200, "text/html", webPage);
    
    autonoom = 0;
  });

  server.begin();
  Serial.println("HTTP server gestart");

}
 
void loop() {
  
  while (autonoom == 1){
    rijden();
    server.handleClient();
  }
  server.handleClient();
}

void rijden(){
 //we roepen hier eerst de sensoren op zodat we kunnen kijken of er mensen zijn
 sensoren();
 if (peoplefinder == 1){
  //als er mensen zijn zal de buzzer van de auto piepen want die staat aangesloten op de reeds
  stoppen();
  delay(3000);
  forward();
  delay(500);
 }
 else{
     //hier word gekeken naar de IR's om te zorgen dat de auto de lijnen zal gaan volgen
     //we hebben een lijnkant variable gemaakt om te helpen met bepalen welke kant hij op moet gaan in bochten
     //ook wordt er gekeken naar de ultrasoon om te zorgen dat die langs een obstakel kan rijden
     if (irArray[1] == 0 && irArray[2] == 0 && irArray[3] == 0){
      ultrasoon();
      if (distance < 10 && lijnkant == 'l'){
        backwards();
        delay(1000);
        turn90r();
        forward();
        delay(1800);
        turn90L();
      }
      else if (distance < 10 && lijnkant == 'r'){
        backwards();
        delay(1000);
        turn90L();
        forward();
        delay(1800);
        turn90r();
      }
      else{
        forward();
      }
     }
     else if (irArray[1] == 1 && irArray[2] == 0 && irArray[3] == 0){
      test();
      right();
      delay(200);
      forward();
      lijnkant = 'l';
     }
     else if (irArray[1] == 0 && irArray[2] == 0 && irArray[3] == 1){
      test();
      left();
      delay(200);
      forward();
      lijnkant = 'r';
     }
     else if (irArray[1] == 0 && irArray[2] == 1 && irArray[3] == 0 && lijnkant == 'l'){
      test();
      backwards();
      delay(1500);
      turn90L();
     }
     else if (irArray[1] == 0 && irArray[2] == 1 && irArray[3] == 0 && lijnkant == 'r'){
      test();
      backwards();
      delay(1500);
      turn90L();
     }
     else if (irArray[1] == 1 && irArray[2] == 1 && irArray[3] == 0){
      test();
      turn90r();
     }
     else if (irArray[1] == 0 && irArray[2] == 1 && irArray[3] == 1){
      test();
      turn90L();
     }
     else if (irArray[1] == 1 && irArray[2] == 0 && irArray[3] == 1){
      test();
      backwards();
     }
     else if (irArray[1] == 1 && irArray[2] == 1 && irArray[3] == 1 && lijnkant == 'r'){
      test();
      stoppen();
      delay(4000);
      backwards();
      delay(3000);
      turn90L();
     }
      else if (irArray[1] == 1 && irArray[2] == 1 && irArray[3] == 1 && lijnkant == 'l'){
      test();
      stoppen();
      delay(4000);
      backwards();
      delay(3000);
      turn90r();
     }
   }
}

void stoppen(){
  //hier worden de motoren uit gezet om de auto te laten stoppen
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW); 

 server.handleClient();
}

void forward(){
 //hier worden de motoren aangezet zodat ze naar voren gaan
 digitalWrite(in1, LOW);
 digitalWrite(in3, LOW);
 Serial.println("RIJDEN");
 digitalWrite(in2, HIGH);
 digitalWrite(in4, HIGH);
 
 server.handleClient();

}

void backwards(){
 //hier worden de motoren aangezet zodat ze naar achteren gaan
 digitalWrite(in2, LOW);
 digitalWrite(in4, LOW);
 Serial.println("RIJDEN");
 digitalWrite(in1, HIGH);
 digitalWrite(in3, HIGH);

 server.handleClient();
}

void left(){
  //hier worden de motoren aangezet zodat ze naar links gaan
  digitalWrite(in1, LOW);
  digitalWrite(in4, LOW);
  Serial.println("RIJDEN");
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);

 server.handleClient();
}

void right(){
  //hier worden de motoren aangezet zodat ze naar rechts gaan
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  Serial.println("RIJDEN");
  digitalWrite(in1, HIGH);
  digitalWrite(in4, HIGH);

  server.handleClient();
}

void turn90L(){
  //hier worden de motoren aangezet zodat de auto een hoek van 90graden naar links maakt
  digitalWrite(in1, LOW);
  digitalWrite(in4, LOW);
  Serial.println("RIJDEN");
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);

  delay(1535);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW); 
}

void turn90r(){
  //hier worden de motoren aangezet zodat de auto een hoek van 90graden naar rechts maakt
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  Serial.println("RIJDEN");
  digitalWrite(in1, HIGH);
  digitalWrite(in4, HIGH); 

  delay(1535);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);   

  
}


void sensoren(){
  //hier worden de sensoren gelezen en zo neer gezet dat we er waardes uit kunnen halen
  peoplefinder = 1- digitalRead(pinSwitch);
  irArray [1] =  digitalRead(ProxLinks);
  irArray [2] =  digitalRead(ProxVoor);
  int sensorir =  analogRead(ProxRechts);
  if (sensorir > 100){
    irArray [3] = 1;
  }
  else{
    irArray [3] = 0;
  } 
  
}
void ultrasoon(){
  //hier lezen we de ultrasoon uit en zorgen dat we een distance als waarde krijgen in cm
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance= duration*0.034/2;

}
