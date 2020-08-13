/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:           GLCD.h
** Descriptions:        TFT (IO)
**
**------------------------------------------------------------------------------------------------------
** Created by:          AVRman
** Created date:        2011-1-26
** Version:             1.0
** Descriptions:        The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
********************************************************************************************************/
#include "msp.h"
#ifndef __GLCD_H 
#define __GLCD_H

/* Includes ------------------------------------------------------------------*/
//#include "lpc17xx_gpio.h"
//#include "lpc17xx_pinsel.h"

/* Private define ------------------------------------------------------------*/

/* LCD Interface */
#define PIN_CS      (1 << 8)
#define PIN_RS      (1 << 27)
#define PIN_WR      (1 << 28)
#define PIN_RD      (1 << 29)

//#define LCD_CS(x)   ((x) ? GPIO_SetValue(2, PIN_CS ) : GPIO_ClearValue(2, PIN_CS ) );
#define LCD_CS(x)   ((x) ? (LPC_GPIO2->FIOSET = PIN_CS) : (LPC_GPIO2->FIOCLR = PIN_CS));

//#define LCD_RS(x)   ((x) ? GPIO_SetValue(1, PIN_RS ) : GPIO_ClearValue(1, PIN_RS ) );
#define LCD_RS(x)   ((x) ? (LPC_GPIO1->FIOSET = PIN_RS) : (LPC_GPIO1->FIOCLR = PIN_RS));

//#define LCD_WR(x)   ((x) ? GPIO_SetValue(1, PIN_WR ) : GPIO_ClearValue(1, PIN_WR ) );
#define LCD_WR(x)   ((x) ? (LPC_GPIO1->FIOSET = PIN_WR) : (LPC_GPIO1->FIOCLR = PIN_WR));

//#define LCD_RD(x)   ((x) ? GPIO_SetValue(1, PIN_RD ) : GPIO_ClearValue(1, PIN_RD ) );
#define LCD_RD(x)   ((x) ? (LPC_GPIO1->FIOSET = PIN_RD) : (LPC_GPIO1->FIOCLR = PIN_RD));


/* Private define ------------------------------------------------------------*/
#define DISP_ORIENTATION  0  /* angle 0 90 */ 

#if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )

#define  MAX_X  320
#define  MAX_Y  240   

#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )

#define  MAX_X  320
#define  MAX_Y  240

#endif

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

/* ORIGINAL DRIVER MACROS */
/* Screen size */
#define MAX_SCREEN_X     320
#define MAX_SCREEN_Y     240
#define MIN_SCREEN_X     0
#define MIN_SCREEN_Y     0
#define SCREEN_SIZE      76800

/* Register details */
#define SPI_START   (0x70)     /* Start byte for SPI transfer  */
#define SPI_RD      (0x01)     /* WR bit 1 within start        */
#define SPI_WR      (0x00)     /* WR bit 0 within start        */
#define SPI_DATA    (0x02)     /* RS bit 1 within start byte   */
#define SPI_INDEX   (0x00)     /* RS bit 0 within start byte   */

/* CS LCD*/
#define SPI_CS_LOW P10OUT &= ~BIT4
#define SPI_CS_HIGH P10OUT |= BIT4

/* CS Touchpanel */
#define SPI_CS_TP_LOW   P10OUT &= ~BIT5
#define SPI_CS_TP_HIGH  P10OUT |= BIT5
#define MASK_TP_IRQ     __NVIC_DisableIRQ(PORT4_IRQn)
#define UNMASK_TP_IRQ   __NVIC_EnableIRQ(PORT4_IRQn)

/* XPT2046 registers definition for X and Y coordinate retrieval */
#define CHX         0x90
#define CHY         0xD0

/* LCD colors */
#define LCD_WHITE          0xFFFF
#define LCD_BLACK          0x0000
#define LCD_BLUE           0x0197
#define LCD_RED            0xF800
#define LCD_MAGENTA        0xF81F
#define LCD_GREEN          0x07E0
#define LCD_CYAN           0x7FFF
#define LCD_YELLOW         0xFFE0
#define LCD_GRAY           0x2104
#define LCD_PURPLE         0xF11F
#define LCD_ORANGE         0xFD20
#define LCD_PINK           0xfdba
#define LCD_OLIVE          0xdfe4
#define LCD_BROWN          0x9304
#define LCD_BG             LCD_WHITE

