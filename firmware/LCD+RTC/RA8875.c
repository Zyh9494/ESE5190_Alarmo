#include "RA8875.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>


/***************************************
 * ????????????????
 ***************************************/

static void RA8875_PLLinit(void);
static void RA8875_initializeDisplay(void);
static void RA8875_setActiveWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/***************************************
 * ?????
 ***************************************/
void RA8875_init(void)
{
    // ???????
    LCD_DDR |= (1 << LCD_RST) | (1 << LCD_MOSI) | (1 << LCD_SCK) | (1 << LCD_TFT_CS);
    LCD_DDR &= ~(1 << LCD_MISO);
    
    LCD_PORT |= (1 << LCD_TFT_CS);  // ? CS ???????

    RA8875_hardwareReset();
    
    // ??? SPI ??
    SPI_init();

    // ??? PLL
    RA8875_PLLinit();

    // ??????
    RA8875_initializeDisplay();

    // ????
    RA8875_displayOn(1);
    
    RA8875_GPIOXOn(1);

    // ??
    RA8875_fillScreen(RA8875_BLACK);
}

/***************************************
 * SPI ????
 ***************************************/
void SPI_init(void)
{ 
    SPCR0 = (1<<SPE) | (1<<MSTR);		//Enable SPI, Master, set clock rate fck/64
//	SPSR0 = (1<<SPI2X);										//SPI 2X speed
}

void RA8875_writeCommand(uint8_t cmd)
{
    // ????
    CS_LOW();

    // ??????
    SPDR0 = RA8875_CMDWRITE;
    while (!(SPSR0 & (1 << SPIF)));
    // ????
    SPDR0 = cmd;
    while (!(SPSR0 & (1 << SPIF)));

    // ????
    CS_HIGH();
}

void RA8875_writeData(uint8_t data)
{
    // ????
    CS_LOW();

    // ??????
    SPDR0 = RA8875_DATAWRITE;
    while (!(SPSR0 & (1 << SPIF)));
    // ????
    SPDR0 = data;
    while (!(SPSR0 & (1 << SPIF)));

    // ????
    CS_HIGH();
}

uint8_t RA8875_readData(void)
{
    uint8_t data;

    // CS ????? SPI ??
    CS_LOW();

    // ???????
    SPDR0 = RA8875_DATAREAD;
    while (!(SPSR0 & (1 << SPIF))); // ??????

    _delay_us(10);

    // ???????????????
    SPDR0 = 0x00;
    while (!(SPSR0 & (1 << SPIF))); // ??????

    // ???????
    data = SPDR0;

    // CS ???????
    CS_HIGH();

    return data;
}


void RA8875_writeReg(uint8_t reg, uint8_t val)
{
    RA8875_writeCommand(reg);
    RA8875_writeData(val);
}

uint8_t RA8875_readReg(uint8_t reg)
{
    uint8_t val;
    RA8875_writeCommand(reg);
    _delay_us(10); 
    val = RA8875_readData();
    return val;
}

/***************************************
 * ????
 ***************************************/
void Delay_ms(uint16_t ms)
{
    while (ms--)
    {
        _delay_ms(1);
    }
}

/***************************************
 * ??????
 ***************************************/
void RA8875_displayOn(uint8_t on)
{
    if (on)
    {
        RA8875_writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
    }
    else
    {
        RA8875_writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
    }
    Delay_ms(1);
}

void RA8875_sleep(uint8_t sleep)
{
    if (sleep)
    {
        RA8875_writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP);
    }
    else
    {
        RA8875_writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_NORMAL);
    }
    Delay_ms(1);
}

/***************************************
 * ????
 ***************************************/
//void RA8875_setXY(uint16_t x, uint16_t y)
//{
//    RA8875_writeReg(0x46, x & 0xFF);          // CURH0
//    RA8875_writeReg(0x47, (x >> 8) & 0xFF);   // CURH1
//    RA8875_writeReg(0x48, y & 0xFF);          // CURV0
//    RA8875_writeReg(0x49, (y >> 8) & 0xFF);   // CURV1
//}
//
//void RA8875_drawPixel(uint16_t x, uint16_t y, uint16_t color)
//{
//    // ??????
//    RA8875_writeReg(0x46, x & 0xFF);          // CURH0
//    RA8875_writeReg(0x47, (x >> 8) & 0xFF);   // CURH1
//    RA8875_writeReg(0x48, y & 0xFF);          // CURV0
//    RA8875_writeReg(0x49, (y >> 8) & 0xFF);   // CURV1
//
//    CS_LOW();
//    RA8875_writeCommand(RA8875_MRWC);
//    RA8875_writeData((color >> 8) & 0xFF); // ???????
//    RA8875_writeData(color & 0xFF);        // ???????
//    CS_HIGH();
//
//}

int16_t _rotation = 0 ;

int16_t RA8875_applyRotationX(int16_t x) {
    switch (_rotation) {
    case 2: // ??? 180° ??
        x = RA8875_WIDTH - 1 - x; // ?? X ??
        break;
    }
    return x;
}

int16_t RA8875_applyRotationY(int16_t y) {
    switch (_rotation) {
    case 2: // ??? 180° ??
        y = RA8875_HEIGHT - 1 - y; // ?? Y ??
        break;
    }
    return y ; // ????????????
}

