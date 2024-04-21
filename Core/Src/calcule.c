#include "calcule.h"
#include "delay.h"
#include "stm32l1xx_hal.h"

// Fonction pour configurer une broche GPIO en mode sortie (vers le capteur)
void Data_Output(GPIO_TypeDef *PORT, uint16_t PIN)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0}; // Initialise une structure de configuration GPIO

    GPIO_InitStruct.Pin = PIN; // Définit la broche GPIO à configurer
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // Mode de sortie Push-Pull
    HAL_GPIO_Init(PORT, &GPIO_InitStruct); // Initialise la broche GPIO avec les paramètres spécifiés
}

// Fonction pour configurer une broche GPIO en mode entrée (vers le microcontrôleur)
void Data_Input(GPIO_TypeDef *PORT, uint16_t PIN)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0}; // Initialise une structure de configuration GPIO

    GPIO_InitStruct.Pin = PIN; // Définit la broche GPIO à configurer
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // Mode d'entrée
    HAL_GPIO_Init(PORT, &GPIO_InitStruct); // Initialise la broche GPIO avec les paramètres spécifiés
}

// Fonction pour lire les données provenant du capteur
void Read_data(uint8_t *data)
{
    int i, k; // Déclaration des variables de boucle

    for (i = 0; i < 8; i++) // Boucle pour lire chaque bit de données
    {
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET) // Vérifie si le bit de données est 0
        {
            (*data) &= ~(1 << (7 - i)); // Efface le bit correspondant dans la variable de données
            while (!(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))); // Attend la fin du signal bas
            DWT_Delay_us(40); // Attend pour le prochain bit
        }
        else // Si le bit de données est 1
        {
            (*data) |= (1 << (7 - i)); // Définit le bit correspondant dans la variable de données
            for (k = 0; k < 1000; k++) // Boucle pour attendre la fin du signal haut
            {
                if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET)
                {
                    break; // Sort de la boucle si le signal devient bas
                }
            }
            while (!(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))); // Attend la fin du signal haut
            DWT_Delay_us(40); // Attend pour le prochain bit
        }
    }
}
