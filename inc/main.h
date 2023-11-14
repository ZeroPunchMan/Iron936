/**
  ******************************************************************************
  * @file    HK32F030Mxx_StdPeriph_Templates/main.h 
  * @author  Alexander
  * @version V1.0.0
  * @date    04-January-2023
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 HKMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "hk32f030m.h"

#ifdef USE_HK32F030xMF4P6_EVAL
#include "hk32f030xmf4p6_eval.h"
#endif /* USE_HK32F030xMF4P6_EVAL */

/* Exported types ------------------------------------------------------------*/
typedef struct 
{
  uint8_t Data[256];
  uint8_t Len;
  uint8_t Index;
	uint8_t FinishedFlag;
}USART_IT_TX_Def;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

