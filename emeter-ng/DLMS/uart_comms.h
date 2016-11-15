#include <stdint.h>
//#if defined(__MSP430F5438__)  ||  defined(__XMS430F5438__) || defined(__MSP430F5419__)
//#include <msp430x54x.h>
//#define MAX_USCI_PORTS      4
//#define ARCH_5XX
//#endif

//#if defined(__MSP430F47196__)  ||  defined(__MSP430F47197__)
//#include <msp430x471x7.h>
//#define TA0R                TAR       /* Timer A */
//#define TA0CTL              TACTL     /* Timer A Control */
//#define TA0CCTL0            TACCTL0   /* Timer A Capture/Compare Control 0 */
//#define TA0CCTL1            TACCTL1   /* Timer A Capture/Compare Control 1 */
//#define TA0CCTL2            TACCTL2   /* Timer A Capture/Compare Control 2 */
//#define TA0CCR0             TACCR0    /* Timer A Capture/Compare 0 */
//#define TA0CCR1             TACCR1    /* Timer A Capture/Compare 1 */
//#define TA0CCR2             TACCR2    /* Timer A Capture/Compare 2 */
//#define TIMER0_A0_VECTOR    TIMERA0_VECTOR
//
//#define MAX_USCI_PORTS      2
//#endif

//#if defined(__MSP430F4794__)
//#include <msp430x47x4.h>
//#define TA0R                TAR       /* Timer A */
//#define TA0CTL              TACTL     /* Timer A Control */
//#define TA0CCTL0            TACCTL0   /* Timer A Capture/Compare Control 0 */
//#define TA0CCTL1            TACCTL1   /* Timer A Capture/Compare Control 1 */
//#define TA0CCTL2            TACCTL2   /* Timer A Capture/Compare Control 2 */
//#define TA0CCR0             TACCR0    /* Timer A Capture/Compare 0 */
//#define TA0CCR1             TACCR1    /* Timer A Capture/Compare 1 */
//#define TA0CCR2             TACCR2    /* Timer A Capture/Compare 2 */
//#define TIMER0_A0_VECTOR    TIMERA0_VECTOR
//
//#define MAX_USCI_PORTS      2
//#endif



//#if defined(__MSP430F6720__) || (__MSP430F6721__) || (__MSP430F6722__) || (__MSP430F6723__) || (__MSP430F6724__) || (__MSP430F6725__) || (__MSP430F6726__)
//#include <msp430f6726.h>
//#define MAX_USCI_PORTS      3
//#define ARCH_6XX
//#define ARCH_67SP
//#endif

#if defined(__MSP430F6730__) || (__MSP430F6731__) || (__MSP430F6732__) || (__MSP430F6733__) || (__MSP430F6734__) || (__MSP430F6735__) || defined(__MSP430F6736__)
#include <msp430f6736.h>
#define MAX_USCI_PORTS      2
#define ARCH_6XX
#define ARCH_67SP
#endif


//#if defined(__MSP430F6745__) || (__MSP430F6746__) || (__MSP430F6747__) || (__MSP430F6748__) || (__MSP430F6749__) 
//#include <msp430f6749.h>
//#define MAX_USCI_PORTS      4
//#define ARCH_6XX
//#define ARCH_67TP
//#endif

//#if defined(__MSP430F67451__) || (__MSP430F67461__) || (__MSP430F67471__) || (__MSP430F67481__) || (__MSP430F67491__) 
//#include <msp430f67491.h>
//#define MAX_USCI_PORTS      4
//#define ARCH_6XX
//#define ARCH_67TP
//#endif


//#if defined(__MSP430F6765__) || (__MSP430F6766__) || (__MSP430F6767__) || (__MSP430F6768__) || (__MSP430F6769__) 
//#include <msp430f6769.h>
//#define MAX_USCI_PORTS      4
//#define ARCH_6XX
//#define ARCH_67TP
//#endif

//#if defined(__MSP430F67651__) || (__MSP430F67661__) || (__MSP430F67671__) || (__MSP430F67681__) || (__MSP430F67691__) 
//#include <msp430f67691.h>
//#define MAX_USCI_PORTS      4
//#define ARCH_6XX
//#define ARCH_67TP
//#endif

//#if defined(__MSP430F6775__) || (__MSP430F6776__) || (__MSP430F6777__) || (__MSP430F6778__) || (__MSP430F6779__) 
//#include <msp430f6779.h>
//#define MAX_USCI_PORTS      4
//#define ARCH_6XX
//#define ARCH_67TP
//#endif

//#if defined(__MSP430F67751__) || (__MSP430F67761__) || (__MSP430F67771__) || (__MSP430F67781__) || (__MSP430F67791__) 
//#include <msp430f67791.h>
//#define MAX_USCI_PORTS      4
//#define ARCH_6XX
//#define ARCH_67TP
//#endif


uint32_t current_time(void);

void configure_uart_port(int port, int mode);
