#include <SD.h>
#include <Adafruit_BME280.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <ChainableLED.h>

#define boutonRouge 2
#define boutonVert 3
#define adresseEEPROM 0
#define ledPin1 6
#define ledPin2 7

File fichier;
Adafruit_BME280 bme;
RTC_DS3231 rtc;
ChainableLED led(ledPin1,ledPin2,1);

struct variables
{ long LOG_INTERVAL;
  int FILE_MAX_SIZE;
  byte TIMEOUT;
  byte LUMIN;
  int LUMIN_LOW;
  int LUMIN_HIGH;
  byte TEMP_AIR;
  int MIN_TEMP_AIR;
  int MAX_TEMP_AIR;
  byte HYGR;
  int HYGR_MINT;
  int HYGR_MAXT;
  byte PRESSURE;
  int PRESSURE_MIN;
  int PRESSURE_MAX;
}variables;

int mode=1;

long derniereExecutionStandard=0;
long derniereExecutionMaintenance=0;
long derniereExecutionEco=0;


String Mesure() {
  // Manque le timeout
  String message;
  char temps[10];
  sprintf(temps, "%02d:%02d.%02d  ", rtc.now().hour(), rtc.now().minute(), rtc.now().second());
  message = String(temps);
  message+="Temperature: "+String(bme.readTemperature())+"°C  ";
  message+=" Humidite: "+String(bme.readHumidity())+"%  ";
  if (mode!=4){
    message+=" Pression: "+String(bme.readPressure()/100)+" hPa ";
    message+=" Luminosite: "+String(analogRead(A0));
  }
  return message;
}
void Enregistrement(String message) {
  // Manque le file_max_size et revision
  DateTime date=rtc.now();
  int revision=0;
  char nomFichier[20];
  sprintf(nomFichier, "%04d%02d%02d.LOG", date.year(), date.month(), date.day());
  fichier = SD.open(nomFichier, FILE_WRITE);
  if (fichier) {
    Serial.println(F("Enregistrement dans "));
    Serial.println(nomFichier);
    fichier.println(message);
    fichier.close();
  } else {
    Serial.println(F("Erreur ouverture fichier"));
  }
}

void ModeStandard(){
  mode=1;
  Serial.println(F("Mode Standard"));
  String log = Mesure();
  Serial.println(log); // A enlever
  Enregistrement(Mesure());
  derniereExecutionStandard=millis();
}

void ResetEEPROM() {
  struct variables{
  long LOG_INTERVAL=10;
  int FILE_MAX_SIZE=4096;
  byte TIMEOUT=30;
  byte LUMIN=1;
  int LUMIN_LOW=255;
  int LUMIN_HIGH=768;
  byte TEMP_AIR=1;
  int MIN_TEMP_AIR=-10;
  int MAX_TEMP_AIR=60;
  byte HYGR=1;
  int HYGR_MINT=0;
  int HYGR_MAXT=50;
  byte PRESSURE=1;
  int PRESSURE_MIN=850;
  int PRESSURE_MAX=1080;
  }variablesReset;
  EEPROM.put(adresseEEPROM, variablesReset);

}

void ModeConfiguration() {
  //Manque le retour au bout de 30 minutes
  mode = 2;
  Serial.println(F("Mode Configuration"));
  const __FlashStringHelper* listeCommandes[] = {
    F("LOG_INTERVAL"), F("FILE_MAX_SIZE"), F("TIMEOUT"), F("LUMIN"), F("LUMIN_LOW"), F("LUMIN_HIGH"),
    F("TEMP_AIR"), F("MIN_TEMP_AIR"), F("MAX_TEMP_AIR"), F("HYGR"), F("HYGR_MINT"), F("HYGR_MAXT"),
    F("PRESSURE"), F("PRESSURE_MIN"), F("PRESSURE_MAX"), F("HEURE"),F("MINUTE"),F("SECONDE"),F("MOIS"),F("JOUR"),F("ANNEE"),F("JOUR_SEMAINE"),F("RESET")};

  // Trop couteux en mémoire
  //const int min[23] = {1, 512, 1, 0, 0, 0, 0, -40, -40, 0, -40, -40, 0, 300, 300, 0, 0, 0, 1, 1, 2000, 1, 0};
  //const int max[23] = {60, 8192, 60, 1, 1023, 1023, 1, 85, 85, 1, 85, 85, 1, 1100, 1100, 23, 59, 59, 12, 31, 2099, 7, 1};

  for (int i = 0; i < 23; i++) {
    Serial.print(i + 1);
    Serial.print(F(": "));
    Serial.println(listeCommandes[i]);
  }

  while (!Serial.available()) {}
  String entree = Serial.readStringUntil('\n');
  int numeroCommande = entree.toInt();

  Serial.print(F("Entrez la nouvelle valeur pour "));
  Serial.println(listeCommandes[numeroCommande - 1]);

  while (!Serial.available()) {}
  entree = Serial.readStringUntil('\n');
  int nouvelleValeur = entree.toInt();

  //if (nouvelleValeur < min[numeroCommande - 1] || nouvelleValeur > max[numeroCommande - 1]) {
  //  Serial.println(F("Valeur hors du domaine de définition"));
  //  return;
  //}

  DateTime temps = rtc.now();

  switch (numeroCommande) {
    case 1:
      variables.LOG_INTERVAL = nouvelleValeur; 
      break;
    case 2: 
      variables.FILE_MAX_SIZE = nouvelleValeur; 
      break;
    case 3: 
      variables.TIMEOUT = nouvelleValeur; 
      break;
    case 4: 
      variables.LUMIN = nouvelleValeur; 
      break;
    case 5: 
      variables.LUMIN_LOW = nouvelleValeur; 
      break;
    case 6: 
      variables.LUMIN_HIGH = nouvelleValeur; 
      break;
    case 7: 
      variables.TEMP_AIR = nouvelleValeur; 
      break;
    case 8: 
      variables.MIN_TEMP_AIR = nouvelleValeur; 
      break;
    case 9: 
      variables.MAX_TEMP_AIR = nouvelleValeur; 
      break;
    case 10: 
      variables.HYGR = nouvelleValeur; 
      break;
    case 11: 
      variables.HYGR_MINT = nouvelleValeur; 
      break;
    case 12: 
      variables.HYGR_MAXT = nouvelleValeur; 
      break;
    case 13:  
      variables.PRESSURE = nouvelleValeur; 
      break;
    case 14: 
      variables.PRESSURE_MIN = nouvelleValeur; 
      break;
    case 15: 
      variables.PRESSURE_MAX = nouvelleValeur; 
      break;
    case 16:
      temps = DateTime(temps.year(), temps.month(), temps.day(), nouvelleValeur, temps.minute(), temps.second());
      rtc.adjust(temps);
      break;
    case 17:
      temps = DateTime(temps.year(), temps.month(), temps.day(), temps.hour(), nouvelleValeur, temps.second());
      rtc.adjust(temps);
      break;
    case 18:
      temps = DateTime(temps.year(), temps.month(), temps.day(), temps.hour(), temps.minute(), nouvelleValeur);
      rtc.adjust(temps);
      break;
    case 19:
      temps = DateTime(temps.year(), nouvelleValeur, temps.day(), temps.hour(), temps.minute(), temps.second());
      rtc.adjust(temps);
      break;
    case 20:
      temps = DateTime(temps.year(), temps.month(), nouvelleValeur, temps.hour(), temps.minute(), temps.second());
      rtc.adjust(temps);
      break;
    case 21:
      temps = DateTime(nouvelleValeur, temps.month(), temps.day(), temps.hour(), temps.minute(), temps.second());
      rtc.adjust(temps);
      break;
    case 22:
      break;
    case 23:
      ResetEEPROM();
      break;
    default: 
      Serial.println(F("Erreur de saisie"));
  }
  EEPROM.put(adresseEEPROM, variables);
}

