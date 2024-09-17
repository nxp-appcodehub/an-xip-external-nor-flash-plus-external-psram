/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright 2008-2013 Code Red Technologies Ltd,
 * Copyright 2013-2024 NXP
 * Generated linker script file for RW612
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.30
 * MCUXpresso IDE v11.9.0 [Build 2144] [2024-01-05] on Aug 12, 2024, 4:18:30 PM
 */

MEMORY
{
  /* Define each memory region */
  QSPI_FLASH (rx) : ORIGIN = 0x8000000, LENGTH = 0x800000 /* 8M bytes (alias Flash) */  
  SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x130000 /* 1216K bytes (alias RAM) */  
  PSRAM (rwx) : ORIGIN = 0x2c000000, LENGTH = 0x800000 /* 8M bytes (alias RAM2) */  
}

  /* Define a symbol for the top of each memory region */
  __base_QSPI_FLASH = 0x8000000  ; /* QSPI_FLASH */  
  __base_Flash = 0x8000000 ; /* Flash */  
  __top_QSPI_FLASH = 0x8000000 + 0x800000 ; /* 8M bytes */  
  __top_Flash = 0x8000000 + 0x800000 ; /* 8M bytes */  
  __base_SRAM = 0x20000000  ; /* SRAM */  
  __base_RAM = 0x20000000 ; /* RAM */  
  __top_SRAM = 0x20000000 + 0x130000 ; /* 1216K bytes */  
  __top_RAM = 0x20000000 + 0x130000 ; /* 1216K bytes */  
  __base_PSRAM = 0x2c000000  ; /* PSRAM */  
  __base_RAM2 = 0x2c000000 ; /* RAM2 */  
  __top_PSRAM = 0x2c000000 + 0x800000 ; /* 8M bytes */  
  __top_RAM2 = 0x2c000000 + 0x800000 ; /* 8M bytes */  
