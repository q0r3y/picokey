#include <stdio.h>
#include "pico/stdlib.h"
#include <hardware/flash.h>

    // Flash-based address of the last sector
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

// https://www.makermatrix.com/blog/read-and-write-data-with-the-pi-pico-onboard-flash/

int main() {

    stdio_init_all();
    sleep_ms(5000);


    int buf[FLASH_PAGE_SIZE/sizeof(int)];  // One page worth of 32-bit ints
    int mydata = 123456;  // The data I want to store

    buf[0] = mydata;  // Put the data into the first four bytes of buf[]

    // Erase the last sector of the flash
    flash_range_erase((PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE), FLASH_SECTOR_SIZE);

    // Program buf[] into the first page of this sector
    flash_range_program((PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE), (uint8_t *)buf, FLASH_PAGE_SIZE);
    
    int *p, addr, value;

    // Compute the memory-mapped address, remembering to include the offset for RAM
    addr = XIP_BASE +  FLASH_TARGET_OFFSET;
    p = (int *)addr; // Place an int pointer at our memory-mapped address
    value = *p; // Store the value at this address into a variable for later use

    printf(value);
}