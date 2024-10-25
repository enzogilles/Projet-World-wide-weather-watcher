// Importation des librairies //
#include <stdio.h>
#include <time.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

// Déclaration des variables globales (permet de définir le branchement de l'élement concerné)

#define ledPin 11
#define boutonRouge 2
#define boutonVert 3
#define carteSD 4
 

// Données RTC
int heure = 0, minute = 0, seconde = 0;
int jour = 1, mois = 1, annee = 2000;
char jour_semaine[20] = "Lundi";      

// Variables pour les capteurs
float pression, temperature_air, hygrometrie; // "Float" est utilisé pour des nombres à virgule flottante (taille 4 octets)
int gps_data;  // "int" est utilisé pour les entiers (taille 2 octets)
float luminosite;
char etat_systeme[20]; // "char" permet de sotcker un petit entier ou un caractère unique (taille 2 octets)

// Variables fichier de log
int nb_capteurs=3;
int capteurs[1] = {1};
char fichier_log[20];              
int revision = 0;           
unsigned long taille_fichier = 0; //  "unsigned long" est utilisé pour Un entier long non signé, c'est-à-dire un entier positif (il ne peut pas être négatif, taille 4 octets)

// Variables globales
int LOG_INTERVAL = 10;  
int FILE_MAX_SIZE = 4;  
int TIMEOUT = 30;       
int LUMIN = 1;          
int LUMIN_LOW = 100;   
int LUMIN_HIGH = 255; 

int mode=1 // 1: Standard, 2: Config, 3: Maintenance, 4 : Eco

int mesures[10];

void Mesure_donnees(){
    Serial.println("Temperature");
    Serial.println(bme.readTemperature());
    mesures[6]=bme.readTemperature();
    for (int i=0;i<nb_capteurs;i++){
        mesures[i]=analogRead(capteurs[i]);
    }
}

void enregistrer_donnees(int* donnees){
    for (int i=0;i<nb_capteurs;i++){
        Serial.print("Données");
        Serial.println(donnees[i]);
    }
}

void Mode_standard(){
    mode=1;
    // On utilise tous les capteurs (Mode par défaut)
    nb_capteurs=5; // Pression, Température de l'air, Hygrométrie, GPS, Luminosité.

    Mesure_donnees(); // Appel de la fonction
    enregistrer_donnees(mesures); // Appel de la fonction
    delay(LOG_INTERVAL*60*1000); // Simulation de toutes les opérations
    
    // 
    // 
}


void Mode_maintenance() {
    mode=3;
    // Vérification de la taille du fichier
    if (taille_fichier <= 2000) {
        Serial.println("Mode maintenance activé.");
        Serial.println("Accès à la carte SD désactivé, vous pouvez désormais remplacer la carte.");
        

        while (1) {
            // Lecture des capteurs (assignation des pins capteurs)
            int pression = analogRead(5);
            int temperature_air = analogRead(6);
            int hygrometrie = analogRead(7);

            // Affichage des valeurs des capteurs dans la console série
            Serial.print("La pression actuelle est: ");
            Serial.println(pression);
            delay(2500);

            Serial.print("La température de l'air est: ");
            Serial.println(temperature_air);
            delay(2500);

            Serial.print("L'hygrométrie actuelle est: ");
            Serial.println(hygrometrie);
            delay(2500);
        }
    } 
} 

// Début du mode Configuration 
void resetParameters();
void VersionLogiciel();
void Mode_configuration(); 


unsigned long lastActivityTime; 
const unsigned long timeoutDuration = 30 * 60 * 1000; 

