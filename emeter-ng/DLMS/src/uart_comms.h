#include <stdint.h>

#if defined(__MSP430F6730__) || (__MSP430F6731__) || (__MSP430F6732__) || (__MSP430F6733__) || (__MSP430F6734__) || (__MSP430F6735__) || defined(__MSP430F6736__)
#include <msp430f6736.h>
#define MAX_USCI_PORTS      2
#define ARCH_6XX
#define ARCH_67SP
#endif

void change_uart_a1_to_7e1_x_bd(int x);
void change_uart_a1_to_8n1_x_bd(int x);
void change_boud_rate(uint8_t x);
extern int local_comm_exchange_mode_flag;
extern uint8_t usb_comm_speed;
extern uint8_t optical_comm_speed;
uint32_t current_time(void);
void configure_uart_port(int port, int mode);
