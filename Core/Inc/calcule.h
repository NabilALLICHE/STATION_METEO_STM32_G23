#include "stm32l1xx_hal.h"

#ifndef INC_CALCULE_H_ // Début de l'inclusion conditionnelle du fichier d'en-tête
#define INC_CALCULE_H_

// Déclaration des fonctions disponibles dans ce fichier d'en-tête

// Fonction pour configurer une broche GPIO en mode sortie (vers le capteur)
void Data_Output(GPIO_TypeDef *PORT, uint16_t PIN);

// Fonction pour configurer une broche GPIO en mode entrée (vers le microcontrôleur)
void Data_Input(GPIO_TypeDef *PORT, uint16_t PIN);

// Fonction pour lire les données provenant du capteur
void Read_data(uint8_t *data);

#endif /* INC_CALCULE_H_ */ // Fin de l'inclusion conditionnelle du fichier d'en-tête