void Mode_configuration() {
    mode=2;
    Serial.println("Mode configuration : Entrez le numéro du paramètre à modifier");
    Serial.println("1: LOG_INTERVAL");
    Serial.println("2: FILE_MAX_SIZE");
    Serial.println("3: TIMEOUT");
    Serial.println("4: LUMIN");
    Serial.println("5: LUMIN_LOW");
    Serial.println("6: LUMIN_HIGH");
    Serial.println("7: RESET");
    Serial.println("8: VERSION");

    // Affichage des valeurs actuelles
    Serial.println("Valeurs actuelles :");
    Serial.print("LOG_INTERVAL: ");
    Serial.println(LOG_INTERVAL);
    Serial.print("FILE_MAX_SIZE: ");
    Serial.println(FILE_MAX_SIZE);
    Serial.print("TIMEOUT: ");
    Serial.println(TIMEOUT);
    Serial.print("LUMIN: ");
    Serial.println(LUMIN);
    Serial.print("LUMIN_LOW: ");
    Serial.println(LUMIN_LOW);
    Serial.print("LUMIN_HIGH: ");
    Serial.println(LUMIN_HIGH);
    Serial.println();

    lastActivityTime = millis(); // Réinitialiser le temps d'activité

    while (true) {  
        // Vérifier si 30 minutes se sont écoulées
        if (millis() - lastActivityTime >= timeoutDuration) {
            Serial.println("Passage en mode standard après 30 minutes d'inactivité.");
            return; // Quitter le mode configuration et revenir à la boucle principale
        }

    while (Serial.available() == 0) {}
    lastActivityTime = millis(); // Mettre à jour le temps d'activité
    int commande = Serial.parseInt();  // Lire la commande utilisateur

    switch (commande) {
        case 1:
            Serial.println("Entrez la nouvelle valeur pour LOG_INTERVAL (en minutes) :");
            while (Serial.available() == 0) {}
            LOG_INTERVAL = Serial.parseInt();
            Serial.print("LOG_INTERVAL mis à jour à : ");
            Serial.println(LOG_INTERVAL);
            break;
        case 2:
                Serial.println("Entrez la nouvelle valeur pour FILE_MAX_SIZE (en Ko) :");
                while (Serial.available() == 0) {}
                FILE_MAX_SIZE = Serial.parseInt();
                Serial.print("FILE_MAX_SIZE mis à jour à : ");
                Serial.println(FILE_MAX_SIZE);
                break;

            case 3:
                Serial.println("Entrez la nouvelle valeur pour TIMEOUT (en secondes) :");
                while (Serial.available() == 0) {}
                TIMEOUT = Serial.parseInt();
                Serial.print("TIMEOUT mis à jour à : ");
                Serial.println(TIMEOUT);
                break;

            case 4:
                Serial.println("Activer (1) ou désactiver (0) LUMIN :");
                while (Serial.available() == 0) {}
                LUMIN = Serial.parseInt();
                Serial.print("LUMIN mis à jour à : ");
                Serial.println(LUMIN);
                break;

            case 5:
                Serial.println("Entrez une nouvelle valeur pour LUMIN_LOW :");
                while (Serial.available() == 0) {}
                LUMIN_LOW = Serial.parseInt();
                Serial.print("LUMIN_LOW mis à jour à : ");
                Serial.println(LUMIN_LOW);
                break;

            case 6:
                Serial.println("Entrez une nouvelle valeur pour LUMIN_HIGH :");
                while (Serial.available() == 0) {}
                LUMIN_HIGH = Serial.parseInt();
                Serial.print("LUMIN_HIGH mis à jour à : ");
                Serial.println(LUMIN_HIGH);
                break;

            case 7: 
                resetParameters(); 
                break;

            case 8: 
                VersionLogiciel();
                break;

            case 9: 
                Serial.println("Quitter le mode de configuration.");
                return;
        default:
            Serial.println("Commande non reconnue");
            break;
    }
}
}
void resetParameters() {
    LOG_INTERVAL = 10;  
    FILE_MAX_SIZE = 4;  
    TIMEOUT = 30;       
    LUMIN = 1;          
    LUMIN_LOW = 100;    
    LUMIN_HIGH = 255;   
    Serial.println("Tous les paramètres ont été réinitialisés.");
}

void VersionLogiciel() {
    Serial.println("Version du programme : 1.0.0");
}

// Fin du mode Configuration


volatile bool etatPrecedentRouge = HIGH;
unsigned long temps_appui_rouge = 0;  // Variable pour stocker le temps d'appui du bouton rouge

void BasculeRouge() {
  if (millis()-temps_appui_rouge>100){
      bool etatActuel = digitalRead(boutonRouge);
  if (etatActuel == LOW && etatPrecedentRouge == HIGH) {
    Serial.print("Appui à ");
    Serial.println(millis());
    temps_appui_rouge=millis();
  } else if (etatActuel == HIGH && etatPrecedentRouge == LOW) {
    Serial.print("Relache à ");
    Serial.println(millis());
    if (millis()-temps_appui_rouge>5000){
      Serial.print("Ecart : ");
      Serial.println(millis()-temps_appui_rouge);
      if (mode=1){
        Mode_maintenance();
      }
      elif (mode=3 || mode=4)
      {
        Mode_standard();
      }
      
    }
    else{
      Mode_configuration();
    }
    temps_appui_rouge=millis();
  }
  etatPrecedentRouge = etatActuel; // Met à jour l'état précédent
  }
}


volatile bool etatPrecedentVert = HIGH;
unsigned long temps_appui_vert = 0;  // Variable pour stocker le temps d'appui du bouton rouge

void BasculeVert() {
  if (millis()-temps_appui_vert>100){
      bool etatActuel = digitalRead(boutonVert);
  if (etatActuel == LOW && etatPrecedentVert == HIGH) {
    Serial.print("Appui à ");
    Serial.println(millis());
    temps_appui_vert=millis();
  } else if (etatActuel == HIGH && etatPrecedentVert == LOW) {
    Serial.print("Relache à ");
    Serial.println(millis());
    if (millis()-temps_appui_vert>5000){
      Serial.print("Ecart : ");
      Serial.println(millis()-temps_appui_vert);
      Serial.println("Mode éco");
      
    }
    temps_appui_vert=millis();
  }
  etatPrecedentVert = etatActuel; // Met à jour l'état précédent
  }
}

void InitInterrupt(){
    attachInterrupt(digitalPinToInterrupt(boutonRouge), BasculeRouge, CHANGE);
    attachInterrupt(digitalPinToInterrupt(boutonVert), BasculeVert, CHANGE);
}

void setup(){
  Serial.begin(9600);
  Serial.println("setup");
  pinMode(boutonRouge, INPUT);
  pinMode(boutonVert, INPUT);
  Serial.println(F("BME280 et capteur de luminosité avec boutons"));

  // Initialisation du capteur BME280
  if (!bme.begin(0x76)) {
    Serial.println("Impossible de trouver un capteur BME280, vérifiez le câblage !");
    while (1);
  }
  InitInterrupt();
}

void loop(){
  Serial.println("loop");
  Mode_standard();
}
