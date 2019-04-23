/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			GLCD.h
** Descriptions:		TFT (IO)
**
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2011-1-26
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GLCD.h" 
#include "AsciiLib.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Code;

/* Private define ------------------------------------------------------------*/
#define  ILI9325    1  /* 0x9325 */

/*******************************************************************************
* Function Name  : Lcd_Configuration
* Description    : Configures LCD Control lines
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_Configuration(void)
{
	/* Configure the LCD Control pins */
    P10->DIR = 0x12;
    P6->DIR = 0x0C;
    P7->DIR = 0x0F;
    P8->DIR = 0xFC;
    P9->DIR = 0x6F;
    BITBAND_PERI(P10->OUT,  4) = 0;   /* CS low */
    P10DIR |= BIT0;
    P10OUT |= BIT0;  // high
    delay_ms(100);
    P10OUT &= ~BIT0; // low
    delay_ms(100);
    P10OUT |= BIT0;  // high

}

/*******************************************************************************
* Function Name  : LCD_Send
* Description    : LCD write data
* Input          : - byte: byte to be sent
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_Send (uint16_t byte) 
{
	uint32_t temp;
	temp = byte;
	BITBAND_PERI(P8->OUT,  2) = (temp & 0x00000001) >> 0 ;        /* Write D0..D7 */
	BITBAND_PERI(P8->OUT,  3) = (temp & 0x00000002) >> 1 ;        /* Write D0..D7 */
	BITBAND_PERI(P8->OUT,  4) = (temp & 0x00000004) >> 2 ;        /* Write D0..D7 */
	BITBAND_PERI(P8->OUT,  5) = (temp & 0x00000008) >> 3 ;        /* Write D0..D7 */
	BITBAND_PERI(P8->OUT,  6) = (temp & 0x00000010) >> 4 ;        /* Write D0..D7 */
	BITBAND_PERI(P8->OUT,  7) = (temp & 0x00000020) >> 5 ;        /* Write D0..D7 */
	BITBAND_PERI(P9->OUT,  0) = (temp & 0x00000040) >> 6 ;        /* Write D0..D7 */
	BITBAND_PERI(P9->OUT,  1) = (temp & 0x00000080) >> 7 ;        /* Write D0..D7 */

	BITBAND_PERI(P9->OUT,  2) = (temp & 0x00000100) >> 8 ; /* Write D8..D15 */
	BITBAND_PERI(P9->OUT,  3) = (temp & 0x00000200) >> 9 ; /* Write D8..D15 */
	BITBAND_PERI(P6->OUT,  2) = (temp & 0x00000400) >> 10 ; /* Write D8..D15 */
	BITBAND_PERI(P6->OUT,  3) = (temp & 0x00000800) >> 11 ; /* Write D8..D15 */
	BITBAND_PERI(P7->OUT,  0) = (temp & 0x00001000) >> 12 ; /* Write D8..D15 */
	BITBAND_PERI(P7->OUT,  3) = (temp & 0x00002000) >> 13 ; /* Write D8..D15 */
	BITBAND_PERI(P9->OUT,  5) = (temp & 0x00004000) >> 14 ; /* Write D8..D15 */
	BITBAND_PERI(P9->OUT,  6) = (temp & 0x00008000) >> 15 ; /* Write D8..D15 */
}

/*******************************************************************************
* Function Name  : LCD_WriteIndex
* Description    : LCD write register address
* Input          : - index: register address
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_WriteIndex(uint16_t index)
{

    BITBAND_PERI(P7->OUT,  2) = 0;   /* RS low */
    BITBAND_PERI(P7->OUT,  1) = 1;   /* RD high */

        /* write data */
	LCD_Send( index );

	BITBAND_PERI(P10->OUT,  1) = 0 ;   /* Wr low */
	BITBAND_PERI(P10->OUT,  1) = 1 ;   /* Wr high */
}

/*******************************************************************************
* Function Name  : LCD_WriteData
* Description    : LCD write register data
* Input          : - index: register data
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_WriteData(uint16_t data)
{				
    //BITBAND_PERI(P10->OUT,  4) = 0;   /* CS low */

    BITBAND_PERI(P7->OUT,  2) = 1;   /* RS high */

        /* write data */
	LCD_Send( data );

	BITBAND_PERI(P10->OUT,  1) = 0;   /* Wr low */
    //for(int i = 0; i < 10; i++){

    //}
	BITBAND_PERI(P10->OUT,  1) = 1;   /* Wr high */

	//BITBAND_PERI(P10->OUT,  4) = 1;   /* CS high */
}


