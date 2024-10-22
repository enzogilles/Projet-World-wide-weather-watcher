// Importation des librairies //
#include <stdio.h>
#include <string.h> 
#include <time.h>
#include <EEPROM.h>
#include <RTClib.h> 

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
int nb_capteurs=5;
int capteurs[5] = {5, 6, 7, 8, 9};
char fichier_log[20];              
int revision = 0;
int LOG_INTERVAL=10;
int FILE_MAX_SIZE=2000; 
int TIMEOUT=30;            
unsigned long taille_fichier = 0; //  "unsigned long" est utilisé pour Un entier long non signé, c'est-à-dire un entier positif (il ne peut pas être négatif, taille 4 octets)
unsigned long temps_appui_rouge=0;
unsigned long temps_appui_vert=0;

int mesures[5];

void Mesure_donnees(){
    for (int i=0;i<nb_capteurs;i++){
        mesures[i]=analogRead(capteurs[i]);
    }
}

void enregistrer_donnees(int* donnees){
    for (int i=0;i<nb_capteurs;i++){
        Serial.print(donnees[i]);
    }
}

void Mode_standard(){
    setLedcolor(ledPin,0,255,0);
    // On utilise tous les capteurs (Mode par défaut)
    nb_capteurs=5; // Pression, Température de l'air, Hygrométrie, GPS, Luminosité.

    Mesure_donnees(); // Appel de la fonction
    enregistrer_donnees(mesures); // Appel de la fonction
    delay(LOG_INTERVAL*60*1000); // Simulation de toutes les opérations
    
    // 
    // 
}



volatile long temps_appui_rouge = 0;  // Variable pour stocker le temps d'appui du bouton rouge

void Mode_maintenance() {
    setLedcolor(ledPin, 255, 123, 0);  // LED ORANGE

    // Vérification de la taille du fichier
    if (taille_fichier <= 2000) {
        Serial.println("Mode maintenance activé.");

      
        attachInterrupt(digitalPinToInterrupt(boutonRouge), appuiRouge, RISING);
        attachInterrupt(digitalPinToInterrupt(boutonRouge), relacheRouge, FALLING);

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





void Mode_configuration() {
    setLedcolor(ledPin, 255, 255, 0);  // LED jaune pour signaler le mode configuration
    Serial.println("Mode configuration : Entrez le numéro du paramètre à modifier");
    Serial.println("1: LOG_INTERVAL");
    Serial.println("2: FILE_MAX_SIZE");
    Serial.println("3: TIMEOUT");
    Serial.println("4: LUMIN");
    Serial.println("5: LUMIN_LOW");
    Serial.println("6: LUMIN_HIGH");
    Serial.println("7: RESET");
    Serial.println("8: VERSION");

    while (Serial.available() == 0) {}

    int commande = Serial.parseInt();  // Lire la commande utilisateur

    switch (commande) {
        case 1:
            Serial.println("Entrez la nouvelle valeur pour LOG_INTERVAL (en minutes) :");
            while (Serial.available() == 0) {}
            LOG_INTERVAL = Serial.parseInt();
            EEPROM.put(0, LOG_INTERVAL);  // Enregistrer dans l'EEPROM à l'adresse 0
            Serial.print("LOG_INTERVAL mis à jour à : ");
            Serial.println(LOG_INTERVAL);
            break;

        case 2:
            Serial.println("Entrez la nouvelle valeur pour FILE_MAX_SIZE (en Ko) :");
            while (Serial.available() == 0) {}
            FILE_MAX_SIZE = Serial.parseInt();
            EEPROM.put(4, FILE_MAX_SIZE);  // Adresse différente dans l'EEPROM
            Serial.print("FILE_MAX_SIZE mis à jour à : ");
            Serial.println(FILE_MAX_SIZE);
            break;

        case 3:
            Serial.println("Entrez la nouvelle valeur pour TIMEOUT (en secondes) :");
            while (Serial.available() == 0) {}
            TIMEOUT = Serial.parseInt();
            EEPROM.put(8, TIMEOUT);  // Adresse suivante dans l'EEPROM
            Serial.print("TIMEOUT mis à jour à : ");
            Serial.println(TIMEOUT);
            break;

        case 4:
            Serial.println("Activer (1) ou désactiver (0) LUMIN :");
            while (Serial.available() == 0) {}
            LUMIN = Serial.parseInt();
            EEPROM.put(12, LUMIN);
            Serial.print("LUMIN mis à jour à : ");
            Serial.println(LUMIN);
            break;

        case 5:
            Serial.println("Entrez une nouvelle valeur pour LUMIN_LOW :");
            while (Serial.available() == 0) {}
            LUMIN_LOW = Serial.parseInt();
            EEPROM.put(16, LUMIN_LOW);
            Serial.print("LUMIN_LOW mis à jour à : ");
            Serial.println(LUMIN_LOW);
            break;

        case 6:
            Serial.println("Entrez une nouvelle valeur pour LUMIN_HIGH :");
            while (Serial.available() == 0) {}
            LUMIN_HIGH = Serial.parseInt();
            EEPROM.put(20, LUMIN_HIGH);
            Serial.print("LUMIN_HIGH mis à jour à : ");
            Serial.println(LUMIN_HIGH);
            break;

        case 7: // Réinitialisation
            resetParameters();
            break;

        case 8: // Affichage de la version
            displayVersion();
            break;

        default:
            Serial.println("Commande non reconnue");
            break;
    }
}

void checkSensors() {
    int sensorPin = A0;  // Exemple de broche pour un capteur
    int retries = 0;
    bool sensorError = false;

    while (retries < 2) {  // Utilisez le nombre de tentatives défini
        Serial.print("Lecture du capteur (tentative ");
        Serial.print(retries + 1);
        Serial.println(")...");

        long startTime = millis();
        int data = readSensorData(sensorPin);  // Lire les données du capteur

        // Vérifier si la lecture a réussi
        if (data != -1) {
            Serial.print("Données du capteur : ");
            Serial.println(data);
            break;  // Sortir de la boucle si la lecture a réussi
        }

        // Vérification du timeout
        if (millis() - startTime > TIMEOUT * 1000) {
            Serial.println("Timeout lors de la lecture du capteur !");
            retries++;
        }
    }

    // Si nous avons épuisé les tentatives sans succès
    if (retries >= 2) {
        sensorError = true;
        Serial.println("Erreur : Le capteur ne répond pas après plusieurs tentatives.");
    }
}