// ??????? X/Y ??
void RA8875_setXY(uint16_t x, uint16_t y) {
    RA8875_writeReg(RA8875_CURH0, x & 0xFF);
    RA8875_writeReg(RA8875_CURH1, (x >> 8) & 0xFF);
    RA8875_writeReg(RA8875_CURV0, y & 0xFF);
    RA8875_writeReg(RA8875_CURV1, (y >> 8) & 0xFF);
}

// ??????
void RA8875_drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    x = RA8875_applyRotationX(x);
    y = RA8875_applyRotationY(y);

    RA8875_setXY(x, y);
    RA8875_writeCommand(RA8875_MRWC);

    RA8875_writeData(color >> 8); // ?????
    RA8875_writeData(color & 0xFF); // ?????
}

void RA8875_drawPixels(uint16_t *p, uint32_t num, uint16_t x, uint16_t y) {
    x = RA8875_applyRotationX(x);
    y = RA8875_applyRotationY(y);

    RA8875_setXY(x, y);

    uint8_t dir = RA8875_MWCR0_LRTD;
    if (_rotation == 2) {
        dir = RA8875_MWCR0_RLTD;
    }
    RA8875_writeReg(RA8875_MWCR0, 
        (RA8875_readReg(RA8875_MWCR0) & ~RA8875_MWCR0_DIRMASK) | dir);

    RA8875_writeCommand(RA8875_MRWC);
    while (num--) {
        RA8875_writeData((*p) >> 8); // ?????
        RA8875_writeData((*p) & 0xFF); // ?????
        p++;
    }
}

void RA8875_drawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color) {
    RA8875_drawLine(x, y, x, y + h, color);
}

void RA8875_drawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
    RA8875_drawLine(x, y, x + w, y, color);
}

void RA8875_drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    RA8875_rectHelper(x, y, x + w - 1, y + h - 1, color, 0); // false for outline
}

void RA8875_fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    RA8875_rectHelper(x, y, x + w - 1, y + h - 1, color, 1); // true for fill
}

void RA8875_fillScreen(uint16_t color) {
    RA8875_rectHelper(0, 0, RA8875_WIDTH - 1, RA8875_HEIGHT - 1, color, 1);
}

void RA8875_drawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color) {
    RA8875_circleHelper(x, y, r, color, 0); // 0 ?????
}

void RA8875_fillCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color) {
    RA8875_circleHelper(x, y, r, color, 1); // 1 ?????
}

void RA8875_drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                         uint16_t x2, uint16_t y2, uint16_t color) {
    RA8875_triangleHelper(x0, y0, x1, y1, x2, y2, color, 0); // 0 ????
}

void RA8875_fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                         uint16_t x2, uint16_t y2, uint16_t color) {
    RA8875_triangleHelper(x0, y0, x1, y1, x2, y2, color, 1); // 1 ????
}

void RA8875_drawEllipse(uint16_t xCenter, uint16_t yCenter,
                        uint16_t longAxis, uint16_t shortAxis, uint16_t color) {
    RA8875_ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, 0);
}

void RA8875_fillEllipse(uint16_t xCenter, uint16_t yCenter,
                        uint16_t longAxis, uint16_t shortAxis, uint16_t color) {
    RA8875_ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, 1);
}

void RA8875_drawCurve(uint16_t xCenter, uint16_t yCenter,
                      uint16_t longAxis, uint16_t shortAxis,
                      uint8_t curvePart, uint16_t color) {
    RA8875_curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, 0);
}

void RA8875_fillCurve(uint16_t xCenter, uint16_t yCenter,
                      uint16_t longAxis, uint16_t shortAxis,
                      uint8_t curvePart, uint16_t color) {
    RA8875_curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, 1);
}

void RA8875_drawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                          uint16_t r, uint16_t color) {
    RA8875_roundRectHelper(x, y, x + w, y + h, r, color, 0);
}

void RA8875_fillRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                          uint16_t r, uint16_t color) {
    RA8875_roundRectHelper(x, y, x + w, y + h, r, color, 1);
}

