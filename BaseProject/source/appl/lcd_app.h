/*
 * lcd_app.h
 *
 *  Created on: 25/01/2018
 *      Author: mas1ga
 */

#ifndef LCD_APP_H_
#define LCD_APP_H_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

//#include "eve.h"

/*------------------------------------------------------------------------------
 *         Defines
 *----------------------------------------------------------------------------*/

#define FT81x_ACTIVE    0x00            // Initializes FT800


/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

// Application Layer
void APP_Init(void);

void APP_FlashingDot(void);
void APP_VertexTranslate(void);
void APP_Text(void);
void APP_ConvertedBitmap(void);
void APP_ConvertedBitmap_FirstTime(void);
void APP_Calibrate(void);
void APP_SliderandButton(void);


#endif /* LCD_APP_H_ */
