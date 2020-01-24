#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

int in1 = 14;
int in2 = 12;
int in3 = 13;
int in4 = 15;

MDNSResponder mdns;
 
ESP8266WebServer server(80);
//hier vullen we de gegevens in van het wifi waar mee we willen verbinden
String webPage;
const char* ssid     = "Tesla IoT";      //wifi name
const char* password = "fsL6HgjN";  //wifi password
 
void setup() {

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
    
    
  });

    server.on("/socket1UIT", [](){
    server.send(200, "text/html", webPage);
    
    
  });

    server.begin();
    Serial.println("HTTP server gestart");

}
 
void loop() {
  //deze loop is om terug te kijken naar de server
  server.handleClient();
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

void right(){
  //hier worden de motoren aangezet zodat ze naar rechts gaan
  digitalWrite(in1, LOW);
  digitalWrite(in4, LOW);
  Serial.println("RIJDEN");
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);

 server.handleClient();
}

void left(){
  //hier worden de motoren aangezet zodat ze naar links gaan
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  Serial.println("RIJDEN");
  digitalWrite(in1, HIGH);
  digitalWrite(in4, HIGH);

  server.handleClient();
}

void turn90r(){
  //hier worden de motoren aangezet zodat de auto een hoek van 90graden naar rechts maakt
  digitalWrite(in1, LOW);
  digitalWrite(in4, LOW);
  Serial.println("RIJDEN");
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);

  delay(1600);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW); 
}

void turn90L(){
  //hier worden de motoren aangezet zodat de auto een hoek van 90graden naar links maakt
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  Serial.println("RIJDEN");
  digitalWrite(in1, HIGH);
  digitalWrite(in4, HIGH); 

  delay(1700);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);   

  
}