//void RA8875_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
//{
//    // ????
//    RA8875_writeReg(0x91, x0 & 0xFF);          // DLHSR0
//    RA8875_writeReg(0x92, (x0 >> 8) & 0xFF);   // DLHSR1
//    RA8875_writeReg(0x93, y0 & 0xFF);          // DLVSR0
//    RA8875_writeReg(0x94, (y0 >> 8) & 0xFF);   // DLVSR1
//
//    // ????
//    RA8875_writeReg(0x95, x1 & 0xFF);          // DLHER0
//    RA8875_writeReg(0x96, (x1 >> 8) & 0xFF);   // DLHER1
//    RA8875_writeReg(0x97, y1 & 0xFF);          // DLVER0
//    RA8875_writeReg(0x98, (y1 >> 8) & 0xFF);   // DLVER1
//
//    // ??????
//    RA8875_setForegroundColor(color);
//
//    // ??????
//    RA8875_writeReg(RA8875_DCR, RA8875_DCR_LINESQUTRI_START | RA8875_DCR_DRAWLINE);
//
//    // ??????
//    while (RA8875_readReg(RA8875_DCR) & RA8875_DCR_LINESQUTRI_STATUS);
//}
//
//void RA8875_drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, uint8_t filled)
//{
//    // ???????
//    RA8875_writeReg(0x91, x & 0xFF);          // DLHSR0
//    RA8875_writeReg(0x92, (x >> 8) & 0xFF);   // DLHSR1
//    RA8875_writeReg(0x93, y & 0xFF);          // DLVSR0
//    RA8875_writeReg(0x94, (y >> 8) & 0xFF);   // DLVSR1
//
//    // ???????
//    RA8875_writeReg(0x95, (x + w - 1) & 0xFF);        // DLHER0
//    RA8875_writeReg(0x96, ((x + w - 1) >> 8) & 0xFF); // DLHER1
//    RA8875_writeReg(0x97, (y + h - 1) & 0xFF);        // DLVER0
//    RA8875_writeReg(0x98, ((y + h - 1) >> 8) & 0xFF); // DLVER1
//
//    // ??????
//    RA8875_setForegroundColor(color);
//
//    // ??????
//    uint8_t cmd = RA8875_DCR_LINESQUTRI_START | RA8875_DCR_DRAWSQUARE;
//    if (filled)
//        cmd |= RA8875_DCR_FILL;
//    else
//        cmd |= RA8875_DCR_NOFILL;
//
//    RA8875_writeReg(RA8875_DCR, cmd);
//
//    // ??????
//    while (RA8875_readReg(RA8875_DCR) & RA8875_DCR_LINESQUTRI_STATUS);
//}
//
//void RA8875_drawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint8_t filled)
//{
//    // ??????
//    RA8875_writeReg(0x99, x & 0xFF);          // DCHR0
//    RA8875_writeReg(0x9A, (x >> 8) & 0xFF);   // DCHR1
//    RA8875_writeReg(0x9B, y & 0xFF);          // DCVR0
//    RA8875_writeReg(0x9C, (y >> 8) & 0xFF);   // DCVR1
//
//    // ????
//    RA8875_writeReg(0x9D, r);                 // DCRR
//
//    // ??????
//    RA8875_setForegroundColor(color);
//
//    // ??????
//    uint8_t cmd = RA8875_DCR_CIRCLE_START;
//    if (filled)
//        cmd |= RA8875_DCR_FILL;
//    else
//        cmd |= RA8875_DCR_NOFILL;
//
//    RA8875_writeReg(RA8875_DCR, cmd);
//
//    // ??????
//    while (RA8875_readReg(RA8875_DCR) & RA8875_DCR_CIRCLE_STATUS);
//}

void RA8875_setForegroundColor(uint16_t color)
{
    RA8875_writeReg(RA8875_FGCR, (color >> 11) << 1);      // ???? 5 ???? 1 ?
    RA8875_writeReg(RA8875_FGCG, (color >> 5) & 0x3F);     // ???6 ?
    RA8875_writeReg(RA8875_FGCB, (color & 0x1F) << 1);     // ???? 5 ???? 1 ?
}

void RA8875_setBackgroundColor(uint16_t color)
{
    RA8875_writeReg(RA8875_BGCR, (color >> 11) << 1);      // ??
    RA8875_writeReg(RA8875_BGCG, (color >> 5) & 0x3F);     // ??
    RA8875_writeReg(RA8875_BGCB, (color & 0x1F) << 1);     // ??
}

/***************************************
 * ???????
 ***************************************/
void RA8875_textMode(void)
{
    uint8_t temp = RA8875_readReg(0x40); // Draw/Write Control Register
    temp |= 0x80; // ???????
    RA8875_writeReg(0x40, temp);
}

void RA8875_graphicsMode(void)
{
    uint8_t temp = RA8875_readReg(0x40); // Draw/Write Control Register
    temp &= ~0x80; // ???????????????
    RA8875_writeReg(0x40, temp);
}

void RA8875_setFontSize(uint8_t size)
{
    uint8_t temp = RA8875_readReg(RA8875_FNCR1);
    temp &= ~(0xF0); // ???????

    switch (size)
    {
        case 0: // 1X
            temp |= 0x00;
            break;
        case 1: // 2X
            temp |= 0x50;
            break;
        case 2: // 3X
            temp |= 0xA0;
            break;
        case 3: // 4X
            temp |= 0xF0;
            break;
        default:
            temp |= 0x00;
            break;
    }

    RA8875_writeReg(RA8875_FNCR1, temp);
}

void RA8875_textWrite(const char* buffer, uint16_t len)
{
    RA8875_writeCommand(RA8875_MRWC); // Memory Write Command

    for (uint16_t i = 0; i < len; i++)
    {
        RA8875_writeData(buffer[i]);
    }
}

void RA8875_setCursor(uint16_t x, uint16_t y)
{
    RA8875_writeReg(RA8875_CURH0, x & 0xFF);
    RA8875_writeReg(RA8875_CURH1, (x >> 8) & 0xFF);
    RA8875_writeReg(RA8875_CURV0, y & 0xFF);
    RA8875_writeReg(RA8875_CURV1, (y >> 8) & 0xFF);
}

void RA8875_setTextColor(uint16_t foreColor, uint16_t bgColor)
{
    RA8875_setForegroundColor(foreColor);
    RA8875_setBackgroundColor(bgColor);
}

/***************************************
 * ??????
 ***************************************/
void RA8875_backlight(uint8_t on, uint8_t PWM)
{
    if (on)
    {
        // ??????? PWM1
        RA8875_writeReg(RA8875_P1CR, 0x80 | 0x00); // PWM1 enable, PWM output
        RA8875_writeReg(RA8875_P1DCR, PWM);        // ??????255 ?????
    }
    else
    {
        // ????
        RA8875_writeReg(RA8875_P1CR, 0x00); // PWM1 disable
    }
}

