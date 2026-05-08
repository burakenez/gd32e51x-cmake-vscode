/*!
    \file    main.c
    \brief   deepsleep wakeup through exti interrupt

    \version 2026-02-09, V1.4.0, firmware for GD32E51x
*/

/*
    Copyright (c) 2025, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32e51x.h"
#include "gd32e517z_eval.h"
#include "systick.h"
#include "main.h"

void led_config(void);

/* software delay to prevent the impact of Vcore fluctuations.
   It is strongly recommended to include it to avoid issues caused by self-removal. */
static void _soft_delay_(uint32_t time)
{
    __IO uint32_t i;
    for(i=0; i<time*10; i++){
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* systick configuration */
    systick_config();
    /* LED configuration */
    led_config();
    /* enable clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* wakeup key(KEY A) configuration */
    gd_eval_key_init(KEY_A, KEY_MODE_GPIO);
    /* tamper key(KEY B) EXTI configuration */
    gd_eval_key_init(KEY_B, KEY_MODE_EXTI);
    /* press wakeup key(KEY A) to enter deepsleep mode and use tamper key(KEY B) generate a exti interrupt to wakeup mcu */
    while(1) {
        if(RESET == gpio_input_bit_get(KEY_A_GPIO_PORT, KEY_A_PIN)) {
            /* The following is to prevent Vcore fluctuations caused by frequency switching. 
               It is strongly recommended to include it to avoid issues caused by self-removal. */
            _soft_delay_(0x100);
            rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV2);
            _soft_delay_(0x100);
            rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV4);
            _soft_delay_(0x100);
            rcu_system_clock_source_config(RCU_CKSYSSRC_IRC8M);
            _soft_delay_(100U);
            rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);
            pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, PMU_LOWDRIVER_DISABLE, WFI_CMD);
        }
    }
}

/*!
    \brief      toggle the LED
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t timingdelaylocal = 0;

    if(timingdelaylocal != 0x00) {
        /* all the LEDs on */
        if(timingdelaylocal < 200) {
            gd_eval_led_on(LED1);
            gd_eval_led_on(LED2);
            gd_eval_led_on(LED3);
            gd_eval_led_on(LED4);
        } else {
            /* all the LEDs off */
            gd_eval_led_off(LED1);
            gd_eval_led_off(LED2);
            gd_eval_led_off(LED3);
            gd_eval_led_off(LED4);
        }
        timingdelaylocal--;
    } else {
        timingdelaylocal = 400;
    }
}

/*!
    \brief      LED configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
}
