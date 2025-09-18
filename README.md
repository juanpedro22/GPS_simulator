# FRDM-KL25Z — Simulador de Coordenadas GPS (UART)

Projeto para a **FRDM-KL25Z (MKL25Z128VLK4)** que simula **Latitude, Longitude e Altitude** e envia para o PC pela **UART0** (via **OpenSDA/USB**, porta COM virtual).  
As amostras são geradas em **intervalo fixo**, respeitando:

- **Latitude** ∈ **[-90, +90]**  
- **Longitude** ∈ **[-180, +180]**  
- **Altitude** **> 0** (até 12 000 m)

Baseado em **Processor Expert/KSDK + OSA + Debug Console (`PRINTF`)**.

---

## 🎯 Saída esperada

No terminal serial (115200 8N1):

```
LAT=-23.203400  LON=-45.807800  ALT=650.0 m
LAT=-23.184910  LON=-45.822043  ALT=663.7 m
LAT=-23.172881  LON=-45.836520  ALT=651.2 m
...
```

---
<img width="1042" height="673" alt="image" src="https://github.com/user-attachments/assets/05a51e65-f8ba-4670-ae60-85fc8cc8cd10" />


## 🧱 Estrutura (resumo)

- `Sources/main.c` — loop principal, gerador e `PRINTF`
- `Generated_Code/` — código do Processor Expert (clocks, pinos, OSA, etc.)
- `SDK/` — KSDK (headers/drivers/utilities)

> **Importante:** mantenha `Sources/`, `Generated_Code/`, `Project_Settings/` e `SDK/` versionados.

---

## ⚙️ Pré-requisitos

- **IDE**: Kinetis Design Studio (ou MCUXpresso com projeto PE/KSDK equivalente)
- **Board**: FRDM-KL25Z (OpenSDA)
- **Componentes no projeto**:
  - `clockMan1`, `pin_init`, `osa1` (bare-metal), `DbgCs1` (Debug Console)
- **Pinos** (em *Pins* do Processor Expert):
  - **PTA1 = UART0_RX (ALT2)**
  - **PTA2 = UART0_TX (ALT2)**
 
    <img width="344" height="190" alt="image" src="https://github.com/user-attachments/assets/cbbed55c-85f7-4511-b584-43ed832ff539" />


---

## 🔧 Build & Flash

1. **Gerar código** (Processor Expert): `Generate Code`.
2. **Habilitar float no PRINTF** (necessário para `%f`):
   - Project → Properties → C/C++ Build → Settings → Cross ARM C Compiler → **Preprocessor → Defined symbols**
   - Adicione:
     ```
     PRINTF_ADVANCED_ENABLE=1
     PRINTF_FLOAT_ENABLE=1
     ```
   - `Clean` + `Build`.
3. **Gravar**:
   - Via **Debug (P&E)**: Inicie Debug, depois **Resume/Run** (o debugger costuma parar em “Stop at main”).
   - Ou arraste o `.bin` para o drive do **OpenSDA/DAPLink**.

---

## 🔌 Teste no PC

1. Conecte o USB do **OpenSDA** (o mesmo usado para gravar).  
2. Abra um terminal (RealTerm, PuTTY, TeraTerm) na **porta COM** do OpenSDA.  
3. **115200 baud**, **8-N-1**, **Flow: None**.  
4. **Reset** na placa — as linhas começam a aparecer no terminal.

---

## 🧠 Como funciona (resumo do código)

- **Gerador**: “random-walk” suave + *clamp* de limites:
  - `lat` e `lon` variam a cada período, delimitadas por [-90,+90] e [-180,+180].
  - `alt` varia em metros, sempre **> 0** (clamp para [1, 12000]).
- **Temporização**: `OSA_TimeDelay(period_ms)` (usa `SysTick` do OSA)  
- **Print**: `PRINTF("LAT=%+.6f  LON=%+.6f  ALT=%.1f m\r\n", lat, lon, alt);`

### Parâmetros de simulação (em `main.c`)

```c
/* Ajuste de passo e modo */
#define GPS_MODE_RANDOM_WALK   1      // 1 = passeio suave; 0 = valores independentes a cada amostra
#define GPS_STEP_DEG           0.02f  // passo máx. ±0.02° (~±2.2 km)
#define GPS_STEP_ALT           15.0f  // passo máx. ±15 m

/* Período fixo (ms) */
const uint32_t period_ms = 1000;      // 1 Hz
```

Para variar mais/menos a posição, ajuste `GPS_STEP_DEG`. Para mudar a taxa, altere `period_ms`.

---

## 🛠️ Trecho-chave do loop

```c
/* Inicialização típica */
PE_low_level_init();
DbgConsole_Init(0 /* UART0 */, 115200, kDebugConsoleUART);
OSA_Init();

/* Estado inicial */
float lat = -23.2034f, lon = -45.8078f, alt = 650.0f;

/* Loop periódico */
for (;;) {
  gps_next(&lat, &lon, &alt);  // atualiza com limites
  PRINTF("LAT=%+.6f  LON=%+.6f  ALT=%.1f m\r\n", lat, lon, alt);
  (void)OSA_TimeDelay(period_ms);
}
```



## ➕ Extensões sugeridas

- **Formato NMEA `$GPGGA`** para integrar com softwares de GPS.  
- **Comandos por UART** (`START/STOP/RATE <ms>`) para controlar o período e a geração.  
- **Rota pré-definida** (círculo, linha, pontos-de-interesse) em vez de passeio aleatório.

Abra uma issue se quiser que eu já deixe a saída em **NMEA GPGGA** ou adicione um **parser de comandos**. 😄

---





