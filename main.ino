// Importation des librairies
#include <stdio.h>
#include <time.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ChainableLED.h>
#include <SD.h>
#include <SPI.h>

#define DATA_PIN 6        
#define CLOCK_PIN 7       

ChainableLED leds(DATA_PIN, CLOCK_PIN, 1); 
Adafruit_BME280 bme;
RTC_DS3231 rtc;
File fichier;

// Déclaration des variables globales
#define ledPin 11
#define boutonRouge 2
#define boutonVert 3
#define carteSD 4

// Données RTC
int heure = 0, minute = 0, seconde = 0;
int jour = 1, mois = 1, annee = 2000;
char jour_semaine[20] = "Lundi";

// Variables pour les capteurs
float pression, temperature_air, hygrometrie;
int gps_data;
float luminosite;
char etat_systeme[20];

// Variables fichier de log
int nb_capteurs = 3;
int capteurs[1] = {1};
char fichier_log[20];
int revision = 0;
unsigned long taille_fichier = 0;

// Variables globales
int LOG_INTERVAL = 10;
int FILE_MAX_SIZE = 2048;  // Taille en octets (2ko)
int TIMEOUT = 30;
int LUMIN = 1;
int LUMIN_LOW = 100;
int LUMIN_HIGH = 255;

int mode = 1; // 1: Standard, 2: Config, 3: Maintenance, 4 : Eco

int mesures[10];

void setCouleur(int rouge, int vert, int bleu) {
  leds.setColorRGB(0, rouge, vert, bleu);
}

// Vérification stockage de la carte SD
bool verifierEspaceSD() {
  taille_fichier = fichier.size();
  if (taille_fichier >= FILE_MAX_SIZE) {
    setCouleur(255, 255, 255); // Rouge et blanc pour signaler l'erreur
    Serial.println("Erreur : Stockage saturé ou erreur d'écriture de la carte SD.");
    fichier.close();
    return false;
  }
  return true;
}

void Mesure_donnees() {
    Serial.println("Temperature");
    Serial.println(bme.readTemperature());
    mesures[6] = bme.readTemperature();
    for (int i = 0; i < nb_capteurs; i++) {
        mesures[i] = analogRead(capteurs[i]);
    }
}

void enregistrer_donnees(int* donnees){
    fichier = SD.open("datalog.txt", FILE_WRITE);
    if (!verifierEspaceSD()) return; // Vérifie l'espace avant d'écrire

    for (int i = 0; i < nb_capteurs; i++) {
        Serial.print("Données");
        Serial.println(donnees[i]);
        if (fichier) {
          Serial.print("Ecriture sur la carte...");
          fichier.println(donnees[i]);
          Serial.println("done.");
        } else {
          Serial.println("Erreur d'ouverture du fichier datalog.txt");
        }
    }
    fichier.close();
}

void Mode_standard(){
    setCouleur(0,255,0); // Couleur Mode Standard
    mode=1;
    // On utilise tous les capteurs (Mode par défaut)
    nb_capteurs=5; // Pression, Température de l'air, Hygrométrie, GPS, Luminosité.

    Mesure_donnees(); // Appel de la fonction
    enregistrer_donnees(mesures); // Appel de la fonction
    delay(LOG_INTERVAL*60*1000); // Simulation de toutes les opérations
    
    // 
    // 
}

