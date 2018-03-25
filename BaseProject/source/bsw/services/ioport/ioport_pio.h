/**
 * \file
 *
 * \brief SAM architecture specific IOPORT service implementation header file.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef IOPORT_SAM_H
#define IOPORT_SAM_H

#include <sysclk.h>


#define IOPORT_BASE_ADDRESS (uintptr_t)PIOA
#define IOPORT_PIO_OFFSET   ((uintptr_t)PIOB - (uintptr_t)PIOA)

#define IOPORT_MODE_PULLUP              (  1 << 3) /*!< Pull-up */

/**
 * \weakgroup ioport_group
 * \section ioport_modes IOPORT Modes
 *
 *
 * @{
 */

typedef uint32_t ioport_mode_t;
typedef uint32_t ioport_pin_t;
typedef uint32_t ioport_port_t;
typedef uint32_t ioport_port_mask_t;

__always_inline static ioport_port_t arch_ioport_pin_to_port_id(ioport_pin_t pin)
{
    return pin >> 5;
}

__always_inline static Pio *arch_ioport_port_to_base(ioport_port_t port)
{
	return (Pio *)((uintptr_t)IOPORT_BASE_ADDRESS + (IOPORT_PIO_OFFSET * port));
}

__always_inline static Pio *arch_ioport_pin_to_base(ioport_pin_t pin)
{
	return arch_ioport_port_to_base(arch_ioport_pin_to_port_id(pin));
}

__always_inline static ioport_port_mask_t arch_ioport_pin_to_mask(ioport_pin_t pin)
{
    return 1U << (pin & 0x1F);
}

__always_inline static bool arch_ioport_get_pin_level(ioport_pin_t pin)
{
	return arch_ioport_pin_to_base(pin)->PIO_PDSR & arch_ioport_pin_to_mask(pin);
}



#endif /* IOPORT_SAM_H */
