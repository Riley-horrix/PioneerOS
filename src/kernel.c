#include "common.h"
#include "peripheral/mini_uart.h"

int kernel_main(void) {
  uart_init();
  uart_send_string("PIoneerOS Operating System Initialising...\n");

#if PI_VERSION == 1
  uart_send_string("Board Number : Raspberry Pi 1\n");
#endif

  uart_send_string("Done\n");

  while (1) {
    uart_send(uart_recv());
  }
}