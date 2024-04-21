/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "calcule.h"  // Inclusion de la bibliothèque pour la communication avec le DHT22
#include "delay.h"    // Inclusion de la bibliothèque pour le retards en us
#include<lib_lcd.h>   // Inclusion de la bibliothèque pour l'affichage LCD
#include"stdio.h"     // Inclusion de la bibliothèque d'entrées/sorties standard
#include <string.h>   // Inclusion de la bibliothèque pour la manipulation de chaînes de caractères
#include "rtc.h"      // Inclusion de la bibliothèque pour le RTC (Real-Time Clock)
#include <stdarg.h>   // Inclusion de la bibliothèque pour les fonctions à arguments variables


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
static rgb_lcd lcdData;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//sSPI_HandleTypeDef hspi2;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
      float Temperature = 0, Humidite = 0;  // Déclaration de variables pour la température et l'humidité
      uint16_t RH = 0, TEMP = 0;  // Déclaration de variables pour les données du capteur
      uint8_t dataH1;  // Variable pour stocker la partie haute de l'humidité
      uint8_t dataH2;  // Variable pour stocker la partie basse de l'humidité
      uint8_t dataT1;  // Variable pour stocker la partie haute de la température
      uint8_t dataT2;  // Variable pour stocker la partie basse de la température
      uint8_t SUM;     // Variable pour stocker la somme de contrôle
      uint8_t check;   // Variable pour vérifier les données reçues
      char bufRH[30];  // Tableau de caractères pour stocker la valeur de l'humidité à afficher sur l'écran LCD
      char bufT[30];   // Tableau de caractères pour stocker la valeur de la température à afficher sur l'écran LCD
      char bufRH_SD[50];  // Tableau de caractères pour stocker la valeur de l'humidité à enregistrer sur la carte SD
      char bufT_SD[50];   // Tableau de caractères pour stocker la valeur de la température à enregistrer sur la carte SD
      char time[10];    // Tableau de caractères pour stocker l'heure actuelle
      char date[10];    // Tableau de caractères pour stocker la date actuelle
      char time_U[10];  // Tableau de caractères pour stocker l'heure universelle actuelle
      char date_U[10];  // Tableau de caractères pour stocker la date universelle actuelle
      /////////// set DATE & TIME /////////////
      RTC_TimeTypeDef sTime = {0};  // Structure pour stocker l'heure à définir
      RTC_DateTypeDef sDate = {0};  // Structure pour stocker la date à définir
      /////////// Get DATE & TIME /////////////
      RTC_DateTypeDef gDate;  // Structure pour stocker la date actuelle
      RTC_TimeTypeDef gTime;  // Structure pour stocker l'heure actuelle


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void UART_TransmitTemperature(float Temperature);  // Déclaration de la fonction pour transmettre la température via UART
void UART_TransmitHummidity(float Humidite );     // Déclaration de la fonction pour transmettre l'humidité via UART
void UART_Init(void);                             // Déclaration de la fonction d'initialisation de l'UART
void UART_TransmitTemperature_SD(float Temperature );  // Déclaration de la fonction pour transmettre la température à la carte SD via UART
void UART_TransmitHummidity_SD(float Humidite );       // Déclaration de la fonction pour transmettre l'humidité à la carte SD via UART
void get_time(void);   // Déclaration de la fonction pour obtenir l'heure actuelle
void set_Time();       // Déclaration de la fonction pour définir l'heure actuelle

void myprintf(const char *fmt, ...);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void UART_TransmitTemperature(float Temperature ) {
        sprintf(bufT, " %.2f C\t", Temperature);
        HAL_UART_Transmit(&huart2, (uint8_t*)bufT, strlen(bufT), HAL_MAX_DELAY);

    }
    void UART_TransmitHummidity(float Humidite ) {
            sprintf(bufRH, " %.2f %%\r\n", Humidite);
            HAL_UART_Transmit(&huart2, (uint8_t*)bufRH, strlen(bufRH), HAL_MAX_DELAY);

        }
    void UART_TransmitTemperature_SD(float Temperature ) {
    	 sprintf(bufT_SD, "Temperature: %.2f C\n", Temperature);
    	 HAL_UART_Transmit(&huart1, (uint8_t*)bufT_SD, strlen(bufT_SD), HAL_MAX_DELAY);

    	}


    void UART_TransmitHummidity_SD(float Humidite ) {
               sprintf(bufRH_SD, "Humidity: %.2f %%\r\n", Humidite);
               HAL_UART_Transmit(&huart1, (uint8_t*)bufRH_SD, strlen(bufRH_SD), HAL_MAX_DELAY);

           }


    /////////////////////////////////////real time ////////////////////////////////

    void set_Time(){
      /** Initialize RTC and set the Time and Date
      */
      sTime.Hours = 0x12;
      sTime.Minutes = 0x22;
      sTime.Seconds = 0x1;
      sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
      sTime.StoreOperation = RTC_STOREOPERATION_RESET;
      if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
      {
        Error_Handler();
      }
    }
    void get_time(void)
    {


      /* Get the RTC current Time */
      HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
      /* Get the RTC current Date */
      HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

      /* Display time Format: hh:mm:ss */
      sprintf((char*)time,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);

      /* Display date Format: mm-dd-yy */
      sprintf((char*)date,"%02d-%02d-%2d",gDate.Date, gDate.Month, 2024 + gDate.Year);  // I like the date first

      /* UART date Format: mm-dd-yy */
                  sprintf((char*)date_U,"%02d-%02d-%2d\t",gDate.Date, gDate.Month, 2024 + gDate.Year);
      /* UART time Format: hh:mm:ss */
            sprintf((char*)time_U,"%02d:%02d:%02d\t",gTime.Hours, gTime.Minutes, gTime.Seconds);

            	HAL_UART_Transmit(&huart2, (uint8_t*)date_U, strlen(date_U), HAL_MAX_DELAY);
                HAL_UART_Transmit(&huart2, (uint8_t*)time_U, strlen(time_U), HAL_MAX_DELAY);
                HAL_UART_Transmit(&huart1, (uint8_t*)date_U, strlen(date_U), HAL_MAX_DELAY);
                HAL_UART_Transmit(&huart1, (uint8_t*)time_U, strlen(time_U), HAL_MAX_DELAY);

    }


    //Let's display the time and date on lcd

   void display_time (void)
    {
    	 	 	 lcd_position(&hi2c1,0,0);
    		  	  lcd_print(&hi2c1,date);
    		  	  lcd_position(&hi2c1,1,1);
    		  	  lcd_print(&hi2c1,time);
    }