/***************************************
 * ???????????
 ***************************************/
void RA8875_touchEnable(uint8_t on)
{
    if (on)
    {
        RA8875_writeReg(RA8875_TPCR0, 0x80 | 0x08); // ???????????
        RA8875_writeReg(RA8875_INTC1, 0x04);        // ??????
    }
    else
    {
        RA8875_writeReg(RA8875_TPCR0, 0x00);        // ??????
        RA8875_writeReg(RA8875_INTC1, 0x00);        // ??????
    }
}

uint8_t RA8875_touchDetect(void)
{
    uint8_t status = RA8875_readReg(RA8875_INTC2);
    if (status & 0x01)
    {
        // ????????
        RA8875_writeReg(RA8875_INTC2, 0x01);
        return 1;
    }
    return 0;
}

void RA8875_touchRead(uint16_t* x, uint16_t* y)
{
    uint16_t tx, ty;
    uint8_t temp;

    temp = RA8875_readReg(RA8875_TPXH);
    tx = temp & 0x7F;
    tx <<= 2;
    temp = RA8875_readReg(RA8875_TPXYL);
    tx |= (temp & 0xC0) >> 6;

    temp = RA8875_readReg(RA8875_TPYH);
    ty = temp & 0x7F;
    ty <<= 2;
    temp = RA8875_readReg(RA8875_TPXYL);
    ty |= (temp & 0x30) >> 4;

    *x = tx;
    *y = ty;
}

/***************************************
 * ??????
 ***************************************/
void RA8875_softReset(void)
{
    RA8875_writeReg(RA8875_PWRR, RA8875_PWRR_SOFTRESET);
    Delay_ms(1);
}

void RA8875_hardwareReset(void)
{
    // ?? RST ??
    RST_LOW();
    Delay_ms(10);
    // ?? RST ??
    RST_HIGH();
    Delay_ms(100);
}

/***************************************
 * ??????
 ***************************************/
static void RA8875_PLLinit(void)
{
    RA8875_writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 11); // ?? PLLC1
    Delay_ms(1);
    RA8875_writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);         // ?? PLLC2
    Delay_ms(1);
}

//static void RA8875_initializeDisplay(void)
//{
//    // ??????
//    RA8875_writeReg(RA8875_SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);
//    Delay_ms(1);
//
//    // ??????
//    RA8875_writeReg(RA8875_PCSR, RA8875_PCSR_PDATL | RA8875_PCSR_2CLK);
//    Delay_ms(1);
//
//    // ?? LCD ??
//    RA8875_writeReg(RA8875_HDWR, (RA8875_WIDTH / 8) - 1);          // ????
//    RA8875_writeReg(RA8875_HNDFTR, 0x00);                          // ????????
//    RA8875_writeReg(RA8875_HNDR, 0x02);                            // ??????
//    RA8875_writeReg(RA8875_HSTR, 0x02);                            // HSYNC ????
//    RA8875_writeReg(RA8875_HPWR, 0x00);                            // HSYNC ????
//
//    RA8875_writeReg(RA8875_VDHR0, (RA8875_HEIGHT - 1) & 0xFF);     // ???????
//    RA8875_writeReg(RA8875_VDHR1, (RA8875_HEIGHT - 1) >> 8);       // ???????
//    RA8875_writeReg(RA8875_VNDR0, 0x0A);                           // ?????????
//    RA8875_writeReg(RA8875_VNDR1, 0x00);                           // ?????????
//    RA8875_writeReg(RA8875_VSTR0, 0x00);                           // VSYNC ???????
//    RA8875_writeReg(RA8875_VSTR1, 0x00);                           // VSYNC ???????
//    RA8875_writeReg(RA8875_VPWR, 0x00);                            // VSYNC ????
//
//    // ??????
//    RA8875_setActiveWindow(0, 0, RA8875_WIDTH - 1, RA8875_HEIGHT - 1);
//
//    // ???????????
//    // RA8875_touchEnable(1);
//}

static void RA8875_initializeDisplay(void)
{
    // ??????
    RA8875_writeReg(RA8875_SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);
    Delay_ms(1);

    // ??????
    RA8875_writeReg(RA8875_PCSR, RA8875_PCSR_PDATL | RA8875_PCSR_2CLK);
    Delay_ms(1);
    
    uint8_t hsync_start = 32;    // ??????????
    uint8_t hsync_pw = 96;       // ????????
    uint8_t hsync_finetune = 0;  // ?????????? 0
    uint8_t hsync_nondisp = 26;  // ???????
    uint8_t vsync_pw = 2;        // ????????
    uint16_t vsync_nondisp = 32; // ???????
    uint16_t vsync_start = 23;   // ??????????
    int16_t _voffset = 0;        // ?????


    // ?? LCD ??
    RA8875_writeReg(RA8875_HDWR, (RA8875_WIDTH / 8) - 1);          // ????
    RA8875_writeReg(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);                          // ????????
    RA8875_writeReg(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2) /
                            8);                            // ??????
    RA8875_writeReg(RA8875_HSTR, hsync_start / 8 - 1);                            // HSYNC ????
    RA8875_writeReg(RA8875_HPWR,  RA8875_HPWR_LOW + (hsync_pw / 8 - 1));                            // HSYNC ????

    RA8875_writeReg(RA8875_VDHR0, (RA8875_HEIGHT - 1 + _voffset) & 0xFF);     // ???????
    RA8875_writeReg(RA8875_VDHR1, (RA8875_HEIGHT - 1 + _voffset) >> 8);       // ???????
    RA8875_writeReg(RA8875_VNDR0, vsync_nondisp - 1);                           // ?????????
    RA8875_writeReg(RA8875_VNDR1, vsync_nondisp >> 8);                           // ?????????
    RA8875_writeReg(RA8875_VSTR0, vsync_start - 1);                           // VSYNC ???????
    RA8875_writeReg(RA8875_VSTR1, vsync_start >> 8);                           // VSYNC ???????
    RA8875_writeReg(RA8875_VPWR, RA8875_VPWR_LOW + vsync_pw - 1);                            // VSYNC ????

    // ??????
    RA8875_setActiveWindow(0, 0, RA8875_WIDTH - 1, RA8875_HEIGHT - 1);

    // ???????????
    // RA8875_touchEnable(1);
}

