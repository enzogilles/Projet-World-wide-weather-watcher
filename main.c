// Importation des librairies //
#include <stdio.h>
#include <string.h> 
#include <time.h>

// Déclaration des variables globales (permet de définir le branchement de l'élement concerné)

#define ledPin 11
#define boutonRouge 2
#define boutonVert 3
#define capteurs[10] {5,6,7,8,9}
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
char fichier_log[20];              
int revision = 0;
int LOG_INTERVAL=10;
int FILE_MAX_SIZE=2000; 
int TIMEOUT=30;            
unsigned long taille_fichier = 0; //  "unsigned long" est utilisé pour Un entier long non signé, c'est-à-dire un entier positif (il ne peut pas être négatif, taille 4 octets)
unsigned long temps_appui_rouge=0;
unsigned long temps_appui_vert=0;

char* Mesure_donnees(){
    for (int i=0;i<nb_capteurs;i++){
        // Lecture des données sur les différents capteurs
    }
    // Renvoie un pointeur vers la première mesure de la liste alouée dynamiquement
}

void enregistrer_donnees(char* donnees){
    // Enregistrement des données sur la carte SD
}

void Mode_standard(){
    setLedcolor(ledPin,0,255,0);
    // On utilise tous les capteurs (Mode par défaut)
    nb_capteurs=5; // Pression, Température de l'air, Hygrométrie, GPS, Luminosité.

    char* donnees=Mesure_donnees(); // Appel de la fonction
    enregistrer_donnees(donnees); // Appel de la fonction
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








void Mode_Economie() {
    setLedcolor(ledPin,0,0,255);
    // On utilise moins de capteurs lors du mode éco
    nb_capteurs=3;

    char* donnees=Mesure_donnees();
    enregistrer_donnees(donnees);
    delay(LOG_INTERVAL*60*1000*2);

    Serial.print("Bleu Continu", "Mode économique");
}

void Mode_configuration() {
    setLedcolor(ledPin,255,255,0);
    Serial.println("Entrez un numéro de paramètre à modifier");
    int commande=Serial.read();
    switch (commande)
    {
    case 1:
    LOG_INTERVAL=Serial.read();
    // Modification de la premiere variable
    case 2:
    FILE_MAX_SIZE=Serial.read();
    // Modification de la deuxième variable
    }
   // ...
} 

    void appuiRouge(){
        temps_appui_rouge=millis();
        // Enregistre le temps à l'instant où le bouton est pressé
    }

    void relacheRouge(){
    if (millis()-temps_appui_rouge>=5000)
    {
        Mode_maintenance();
        // Si pressé pendant 5 sec --> Mode maintenance
    }
    else{
        Mode_configuration();
        // Sinon --> Mode configuration
    }
    temps_appui_rouge=millis();
    }



void appuiVert(){
	temps_appui_vert=millis();
    // Enregistre le temps à l'instant où le bouton est pressé
}

void relacheVert(){
  if (millis()-temps_appui_vert>=5000)
  {
    Mode_Economie();
    // Si pressé pendant 5 sec --> Mode économique
  }
  temps_appui_vert=millis();
}

void init_interruption(){
    // Le RISING va enregistrer le temps d'appui.
    // Le FALLING changera de mode en fonction du temps d'appui une fois relâché.

    attachInterrupt(digitalPinToInterrupt(boutonRouge),appuiRouge,RISING);      // Enregistre le temps d'appui du bouton rouge.
    attachInterrupt(digitalPinToInterrupt(boutonRouge),relacheRouge,FALLING);   // Change de mode en fonction du temps d'appui une fois le bouton rouge relâché (Configuration si moins de 5s, Maintenance si au moins 5s).
    attachInterrupt(digitalPinToInterrupt(boutonVert),appuiVert,RISING);        // Enregistre le temps d'appui du bouton vert.
    attachInterrupt(digitalPinToInterrupt(boutonVert),relacheVert,FALLING);     // Change de mode en fonction du temps d'appui une fois le bouton vert relâché (Économie si au moins 5s).

}

int main(){
    init_interruption();
    Mode_standard();
    
    return 0;
}

void setLedcolor(int pin,int r,int g,int b){
    analogWrite(pin,r);   // Partie rouge
    analogWrite(pin+1,g); // Partie verte (+1 pour aller sur le pin suivant)
    analogWrite(pin+2,b); // Partie bleue (+2 pour aller sur le dernier pin)
}