/* ILI 9325 registers definition */
#define READ_ID_CODE                        0x00
#define DRIVER_OUTPUT_CONTROL               0x01
#define DRIVING_WAVE_CONTROL                0x02
#define ENTRY_MODE                          0x03
#define RESIZING_CONTROL                    0x04
#define DISPLAY_CONTROL_1                   0x07
#define DISPLAY_CONTROL_2                   0x08
#define DISPLAY_CONTROL_3                   0x09
#define DISPLAY_CONTROL_4                   0x0A
#define RGB_DISPLAY_INTERFACE_CONTROL_1     0x0C
#define FRAME_MARKER_POSITION               0x0D
#define RGB_DISPLAY_INTERFACE_CONTROL_2     0x0F
#define POWER_CONTROL_1                     0x10
#define POWER_CONTROL_2                     0x11
#define POWER_CONTROL_3                     0x12
#define POWER_CONTROL_4                     0x13
#define GRAM_HORIZONTAL_ADDRESS_SET         0x20
#define GRAM_VERTICAL_ADDRESS_SET           0x21
#define DATA_IN_GRAM                        0x22
#define POWER_CONTROL_7                     0x29
#define FRAME_RATE_AND_COLOR_CONTROL        0x2B

#define GAMMA_CONTROL_1                     0x30
#define GAMMA_CONTROL_2                     0x31
#define GAMMA_CONTROL_3                     0x32
#define GAMMA_CONTROL_4                     0x35
#define GAMMA_CONTROL_5                     0x36
#define GAMMA_CONTROL_6                     0x37
#define GAMMA_CONTROL_7                     0x38
#define GAMMA_CONTROL_8                     0x39
#define GAMMA_CONTROL_9                     0x3C
#define GAMMA_CONTROL_10                    0x3D

#define HOR_ADDR_START_POS                  0x50
#define HOR_ADDR_END_POS                    0x51
#define VERT_ADDR_START_POS                 0x52
#define VERT_ADDR_END_POS                   0x53
#define GATE_SCAN_CONTROL_0X60              0x60
#define GATE_SCAN_CONTROL_0X61              0x61
#define GATE_SCAN_CONTROL_0X6A              0x6A
#define PART_IMAGE_1_DISPLAY_POS            0x80
#define PART_IMG_1_START_END_ADDR_0x81      0x81
#define PART_IMG_1_START_END_ADDR_0x82      0x81
#define PART_IMAGE_2_DISPLAY_POS            0x83
#define PART_IMG_2_START_END_ADDR_0x84      0x84
#define PART_IMG_2_START_END_ADDR_0x85      0x85
#define PANEL_ITERFACE_CONTROL_1            0x90
#define PANEL_ITERFACE_CONTROL_2            0x92
#define PANEL_ITERFACE_CONTROL_4            0x95

#define GRAM                                0x22
#define HORIZONTAL_GRAM_SET                 0x20
#define VERTICAL_GRAM_SET                   0x21

/******************************************************************************
* Function Name  : RGB565CONVERT
* Description    : 24bit to 16bit color
* Input          : - red: R
*                  - green: G 
*          - blue: B
* Output         : None
* Return         : RGB color
* Attention  : None
*******************************************************************************/
#define RGB565CONVERT(red, green, blue)\
(uint16_t)( (( red   >> 3 ) << 11 ) | \
(( green >> 2 ) << 5  ) | \
( blue  >> 3 ))

/* Private function prototypes -----------------------------------------------*/
void LCD_Initializtion(void);
void LCD_Clear(uint16_t Color);
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos);
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point);
void LCD_DrawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color );
void PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor); //, uint16_t bkColor );
void LCD_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color); //, uint16_t bkColor);
void LCD_DrawRectangle(int16_t xStart, int16_t xEnd, int16_t yStart, int16_t yEnd, uint16_t Color);
void LCD_fillCircle(int poX, int poY, int r,uint16_t color);
uint16_t LCD_Read (void);
uint16_t LCD_ReadData(void);
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos);
void delay_ms(uint16_t ms);
#endif 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
