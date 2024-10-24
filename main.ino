// Importation des librairies //
#include <stdio.h>
#include <time.h>


// Déclaration des variables globales (permet de définir le branchement de l'élement concerné)

#define ledPin 11
#define boutonRouge 2
#define boutonVert 3
#define carteSD 4
 

     
unsigned long taille_fichier = 0; //  "unsigned long" est utilisé pour Un entier long non signé, c'est-à-dire un entier positif (il ne peut pas être négatif, taille 4 octets)
unsigned long temps_appui_rouge=0;
unsigned long temps_appui_vert=0;




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
    

}


void Mode_maintenance() {
    setLedcolor(ledPin, 255, 123, 0);  // LED ORANGE

    // Vérification de la taille du fichier
    if (taille_fichier <= 2000) {
        Serial.println("Mode maintenance activé.");

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

// Variables globales
int LOG_INTERVAL = 10;  
int FILE_MAX_SIZE = 4;  
int TIMEOUT = 30;       
int LUMIN = 1;          
int LUMIN_LOW = 100;   
int LUMIN_HIGH = 255;   

unsigned long lastActivityTime; 
const unsigned long timeoutDuration = 30 * 60 * 1000; 

void setup() {
    Serial.begin(9600);  
    Serial.println("Demarrage du systeme...");
    lastActivityTime = millis(); 
}

void loop() {
    Mode_configuration(); 
}

void Mode_configuration() {
    Serial.println("Mode configuration : Entrez le numero du parametre a modifier");
    Serial.println("1: LOG_INTERVAL");
    Serial.println("2: FILE_MAX_SIZE");
    Serial.println("3: TIMEOUT");
    Serial.println("4: LUMIN");
    Serial.println("5: LUMIN_LOW");
    Serial.println("6: LUMIN_HIGH");
    Serial.println("7: Reinitialiser les parametres"); 
    Serial.println("8: VERSION");
    Serial.println("9: Quitter"); 

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
        int commande = Serial.parseInt();  

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



volatile long temps_appui_rouge = 0;  // Variable pour stocker le temps d'appui du bouton rouge

void appuiRouge(){
    temps_appui_rouge=millis();
}

void relacheRouge(){
if (millis()-temps_appui_rouge>=5000)
{
    Mode_maintenance();
}
else{
    Mode_configuration();
}
temps_appui_rouge=millis();
}


volatile long temps_appui_vert = 0;  // Variable pour stocker le temps d'appui du bouton vert

void appuiVert(){
    temps_appui_rouge=millis();
}

void relacheVert(){
if (millis()-temps_appui_vert>=5000)
{
    Mode_eco();
}
temps_appui_vert=millis();
}

void InitInterrupt(){
    attachInterrupt(digitalPinToInterrupt(boutonRouge), appuiRouge, RISING);
    attachInterrupt(digitalPinToInterrupt(boutonRouge), relacheRouge, FALLING);
    attachInterrupt(digitalPinToInterrupt(boutonVert), appuiVert, RISING);
    attachInterrupt(digitalPinToInterrupt(boutonVert), relacheVert, FALLING);
}

int main(){
    InitInterrupt();
    Mode_standard();
    return 0;
}
