/**
 ******************************************************************************
 * @file    hk32f030m_it.c
 * @author  Alexander
 * @version V1.0.0
 * @date    04-January-2023
 * @brief   Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "hk32f030m_it.h"
#include "systime.h"
#include "usart.h"
#include "adc.h"
#include "cl_log.h"
#include "seg_dp.h"
#include "heater.h"
/** @addtogroup HK32F030M_StdPeriph_Examples
 * @{
 */

/** @addtogroup USART_TXE_Interrupt
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
    SysTimeInc(1);
    SegDp_Update();
}

/******************************************************************************/
/*             HK32F030M Peripherals Interrupt Handlers                       */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (KEIL_startup_hk32f030m.s).                                          */
/******************************************************************************/
/**
 * @brief  This function handles USART1 interrupt request.
 * @param  None
 * @retval None
 */
void USART1_IRQHandler(void)
{
    /* USART1 transfer empty interrupt handle */
    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_TXE);

        uint8_t data;
        if (Usart1_PollSendByte(&data) == CL_ResSuccess)
            USART1->TDR = data;
        else
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
}

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
{
}*/

/**
 * @}
 */

/**
 * @}
 */