/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	 int k = 0; //variable pour les bloucles while



  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
//   UART_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  lcd_init(&hi2c1, &lcdData); // initialise le lcd
  ////////////////////Real time////////////////
  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2)
         {
         //   Set the time
	  set_Time();
         }
  //////////////
  DWT_Delay_Init();

    lcd_position(&hi2c1,0,0);
    lcd_print(&hi2c1,"STATION METEO");
    reglagecouleur(0,0,255);
    lcd_position(&hi2c1,0,1);
    lcd_print(&hi2c1,"ALLICHE & MAHAR");
    HAL_Delay(3000);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  /*commence la communication avec le capteur*/
	  	  	  	  	  	  HAL_Delay(6000); // Attend 10 minute (afin d'avoir la mesure de la T° et RH% chaque 10min )
		        	  	  Data_Output(GPIOA, GPIO_PIN_4); // Configure la broche GPIOA_Pin_4 en mode sortie (info vers le capteur)
		        	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // Met la broche GPIOA_Pin_4 à l'état bas (remet à l'état bas)
		        	  	  DWT_Delay_us(1200); // Attend un délai de 1200 µs (signal de commande > 1ms)
		        	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // Met la broche GPIOA_Pin_4 à l'état haut (état haut)
		        	  	  DWT_Delay_us(30); // Attend un délai de 30 µs (signal de commande)
		        	  	  Data_Input(GPIOA, GPIO_PIN_4); // Configure la broche GPIOA_Pin_4 en mode entrée (info vers le microcontrôleur)

		        	  	  /*commence la réception de données*/

		        	  	  while(!(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))); // Attend que la broche GPIOA_Pin_4 soit à l'état haut

		        	  	  for ( k=0;k<1000;k++) // Boucle pour attendre jusqu'à 1000 fois
		        	  	  {
		        	  		  if (HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET) // Vérifie si la broche GPIOA_Pin_4 est à l'état bas
		        	  		  {
		        	  	  	  	break; // Sort de la boucle si la condition est vraie
		        	  	  	  }
		        	  	  }

		        	  	  while(!(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))); // Attend que la broche GPIOA_Pin_4 soit à l'état haut
		        	  	  DWT_Delay_us(40); // Attend un délai de 40 µs

		        	  	  Read_data(&dataH1); // Lit les données du capteur (dans la bibliothèque HT.c)
		        	  	  Read_data(&dataH2);
		        	  	  Read_data(&dataT1);
		        	  	  Read_data(&dataT2);
		        	  	  Read_data(&SUM);

		        	  	  check = dataH1 + dataH2 + dataT1 + dataT2; // Calcule la somme des données pour vérification

		        	  	 if(check == (SUM)) // Vérifie si la somme est égale au checksum
		        	  	  {

		        	  	  RH = (dataH1<<8) | dataH2; // Calcule l'humidité
		        	  	  TEMP = (dataT1<<8) | dataT2; // Calcule la température

		        	  	  }


		        	  	  Humidite = RH / 10.0; // Calcule l'humidité finale
		        	  	  Temperature = TEMP / 10.0; // Calcule la température finale


	  	 /////////Real time////////////
	  	  	  get_time();//  fonction pour récupérer l'heure actuelle via le RTC
	  	  	 clearlcd();
	  		  display_time();// Affichage de l'heure actuelle sur l'écran LCD
	  		  HAL_Delay(5000);
	  		  /////////////////////////
	  		  /*commence transmission vers LCD*/
	  		 clearlcd(); // Efface l'affichage LCD
	  		  sprintf(bufRH,"Humidite: %.1f", Humidite); // Formatage de l'humidité pour l'affichage
	  		  sprintf(bufT, "Temp.: %.1f C", Temperature); // Formatage de la température pour l'affichage
	  		 lcd_position(&hi2c1,1,0); // Positionne le curseur LCD
	  		  lcd_print(&hi2c1,bufRH); // Affiche l'humidité sur le LCD
	  		 lcd_print(&hi2c1,"%"); // Affiche le symbole "%" sur le LCD
	  		 lcd_position(&hi2c1,1,1); // Positionne le curseur LCD
	  		lcd_print(&hi2c1,bufT); // Affiche la température sur le LCD
	  			///////////////////////

	  	 UART_TransmitTemperature( Temperature); // Transmet la température via UART 2
	  	  UART_TransmitHummidity( Humidite ); // Transmet l'humidité via UART 2
		  ///////////OPENLOG////////////
	  UART_TransmitTemperature_SD(Temperature);//Transmet la température a la carte SD via UART 1
	  UART_TransmitHummidity_SD(Humidite);//Transmet l'humidité a la carte SD via UART 1
	  	  /////////////////////////////



  }


  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