/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{ 
	/* Write 16-bit Index, then Write Reg */  
	LCD_WriteIndex(LCD_Reg);         
	/* Write 16-bit Reg */
	LCD_WriteData(LCD_RegValue);  
}


/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
    #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
	
 	uint16_t temp = Xpos;

			 Xpos = Ypos;
			 Ypos = ( MAX_X - 1 ) - temp;  

	#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )

	#endif

  switch( LCD_Code )
  {
     default:		 /* 0x9320 0x9325 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 */
          LCD_WriteReg(0x0020, Ypos );  // inverted to maximize speed
          LCD_WriteReg(0x0021, Xpos );  // inverted to maximize speed
	      break;
  }
}

/*******************************************************************************
* Function Name  : delay_ms
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
} 

/*******************************************************************************
* Function Name  : LCD_Initializtion
* Description    : Initialize TFT Controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Initializtion(void)
{
	uint16_t DeviceCode;
	
	LCD_Configuration();
	delay_ms(100);
	DeviceCode = 0x9325;	/* read the LCD ID, if Controller is ILI9320, The ID value is 0x9320 */

		LCD_Code = ILI9325;
        LCD_WriteReg(0x00ff,0x0010);
        LCD_WriteReg(0x00e7,0x0010);
        LCD_WriteReg(0x0000,0x0001);  	/* start internal osc */
        LCD_WriteReg(0x0001,0x0100);
        LCD_WriteReg(0x0002,0x0700); 	/* power on sequence */
        LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3) ); 	/* importance */
        LCD_WriteReg(0x0004,0x0000);
        LCD_WriteReg(0x0008,0x0207);
        LCD_WriteReg(0x0009,0x0000);
        LCD_WriteReg(0x000a,0x0000); 	/* display setting */
        LCD_WriteReg(0x000c,0x0001);	/* display setting */
        LCD_WriteReg(0x000d,0x0000);
        LCD_WriteReg(0x000f,0x0000);
        /* Power On sequence */
        LCD_WriteReg(0x0010,0x0000);
        LCD_WriteReg(0x0011,0x0007);
        LCD_WriteReg(0x0012,0x0000);
        LCD_WriteReg(0x0013,0x0000);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0010,0x1590);
        LCD_WriteReg(0x0011,0x0227);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0012,0x009c);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0013,0x1900);
        LCD_WriteReg(0x0029,0x0023);
        LCD_WriteReg(0x002b,0x000e);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0020,0x0000);
        LCD_WriteReg(0x0021,0x0000);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0030,0x0007);
        LCD_WriteReg(0x0031,0x0707);
        LCD_WriteReg(0x0032,0x0006);
        LCD_WriteReg(0x0035,0x0704);
        LCD_WriteReg(0x0036,0x1f04);
        LCD_WriteReg(0x0037,0x0004);
        LCD_WriteReg(0x0038,0x0000);
        LCD_WriteReg(0x0039,0x0706);
        LCD_WriteReg(0x003c,0x0701);
        LCD_WriteReg(0x003d,0x000f);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0050,0x0000);
        LCD_WriteReg(0x0051,0x00ef);
        LCD_WriteReg(0x0052,0x0000);
        LCD_WriteReg(0x0053,0x013f);
        LCD_WriteReg(0x0060,0xa700);
        LCD_WriteReg(0x0061,0x0001);
        LCD_WriteReg(0x006a,0x0000);
        LCD_WriteReg(0x0080,0x0000);
        LCD_WriteReg(0x0081,0x0000);
        LCD_WriteReg(0x0082,0x0000);
        LCD_WriteReg(0x0083,0x0000);
        LCD_WriteReg(0x0084,0x0000);
        LCD_WriteReg(0x0085,0x0000);

        LCD_WriteReg(0x0090,0x0010);
        LCD_WriteReg(0x0092,0x0000);
        LCD_WriteReg(0x0093,0x0003);
        LCD_WriteReg(0x0095,0x0110);
        LCD_WriteReg(0x0097,0x0000);
        LCD_WriteReg(0x0098,0x0000);
        /* display on sequence */
        LCD_WriteReg(0x0007,0x0133);

        LCD_WriteReg(0x0020,0x0000);  /* Line first address 0 */
        LCD_WriteReg(0x0021,0x0000);  /* Column first site 0 */

         // LCD_WriteReg(0xE5, 0x78F0); /* set SRAM internal timing */
         // LCD_WriteReg(DRIVER_OUTPUT_CONTROL, 0x0100); /* set Driver Output Control */ // S720 to S1
         // LCD_WriteReg(DRIVING_WAVE_CONTROL, 0x0700); /* set 1 line inversion */
         // LCD_WriteReg(ENTRY_MODE, (1<<12)|(0<<5)|(1<<4)|(0<<3)); /* set GRAM write direction and BGR=1 */ // left to right, top to bottom, addresses in vertical, BGR writing to GRAM
         // LCD_WriteReg(RESIZING_CONTROL, 0x0000); /* Resize register */
         // LCD_WriteReg(DISPLAY_CONTROL_2, 0x0207); /* set the back porch and front porch */ // RGB requires >= 2 lines
         // LCD_WriteReg(DISPLAY_CONTROL_3, 0x0000); /* set non-display area refresh cycle ISC[3:0] */
         // LCD_WriteReg(DISPLAY_CONTROL_4, 0x0000); /* FMARK function */
         // LCD_WriteReg(RGB_DISPLAY_INTERFACE_CONTROL_1, 0x0000); /* RGB interface setting */ // 18 bit 1 transfer per pixel fills SPI buffer 3 times
         // LCD_WriteReg(FRAME_MARKER_POSITION, 0x0000); /* Frame marker Position */ // FMARK outputs at the start of the back porch
         // LCD_WriteReg(RGB_DISPLAY_INTERFACE_CONTROL_2, 0x0000); /* RGB interface polarity */ //input clocked in on rising edge of DOTCLK
         //
         // /* Power On sequence */
         // LCD_WriteReg(POWER_CONTROL_1, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
         // LCD_WriteReg(POWER_CONTROL_2, 0x0007); /* DC1[2:0], DC0[2:0], VC[2:0] */
         // LCD_WriteReg(POWER_CONTROL_3, 0x0000); /* VREG1OUT voltage */
         // LCD_WriteReg(POWER_CONTROL_4, 0x0000); /* VDV[4:0] for VCOM amplitude */
         // LCD_WriteReg(DISPLAY_CONTROL_1, 0x0001);
         // Delay(200);
         //
         // /* Dis-charge capacitor power voltage */
         // LCD_WriteReg(POWER_CONTROL_1, 0x1090); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
         // LCD_WriteReg(POWER_CONTROL_2, 0x0227); /* Set DC1[2:0], DC0[2:0], VC[2:0] */
         // Delay(50); /* Delay 50ms */
         // LCD_WriteReg(POWER_CONTROL_3, 0x001F);
         // Delay(50); /* Delay 50ms */
         // LCD_WriteReg(POWER_CONTROL_4, 0x1500); /* VDV[4:0] for VCOM amplitude */
         // LCD_WriteReg(POWER_CONTROL_7, 0x0027); /* 04 VCM[5:0] for VCOMH */
         // LCD_WriteReg(FRAME_RATE_AND_COLOR_CONTROL, 0x000D); /* Set Frame Rate */ // 90 FPS (0x1 for 30 FPS)
         // Delay(50); /* Delay 50ms */
         // LCD_WriteReg(GRAM_HORIZONTAL_ADDRESS_SET, 0x0000); /* GRAM horizontal Address */
         // LCD_WriteReg(GRAM_VERTICAL_ADDRESS_SET, 0x0000); /* GRAM Vertical Address */
         //
         // /* Adjust the Gamma Curve */
         // LCD_WriteReg(GAMMA_CONTROL_1,    0x0000);
         // LCD_WriteReg(GAMMA_CONTROL_2,    0x0707);
         // LCD_WriteReg(GAMMA_CONTROL_3,    0x0307);
         // LCD_WriteReg(GAMMA_CONTROL_4,    0x0200);
         // LCD_WriteReg(GAMMA_CONTROL_5,    0x0008);
         // LCD_WriteReg(GAMMA_CONTROL_6,    0x0004);
         // LCD_WriteReg(GAMMA_CONTROL_7,    0x0000);
         // LCD_WriteReg(GAMMA_CONTROL_8,    0x0707);
         // LCD_WriteReg(GAMMA_CONTROL_9,    0x0002);
         // LCD_WriteReg(GAMMA_CONTROL_10,   0x1D04);

         /* Set GRAM area */
         LCD_WriteReg(HOR_ADDR_START_POS, 0x0000);     /* Horizontal GRAM Start Address */
         LCD_WriteReg(HOR_ADDR_END_POS, (MAX_SCREEN_Y - 1));  /* Horizontal GRAM End Address */
         LCD_WriteReg(VERT_ADDR_START_POS, 0x0000);    /* Vertical GRAM Start Address */
         LCD_WriteReg(VERT_ADDR_END_POS, (MAX_SCREEN_X - 1)); /* Vertical GRAM Start Address */
         LCD_WriteReg(GATE_SCAN_CONTROL_0X60, 0x2700); /* Gate Scan Line */
         LCD_WriteReg(GATE_SCAN_CONTROL_0X61, 0x0001); /* NDL,VLE, REV */
         LCD_WriteReg(GATE_SCAN_CONTROL_0X6A, 0x0000); /* set scrolling line */

         /* Partial Display Control */
         LCD_WriteReg(PART_IMAGE_1_DISPLAY_POS, 0x0000);
         LCD_WriteReg(PART_IMG_1_START_END_ADDR_0x81, 0x0000);
         LCD_WriteReg(PART_IMG_1_START_END_ADDR_0x82, 0x0000);
         LCD_WriteReg(PART_IMAGE_2_DISPLAY_POS, 0x0000);
         LCD_WriteReg(PART_IMG_2_START_END_ADDR_0x84, 0x0000);
         LCD_WriteReg(PART_IMG_2_START_END_ADDR_0x85, 0x0000);

         /* Panel Control */
         LCD_WriteReg(PANEL_ITERFACE_CONTROL_1, 0x0010);
         LCD_WriteReg(PANEL_ITERFACE_CONTROL_2, 0x0600);
         LCD_WriteReg(DISPLAY_CONTROL_1, 0x0133); /* 262K color and display ON */
         Delay(50); /* delay 50 ms */

	    LCD_Clear(LCD_BLACK);

    delay_ms(50);   /* delay 50 ms */	
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : clear LCD screen
* Input          : - Color: Screen Color
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint32_t index;
	
	LCD_SetCursor(0,0);

    LCD_WriteReg(HOR_ADDR_START_POS, 0x0000);     /* Horizontal GRAM Start Address */
    LCD_WriteReg(HOR_ADDR_END_POS, (MAX_SCREEN_Y - 1));  /* Horizontal GRAM End Address */
    LCD_WriteReg(VERT_ADDR_START_POS, 0x0000);    /* Vertical GRAM Start Address */
    LCD_WriteReg(VERT_ADDR_END_POS, (MAX_SCREEN_X - 1)); /* Vertical GRAM Start Address */

	LCD_WriteIndex(0x0022);
	LCD_Send( Color );
	BITBAND_PERI(P7->OUT,  2) = 1;   /* RS high */
	for( index = 0; index < MAX_X * MAX_Y; index++ )
	{
        BITBAND_PERI(P10->OUT,  1) = 0;   /* Wr low */
        //for(int i = 0; i < 10; i++){

        //}
        BITBAND_PERI(P10->OUT,  1) = 1;   /* Wr high */
	}
}

