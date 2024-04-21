#include "stm32l1xx_hal.h"

#ifndef INC_DELAY_H_ // Début de l'inclusion conditionnelle du fichier d'en-tête
#define INC_DELAY_H_

// Déclaration de la fonction d'initialisation du compteur de cycles DWT pour le délai
uint32_t DWT_Delay_Init(void);

// Cette fonction fournit un délai en microsecondes en utilisant le compteur de cycles DWT
__STATIC_INLINE void DWT_Delay_us(volatile uint32_t microseconds)
{
  uint32_t initial_ticks = DWT->CYCCNT; // Définit le nombre de ticks initial du compteur de cycles DWT
  uint32_t ticks = (HAL_RCC_GetHCLKFreq() / 1000000); // Calcule le nombre de ticks par microseconde
  microseconds *= ticks; // Convertit la durée du délai en ticks
  while ((DWT->CYCCNT - initial_ticks) < microseconds); // Attend que le nombre de ticks requis soit atteint
}

#endif /* INC_DELAY_H_ */ // Fin de l'inclusion conditionnelle du fichier d'en-tête