void Mode_Economique(){
  mode=4;
  setCouleur(0,0,255); // Couleur Mode Éco
  // Lire et afficher la température
  Serial.print("Température = ");
  Serial.print(bme.readTemperature());
  Serial.println(" °C");

  // Lire et afficher la pression atmosphérique
  Serial.print("Pression = ");
  Serial.print(bme.readPressure());
  Serial.println(" Pa");

  // Lire et afficher l'humidité
  Serial.print("Humidité = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  delay(LOG_INTERVAL*60*2000);  // Simulation de toutes les opérations
}

void Mode_maintenance() {
    setCouleur(255, 128, 0); // Couleur Mode Maintenance
    mode = 3;
    Serial.println("Mode maintenance activé.");
    Serial.println("Accès à la carte SD désactivé, vous pouvez désormais remplacer la carte.");

    unsigned long Temps_debut = millis();
    
    while (true) {
        int pression = analogRead(5);
        int temperature_air = analogRead(6);
        int hygrometrie = analogRead(7);

        Serial.print("Pression : ");
        Serial.println(pression);
        delay(2500);

        Serial.print("Température de l'air : ");
        Serial.println(temperature_air);
        delay(2500);

        Serial.print("Hygrométrie : ");
        Serial.println(hygrometrie);
        delay(2500);

        // Rester appuyer sur le bouton rouge pendant 5sec --> changement d'état
        if (digitalRead(boutonRouge) == LOW) {
            unsigned long currentPressTime = millis();
            if (currentPressTime - Temps_debut >= 5000) { 
                Mode_standard();
                break;
            }
        } else {
            Temps_debut = millis(); // Réinitialiser temps
        }
    }
}

// Début du mode Configuration 

void Mode_configuration() {
    Serial.println("Config");
}

// Fonction pour afficher l'heure courante
void afficherHeureCourante() {
    DateTime now = rtc.now();
    Serial.print("Date: ");
    Serial.print(now.day());
    Serial.print("/");
    Serial.print(now.month());
    Serial.print("/");
    Serial.print(now.year());
    Serial.print(" Heure: ");
    Serial.print(now.hour());
    Serial.print(":");
    Serial.print(now.minute());
    Serial.print(":");
    Serial.println(now.second());
}

// Fin du mode Configuration

volatile bool etatPrecedentRouge = HIGH;
unsigned long temps_appui_rouge = 0; // Variable pour stocker le temps d'appui du bouton rouge

void BasculeRouge() {
  if (millis() - temps_appui_rouge > 100) {
    bool etatActuel = digitalRead(boutonRouge);
    if (etatActuel == LOW && etatPrecedentRouge == HIGH) {
      Serial.print("Appui à ");
      Serial.println(millis());
      temps_appui_rouge = millis();
    } else if (etatActuel == HIGH && etatPrecedentRouge == LOW) {
      Serial.print("Relache à ");
      Serial.println(millis());
      if (millis() - temps_appui_rouge > 5000) {
        Serial.print("Ecart : ");
        Serial.println(millis() - temps_appui_rouge);
        if (mode == 1) {
          Mode_maintenance();
        }
        else if (mode == 3 || mode == 4) {
          Mode_standard();
        }
      } else {
        Mode_configuration();
      }
      temps_appui_rouge = millis();
    }
    etatPrecedentRouge = etatActuel; // Met à jour l'état précédent
  }
}

volatile bool etatPrecedentVert = HIGH; 
unsigned long temps_appui_vert = 0; // Variable pour stocker le temps d'appui du bouton rouge

void BasculeVert() {
  if (millis() - temps_appui_vert > 100) {
    bool etatActuel = digitalRead(boutonVert);
    if (etatActuel == LOW && etatPrecedentVert == HIGH) {
      Serial.print("Appui à ");
      Serial.println(millis());
      temps_appui_vert = millis();
    } else if (etatActuel == HIGH && etatPrecedentVert == LOW) {
      Serial.print("Relache à ");
      Serial.println(millis());
      if (millis() - temps_appui_vert > 5000) {
        Serial.print("Ecart : ");
        Serial.println(millis() - temps_appui_vert);
        Mode_Economique();
      }
      temps_appui_vert = millis();
    }
    etatPrecedentVert = etatActuel; // Met à jour l'état précédent
  }
}

void InitInterrupt() {
    attachInterrupt(digitalPinToInterrupt(boutonRouge), BasculeRouge, CHANGE);
    attachInterrupt(digitalPinToInterrupt(boutonVert), BasculeVert, CHANGE);
}

void setup() {
    Serial.begin(9600);
    Serial.println("setup");
    Serial.print("Initialisation de la carte SD...");

    if (!SD.begin(4)) {
        Serial.println("Erreur d'initialisation de la carte SD !");
        while (1);
    }
    Serial.println("Initialisation réussie.");
    leds.init();
    pinMode(boutonRouge, INPUT);
    pinMode(boutonVert, INPUT);
    Serial.println(F("BME280 et capteur de luminosité avec boutons"));

    if (!rtc.begin()) {
        Serial.println("Erreur lors de l'initialisation du RTC, vérifiez le câblage !");
        while (1);
    }

    if (rtc.lostPower()) {
        Serial.println("L'horloge a perdu l'alimentation, réglage de la date et l'heure !");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    if (!bme.begin(0x76)) {
        Serial.println("Impossible de trouver un capteur BME280, vérifiez le câblage !");
        while (1);
    }
    InitInterrupt();
}

void loop() {
    afficherHeureCourante();
    Serial.println("loop");
    Mode_standard();
}
void setCouleur(int rouge, int vert, int bleu) {
  leds.setColorRGB(0, rouge, vert, bleu);  // Code RGB (0 à 255)
}
