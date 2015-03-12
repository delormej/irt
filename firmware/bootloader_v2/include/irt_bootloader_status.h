#ifndef IRT_BOOTLOADER_STATUS_H
#define IRT_BOOTLOADER_STATUS_H

#include <stdbool.h>

void on_bootloader_advertising(bool advertising);

void on_bootloader_connected(bool connected);

void on_bootloader_in_progress();

void on_bootloader_finished();

void bootloader_status_init();

#endif // IRT_BOOTLOADER_STATUS_H
