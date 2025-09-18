/* ###################################################################
**     Filename    : main.c
**     Project     : gps
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2025-09-18, 10:11, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */

#include "Cpu.h"
#include "Events.h"
#include "clockMan1.h"
#include "pin_init.h"
#include "osa1.h"
#include "DbgCs1.h"
#if CPU_INIT_CONFIG
  #include "Init_Config.h"
#endif

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "fsl_debug_console.h"

/* ========= UART0 em PTA2(TX)/PTA1(RX) + SysTick para delay =========== */
#include <stdint.h>

/* ====== Ajustes do simulador ====== */
#define GPS_MODE_RANDOM_WALK   1    /* 1 = passeio suave; 0 = reamostrar tudo a cada passo */
#define GPS_STEP_DEG           0.02f /* passo máximo de LAT/LON por amostra (±0.02° ≈ ±2.2 km) */
#define GPS_STEP_ALT           15.0f /* passo máximo de ALT por amostra (±15 m) */

static uint32_t rng_state = 1234567u;          /* seed */

static float rand01(void) {                    /* [0,1) */
    rng_state = rng_state * 1103515245u + 12345u;
    return (rng_state & 0x7FFFFFFFu) / 2147483647.0f;
}

static float clampf(float v, float lo, float hi) {
    if (v < lo) return lo; if (v > hi) return hi; return v;
}

/* ponto inicial */
static void gps_init(float *lat, float *lon, float *alt) {
    *lat = -23.2100f; *lon = -45.8600f; *alt = 650.0f;
}

static void gps_next(float *lat, float *lon, float *alt) {
#if GPS_MODE_RANDOM_WALK
    /* passeio aleatório com passos mais visíveis */
    float dlat = (rand01() - 0.5f) * (2.0f * GPS_STEP_DEG);   /* ±GPS_STEP_DEG */
    float dlon = (rand01() - 0.5f) * (2.0f * GPS_STEP_DEG);
    float dalt = (rand01() - 0.5f) * (2.0f * GPS_STEP_ALT);   /* ±GPS_STEP_ALT */

    *lat = clampf(*lat + dlat,  -90.0f,   90.0f);
    *lon = clampf(*lon + dlon, -180.0f,  180.0f);

    float a = *alt + dalt;
    if (a < 1.0f)        a = 1.0f;       /* ALT > 0 */
    if (a > 12000.0f)    a = 12000.0f;
    *alt = a;
#else
    /* reamostragem “independente” a cada período */
    *lat = (rand01() * 180.0f)  - 90.0f;    /* [-90,+90] */
    *lon = (rand01() * 360.0f)  - 180.0f;   /* [-180,+180] */
    float a = (rand01() * 12000.0f) + 1.0f; /* (0,12000] */
    *alt = a;
#endif
}

/* Inicializa UART0 em 'baud' (ex.: 115200). KL25Z usa divisor inteiro (BDH/BDL). */
static void UART0_Init(uint32_t baud)
{
    uint32_t uartclk = SystemCoreClock;                 // clock base (tipicamente 48 MHz)
    uint16_t sbr     = (uint16_t)(uartclk / (baud * 16u));
    if (sbr == 0) sbr = 1;

    // 1) Clocks dos módulos
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;                 // habilita clock da UART0
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;                 // habilita clock do PORTA (pinos)

    // 2) Multiplexa pinos: PTA1=UART0_RX (ALT2), PTA2=UART0_TX (ALT2)
    PORTA->PCR[1] = PORT_PCR_MUX(2);
    PORTA->PCR[2] = PORT_PCR_MUX(2);

    // 3) Seleciona fonte de clock para UART0: MCGFLLCLK/PLL/2 (valor '1')
    SIM->SOPT2 = (SIM->SOPT2 & ~SIM_SOPT2_UART0SRC_MASK) | SIM_SOPT2_UART0SRC(1);

    // 4) Garante que RX/TX são roteados para os pinos (não para outras fontes internas)
    SIM->SOPT5 = (SIM->SOPT5 &
                 ~(SIM_SOPT5_UART0RXSRC_MASK | SIM_SOPT5_UART0TXSRC_MASK))
               |  SIM_SOPT5_UART0RXSRC(0)     // 0 = UART0_RX pin
               |  SIM_SOPT5_UART0TXSRC(0);    // 0 = UART0_TX pin

    // 5) Configura baud e habilita UART
    UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK); // desabilita TX/RX p/ configurar
    UART0->BDH = (uint8_t)((sbr >> 8) & UART0_BDH_SBR_MASK);
    UART0->BDL = (uint8_t)(sbr & 0xFFu);
    UART0->C1  = 0x00;                                   // 8N1, sem paridade
    UART0->C4  = 0x00;                                   // KL25Z não tem BRFA -> deixe 0
    UART0->C2  = UART0_C2_TE_MASK | UART0_C2_RE_MASK;    // habilita TX e RX
}

/* Envia 1 caractere (bloqueante) */
static void UART0_Putc(char c)
{
    while (!(UART0->S1 & UART0_S1_TDRE_MASK)) { /* espera TX vazio */ }
    UART0->D = (uint8_t)c;
}

/* Envia string; converte '\n' para CR+LF (compatível com terminais) */
static void UART0_Puts(const char *s)
{
    while (*s) {
        if (*s == '\n') UART0_Putc('\r');
        UART0_Putc(*s++);
    }
}




/*lint -save  -e970 */
int main(void)
/*lint -restore */
{
  PE_low_level_init();

  /* período fixo em ms */
    const uint32_t period_ms = 1000;

    float lat, lon, alt;
    gps_init(&lat, &lon, &alt);

    /* --- Loop: gera e imprime uma amostra por período --- */
    for (;;) {
      gps_next(&lat, &lon, &alt);

      /* Formato: Latitude/Longitude/Altitude com limites garantidos */
      PRINTF("LAT=%+.6f  LON=%+.6f  ALT=%.1f m\r\n", lat, lon, alt);

      /* intervalo fixo */
      (void)OSA_TimeDelay(period_ms);
    }
}

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
