#include <avr/io.h>
#include <util/delay.h>

#ifndef RA8875_H_
#define RA8875_H_

/***************************************
 * ???????
 ***************************************/

// ??? ST7735 ???????
#define LCD_PORT        PORTB
#define LCD_DDR         DDRB
#define LCD_RST         PORTB1    // ????
#define LCD_TFT_CS      PORTB2    // ??????
#define LCD_MOSI        PORTB3    // SPI MOSI ??
#define LCD_MISO        PORTB4    // SPI MISO ??
#define LCD_SCK         PORTB5    // SPI ????

// ??? LCD ??????????
#define LCD_LITE_PORT   PORTD
#define LCD_LITE_DDR    DDRD
#define LCD_LITE        PORTD6    // ???????PWM?

// ????
#define RA8875_WIDTH    800
#define RA8875_HEIGHT   480

/***************************************
 * ????
 ***************************************/
typedef uint8_t  uchar;
typedef uint16_t uint;
typedef uint32_t ulong;

/***************************************
 * RA8875 ????????
 ***************************************/

// Colors (RGB565)
#define RA8875_BLACK 0x0000   ///< Black Color
#define RA8875_BLUE 0x001F    ///< Blue Color
#define RA8875_RED 0xF800     ///< Red Color
#define RA8875_GREEN 0x07E0   ///< Green Color
#define RA8875_CYAN 0x07FF    ///< Cyan Color
#define RA8875_MAGENTA 0xF81F ///< Magenta Color
#define RA8875_YELLOW 0xFFE0  ///< Yellow Color
#define RA8875_WHITE 0xFFFF   ///< White Color

#define RA8875_FGCR        0x63    // Foreground Color Register R
#define RA8875_FGCG        0x64    // Foreground Color Register G
#define RA8875_FGCB        0x65    // Foreground Color Register B
#define RA8875_BGCR        0x60    // Background Color Register R
#define RA8875_BGCG        0x61    // Background Color Register G
#define RA8875_BGCB        0x62    // Background Color Register B

#define RA8875_FNCR0       0x21    // Font Control Register 0
#define RA8875_FNCR1       0x22    // Font Control Register 1
#define RA8875_CGSR        0x23    // CGRAM Select Register

// Command/Data pins for SPI
#define RA8875_DATAWRITE 0x00 ///< See datasheet
#define RA8875_DATAREAD 0x40  ///< See datasheet
#define RA8875_CMDWRITE 0x80  ///< See datasheet
#define RA8875_CMDREAD 0xC0   ///< See datasheet

// Registers & bits
#define RA8875_PWRR 0x01           ///< See datasheet
#define RA8875_PWRR_DISPON 0x80    ///< See datasheet
#define RA8875_PWRR_DISPOFF 0x00   ///< See datasheet
#define RA8875_PWRR_SLEEP 0x02     ///< See datasheet
#define RA8875_PWRR_NORMAL 0x00    ///< See datasheet
#define RA8875_PWRR_SOFTRESET 0x01 ///< See datasheet

#define RA8875_MRWC 0x02 ///< See datasheet

#define RA8875_GPIOX 0xC7 ///< See datasheet

#define RA8875_PLLC1 0x88         ///< See datasheet
#define RA8875_PLLC1_PLLDIV2 0x80 ///< See datasheet
#define RA8875_PLLC1_PLLDIV1 0x00 ///< See datasheet

#define RA8875_PLLC2 0x89        ///< See datasheet
#define RA8875_PLLC2_DIV1 0x00   ///< See datasheet
#define RA8875_PLLC2_DIV2 0x01   ///< See datasheet
#define RA8875_PLLC2_DIV4 0x02   ///< See datasheet
#define RA8875_PLLC2_DIV8 0x03   ///< See datasheet
#define RA8875_PLLC2_DIV16 0x04  ///< See datasheet
#define RA8875_PLLC2_DIV32 0x05  ///< See datasheet
#define RA8875_PLLC2_DIV64 0x06  ///< See datasheet
#define RA8875_PLLC2_DIV128 0x07 ///< See datasheet

