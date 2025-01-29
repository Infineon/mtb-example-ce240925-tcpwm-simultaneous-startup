/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the TCPWM Simultaneous Startup
*              Example for ModusToolbox.
*
* Related Document: See README.md
*
*******************************************************************************
* Copyright 2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "cy_pdl.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <inttypes.h>

/*******************************************************************************
* Macros
********************************************************************************/

/*******************************************************************************
* Function Prototypes
********************************************************************************/

/*******************************************************************************
* Global Variables
********************************************************************************/

/********************************************************************************
* Function Name: HandleError
*********************************************************************************
* Summary:
*  User defined error handling function.
* Parameters:
*  status - status for evaluation.
* Return:
*  none
*
********************************************************************************/
void HandleError(cy_rslt_t status)
{
    if (CY_RSLT_SUCCESS != status)
    {
        /* Halt the CPU while debugging */
        CY_ASSERT(0);
    }
}

/********************************************************************************
* Function Name: main
*********************************************************************************
* Summary:
*  This is the main function for the CPU. It configures the PWM and puts the CPU
*  in Sleep mode to save power.
* Parameters:
*  none
* Return:
*  int
*
********************************************************************************/ 
int main(void)
{
    /* API return code */
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    HandleError(result);

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    Cy_SCB_UART_Init(UART_HW, &UART_config, NULL);
    Cy_SCB_UART_Enable(UART_HW);
    cy_retarget_io_init(UART_HW);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("****************** "
           "PWM square wave simultaneous sw trigger"
           "****************** \r\n\n");

    /* TCPWM configuration to use group trigger #0 as its starting trigger */
    PWM_LEDA_config.startInputMode = CY_TCPWM_INPUT_RISINGEDGE;
    PWM_LEDA_config.startInput = 2 + TCPWM_TR_ONE_CNT_NR;
    PWM_LEDB_config.startInputMode = CY_TCPWM_INPUT_RISINGEDGE;
    PWM_LEDB_config.startInput = 2 + TCPWM_TR_ONE_CNT_NR;

    /* Initialize the TCPWM block */
    Cy_TCPWM_PWM_Init(PWM_LEDA_HW, PWM_LEDA_NUM, &PWM_LEDA_config);
    Cy_TCPWM_PWM_Init(PWM_LEDB_HW, PWM_LEDB_NUM, &PWM_LEDB_config);

    /* Enable the TCPWM block */
    Cy_TCPWM_PWM_Enable(PWM_LEDA_HW, PWM_LEDA_NUM);
    Cy_TCPWM_PWM_Enable(PWM_LEDB_HW, PWM_LEDB_NUM);

    /* Start the TCPWM block simultaneously */
    Cy_TrigMux_SwTrigger(TRIG_OUT_MUX_4_TCPWM0_ALL_CNT_TR_IN0, CY_TRIGGER_TWO_CYCLES);

    printf("PWM started successfully...\r\n");
    /* Put the CPU into sleep mode to save power */
    printf("Entering the sleep mode...\r\n");
    for (;;)
    {
    /* Scenario: There is a need to put the CPU into Sleep mode */
    /* Prepare the system for CPU Sleep power mode here */
    if(CY_SYSPM_SUCCESS != Cy_SysPm_CpuEnterSleep(CY_SYSPM_WAIT_FOR_INTERRUPT))
        {
        /* The CPU Sleep mode was not entered because a registered
        *  Sleep "check ready" callback returned a "not success" status
        */
        }
    else
        {
        /* If the program has reached here, the CPU has just woken up
         * from the CPU Sleep mode
         */
        }
    }
}

/* [] END OF FILE */