static void RA8875_setActiveWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    RA8875_writeReg(RA8875_HSAW0, x0 & 0xFF);
    RA8875_writeReg(RA8875_HSAW1, (x0 >> 8) & 0xFF);
    RA8875_writeReg(RA8875_HEAW0, x1 & 0xFF);
    RA8875_writeReg(RA8875_HEAW1, (x1 >> 8) & 0xFF);

    RA8875_writeReg(RA8875_VSAW0, y0 & 0xFF);
    RA8875_writeReg(RA8875_VSAW1, (y0 >> 8) & 0xFF);
    RA8875_writeReg(RA8875_VEAW0, y1 & 0xFF);
    RA8875_writeReg(RA8875_VEAW1, (y1 >> 8) & 0xFF);
}

/***************************************
 * GPIO ????
 ***************************************/

void RA8875_GPIOXOn(uint8_t on) {
    if (on) {
        RA8875_writeReg(RA8875_GPIOX, 1); // ?? GPIOX
    } else {
        RA8875_writeReg(RA8875_GPIOX, 0); // ?? GPIOX
    }
}


void RA8875_PWM1config(uint8_t on, uint8_t clock)
{
    if (on)
        RA8875_writeReg(RA8875_P1CR, 0x80 | (clock & 0x0F));
    else
        RA8875_writeReg(RA8875_P1CR, 0x00);
}

void RA8875_PWM2config(uint8_t on, uint8_t clock)
{
    if (on)
        RA8875_writeReg(RA8875_P2CR, 0x80 | (clock & 0x0F));
    else
        RA8875_writeReg(RA8875_P2CR, 0x00);
}

/***************************************
 * ??????
 ***************************************/
void RA8875_interruptEnable(uint8_t source)
{
    uint8_t temp = RA8875_readReg(RA8875_INTC1);
    temp |= source;
    RA8875_writeReg(RA8875_INTC1, temp);
}

void RA8875_interruptDisable(uint8_t source)
{
    uint8_t temp = RA8875_readReg(RA8875_INTC1);
    temp &= ~source;
    RA8875_writeReg(RA8875_INTC1, temp);
}

uint8_t RA8875_readInterruptStatus(void)
{
    return RA8875_readReg(RA8875_INTC2);
}

void RA8875_clearInterrupt(uint8_t source)
{
    RA8875_writeReg(RA8875_INTC2, source);
}

/**************************************************************************/
/*!
      Helper function for higher level rectangle drawing code
*/
/**************************************************************************/
//typedef uint8_t bool; // ???????
//#define true  1
//#define false 0
//
//bool RA8875_waitPoll(uint8_t regname, uint8_t waitflag) {
//    while (1) {
//        uint8_t temp = RA8875_readReg(regname); // ????????
//        if (!(temp & waitflag)) {
//            return true; // ??????? true
//        }
//    }
//    return false; // ?????????
//}

//#define TRUE  1
//#define FALSE 0
//
//// ??????????
//uint8_t RA8875_waitPoll(uint8_t regname, uint8_t waitflag) {
//    while (1) {
//        uint8_t temp = RA8875_readReg(regname); // ??????
//        if (!(temp & waitflag)) {              // ?????
//            return TRUE;                       // ??????? TRUE
//        }
//        // ?????????? CPU ???
//        _delay_ms(1);
//    }
//    return FALSE; // ?????????
//}

#define RA8875_MAX_WAIT 5000 // ????????????


//uint8_t RA8875_waitPoll(uint8_t regname, uint8_t waitflag) {
//    for (uint16_t elapsed = 0; elapsed < RA8875_MAX_WAIT; elapsed++) {
//        uint8_t temp = RA8875_readReg(regname);
//        if (!(temp & waitflag)) {  // ??????????
//            return 1;              // ?? 1 ??????
//        }
//        _delay_ms(1);              // ??????? 1 ??
//    }
//    return 0;                      // ???? 0 ???????
//}

uint8_t RA8875_waitPoll(uint8_t regname, uint8_t waitflag) {
    uint8_t flag = 1;            // ?????????
    uint16_t elapsed = 0;        // ?????

    while (flag && elapsed < RA8875_MAX_WAIT) { // ?????????
        uint8_t temp = RA8875_readReg(regname);
        if (!(temp & waitflag)) { // ????????
            flag = 0;             // ?? flag ? 0?????
        } else {
            _delay_ms(1);         // ?? 1 ??
            elapsed++;            // ?????
        }
    }

    return (flag == 0); // ???????? 1????? 0
}