#define RA8875_SYSR 0x10       ///< See datasheet
#define RA8875_SYSR_8BPP 0x00  ///< See datasheet
#define RA8875_SYSR_16BPP 0x0C ///< See datasheet
#define RA8875_SYSR_MCU8 0x00  ///< See datasheet
#define RA8875_SYSR_MCU16 0x03 ///< See datasheet

#define RA8875_PCSR 0x04       ///< See datasheet
#define RA8875_PCSR_PDATR 0x00 ///< See datasheet
#define RA8875_PCSR_PDATL 0x80 ///< See datasheet
#define RA8875_PCSR_CLK 0x00   ///< See datasheet
#define RA8875_PCSR_2CLK 0x01  ///< See datasheet
#define RA8875_PCSR_4CLK 0x02  ///< See datasheet
#define RA8875_PCSR_8CLK 0x03  ///< See datasheet

#define RA8875_HDWR 0x14 ///< See datasheet

#define RA8875_HNDFTR 0x15         ///< See datasheet
#define RA8875_HNDFTR_DE_HIGH 0x00 ///< See datasheet
#define RA8875_HNDFTR_DE_LOW 0x80  ///< See datasheet

#define RA8875_HNDR 0x16      ///< See datasheet
#define RA8875_HSTR 0x17      ///< See datasheet
#define RA8875_HPWR 0x18      ///< See datasheet
#define RA8875_HPWR_LOW 0x00  ///< See datasheet
#define RA8875_HPWR_HIGH 0x80 ///< See datasheet

#define RA8875_VDHR0 0x19     ///< See datasheet
#define RA8875_VDHR1 0x1A     ///< See datasheet
#define RA8875_VNDR0 0x1B     ///< See datasheet
#define RA8875_VNDR1 0x1C     ///< See datasheet
#define RA8875_VSTR0 0x1D     ///< See datasheet
#define RA8875_VSTR1 0x1E     ///< See datasheet
#define RA8875_VPWR 0x1F      ///< See datasheet
#define RA8875_VPWR_LOW 0x00  ///< See datasheet
#define RA8875_VPWR_HIGH 0x80 ///< See datasheet

#define RA8875_HSAW0 0x30 ///< See datasheet
#define RA8875_HSAW1 0x31 ///< See datasheet
#define RA8875_VSAW0 0x32 ///< See datasheet
#define RA8875_VSAW1 0x33 ///< See datasheet

#define RA8875_HEAW0 0x34 ///< See datasheet
#define RA8875_HEAW1 0x35 ///< See datasheet
#define RA8875_VEAW0 0x36 ///< See datasheet
#define RA8875_VEAW1 0x37 ///< See datasheet

#define RA8875_MCLR 0x8E            ///< See datasheet
#define RA8875_MCLR_START 0x80      ///< See datasheet
#define RA8875_MCLR_STOP 0x00       ///< See datasheet
#define RA8875_MCLR_READSTATUS 0x80 ///< See datasheet
#define RA8875_MCLR_FULL 0x00       ///< See datasheet
#define RA8875_MCLR_ACTIVE 0x40     ///< See datasheet

#define RA8875_DCR 0x90                   ///< See datasheet
#define RA8875_DCR_LINESQUTRI_START 0x80  ///< See datasheet
#define RA8875_DCR_LINESQUTRI_STOP 0x00   ///< See datasheet
#define RA8875_DCR_LINESQUTRI_STATUS 0x80 ///< See datasheet
#define RA8875_DCR_CIRCLE_START 0x40      ///< See datasheet
#define RA8875_DCR_CIRCLE_STATUS 0x40     ///< See datasheet
#define RA8875_DCR_CIRCLE_STOP 0x00       ///< See datasheet
#define RA8875_DCR_FILL 0x20              ///< See datasheet
#define RA8875_DCR_NOFILL 0x00            ///< See datasheet
#define RA8875_DCR_DRAWLINE 0x00          ///< See datasheet
#define RA8875_DCR_DRAWTRIANGLE 0x01      ///< See datasheet
#define RA8875_DCR_DRAWSQUARE 0x10        ///< See datasheet

