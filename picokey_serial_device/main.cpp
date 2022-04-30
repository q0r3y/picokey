#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <hardware/flash.h>
#include <hardware/sync.h>
#include "hardware/gpio.h"
#include <string>
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define BUTTON_PIN 7

// https://www.makermatrix.com/blog/read-and-write-data-with-the-pi-pico-onboard-flash/

int* readFlash();
void writeFlash(int value);

int main() {
    sleep_ms(1000);
    stdio_init_all();
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    sleep_ms(4000);
    //writeFlash(100623);

    while (true) {
        if (!gpio_get(BUTTON_PIN)) {
            printf("Button pressed");

            writeFlash(100623);

            int *p, addr, valueInFlash;
            addr = XIP_BASE +  FLASH_TARGET_OFFSET;
            p = (int *)addr; 
            valueInFlash = *p; 

            std::string s = std::to_string(valueInFlash);
            printf(s.c_str());

        }
        sleep_ms(200);
    }
    



    // while (true) {
        
    //     printf(s.c_str());
    //     sleep_ms(2000);
    // }
    
}

int* readFlash() {
    int *p, addr, valueInFlash;
    // Compute the memory-mapped address, remembering to include the offset for RA
    addr = XIP_BASE +  FLASH_TARGET_OFFSET;
    p = (int *)addr; // Place an int pointer at our memory-mapped address
    valueInFlash = *p; // Store the value at this address into a variable for later use

    return p;
}

void writeFlash(int value) {
    int buf[FLASH_PAGE_SIZE/sizeof(int)];  // One page worth of 32-bit ints
    //int mydata = 123456;  // The data I want to store

    buf[0] = value;  // Put the data into the first four bytes of buf[]

    uint32_t ints = save_and_disable_interrupts();
    // Erase the last sector of the flash
    flash_range_erase((PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE), FLASH_SECTOR_SIZE);
    // Program buf[] into the first page of this sector
    flash_range_program((PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE), (uint8_t *)buf, FLASH_PAGE_SIZE);
    restore_interrupts (ints);
}