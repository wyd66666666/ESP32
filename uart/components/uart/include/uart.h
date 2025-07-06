#ifndef __UART_H
#define __UART_H

#include "sdkconfig.h"
#include <stdio.h>
#include "driver/uart.h"
#include "hal/uart_types.h"

#define UART_TX_PIN     CONFIG_UART_TX_PIN
#define UART_RX_PIN     CONFIG_UART_RX_PIN
#define BAUD_RATE       CONFIG_BAUD_RATE
#define UART_NUM        UART_NUM_0

void uart_init(void);




#endif