void ModeMaintenance(){
  mode=3;
  Serial.println(F("Mode Maintenance"));
  String log = Mesure();
  Serial.println(log);
  derniereExecutionMaintenance=millis();
}

void ModeEco(){
  mode=4;
  Serial.println(F("Mode Eco"));
  String log = Mesure();
  Serial.println(log); // A enlever
  Enregistrement(log);
  derniereExecutionEco=millis();
}
unsigned long temps_appui_rouge = 0;

void BasculeRouge() {
  if (millis() - temps_appui_rouge > 100) {
    if (digitalRead(boutonRouge) == LOW) {
      temps_appui_rouge = millis();
    } else{
      if (millis() - temps_appui_rouge > 5000) {
        if (mode == 1) {
            mode=3;
        }
        else if (mode == 3 || mode == 4) {
            mode=1;
            derniereExecutionStandard=0;
        }
      }
      temps_appui_rouge = millis();
    }
  }
}

unsigned long temps_appui_vert = 0;

void BasculeVert() {
  if (millis() - temps_appui_vert > 100) {
    if (digitalRead(boutonVert)==LOW) {
      temps_appui_vert = millis();
    } else {
      if (millis() - temps_appui_vert > 5000 && mode==1) {
        mode=4;
        derniereExecutionEco=0;
      }
      temps_appui_vert = millis();
    }
  }
}

void InitInterrupt() {
    attachInterrupt(digitalPinToInterrupt(boutonRouge), BasculeRouge, CHANGE);
    attachInterrupt(digitalPinToInterrupt(boutonVert), BasculeVert, CHANGE);
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("Demarrage"));
  led.init();
  EEPROM.get(adresseEEPROM, variables);
  pinMode(boutonRouge, INPUT_PULLUP);
  pinMode(boutonVert, INPUT_PULLUP);
  InitInterrupt();
  if (digitalRead(boutonRouge) == LOW) {
    mode = 2;
  }
  if (!bme.begin(0x76)) {
    Serial.println(F("Erreur initialisation BME280"));
    while(1);
  }
  if (!SD.begin(4)) {
    Serial.println(F("Erreur Initialisation carte SD"));
    while (1);
  }
  if (!rtc.begin()) {
    Serial.println(F("Erreur Initialisation Horloge RTC"));
    while (1);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));
  }
}

void loop() {
  // Manque la gestion des erreurs et la LED
  // Manque l'exploitation des variables
  switch (mode)
  {
  case 1:
    if (derniereExecutionStandard == 0 || millis() - derniereExecutionStandard > variables.LOG_INTERVAL*60*1000) {
      led.setColorRGB(0, 0, 255, 0);
      ModeStandard();
    }
    break;
  case 2:
    led.setColorRGB(0, 0, 255, 255);
    ModeConfiguration();
    break;
  case 3:
    if (derniereExecutionMaintenance == 0 || millis() - derniereExecutionMaintenance > variables.LOG_INTERVAL*60*1000) {
      led.setColorRGB(0, 255, 165, 0);
      ModeMaintenance();
    }
    break;
  case 4:
    if (derniereExecutionEco == 0 || millis() - derniereExecutionEco > variables.LOG_INTERVAL*60*1000*2) {
      led.setColorRGB(0, 0, 0, 255);
      ModeEco();
    }
    break;
  default:
    break;
  }
  delay(100);
}