//uint8_t RA8875_waitPoll(uint8_t regname, uint8_t waitflag) {
//    uint16_t elapsed = 0;
//
//    uart_sendString("Starting RA8875_waitPoll...\n");
//
//    while (1) {
//        uint8_t temp = RA8875_readReg(regname);
//
//        // ?????????UART
//        char buffer[50];
//        sprintf(buffer, "Reg: 0x%02X, Value: 0x%02X\n", regname, temp);
//        uart_sendString(buffer);
//
//        // ????????????
//        if (!(temp & waitflag)) {
//            uart_sendString("Flag cleared, task complete.\n");
//            return 1; // ?? 1 ??????
//        }
//
//        _delay_ms(1); // ?? 1 ??
//        elapsed++;
//
//        // ??????
//        if (elapsed >= RA8875_MAX_WAIT) {
//            uart_sendString("Timeout occurred in RA8875_waitPoll!\n");
//            return 0; // ?? 0 ???????
//        }
//    }
//}


void RA8875_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    x0 = RA8875_applyRotationX(x0);
    y0 = RA8875_applyRotationY(y0);
    x1 = RA8875_applyRotationX(x1);
    y1 = RA8875_applyRotationY(y1);

    // ???? (x0, y0)
    RA8875_writeCommand(0x91);
    RA8875_writeData(x0 & 0xFF);          // ???
    RA8875_writeCommand(0x92);
    RA8875_writeData((x0 >> 8) & 0xFF);  // ???

    RA8875_writeCommand(0x93);
    RA8875_writeData(y0 & 0xFF);          // ???
    RA8875_writeCommand(0x94);
    RA8875_writeData((y0 >> 8) & 0xFF);  // ???

    // ???? (x1, y1)
    RA8875_writeCommand(0x95);
    RA8875_writeData(x1 & 0xFF);          // ???
    RA8875_writeCommand(0x96);
    RA8875_writeData((x1 >> 8) & 0xFF);  // ???

    RA8875_writeCommand(0x97);
    RA8875_writeData(y1 & 0xFF);          // ???
    RA8875_writeCommand(0x98);
    RA8875_writeData((y1 >> 8) & 0xFF);  // ???

    // ????
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xF800) >> 11); // ??
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07E0) >> 5);  // ??
    RA8875_writeCommand(0x65);
    RA8875_writeData(color & 0x001F);         // ??

    // ??????
    RA8875_writeCommand(RA8875_DCR);
    RA8875_writeData(0x80);

    // ??????
    RA8875_waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void RA8875_circleHelper(uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint8_t filled) {
    // ???????
    RA8875_writeCommand(0x99);
    RA8875_writeData(x & 0xFF);
    RA8875_writeCommand(0x9A);
    RA8875_writeData(x >> 8);

    RA8875_writeCommand(0x9B);
    RA8875_writeData(y & 0xFF);
    RA8875_writeCommand(0x9C);
    RA8875_writeData(y >> 8);

    RA8875_writeCommand(0x9D);
    RA8875_writeData(r);

    // ????
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xF800) >> 11);
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07E0) >> 5);
    RA8875_writeCommand(0x65);
    RA8875_writeData(color & 0x001F);

    // ??
    RA8875_writeCommand(RA8875_DCR);
    if (filled) {
        RA8875_writeData(0x60); // ???
    } else {
        RA8875_writeData(0x40); // ????
    }

    // ??????
    RA8875_waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);
}