/******************************************************************************
* Function Name  : LCD_BGR2RGB
* Description    : RRRRRGGGGGGBBBBB change BBBBBGGGGGGRRRRR
* Input          : - color: BRG color value
* Output         : None
* Return         : RGB color value
* Attention	 : None
*******************************************************************************/
unsigned short LCD_BGR2RGB( unsigned short color )
{
	unsigned short  r, g, b, rgb;
	
	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;
	
	rgb =  (b<<11) + (g<<5) + (r<<0);
	
	return( rgb );
}


/******************************************************************************
* Function Name  : LCD_SetPoint
* Description    : Draw point at the specified coordinates
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
	if( Xpos >= MAX_X || Ypos >= MAX_Y )
	{
		return;
	}
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteReg(0x0022,point);
}

/******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : Bresenham's line algorithm
* Input          : - x1: A point line coordinates
*                  - y1: A point column coordinates 
*		   - x2: B point line coordinates
*		   - y2: B point column coordinates 
*		   - color: Line color
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/	 
void LCD_DrawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color )
{
    short dx,dy;      /* The definition of the X Y axis increase the value of the variable */
    short temp;       

    if( x0 > x1 )     
    {
	    temp = x1;
		x1 = x0;
		x0 = temp;   
    }
    if( y0 > y1 )     
    {
		temp = y1;
		y1 = y0;
		y0 = temp;   
    }
  
	dx = x1-x0;       
	dy = y1-y0;       

    if( dx == 0 )     
    {
        do
        { 
            LCD_SetPoint(x0, y0, color);   
            y0++;
        }
        while( y1 >= y0 ); 
		return; 
    }
    if( dy == 0 )     
    {
        do
        {
            LCD_SetPoint(x0, y0, color);
            x0++;
        }
        while( x1 >= x0 ); 
		return;
    }
	
    if( dx > dy )                        
    {
	    temp = 2 * dy - dx;                
        while( x0 != x1 )
        {
	        LCD_SetPoint(x0,y0,color);  
	        x0++;                        
	        if( temp > 0 )               
	        {
	            y0++;                    
	            temp += 2 * dy - 2 * dx; 
	 	    }
            else         
            {
			    temp += 2 * dy;          
			}       
        }
        LCD_SetPoint(x0,y0,color);
    }  
    else
    {
	    temp = 2 * dx - dy;                     
        while( y0 != y1 )
        {
	 	    LCD_SetPoint(x0,y0,color);     
            y0++;                 
            if( temp > 0 )           
            {
                x0++;               
                temp+=2*dy-2*dx; 
            }
            else
			{
                temp += 2 * dy;
			}
        } 
        LCD_SetPoint(x0,y0,color);
	}
} 

