/*
 * lcd.h
 *
 *  This file contains the functions which talk to the MCU
 *
 */

#ifndef LCD_H_
#define LCD_H_

void LDC_Init(void);
void LDC_ChangeClock(void);
uint8_t LCD_SPIReadWrite(uint8_t DataToWrite);

void LCD_CSlow(void);
void LCD_CShigh(void);

void LCD_PDlow(void);
void LCD_PDhigh(void);

void MCU_Delay_20ms(void);
void MCU_Delay_500ms(void);

#endif /* LCD_H_ */