#define RA8875_ELLIPSE 0xA0        ///< See datasheet
#define RA8875_ELLIPSE_STATUS 0x80 ///< See datasheet

#define RA8875_MWCR0 0x40         ///< See datasheet
#define RA8875_MWCR0_GFXMODE 0x00 ///< See datasheet
#define RA8875_MWCR0_TXTMODE 0x80 ///< See datasheet
#define RA8875_MWCR0_CURSOR 0x40  ///< See datasheet
#define RA8875_MWCR0_BLINK 0x20   ///< See datasheet

#define RA8875_MWCR0_DIRMASK 0x0C ///< Bitmask for Write Direction
#define RA8875_MWCR0_LRTD 0x00    ///< Left->Right then Top->Down
#define RA8875_MWCR0_RLTD 0x04    ///< Right->Left then Top->Down
#define RA8875_MWCR0_TDLR 0x08    ///< Top->Down then Left->Right
#define RA8875_MWCR0_DTLR 0x0C    ///< Down->Top then Left->Right

#define RA8875_BTCR 0x44  ///< See datasheet
#define RA8875_CURH0 0x46 ///< See datasheet
#define RA8875_CURH1 0x47 ///< See datasheet
#define RA8875_CURV0 0x48 ///< See datasheet
#define RA8875_CURV1 0x49 ///< See datasheet

#define RA8875_P1CR 0x8A         ///< See datasheet
#define RA8875_P1CR_ENABLE 0x80  ///< See datasheet
#define RA8875_P1CR_DISABLE 0x00 ///< See datasheet
#define RA8875_P1CR_CLKOUT 0x10  ///< See datasheet
#define RA8875_P1CR_PWMOUT 0x00  ///< See datasheet

#define RA8875_P1DCR 0x8B ///< See datasheet

#define RA8875_P2CR 0x8C         ///< See datasheet
#define RA8875_P2CR_ENABLE 0x80  ///< See datasheet
#define RA8875_P2CR_DISABLE 0x00 ///< See datasheet
#define RA8875_P2CR_CLKOUT 0x10  ///< See datasheet
#define RA8875_P2CR_PWMOUT 0x00  ///< See datasheet

#define RA8875_P2DCR 0x8D ///< See datasheet

#define RA8875_PWM_CLK_DIV1 0x00     ///< See datasheet
#define RA8875_PWM_CLK_DIV2 0x01     ///< See datasheet
#define RA8875_PWM_CLK_DIV4 0x02     ///< See datasheet
#define RA8875_PWM_CLK_DIV8 0x03     ///< See datasheet
#define RA8875_PWM_CLK_DIV16 0x04    ///< See datasheet
#define RA8875_PWM_CLK_DIV32 0x05    ///< See datasheet
#define RA8875_PWM_CLK_DIV64 0x06    ///< See datasheet
#define RA8875_PWM_CLK_DIV128 0x07   ///< See datasheet
#define RA8875_PWM_CLK_DIV256 0x08   ///< See datasheet
#define RA8875_PWM_CLK_DIV512 0x09   ///< See datasheet
#define RA8875_PWM_CLK_DIV1024 0x0A  ///< See datasheet
#define RA8875_PWM_CLK_DIV2048 0x0B  ///< See datasheet
#define RA8875_PWM_CLK_DIV4096 0x0C  ///< See datasheet
#define RA8875_PWM_CLK_DIV8192 0x0D  ///< See datasheet
#define RA8875_PWM_CLK_DIV16384 0x0E ///< See datasheet
#define RA8875_PWM_CLK_DIV32768 0x0F ///< See datasheet

