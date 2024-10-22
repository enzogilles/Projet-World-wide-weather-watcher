// Importation des librairies //
#include <stdio.h>
#include <string.h> 
#include <time.h>

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