void RA8875_rectHelper(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, uint8_t filled)
 {

    x = RA8875_applyRotationX(x);
    y = RA8875_applyRotationY(y);
    w = RA8875_applyRotationX(w);
    h = RA8875_applyRotationY(h);

    // ?? X ??
    RA8875_writeCommand(0x91);
    RA8875_writeData(x & 0xFF);       // ? 8 ?
    RA8875_writeCommand(0x92);
    RA8875_writeData((x >> 8) & 0xFF); // ? 8 ?

    // ?? Y ??
    RA8875_writeCommand(0x93);
    RA8875_writeData(y & 0xFF);       // ? 8 ?
    RA8875_writeCommand(0x94);
    RA8875_writeData((y >> 8) & 0xFF); // ? 8 ?

    // ?? X1 ??
    RA8875_writeCommand(0x95);
    RA8875_writeData(w & 0xFF);       // ? 8 ?
    RA8875_writeCommand(0x96);
    RA8875_writeData((w >> 8) & 0xFF); // ? 8 ?

    // ?? Y1 ??
    RA8875_writeCommand(0x97);
    RA8875_writeData(h & 0xFF);       // ? 8 ?
    RA8875_writeCommand(0x98);
    RA8875_writeData((h >> 8) & 0xFF); // ? 8 ?

    // ????
    RA8875_writeCommand(0x63); // ??????
    RA8875_writeData((color & 0xF800) >> 11);
    RA8875_writeCommand(0x64); // ??????
    RA8875_writeData((color & 0x07E0) >> 5);
    RA8875_writeCommand(0x65); // ??????
    RA8875_writeData(color & 0x001F);

    // ????
    RA8875_writeCommand(RA8875_DCR);
    if (filled) {
        RA8875_writeData(0xB0); // ????
    } else {
        RA8875_writeData(0x90); // ?????
    }

    // ??????
    RA8875_waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

// ?????????
void RA8875_triangleHelper(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint8_t filled) {
    // ????
    x0 = RA8875_applyRotationX(x0);
    y0 = RA8875_applyRotationY(y0);
    x1 = RA8875_applyRotationX(x1);
    y1 = RA8875_applyRotationY(y1);
    x2 = RA8875_applyRotationX(x2);
    y2 = RA8875_applyRotationY(y2);

    // ??? 0 ??
    RA8875_writeCommand(0x91);
    RA8875_writeData(x0 & 0xFF);         // ? 0 X ???
    RA8875_writeCommand(0x92);
    RA8875_writeData((x0 >> 8) & 0xFF); // ? 0 X ???
    RA8875_writeCommand(0x93);
    RA8875_writeData(y0 & 0xFF);         // ? 0 Y ???
    RA8875_writeCommand(0x94);
    RA8875_writeData((y0 >> 8) & 0xFF); // ? 0 Y ???

    // ??? 1 ??
    RA8875_writeCommand(0x95);
    RA8875_writeData(x1 & 0xFF);         // ? 1 X ???
    RA8875_writeCommand(0x96);
    RA8875_writeData((x1 >> 8) & 0xFF); // ? 1 X ???
    RA8875_writeCommand(0x97);
    RA8875_writeData(y1 & 0xFF);         // ? 1 Y ???
    RA8875_writeCommand(0x98);
    RA8875_writeData((y1 >> 8) & 0xFF); // ? 1 Y ???

    // ??? 2 ??
    RA8875_writeCommand(0xA9);
    RA8875_writeData(x2 & 0xFF);         // ? 2 X ???
    RA8875_writeCommand(0xAA);
    RA8875_writeData((x2 >> 8) & 0xFF); // ? 2 X ???
    RA8875_writeCommand(0xAB);
    RA8875_writeData(y2 & 0xFF);         // ? 2 Y ???
    RA8875_writeCommand(0xAC);
    RA8875_writeData((y2 >> 8) & 0xFF); // ? 2 Y ???

    // ????
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xF800) >> 11); // ????
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07E0) >> 5);  // ????
    RA8875_writeCommand(0x65);
    RA8875_writeData(color & 0x001F);         // ????

    // ??????
    RA8875_writeCommand(RA8875_DCR);
    if (filled) {
        RA8875_writeData(0xA1); // ???????
    } else {
        RA8875_writeData(0x81); // ???????
    }

    // ??????
    RA8875_waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void RA8875_ellipseHelper(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color, uint8_t filled) {
    // ??????
    xCenter = RA8875_applyRotationX(xCenter);
    yCenter = RA8875_applyRotationY(yCenter);

    // ?????
    RA8875_writeCommand(0xA5);
    RA8875_writeData(xCenter & 0xFF);         // X ???
    RA8875_writeCommand(0xA6);
    RA8875_writeData((xCenter >> 8) & 0xFF); // X ???
    RA8875_writeCommand(0xA7);
    RA8875_writeData(yCenter & 0xFF);         // Y ???
    RA8875_writeCommand(0xA8);
    RA8875_writeData((yCenter >> 8) & 0xFF); // Y ???

    // ???????
    RA8875_writeCommand(0xA1);
    RA8875_writeData(longAxis & 0xFF);         // ?????
    RA8875_writeCommand(0xA2);
    RA8875_writeData((longAxis >> 8) & 0xFF); // ?????
    RA8875_writeCommand(0xA3);
    RA8875_writeData(shortAxis & 0xFF);         // ?????
    RA8875_writeCommand(0xA4);
    RA8875_writeData((shortAxis >> 8) & 0xFF); // ?????

    // ????
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xF800) >> 11); // ??
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07E0) >> 5);  // ??
    RA8875_writeCommand(0x65);
    RA8875_writeData(color & 0x001F);         // ??

    // ??????
    RA8875_writeCommand(0xA0);
    if (filled) {
        RA8875_writeData(0xC0); // ????
    } else {
        RA8875_writeData(0x80); // ????
    }

    // ??????
    RA8875_waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

uint8_t RA8875_getRotation() {
    return _rotation; // ????????
}

void RA8875_setRotation(uint8_t rotation) {
    _rotation = rotation % 4; // ??? 0-3 ????????
}

void RA8875_curveHelper(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint8_t curvePart, uint16_t color, uint8_t filled) {
    // ??????
    xCenter = RA8875_applyRotationX(xCenter);
    yCenter = RA8875_applyRotationY(yCenter);
    curvePart = (curvePart + RA8875_getRotation()) % 4; // ??????????

    // ?????
    RA8875_writeCommand(0xA5);
    RA8875_writeData(xCenter & 0xFF);         // X ???
    RA8875_writeCommand(0xA6);
    RA8875_writeData((xCenter >> 8) & 0xFF); // X ???
    RA8875_writeCommand(0xA7);
    RA8875_writeData(yCenter & 0xFF);         // Y ???
    RA8875_writeCommand(0xA8);
    RA8875_writeData((yCenter >> 8) & 0xFF); // Y ???

    // ???????
    RA8875_writeCommand(0xA1);
    RA8875_writeData(longAxis & 0xFF);         // ?????
    RA8875_writeCommand(0xA2);
    RA8875_writeData((longAxis >> 8) & 0xFF); // ?????
    RA8875_writeCommand(0xA3);
    RA8875_writeData(shortAxis & 0xFF);         // ?????
    RA8875_writeCommand(0xA4);
    RA8875_writeData((shortAxis >> 8) & 0xFF); // ?????

    // ????
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xF800) >> 11); // ??
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07E0) >> 5);  // ??
    RA8875_writeCommand(0x65);
    RA8875_writeData(color & 0x001F);         // ??

    // ??????
    RA8875_writeCommand(0xA0);
    if (filled) {
        RA8875_writeData(0xD0 | (curvePart & 0x03)); // ????
    } else {
        RA8875_writeData(0x90 | (curvePart & 0x03)); // ????
    }

    // ??????
    RA8875_waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

