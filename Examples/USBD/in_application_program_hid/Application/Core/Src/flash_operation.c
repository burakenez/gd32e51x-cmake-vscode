/*!
    \file    flash_operation.c
    \brief   flash operation driver

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

#include "flash_operation.h"

static fmc_state_enum fmc_ready_wait(uint32_t timeout);
static fmc_state_enum fmc_state_get(void);

/*!
    \brief      erase flash
    \param[in]  address: erase start address
    \param[in]  file_length: file length
    \param[out] none
    \retval     state of FMC, refer to fmc_state_enum
*/
fmc_state_enum flash_erase(uint32_t address, uint32_t file_length)
{
    uint16_t page_count = 0U, i = 0U;

    fmc_state_enum fmc_state = FMC_READY;

    if(0U == (file_length % PAGE_SIZE)) {
        page_count = (uint16_t)(file_length / PAGE_SIZE);
    } else {
        page_count = (uint16_t)(file_length / PAGE_SIZE + 1U);
    }

    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END);
    fmc_flag_clear(FMC_FLAG_WPERR);
    fmc_flag_clear(FMC_FLAG_PGAERR);
    fmc_flag_clear(FMC_FLAG_PGERR);

    /* unlock the internal flash */
    fmc_unlock();

    for(i = 0U; i < page_count; i++) {
        /* call the standard flash erase-page function */
        fmc_page_erase(address);
        address += PAGE_SIZE;
    }

    /* lock the internal flash */
    fmc_lock();

    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    return fmc_state;
}

/*!
    \brief      write data to sectors of memory
    \param[in]  data: data to be written
    \param[in]  addr: sector address/code
    \param[in]  len: length of data to be written (in bytes)
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
fmc_state_enum iap_data_write(uint8_t *data, uint32_t addr, uint32_t len)
{
    uint32_t idx = 0U;
    fmc_state_enum fmc_state = FMC_READY;
    uint64_t data0 = 0U;
    uint64_t data1 = 0U;
    uint64_t iap_data = 0U;

    /* unlock the flash program erase controller */
    fmc_unlock();

    /* not an aligned data */
    if(len & 0x07U) {
        for(idx = len; idx < ((len & 0xFFF8U) + 8U); idx++) {
            data[idx] = 0xFFU;
        }
    }

    /* data received are word multiple */
    for(idx = 0U; idx < len; idx += 8U) {
        data0 = data[idx + 0];
        data0 |= data[idx + 1] << 8U;
        data0 |= data[idx + 2] << 16U;
        data0 |= data[idx + 3] << 24U;
        
        data1 |= data[idx + 4];
        data1 |= data[idx + 5] << 8U;
        data1 |= data[idx + 6] << 16U;
        data1 |= data[idx + 7] << 24U;

        iap_data = (uint64_t)((data1 << 32U) | (uint32_t)data0);
        
        if(0xFFFFFFFFFFFFFFFF == iap_data) {
            fmc_state = FMC_READY;
        } else {
            fmc_state = fmc_doubleword_program(addr, iap_data);
        }
        addr += 8U;
        
        iap_data = 0U;
        data1 = 0U;
        data0 = 0U;
    }

    fmc_lock();

    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    return fmc_state;
}

/*!
    \brief      program option byte
    \param[in]  mem_add: target address
    \param[in]  data: pointer to target data
    \param[out] none
    \retval     state of FMC, refer to fmc_state_enum
*/
fmc_state_enum option_byte_write(uint32_t mem_add, uint8_t *data, uint16_t len)
{
    uint8_t index;

    fmc_state_enum status ;

    /* unlock the flash program erase controller */
    fmc_unlock();

    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END);
    fmc_flag_clear(FMC_FLAG_WPERR);
    fmc_flag_clear(FMC_FLAG_PGAERR);
    fmc_flag_clear(FMC_FLAG_PGERR);

    status = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    /* authorize the small information block programming */
    ob_unlock();

    /* start erase the option byte */
    FMC_CTL |= FMC_CTL_OBER;
    FMC_CTL |= FMC_CTL_START;

    status = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    FMC_CTL &= ~FMC_CTL_OBER;
    /* set the OBPG bit */
    FMC_CTL |= FMC_CTL_OBPG;

    /* option bytes always have 16 bytes */
    for(index = 0U; index < 15U; index = index + 2U) {
        *(__IO uint16_t *)mem_add = data[index] & 0xffU;

        mem_add = mem_add + 2U;

        status = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    }

    /* if the program operation is completed, disable the OBPG Bit */
    FMC_CTL &= ~FMC_CTL_OBPG;

    fmc_lock();

    return status;
}
/*!
    \brief      jump to execute address
    \param[in]  addr: execute address
    \param[out] none
    \retval     none
*/
void jump_to_execute(uint32_t addr)
{
    static uint32_t stack_addr = 0U, exe_addr = 0U;

    /* set interrupt vector base address */
    SCB->VTOR = addr;
    __DSB();

    /* init user application's stack pointer and execute address */
    stack_addr = *(uint32_t *)addr;
    exe_addr = *(uint32_t *)(addr + 4U);

    /* re-configure MSP */
    __set_MSP(stack_addr);

    (*((void (*)())exe_addr))();
}

/*!
    \brief      check whether FMC is ready or not
    \param[in]  timeout: timeout count
    \param[out] none
    \retval     fmc_state
*/
static fmc_state_enum fmc_ready_wait(uint32_t timeout)
{
    fmc_state_enum fmc_state = FMC_BUSY;

    /* wait for FMC ready */
    do {
        /* get FMC state */
        fmc_state = fmc_state_get();
        timeout--;
    } while((FMC_BUSY == fmc_state) && (0U != timeout));

    if(FMC_BUSY == fmc_state) {
        fmc_state = FMC_TOERR;
    }
    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief      get the FMC state
    \param[in]  none
    \param[out] none
    \retval     fmc_state
*/
static fmc_state_enum fmc_state_get(void)
{
    fmc_state_enum fmc_state = FMC_READY;

    if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_BUSY)) {
        fmc_state = FMC_BUSY;
    } else {
        if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_WPERR)) {
            fmc_state = FMC_WPERR;
        } else {
            if((uint32_t)0x00U != (FMC_STAT & FMC_STAT_PGERR)) {
                fmc_state = FMC_PGERR;
            }
        }
    }
    /* return the FMC state */
    return fmc_state;
}
