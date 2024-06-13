/*
 * Copyright 2019-2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexspi.h"
#include "fsl_debug_console.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_common.h"
#include "fsl_flexspi.h"
#include "fsl_cache.h"
#include <cr_section_macros.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_FLEXSPI                    BOARD_FLEXSPI_PSRAM
#define EXAMPLE_FLEXSPI_AMBA_BASE          FlexSPI_AMBA_PS_CACHE_BASE
#define EXAMPLE_FLEXSPI_PORT               kFLEXSPI_PortB1
#define HYPERRAM_CMD_LUT_SEQ_IDX_READDATA  11
#define HYPERRAM_CMD_LUT_SEQ_IDX_WRITEDATA 12
#define PSRAM_BUFFER_SIZE                  1024U
#define PSRAM_SIZE                         0x800000U
#define PSRAM_CACHE_START_ADDR             0x2C000000U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint8_t s_hyper_ram_write_buffer[1024];
static uint8_t s_hyper_ram_read_buffer[1024];
static __NOINIT(PSRAM) uint32_t psram_nonInit_buffer[PSRAM_BUFFER_SIZE];

/*******************************************************************************
 * Code
 ******************************************************************************/
__RAMFUNC(SRAM) void FlexSPI_SW_Reset(FLEXSPI_Type *base)
{
    base->MCR0 |= FLEXSPI_MCR0_SWRESET_MASK;
    while (0U != (base->MCR0 & FLEXSPI_MCR0_SWRESET_MASK))
    {
    }
}


status_t flexspi_hyper_ram_ipcommand_write_data(FLEXSPI_Type *base, uint32_t address, uint32_t *buffer, uint32_t length)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write data */
	if(base->FLSHCR0[0] != 0){
		/*If portA1 memory size is configured, then add the size to the portB address*/
		flashXfer.deviceAddress = (address + (base->FLSHCR0[0] * 1024));
	}else{
		/*If portA1 memory not configured, then no change to the address*/
		flashXfer.deviceAddress = address;
	}
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = HYPERRAM_CMD_LUT_SEQ_IDX_WRITEDATA;
    flashXfer.data          = buffer;
    flashXfer.dataSize      = length;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

status_t flexspi_hyper_ram_ipcommand_read_data(FLEXSPI_Type *base, uint32_t address, uint32_t *buffer, uint32_t length)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write data */
	if(base->FLSHCR0[0] != 0){
		/*If portA1 memory size is configured, then add the size to the portB address*/
		flashXfer.deviceAddress = (address + (base->FLSHCR0[0] * 1024));
	}else{
		/*If portA1 memory not configured, then no change to the address*/
		flashXfer.deviceAddress = address;
	}
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = HYPERRAM_CMD_LUT_SEQ_IDX_READDATA;
    flashXfer.data          = buffer;
    flashXfer.dataSize      = length;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

void psram_ip_test(void)
{
    status_t status;
    uint32_t test_val;
    uint32_t set_val;
    uint32_t read_val;

    for(test_val = 0; test_val < PSRAM_SIZE/4; test_val++) {
    	set_val = 0xFFFFFFFF;
    	status = flexspi_hyper_ram_ipcommand_write_data(EXAMPLE_FLEXSPI, test_val*4, (uint32_t *)&set_val, 4);
        if (status != kStatus_Success)
        {
            PRINTF("0 Command Write Failure 0x%x - 0x%x, Failure code : 0x%x!\r\n", test_val*4, test_val, status);
        }
    }

    /* IP Test */
    for(test_val = 0; test_val < PSRAM_SIZE/4; test_val++) {
    	status = flexspi_hyper_ram_ipcommand_write_data(EXAMPLE_FLEXSPI, test_val*4, (uint32_t *)&test_val, 4);
        if (status != kStatus_Success)
        {
            PRINTF("1 Command Write Failure 0x%x - 0x%x, Failure code : 0x%x!\r\n", test_val*4, test_val, status);
        }
    }

    /* Check the memory with IP */
    for(uint32_t test_val = 0; test_val < PSRAM_SIZE/4; test_val++) {
    	read_val = 0;
    	status = flexspi_hyper_ram_ipcommand_read_data(EXAMPLE_FLEXSPI, test_val*4, (uint32_t *)&read_val, 4);
        if (status != kStatus_Success)
        {
            PRINTF("Command Read Failure 0x%x - 0x%x, Failure code : 0x%x!\r\n", test_val*4, test_val, status);
        }
        if(test_val != read_val) {
        	PRINTF(" 1 Compare error at address : 0x%x!\r\n", test_val*4);
        	PRINTF(" 1 Read value 0x%x, Expected value 0x%x\r\n", read_val, test_val);
        	break;
        }
    }

    FlexSPI_SW_Reset(EXAMPLE_FLEXSPI);

    PRINTF("IP Test completed!\r\n");
}

void psram_ahb_test(void)
{
    status_t status;
    uint32_t test_val;
    uint32_t* ahb_ptr = (uint32_t*)PSRAM_CACHE_START_ADDR;
    uint32_t* ahb_ptr2 = (uint32_t*)PSRAM_CACHE_START_ADDR;

    CACHE64_InvalidateCache(CACHE64_CTRL1);
    CACHE64_CleanCache(CACHE64_CTRL1);

    /* Fill the memory with all FFs */
    for(test_val = 0; test_val < PSRAM_SIZE/4; test_val++) {
    	ahb_ptr[test_val] = 0xFFFFFFFF;
    }

    /* Fill the memory with a sequence on the Cache */
    for(test_val = 0; test_val < PSRAM_SIZE/4; test_val++) {
    	ahb_ptr[test_val] = test_val;
    }

    /* Check the sequence with the Cache */
    for(test_val = 0; test_val < PSRAM_SIZE/4; test_val++) {
    	if(ahb_ptr[test_val] != test_val) {
    		PRINTF("Compare error at address : 0x%x!\r\n", ahb_ptr2);
    		PRINTF("Read value 0x%x, Expected value 0x%x\r\n", ahb_ptr[test_val], test_val);
    		break;
    	}
    	ahb_ptr2++;
    }

    PRINTF("AHB Test completed!\r\n");
}


int main(void)
{
    uint32_t i  = 0;
    status_t st = kStatus_Success;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    status_t status = BOARD_InitPsRam();
    if (status != kStatus_Success)
    {
        assert(false);
    }

    PRINTF("FLEXSPI example started!\r\n");

    psram_ip_test();
    psram_ahb_test();

    while (1)
    {
    }
}
