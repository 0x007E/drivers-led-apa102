/**
 * @file apa102.c
 * @brief Implementation of LED control functions using SPI interface.
 *
 * This source file provides functions to initialize and control addressable APA102 LEDs, including setting colors and turning LEDs on or off. It utilizes the SPI communication peripheral to send LED data frames.
 *
 * @author g.raf
 * @date 2026-02-20
 * @version 1.0 Release
 * @copyright
 * Copyright (c) 2026 g.raf
 * Released under the GPLv3 License. (see LICENSE in repository)
 *
 * @note This file is part of a larger embedded systems project and subject to the license specified in the repository. For updates  and the complete revision history, see the GitHub repository.
 *
 * @see https://github.com/0x007e/drivers-led-apa102 "APA102 LED Driver GitHub Repository"
 */

#include "apa102.h"

static void apa102_frame(unsigned char flag, const GFX_RGBA_Color *color)
{
    unsigned char temp = (flag | (color->alpha & APA102_MAX_INTENSITY));

    spi_transfer(temp);
    spi_transfer(color->blue);
    spi_transfer(color->green);
    spi_transfer(color->red);
}

/**
 * @brief Initialize the LED control interface and hardware.
 *
 * @details
 * This function initializes the SPI peripheral to communicate with the LEDs using most significant bit (`MSB`) first, with clock polarity and phase set to rising edges. After SPI initialization, it sends a start-of-frame (`SOF`) signal followed by initializing all configured LEDs with the enable flag and zero color data (LEDs initially off). Finally, it sends an end-of-frame (`EOF`) signal to mark completion of the initialization sequence. This setup prepares the LEDs for subsequent color and blink control operations by configuring the communication and initial states.
 *
 * @note This function must be called before any other LED control operations. It assumes `SPI_MSB`, `SPI_Rising` macros and LED frame configurations are correctly defined.
 *
 * @see APA102_SOF() and APA102_EOF() macros for frame delimiters.
 * @see apa102_frame() for sending individual LED data frames.
 */
void apa102_init(void)
{
    APA102_SOF();
    for (unsigned char i=0; i < APA102_NUMBER_OF_LEDS; i++)
    {
        apa102_frame(APA102_ENABLE_FLAG, &(GFX_RGBA_Color){APA102_MIN_INTENSITY, 0x00, 0x00, 0x00});
    }
    APA102_EOF();
}

/**
 * @brief Transmit a specified value repeatedly over SPI to form a data frame.
 *
 * @param value The byte value to be sent repeatedly.
 *
 * @details
 * This function sends the given `value` repeatedly for `LED_FRAME_SIZE` times via `SPI` using the `spi_transfer` function.
 * It is commonly used to send `start` or `stop` frames for LED data sequences to synchronize communication with the LED hardware.
 *
 * @note The function blocks until all bytes are transmitted. Ensure the SPI interface is initialized before calling this function.
 */
void apa102_xof(APA102_Transmission type)
{
    for (unsigned char i=0; i < APA102_FRAME_SIZE; i++)
    {
        spi_transfer(type);
    }
}

/**
 * @brief Send an LED data frame with specified color and intensity.
 *
 * @param color LED_Data structure containing intensity and RGB color values.
 *
 * @details
 * This function constructs and transmits a single LED data frame over SPI, combining the LED enable flag with the masked intensity value, followed by the blue, green, and red color components. The intensity value is masked with 0x3F to limit it to valid bits.
 *
 * The frame format is:
 * - Mode byte: `LED_ENABLE_FLAG` OR'ed with intensity (`0x3F` mask).
 * - Blue color byte.
 * - Green color byte.
 * - Red color byte.
 *
 * @note Ensure the LED is initialized before calling this function.
 */
void apa102_led(const GFX_RGBA_Color *color)
{
    apa102_frame(APA102_ENABLE_FLAG | (0x3F & color->alpha), color);
}

/**
 * @brief Send an LED data frame to all LEDs with specified color and intensity.
 *
 * @param color LED_Data structure containing intensity and RGB color values.
 *
 * @details
 * This function constructs and transmits a single LED data frame over SPI to multiple LEDs, combining the LED enable flag with the masked intensity value, followed by the blue, green, and red color components. The intensity value is masked with 0x3F to limit it to valid bits.
 *
 * The frame format is:
 * - Mode byte: `LED_ENABLE_FLAG` OR'ed with intensity (`0x3F` mask).
 * - Blue color byte.
 * - Green color byte.
 * - Red color byte.
 *
 * @note Ensure the LED is initialized before calling this function.
 */
void apa102_leds(const GFX_RGBA_Color *color)
{
    APA102_SOF();

    for (unsigned char i=0; i < APA102_NUMBER_OF_LEDS; i++)
    {
        apa102_frame(APA102_ENABLE_FLAG | (0x3F & color->alpha), color);
    }

    APA102_EOF();
}

/**
 * @brief Send an LED data frame to a single LED to turn it off.
 *
 * @details
 * This function constructs and transmits a single LED data frame covered with zeroes over SPI to a single LED, combining the LED enable flag with the minimum intensity value to switch the LED off.
 *
 * The frame format is:
 * - Mode byte: `LED_ENABLE_FLAG` or `APA102_SLEEP_FLAG` if supported
 * - Intensity byte: `APA102_MIN_INTENSITY` (minimum brightness)
 * - Blue color byte set to zero.
 * - Green color byte set to zero.
 * - Red color byte set to zero.
 *
 * @note Ensure the LED is initialized before calling this function.
 */
void apa102_led_off(void)
{
    #ifdef APA102_POWER_SAVING_AVAILABLE
        apa102_frame(APA102_SLEEP_FLAG, &(GFX_RGBA_Color){APA102_MIN_INTENSITY, 0x00, 0x00, 0x00});
    #else
        apa102_frame(APA102_START_FLAG, &(GFX_RGBA_Color){APA102_MIN_INTENSITY, 0x00, 0x00, 0x00});
    #endif
}

/**
 * @brief Disable all LEDs by sending sleep commands (if supported) and turning off the SPI peripheral.
 *
 * @details
 * This function sends sleep commands (if supported) to all configured LEDs to put them into a low power state and disables all leds.
 *
 * This operation is used to safely turn off the LEDs and reduce power consumption when LED functionality is not needed.
 *
 * @note Ensure no ongoing LED data transmission occurs before calling this function to avoid communication issues with the LED hardware. After calling this function, the SPI peripheral may be disabled to save power.
 */
void apa102_leds_off(void)
{
    APA102_SOF();

    for (unsigned char i=0; i < APA102_NUMBER_OF_LEDS; i++)
    {
        apa102_led_off();
    }

    APA102_EOF();
}