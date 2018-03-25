/**
 * \file
 *
 * \brief Common IOPORT service main header file for AVR, UC3 and ARM
 *        architectures.
 *
 * Copyright (c) 2012-2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef IOPORT_H
#define IOPORT_H

#ifdef __cplusplus
extern "C" {
#endif

//#include <parts.h>
#include <compiler.h>

/**
 * \defgroup ioport_group Common IOPORT API
 *
 * See \ref ioport_quickstart.
 *
 * This is common IOPORT service for GPIO pin configuration and control in a
 * standardized manner across the MEGA, MEGA_RF, XMEGA, UC3 and ARM devices.
 *
 * Port pin control code is optimized for each platform, and should produce
 * both compact and fast execution times when used with constant values.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref sysclk_group for clock speed and functions.
 * @{
 */

/**
 * \def IOPORT_CREATE_PIN(port, pin)
 * \brief Create IOPORT pin number
 *
 * Create a IOPORT pin number for use with the IOPORT functions.
 *
 * \param port IOPORT port (e.g. PORTA, PA or PIOA depending on chosen
 *             architecture)
 * \param pin IOPORT zero-based index of the I/O pin
 */

#include "ioport_pio.h"



/**
 * \brief Get current value of an IOPORT pin, which has been configured as an
 * input.
 *
 * \param pin IOPORT pin to read
 * \return Current logical value of the specified pin
 */
static inline bool ioport_get_pin_level(ioport_pin_t pin)
{
	return arch_ioport_get_pin_level(pin);
}




/** @} */

/**
 * \page ioport_quickstart Quick start guide for the common IOPORT service
 *
 * This is the quick start guide for the \ref ioport_group, with
 * step-by-step instructions on how to configure and use the service in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section ioport_quickstart_basic Basic use case
 * In this use case we will configure one IO pin for button input and one for
 * LED control. Then it will read the button state and output it on the LED.
 *
 * \section ioport_quickstart_basic_setup Setup steps
 *
 * \subsection ioport_quickstart_basic_setup_code Example code
 * \code
	 #define MY_LED    IOPORT_CREATE_PIN(PORTA, 5)
	 #define MY_BUTTON IOPORT_CREATE_PIN(PORTA, 6)

	 ioport_init();

	 ioport_set_pin_dir(MY_LED, IOPORT_DIR_OUTPUT);
	 ioport_set_pin_dir(MY_BUTTON, IOPORT_DIR_INPUT);
	 ioport_set_pin_mode(MY_BUTTON, IOPORT_MODE_PULLUP);
\endcode
 *
 * \subsection ioport_quickstart_basic_setup_flow Workflow
 * -# It's useful to give the GPIOs symbolic names and this can be done with
 *    the \ref IOPORT_CREATE_PIN macro. We define one for a LED and one for a
 *    button.
 *   - \code
	#define MY_LED    IOPORT_CREATE_PIN(PORTA, 5)
	#define MY_BUTTON IOPORT_CREATE_PIN(PORTA, 6)
\endcode
 *   - \note The usefulness of the \ref IOPORT_CREATE_PIN macro and port names
 *           differ between architectures:
 *     - MEGA, MEGA_RF and XMEGA: Use \ref IOPORT_CREATE_PIN macro with port definitions
 *              PORTA, PORTB ...
 *     - UC3: Most convenient to pick up the device header file pin definition
 *            and us it directly. E.g.: AVR32_PIN_PB06
 *     - SAM: Most convenient to pick up the device header file pin definition
 *            and us it directly. E.g.: PIO_PA5_IDX<br>
 *            \ref IOPORT_CREATE_PIN can also be used with port definitions
 *            PIOA, PIOB ...
 * -# Initialize the ioport service. This typically enables the IO module if
 *    needed.
 *   - \code ioport_init(); \endcode
 * -# Set the LED GPIO as output:
 *   - \code ioport_set_pin_dir(MY_LED, IOPORT_DIR_OUTPUT); \endcode
 * -# Set the button GPIO as input:
 *   - \code ioport_set_pin_dir(MY_BUTTON, IOPORT_DIR_INPUT); \endcode
 * -# Enable pull-up for the button GPIO:
 *   - \code ioport_set_pin_mode(MY_BUTTON, IOPORT_MODE_PULLUP); \endcode
 *
 * \section ioport_quickstart_basic_usage Usage steps
 *
 * \subsection ioport_quickstart_basic_usage_code Example code
 * \code
	 bool value;

	 value = ioport_get_pin_level(MY_BUTTON);
	 ioport_set_pin_level(MY_LED, value);
\endcode
 *
 * \subsection ioport_quickstart_basic_usage_flow Workflow
 * -# Define a boolean variable for state storage:
 *   - \code bool value; \endcode
 * -# Read out the button level into variable value:
 *   - \code value = ioport_get_pin_level(MY_BUTTON); \endcode
 * -# Set the LED to read out value from the button:
 *   - \code ioport_set_pin_level(MY_LED, value); \endcode
 *
 * \section ioport_quickstart_advanced Advanced use cases
 * - \subpage ioport_quickstart_use_case_1 : Port access
 */

