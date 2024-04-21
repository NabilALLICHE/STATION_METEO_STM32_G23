/*
 * timer.c
 *
 *  Created on: Feb 13, 2021
 *      Author:
 */

#include "delay.h"
#include "stm32l1xx_hal.h"

// Fonction d'initialisation du compteur de cycles DWT pour le délai
uint32_t DWT_Delay_Init(void)
{
    /* Disable TRC */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // Désactive le compteur de cycles TRC
    /* Enable TRC */
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // Active le compteur de cycles TRC

    /* Disable clock cycle counter */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; // Désactive le compteur de cycles
    /* Enable  clock cycle counter */
    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; // Active le compteur de cycles

    /* Reset the clock cycle counter value */
    DWT->CYCCNT = 0; // Réinitialise la valeur du compteur de cycles à zéro

    /* 3 NO OPERATION instructions */
    __ASM volatile ("NOP"); // 3 instructions "NOP" pour attendre un court laps de temps
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");

    /* Check if clock cycle counter has started */
    if(DWT->CYCCNT)
    {
       return 0; // Le compteur de cycles a démarré
    }
    else
    {
      return 1; // Le compteur de cycles n'a pas démarré
    }
}