#define RA8875_TPCR0 0x70               ///< See datasheet
#define RA8875_TPCR0_ENABLE 0x80        ///< See datasheet
#define RA8875_TPCR0_DISABLE 0x00       ///< See datasheet
#define RA8875_TPCR0_WAIT_512CLK 0x00   ///< See datasheet
#define RA8875_TPCR0_WAIT_1024CLK 0x10  ///< See datasheet
#define RA8875_TPCR0_WAIT_2048CLK 0x20  ///< See datasheet
#define RA8875_TPCR0_WAIT_4096CLK 0x30  ///< See datasheet
#define RA8875_TPCR0_WAIT_8192CLK 0x40  ///< See datasheet
#define RA8875_TPCR0_WAIT_16384CLK 0x50 ///< See datasheet
#define RA8875_TPCR0_WAIT_32768CLK 0x60 ///< See datasheet
#define RA8875_TPCR0_WAIT_65536CLK 0x70 ///< See datasheet
#define RA8875_TPCR0_WAKEENABLE 0x08    ///< See datasheet
#define RA8875_TPCR0_WAKEDISABLE 0x00   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV1 0x00   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV2 0x01   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV4 0x02   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV8 0x03   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV16 0x04  ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV32 0x05  ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV64 0x06  ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV128 0x07 ///< See datasheet

#define RA8875_TPCR1 0x71            ///< See datasheet
#define RA8875_TPCR1_AUTO 0x00       ///< See datasheet
#define RA8875_TPCR1_MANUAL 0x40     ///< See datasheet
#define RA8875_TPCR1_VREFINT 0x00    ///< See datasheet
#define RA8875_TPCR1_VREFEXT 0x20    ///< See datasheet
#define RA8875_TPCR1_DEBOUNCE 0x04   ///< See datasheet
#define RA8875_TPCR1_NODEBOUNCE 0x00 ///< See datasheet
#define RA8875_TPCR1_IDLE 0x00       ///< See datasheet
#define RA8875_TPCR1_WAIT 0x01       ///< See datasheet
#define RA8875_TPCR1_LATCHX 0x02     ///< See datasheet
#define RA8875_TPCR1_LATCHY 0x03     ///< See datasheet

#define RA8875_TPXH 0x72  ///< See datasheet
#define RA8875_TPYH 0x73  ///< See datasheet
#define RA8875_TPXYL 0x74 ///< See datasheet

#define RA8875_INTC1 0xF0     ///< See datasheet
#define RA8875_INTC1_KEY 0x10 ///< See datasheet
#define RA8875_INTC1_DMA 0x08 ///< See datasheet
#define RA8875_INTC1_TP 0x04  ///< See datasheet
#define RA8875_INTC1_BTE 0x02 ///< See datasheet

#define RA8875_INTC2 0xF1     ///< See datasheet
#define RA8875_INTC2_KEY 0x10 ///< See datasheet
#define RA8875_INTC2_DMA 0x08 ///< See datasheet
#define RA8875_INTC2_TP 0x04  ///< See datasheet
#define RA8875_INTC2_BTE 0x02 ///< See datasheet

#define RA8875_SCROLL_BOTH 0x00   ///< See datasheet
#define RA8875_SCROLL_LAYER1 0x40 ///< See datasheet
#define RA8875_SCROLL_LAYER2 0x80 ///< See datasheet
#define RA8875_SCROLL_BUFFER 0xC0 ///< See datasheet

/***************************************
 * ???
 ***************************************/

// ???????
#define SET_BIT(port, pin)      ((port) |= (1 << (pin)))
#define CLEAR_BIT(port, pin)    ((port) &= ~(1 << (pin)))

// ??????
#define RST_HIGH()      SET_BIT(LCD_PORT, LCD_RST)
#define RST_LOW()       CLEAR_BIT(LCD_PORT, LCD_RST)
#define CS_HIGH()       SET_BIT(LCD_PORT, LCD_TFT_CS)
#define CS_LOW()        CLEAR_BIT(LCD_PORT, LCD_TFT_CS)

/***************************************
 * ????
 ***************************************/

// ?????
void RA8875_init(void);

// SPI ????
void SPI_init(void);
//void SPI_writeCommand(uint8_t cmd);
//void SPI_writeData(uint8_t data);
//uint8_t SPI_readData(void);
//void SPI_writeReg(uint8_t reg, uint8_t val);
//uint8_t SPI_readReg(uint8_t reg);
void RA8875_writeCommand(uint8_t cmd);
void RA8875_writeData(uint8_t data);
uint8_t RA8875_readData(void);
void RA8875_writeReg(uint8_t reg, uint8_t val);
uint8_t RA8875_readReg(uint8_t reg);