/******************************************************************************
* Function Name  : PutChar
* Description    : Lcd screen displays a character
* Input          : - Xpos: Horizontal coordinate
*                  - Ypos: Vertical coordinate 
*				   - ASCI: Displayed character
*				   - charColor: Character color  
*				   - bkColor: Background color 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor) // , uint16_t bkColor )
{
	uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);  /* get font data */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( (tmp_char >> 7 - j) & 0x01 == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );  /* Character color */
            }
            // time waster........
            // else
            // {
            //     LCD_SetPoint( Xpos + j, Ypos + i, bkColor );  /* Background color */
            // }
        }
    }
}

/******************************************************************************
* Function Name  : GUI_Text
* Description    : Displays the string
* Input          : - Xpos: Horizontal coordinate
*                  - Ypos: Vertical coordinate 
*				   - str: Displayed string
*				   - charColor: Character color   
*				   - bkColor: Background color  
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color)// , uint16_t bkColor)
{
    LCD_WriteReg(HOR_ADDR_START_POS, 0x0000);               /* Horizontal GRAM Start Address */
    LCD_WriteReg(HOR_ADDR_END_POS, (MAX_SCREEN_Y - 1));     /* Horizontal GRAM End Address  */
    LCD_WriteReg(VERT_ADDR_START_POS, 0x0000);              /* Vertical GRAM Start Address */
    LCD_WriteReg(VERT_ADDR_END_POS, (MAX_SCREEN_X - 1));    /* Vertical GRAM Start Address */

    uint8_t TempChar;
    do
    {
        TempChar = *str++;  
        PutChar( Xpos, Ypos, TempChar, Color); //, bkColor );
        if( Xpos < MAX_X - 8 )
        {
            Xpos += 8;
        } 
        else if ( Ypos < MAX_Y - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }   
        else
        {
            Xpos = 0;
            Ypos = 0;
        }    
    }
    while ( *str != 0 );
}

