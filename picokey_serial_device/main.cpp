#include <stdio.h>
#include "pico/stdlib.h"
#include <hardware/flash.h>
#include <hardware/sync.h>
#include "hardware/gpio.h"
#include "SHA1.h"
#include "HMAC.h"
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define BUTTON_PIN 7

void init();
void getValFromFlash(int &var);
void incrementFlashCounter();
std::string calculateHash();

const std::string DEVICE_ID = "486197";
const std::string SECRET_KEY = "3679243789";

int main() {
    init();
    while (true) {
        if (!gpio_get(BUTTON_PIN)) {
            printf("[*] Button pressed");
            printf("\n");
            std::string hash = calculateHash();
            printf("[*] Calculated Hash: ");
            printf(hash.c_str());
            printf("\n");
            incrementFlashCounter();
            std::string otp = DEVICE_ID + hash.substr(26,14);
            printf("[*] OTP: ");
            printf(otp.c_str());
            printf("\n");
        }
        sleep_ms(200);
    }    
}

void init() {
    sleep_ms(1000);
    stdio_init_all();
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    //sleep_ms(4000);
}

std::string calculateHash() {
    int count;
    getValFromFlash(count);
    std::string countt = std::to_string(count);
    printf("[*] Current Counter Value: ");
    printf(countt.c_str());
    printf("\n");
    return hmac<SHA1>(std::to_string(count), SECRET_KEY);
}

void getValFromFlash(int &var) {
    int *p, addr, valueInFlash;
    addr = XIP_BASE +  FLASH_TARGET_OFFSET;
    p = (int *)addr;
    var = *p;
}

void incrementFlashCounter() {
    int currentCount;
    int buf[FLASH_PAGE_SIZE/sizeof(int)];  // One page worth of 32-bit ints
    getValFromFlash(currentCount);
    currentCount++;
    buf[0] = currentCount;  // Put the data into the first four bytes of buf[]
    uint32_t ints = save_and_disable_interrupts();
    // Erase the last sector of the flash
    flash_range_erase((PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE), FLASH_SECTOR_SIZE);
    // Program buf[] into the first page of this sector
    flash_range_program((PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE), (uint8_t *)buf, FLASH_PAGE_SIZE);
    restore_interrupts (ints);   
    printf("[*] Saved Count: ");
    std::string countt = std::to_string(currentCount);
    printf(countt.c_str());
    printf(" to flash.");
    printf("\n");
}