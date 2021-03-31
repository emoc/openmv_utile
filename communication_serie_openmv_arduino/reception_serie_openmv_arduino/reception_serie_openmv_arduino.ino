/*
    Récupérer un message série envoyé par openMV
      sur un port série software de l'arduino (le retransmettre sur le port série hardware arduino)
      la réception des messages série est non bloquante

    Workshop Drone Ecole de Design Nantes Atlantique / 29 mars - 2 avril 2021
    Arduino 1.8.5 @ kirin, Debian 9.5 Stretch
      + lib. SoftwareSerial 1.0 http://www.arduino.cc/en/Reference/SoftwareSerial


    circuit, doc : https://github.com/emoc/openmv_utile

    Les messages sont formatés de cette manière : a,b,c\n
       a : id du tag
       b : position en x du tag (entre 0 et 160)
       c : position en y du tag (entre 0 et 120, 0 en haut)

    Brochage
       OpenMV Cam GND          ----> Arduino GND
       OpenMV Cam UART3_TX(P4) ----> Arduino Uno SoftwareSerial RX (8)
       OpenMV Cam UART3_RX(P5) ----> Arduino Uno SoftwareSerial RX (9)

*/

#include <SoftwareSerial.h>

// Déclarer les broches du port série logiciel
SoftwareSerial mySerial(8, 9); // RX, TX

// longueur maximale d'un message série (avant le caractère de fin de ligne)
const unsigned int MAX_INPUT = 50; 

// Variables liées aux données de marqueurs reçues
boolean changed = false; // signal pour indiquer la réception de nouvelles données 
int idvalue = 0;         // identifiant du marqueur
int xvalue  = 0;         // position en x
int yvalue  = 0;         // position en y (0 en haut)


void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  delay(500);
  pinMode(13, OUTPUT);  // utiliser la LED interne
}


void loop() {

  // LED clignotante pour vérifier que le code est bien non-bloquant
  if (millis() % 2000 > 1000) digitalWrite(13, HIGH);
  else digitalWrite(13, LOW);

  // Traiter les données série si il y en a
  while (mySerial.available() > 0) processIncomingByte (mySerial.read());

  // Si de nouvelles données ont été reçues, les afficher sur le terminal série
  if (changed) {
    Serial.print("id : ");
    Serial.print(idvalue);
    Serial.print(", x : ");
    Serial.print(xvalue);
    Serial.print(", y : ");
    Serial.print(yvalue);
    Serial.println("");
    changed = false;
  }

}

// Extraire des sous-chaînes de caractère séparées par un caractère
// d'après https://arduino.stackexchange.com/a/1237

String getValue(String data, char separator, int index) {
  
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


// Traiter les données une fois le caractère de fin de chaîne reçu

void process_data (const char * data) {
  
  String id   = getValue(data, ',', 0); // première partie du message
  String xval = getValue(data, ',', 1); // etc.
  String yval = getValue(data, ',', 2);

  idvalue = id.toInt();                 // convertir en entier
  xvalue  = xval.toInt();
  yvalue  = yval.toInt();
  changed = true;                       // signal de nouvelles données pour loop()

}  


// Traiter les caractères reçus un par un, jusqu'au caratère de fin de chaîne
// // d'après https://gammon.com.au/serial

void processIncomingByte (const byte inByte) {

  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;

  switch (inByte) {

    case '\n':   // end of text
      input_line [input_pos] = 0;  // terminating null byte
      process_data (input_line);   // terminator reached! process input_line here ...
      input_pos = 0;               // reset buffer for next time
      break;

    case '\r':                     // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1))
        input_line [input_pos++] = inByte;
      break;

  }  // end of switch

} // end of processIncomingByte