void LCD_DrawRectangle(int16_t xStart, int16_t xEnd, int16_t yStart, int16_t yEnd, uint16_t Color)
{
    // SET ALLOWABLE ADDRESS WINDOW
    LCD_WriteReg(HOR_ADDR_START_POS,    yStart  );   /* Horizontal GRAM Start Address    */
    LCD_WriteReg(HOR_ADDR_END_POS,      yEnd    );   /* Horizontal GRAM End Address      */
    LCD_WriteReg(VERT_ADDR_START_POS,   xStart  );   /* Vertical GRAM Start Address      */
    LCD_WriteReg(VERT_ADDR_END_POS,     xEnd    );   /* Vertical GRAM Start Address      */

    LCD_SetCursor(xStart, yStart);

    // fast write in blocks
    unsigned int len = (unsigned int)(xEnd - xStart + 1) * (unsigned int)(yEnd - yStart + 1);

    LCD_WriteIndex(0x0022);
    LCD_Send( Color );
    BITBAND_PERI(P7->OUT,  2) = 1;   /* RS high */
    for( int index = 0; index < len; index++ )
    {
        BITBAND_PERI(P10->OUT,  1) = 0;   /* Wr low */
        //for(int i = 0; i < 10; i++){

        //}
        BITBAND_PERI(P10->OUT,  1) = 1;   /* Wr high */
    }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