/**
 * \page ioport_quickstart_use_case_1 Advanced use case doing port access
 *
 * In this case we will read out the pins from one whole port and write the
 * read value to another port.
 *
 * \section ioport_quickstart_use_case_1_setup Setup steps
 *
 * \subsection ioport_quickstart_use_case_1_setup_code Example code
 * \code
	 #define IN_PORT  IOPORT_PORTA
	 #define OUT_PORT IOPORT_PORTB
	 #define MASK     0x00000060

	 ioport_init();

	 ioport_set_port_dir(IN_PORT, MASK, IOPORT_DIR_INPUT);
	 ioport_set_port_dir(OUT_PORT, MASK, IOPORT_DIR_OUTPUT);
\endcode
 *
 * \subsection ioport_quickstart_basic_setup_flow Workflow
 * -# It's useful to give the ports symbolic names:
 *   - \code
	#define IN_PORT  IOPORT_PORTA
	#define OUT_PORT IOPORT_PORTB
\endcode
 *   - \note The port names differ between architectures:
 *     - MEGA_RF, MEGA and XMEGA: There are predefined names for ports: IOPORT_PORTA,
 *              IOPORT_PORTB ...
 *     - UC3: Use the index value of the different IO blocks: 0, 1 ...
 *     - SAM: There are predefined names for ports: IOPORT_PIOA, IOPORT_PIOB
 *            ...
 * -# Also useful to define a mask for the bits to work with:
 *     - \code #define MASK     0x00000060 \endcode
 * -# Initialize the ioport service. This typically enables the IO module if
 *    needed.
 *   - \code ioport_init(); \endcode
 * -# Set one of the ports as input:
 *   - \code ioport_set_pin_dir(IN_PORT, MASK, IOPORT_DIR_INPUT); \endcode
 * -# Set the other port as output:
 *   - \code ioport_set_pin_dir(OUT_PORT, MASK, IOPORT_DIR_OUTPUT); \endcode
 *
 * \section ioport_quickstart_basic_usage Usage steps
 *
 * \subsection ioport_quickstart_basic_usage_code Example code
 * \code
	 ioport_port_mask_t value;

	 value = ioport_get_port_level(IN_PORT, MASK);
	 ioport_set_port_level(OUT_PORT, MASK, value);
\endcode
 *
 * \subsection ioport_quickstart_basic_usage_flow Workflow
 * -# Define a variable for port date storage:
 *   - \code ioport_port_mask_t value; \endcode
 * -# Read out from one port:
 *   - \code value = ioport_get_port_level(IN_PORT, MASK); \endcode
 * -# Put the read data out on the other port:
 *   - \code ioport_set_port_level(OUT_PORT, MASK, value); \endcode
 */

#ifdef __cplusplus
}
#endif

#endif /* IOPORT_H */