// ????
void Delay_ms(uint16_t ms);

// ??????
void RA8875_displayOn(uint8_t on);
void RA8875_sleep(uint8_t sleep);

//// ????
//void RA8875_setXY(uint16_t x, uint16_t y);
//void RA8875_drawPixel(uint16_t x, uint16_t y, uint16_t color);
//void RA8875_fillScreen(uint16_t color);
//void RA8875_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
//void RA8875_drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, uint8_t filled);
//void RA8875_drawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint8_t filled);
//void RA8875_drawEllipse(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint16_t color, uint8_t filled);
//void RA8875_drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint8_t filled);


void RA8875_setXY(uint16_t x, uint16_t y);
void RA8875_drawPixel(uint16_t x, uint16_t y, uint16_t color);
void RA8875_fillScreen(uint16_t color);
void RA8875_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

// ??
void RA8875_drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void RA8875_fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

// ?
void RA8875_drawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
void RA8875_fillCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);

// ???
void RA8875_drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void RA8875_fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

// ??
void RA8875_drawEllipse(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint16_t color);
void RA8875_fillEllipse(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint16_t color);

// ??
void RA8875_drawCurve(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint8_t curvePart, uint16_t color);
void RA8875_fillCurve(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint8_t curvePart, uint16_t color);

// ????
void RA8875_drawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color);
void RA8875_fillRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color);

// ?????
void RA8875_drawPixels(uint16_t *p, uint32_t num, uint16_t x, uint16_t y);

// ??????
void RA8875_drawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
void RA8875_drawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);


void RA8875_setForegroundColor(uint16_t color);
void RA8875_setBackgroundColor(uint16_t color);

// ???????
void RA8875_textMode(void);
void RA8875_graphicsMode(void);
void RA8875_setFontSize(uint8_t size);
void RA8875_textWrite(const char* buffer, uint16_t len);
void RA8875_setCursor(uint16_t x, uint16_t y);
void RA8875_setTextColor(uint16_t foreColor, uint16_t bgColor);

// ??????
void RA8875_backlight(uint8_t on, uint8_t PWM);
void RA8875_setPWM(uint8_t pwm, uint8_t duty);

// ???????????
void RA8875_touchEnable(uint8_t on);
uint8_t RA8875_touchDetect(void);
void RA8875_touchRead(uint16_t* x, uint16_t* y);

// ??????
void RA8875_softReset(void);
void RA8875_hardwareReset(void);
void RA8875_clearMemory(uint8_t color);

// ??????
void RA8875_interruptEnable(uint8_t source);
void RA8875_interruptDisable(uint8_t source);
uint8_t RA8875_readInterruptStatus(void);
void RA8875_clearInterrupt(uint8_t source);

// DMA ? BTE????????
void RA8875_BTE_move(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint16_t width, uint16_t height);
void RA8875_DMA_block(uint32_t srcAddr, uint32_t destAddr, uint32_t length);

// ?????
void RA8875_setLayer(uint8_t layer);
void RA8875_clearLayer(uint8_t layer, uint16_t color);

// ??????
void RA8875_drawBitmap(uint16_t x, uint16_t y, const uint8_t* bitmap, uint16_t w, uint16_t h);

// GPIO ????
void RA8875_GPIOX_set(uint8_t val);
void RA8875_PWM1config(uint8_t on, uint8_t clock);
void RA8875_PWM2config(uint8_t on, uint8_t clock);

// ????????????...

//void RA8875_writeReg(uint8_t reg, uint8_t value);
//uint8_t RA8875_readReg(uint8_t reg);
void RA8875_setRotation(uint8_t rotation);
uint8_t RA8875_getRotation();

void fillCrescent(uint16_t xCenter, uint16_t yCenter,
                  uint16_t longAxisOuter, uint16_t shortAxisOuter,
                  uint16_t longAxisInner, uint16_t shortAxisInner,
                  uint16_t colorOuter, uint16_t colorInner, uint8_t alignType);

#endif /* RA8875_H_ */