void RA8875_roundRectHelper(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color, uint8_t filled) {
    // ????
    x = RA8875_applyRotationX(x);
    y = RA8875_applyRotationY(y);
    w = RA8875_applyRotationX(w);
    h = RA8875_applyRotationY(h);

    // ??????
    if (x > w) {
        int16_t temp = x;
        x = w;
        w = temp;
    }
    if (y > h) {
        int16_t temp = y;
        y = h;
        h = temp;
    }

    /* ????? X ?? */
    RA8875_writeCommand(0x91);
    RA8875_writeData(x & 0xFF);
    RA8875_writeCommand(0x92);
    RA8875_writeData((x >> 8) & 0xFF);

    /* ????? Y ?? */
    RA8875_writeCommand(0x93);
    RA8875_writeData(y & 0xFF);
    RA8875_writeCommand(0x94);
    RA8875_writeData((y >> 8) & 0xFF);

    /* ????? X1 ?? */
    RA8875_writeCommand(0x95);
    RA8875_writeData(w & 0xFF);
    RA8875_writeCommand(0x96);
    RA8875_writeData((w >> 8) & 0xFF);

    /* ????? Y1 ?? */
    RA8875_writeCommand(0x97);
    RA8875_writeData(h & 0xFF);
    RA8875_writeCommand(0x98);
    RA8875_writeData((h >> 8) & 0xFF);

    /* ?????? */
    RA8875_writeCommand(0xA1);
    RA8875_writeData(r & 0xFF);
    RA8875_writeCommand(0xA2);
    RA8875_writeData((r >> 8) & 0xFF);

    RA8875_writeCommand(0xA3);
    RA8875_writeData(r & 0xFF);
    RA8875_writeCommand(0xA4);
    RA8875_writeData((r >> 8) & 0xFF);

    /* ???? */
    RA8875_writeCommand(0x63);
    RA8875_writeData((color & 0xF800) >> 11); // ??
    RA8875_writeCommand(0x64);
    RA8875_writeData((color & 0x07E0) >> 5);  // ??
    RA8875_writeCommand(0x65);
    RA8875_writeData(color & 0x001F);         // ??

    /* ??? */
    RA8875_writeCommand(RA8875_ELLIPSE);
    if (filled) {
        RA8875_writeData(0xE0); // ????????
    } else {
        RA8875_writeData(0xA0); // ????????
    }

    /* ?????? */
    RA8875_waitPoll(RA8875_ELLIPSE, RA8875_DCR_LINESQUTRI_STATUS);
}

void fillCrescent(uint16_t xCenter, uint16_t yCenter,
                  uint16_t longAxisOuter, uint16_t shortAxisOuter,
                  uint16_t longAxisInner, uint16_t shortAxisInner,
                  uint16_t colorOuter, uint16_t colorInner, uint8_t alignType) {
    switch (alignType) {
        case 0: // X????Y?????????
            for (uint16_t shortAxis = shortAxisOuter; shortAxis >= shortAxisInner; shortAxis--) {
                RA8875_drawCurve(xCenter, yCenter, longAxisOuter, shortAxis, 1, colorOuter); // ????1???
                RA8875_drawCurve(xCenter, yCenter, longAxisOuter, shortAxis, 2, colorOuter); // ????2???
            }
            break;

        case 1: // X????Y?????????
            for (uint16_t shortAxis = shortAxisInner; shortAxis <= shortAxisOuter; shortAxis++) {
                RA8875_drawCurve(xCenter, yCenter, longAxisOuter, shortAxis, 0, colorInner); // ????3???
                RA8875_drawCurve(xCenter, yCenter, longAxisOuter, shortAxis, 3, colorInner); // ????4???
            }
            break;

        case 2: // Y????X?????????
            for (uint16_t longAxis = longAxisOuter; longAxis >= longAxisInner; longAxis--) {
                RA8875_drawCurve(xCenter, yCenter, longAxis, shortAxisOuter, 1, colorOuter); // ????1???
                RA8875_drawCurve(xCenter, yCenter, longAxis, shortAxisOuter, 0, colorOuter); // ????3???
            }
            break;

        case 3: // Y????X?????????
            for (uint16_t longAxis = longAxisInner; longAxis <= longAxisOuter; longAxis++) {
                RA8875_drawCurve(xCenter, yCenter, longAxis, shortAxisOuter, 2, colorInner); // ????2???
                RA8875_drawCurve(xCenter, yCenter, longAxis, shortAxisOuter, 3, colorInner); // ????4???
            }
            break;

        default:
            // ???????? alignType
            break;
    }
}