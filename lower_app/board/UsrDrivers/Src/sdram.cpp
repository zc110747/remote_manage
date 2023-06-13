//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      sdmmc.cpp
//
//  Purpose:
//      sdram driver use fmc.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "sdram.hpp"

BaseType_t sdram_driver::init()
{
    BaseType_t result;
    
    result =  hardware_init();
    if(result == pdPASS)
    {
        //initialize sdram command sequence
        result = initialize_sequence();

        if(result == pdPASS)
        {
            //define sdram refresh rate.
            HAL_SDRAM_ProgramRefreshRate(&hsdram1, 683);
            
            test();
        }
        else
        {
            printf("sdram initialize_sequence failed\r\n");
        }
    }
    else
    {
        printf("sdram hardware_init failed\r\n");
    }

    return result;
}	

BaseType_t sdram_driver::initialize_sequence()
{
    uint32_t temp;
    BaseType_t result;

    result = send_command(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);
    HAL_Delay(1);
    result &= send_command(0, FMC_SDRAM_CMD_PALL, 1, 0);
    result &= send_command(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 1, 0);

    temp = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1
            | SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL
            | SDRAM_MODEREG_CAS_LATENCY_3           
            | SDRAM_MODEREG_OPERATING_MODE_STANDARD 
            | SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;  

    result &= send_command(0, FMC_SDRAM_CMD_LOAD_MODE, 1, temp);
    
    return result;
}


BaseType_t sdram_driver::send_command(uint8_t bank, uint8_t cmd, uint8_t refresh, uint16_t regval)
{
    uint32_t target_bank=0;
    FMC_SDRAM_CommandTypeDef Command;

    if(bank == 0) 
    {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
    }		
    else if(bank==1) 
    {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
    }

    Command.CommandMode = cmd;             
    Command.CommandTarget = target_bank;     
    Command.AutoRefreshNumber = refresh;    
    Command.ModeRegisterDefinition = regval;  
    if(HAL_SDRAM_SendCommand(&hsdram1, &Command, 0x1000) != HAL_OK)
        return pdFAIL;
    
    return pdPASS;
}

BaseType_t sdram_driver::hardware_init()
{
	
    FMC_SDRAM_TimingTypeDef SdramTiming = {0};

    /* USER CODE BEGIN FMC_Init 1 */

    /* USER CODE END FMC_Init 1 */

    /** Perform the SDRAM1 memory initialization sequence
     */
    hsdram1.Instance = FMC_SDRAM_DEVICE;
    /* hsdram1.Init */
    hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
    hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
    hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
    hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
    hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
    hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
    hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
    hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
    hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;

    /* SdramTiming */
    SdramTiming.LoadToActiveDelay = 2;
    SdramTiming.ExitSelfRefreshDelay = 8;
    SdramTiming.SelfRefreshTime = 6;
    SdramTiming.RowCycleDelay = 6;
    SdramTiming.WriteRecoveryTime = 2;
    SdramTiming.RPDelay = 2;
    SdramTiming.RCDDelay = 2;

    if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
        return pdFAIL;
    
    return pdPASS;
}


bool sdram_driver::test()
{
#if SDRAM_TEST == 1
    static uint8_t test_sdram[100] __attribute__((section(".ARM.__at_0xC0000000")));
    int i;

    memset(test_sdram, 0, 100);
    for(i=0; i<100; i++)
    {
        test_sdram[i] = i;
    }

    for(i=0; i<100; i++)
    {
        if(test_sdram[i] != i)
        {
            return false;
        }
    }
#endif
    return true;
}


