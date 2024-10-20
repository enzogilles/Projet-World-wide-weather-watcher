// Importation des librairies //
#include <stdio.h>
#include <string.h> 
#include <time.h>

// Déclaration des variables globales //

// Pin des différents composants
int bouton=2;
int capteurs[10]={5,6,7,8,9};
int nb_capteurs=5;
int carteSD=11;

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
char fichier_log[20];              
int revision = 0;
int LOG_INTERVAL=10;
int FILE_MAX_SIZE=2000; 
int TIMEOUT=30;            
unsigned long taille_fichier = 0;


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
    // On utilise tous les capteurs
    nb_capteurs=5;

    char* donnees=Mesure_donnees();
    enregistrer_donnees(donnees);
    delay(LOG_INTERVAL*60*1000);

}

void Mode_maintenance() {

    if (taille_fichier <= 2000) {
        while (1) {  
            pression = analogRead(5);
            temperature_air = analogRead(6);
            hygrometrie = analogRead(7);
            // Affichage des données sur le port série (Pression, Température, Hygrométrie)

            // On utilise une interruption afin de retirer le mode "Maintenance", on devra utiliser
            // la fonction "DigitalRead" afin de déterminer si l'utilisateur veut changer de mode ou pas.
            // On pourra utiliser la déclaration "BoutonRouge" et son état "High ou Low"
        }
    }
}

void Mode_Economie() {

    // On réduit le nombre de capteurs utilisés
    nb_capteurs=3;

    LOG_INTERVAL *= 2;  // Double l'intervalle entre les mesures
    afficher_etat_LED("Bleu Continu", "Mode économique");
}

void Mode_configuration() {
    
}
