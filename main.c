/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the TCPWM Simultaneous Startup
*              Example for ModusToolbox.
*
* Related Document: See README.md
*
*******************************************************************************
 * (c) 2024-2026, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * This software, associated documentation and materials ("Software") is
 * owned by Infineon Technologies AG or one of its affiliates ("Infineon")
 * and is protected by and subject to worldwide patent protection, worldwide
 * copyright laws, and international treaty provisions. Therefore, you may use
 * this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software. If no license
 * agreement applies, then any use, reproduction, modification, translation, or
 * compilation of this Software is prohibited without the express written
 * permission of Infineon.
 *
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 * IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
 * THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
 * SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
 * Infineon reserves the right to make changes to the Software without notice.
 * You are responsible for properly designing, programming, and testing the
 * functionality and safety of your intended application of the Software, as
 * well as complying with any legal requirements related to its use. Infineon
 * does not guarantee that the Software will be free from intrusion, data theft
 * or loss, or other breaches ("Security Breaches"), and Infineon shall have
 * no liability arising out of any Security Breaches. Unless otherwise
 * explicitly approved by Infineon, the Software may not be used in any
 * application where a failure of the Product or any consequences of the use
 * thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "cy_pdl.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <inttypes.h>
#include "mtb_hal.h"

/*******************************************************************************
* Macros
********************************************************************************/
#if defined (CY_DEVICE_TVIIC2D6M) || defined (CY_DEVICE_TVIIC2D4M)
#define TCPWM_TR_BASE           TCPWM_TR_ONE_CNT_NR
#define TCPWM_TR_LINE           TRIG_OUT_MUX_4_TCPWM0_ALL_CNT_TR_IN0
#else
#define TCPWM_TR_BASE           TCPWM1_TR_ONE_CNT_NR
#define TCPWM_TR_LINE           TRIG_OUT_MUX_5_TCPWM1_ALL_CNT_TR_IN0
#endif

/*******************************************************************************
* Function Prototypes
********************************************************************************/

/*******************************************************************************
* Global Variables
********************************************************************************/
/* For the Retarget -IO (Debug UART) usage */
static cy_stc_scb_uart_context_t    UART_context;          /** UART context */
static mtb_hal_uart_t               UART_hal_obj;          /** Debug UART HAL object */

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

    /* Debug UART init */
    result = (cy_rslt_t)Cy_SCB_UART_Init(UART_HW, &UART_config, &UART_context);

    /* UART init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    Cy_SCB_UART_Enable(UART_HW);

    /* Setup the HAL UART */
    result = mtb_hal_uart_setup(&UART_hal_obj, &UART_hal_config, &UART_context, NULL);

    /* HAL UART init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    result = cy_retarget_io_init(&UART_hal_obj);

    /* HAL retarget_io init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("****************** "
           "PWM square wave simultaneous sw trigger"
           "****************** \r\n\n");

    /* TCPWM configuration to use group trigger #0 as its starting trigger */
    PWM_LEDA_config.startInputMode = CY_TCPWM_INPUT_RISINGEDGE;
    PWM_LEDA_config.startInput = 2 + TCPWM_TR_BASE;
    PWM_LEDB_config.startInputMode = CY_TCPWM_INPUT_RISINGEDGE;
    PWM_LEDB_config.startInput = 2 + TCPWM_TR_BASE;

    /* Initialize the TCPWM block */
    Cy_TCPWM_PWM_Init(PWM_LEDA_HW, PWM_LEDA_NUM, &PWM_LEDA_config);
    Cy_TCPWM_PWM_Init(PWM_LEDB_HW, PWM_LEDB_NUM, &PWM_LEDB_config);

    /* Enable the TCPWM block */
    Cy_TCPWM_PWM_Enable(PWM_LEDA_HW, PWM_LEDA_NUM);
    Cy_TCPWM_PWM_Enable(PWM_LEDB_HW, PWM_LEDB_NUM);

    /* Start the TCPWM block simultaneously */
    Cy_TrigMux_SwTrigger(TCPWM_TR_LINE, CY_TRIGGER_TWO_CYCLES);

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
