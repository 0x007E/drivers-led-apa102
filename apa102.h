/**
 * @file apa102.h
 * @brief APA102 control interface for AVR microcontrollers using SPI.
 *
 * This header file defines the interface for controlling LEDs, including configuration macros and data structures for LED color and intensity. Functions are provided to initialize LED hardware, send data frames via SPI and control LED colors. The LED driver supports multiple LEDs with configurable frame sizes and intensity limits.
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

#ifndef APA102_H_
#define APA102_H_

    #ifndef APA102_HAL_PLATFORM
        /**
         * @def APA102_HAL_PLATFORM
         * @brief Sets the target platform for the APA102 hardware abstraction layer (HAL), e.g., avr or avr0
         * 
         * @details
         * Define this macro with the name of the target platform to select the corresponding platform-specific HAL implementation (such as TWI communication functions) for the APA102 LED driver. Common values are avr (classic AVR architecture) or avr0 (AVR0/1 series).
         * 
         * @note Set this macro as a global compiler symbol to ensure that the correct HAL implementation is used across the entire project.
        */
        #define APA102_HAL_PLATFORM avr0
    #endif

    #ifndef APA102_NUMBER_OF_LEDS
        /**
         * @def APA102_NUMBER_OF_LEDS
         * @brief Defines the total number of LEDs controlled by the driver.
         *
         * @details
         * This macro specifies how many individual LEDs are connected and managed. The default is 2 LEDs.
         *
         * @note Override as needed for the actual number of LEDs in the hardware configuration.
         */
        #define APA102_NUMBER_OF_LEDS 1
    #endif

    #ifndef APA102_FRAME_SIZE
        /**
         * @def APA102_FRAME_SIZE
         * @brief Defines the size of the LED start/stop and data frame in bytes.
         *
         * @details
         * This macro indicates how many bytes make up a start/stop and data frame sent via SPI to the LED. The default frame size is 4 bytes.
         *
         * @note Modify this value if the LED hardware protocol requires a different start/stop and data frame size.
         */
        #define APA102_FRAME_SIZE 4
    #endif

    #ifndef APA102_START_FLAG
        /**
         * @def APA102_START_FLAG
         * @brief Defines the start-of-frame flag value for LED data transmission.
         *
         * @details
         * This macro defines the highest valid intensity level for LED brightness. The default maximum is `0x1F`.
         */
        #define APA102_START_FLAG 0xE0
    #endif

    #ifndef APA102_SOF_VALUE
        /**
         * @def APA102_SOF_VALUE
         * @brief Defines the start-of-frame marker value for LED data transmission.
         *
         * @details
         * This value is sent to indicate the start of a LED data frame sequence. The default start value is `0x00`.
         */
        #define APA102_SOF_VALUE 0x00
    #endif

    #ifndef APA102_EOF_VALUE
        /**
         * @def APA102_EOF_VALUE
         * @brief Defines the end-of-frame marker value for LED data transmission.
         *
         * @details
         * This value is sent to mark the end of a LED data frame sequence. The default stop value is `0xFF`.
         */
        #define APA102_EOF_VALUE 0xFF
    #endif

    #ifndef APA102_MIN_INTENSITY
        /**
         * @def APA102_MIN_INTENSITY
         * @brief The minimum intensity value for LED brightness.
         *
         * @details
         * This macro defines the lowest valid intensity level for LED brightness. The default minimum is `0x01`.
         */
        #define APA102_MIN_INTENSITY 0x01
    #endif

    #ifndef APA102_MAX_INTENSITY
        /**
         * @def APA102_MAX_INTENSITY
         * @brief The maximum intensity value for LED brightness.
         *
         * @details
         * This macro defines the highest valid intensity level for LED brightness. The default maximum is `0x1F`.
         */
        #define APA102_MAX_INTENSITY 0x1F
    #endif

    #ifndef APA102_POWER_SAVING_AVAILABLE
        /**
         * @def APA102_POWER_SAVING_AVAILABLE
         * @brief Flag indicating whether power-saving mode is supported by the LED hardware.
         *
         * @details
         * This macro should be defined as `1` if the LED hardware supports a power-saving mode that can be activated via software commands. If set to `0`, the driver will not attempt to use power-saving features. The Harvatak RGB LED (https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/6794/3147_T3A33BRG-H9C0002X1U1930.pdf) supports a sleep mode.
         */
        //#define APA102_POWER_SAVING_AVAILABLE

        #ifdef _DOXYGEN_    // Used for documentation, can be ignored
            #define APA102_POWER_SAVING_AVAILABLE
        #endif

        #ifndef APA102_ENABLE_FLAG
            /**
             * @def APA102_ENABLE_FLAG
             * @brief Flag used to enable LED operation.
             *
             * @details
             * This flag value, default `0xE0`, is used in communication settings to signal enabling the LED device or functionality.
             */
            #define APA102_ENABLE_FLAG 0xE0
        #endif
        
        #ifndef APA102_SLEEP_FLAG
            /**
             * @def APA102_SLEEP_FLAG
             * @brief Flag used to set the LED device into sleep mode.
             *
             * @details
             * The value `0xA0` is used to command the LED device to enter a low power or sleep state.
             */
            #define APA102_SLEEP_FLAG 0xA0
        #endif
    #endif

    #include "../../../shared/gfx/color.h"
    #include "../../../utils/macros/stringify.h"

    #include _STR(../../../hal/APA102_HAL_PLATFORM/spi/spi.h)

    /**
     * @enum APA102_Transmission_t
     * @brief Enumerates possible LED frame types for APA102 LED strips.
     *
     * @details
     * This enumeration defines the different frame types that can be used in APA102 LED strip communication It includes Start-of-Frame (SOF) and End-of-Frame (EOF) markers.
     */
    enum APA102_Transmission_t
    {
        APA102_Transmission_SOF=APA102_SOF_VALUE,
        APA102_Transmission_EOF=APA102_EOF_VALUE
    };
    /**
     * @typedef APA102_Transmission
     * @brief Alias for enum APA102_Transmission_t representing possible APA102 transmission types.
     */
    typedef enum APA102_Transmission_t APA102_Transmission;

    /**
     * @def APA102_SOF
     * @brief Sends the Start-of-Frame (SOF) signal to the LED strip.
     *
     * @details
     * This macro transmits the predefined APA102_START_VALUE as a start frame delimiter using the function `apa102_xof()`. It inserts a short delay of 10 microseconds to ensure proper timing before subsequent LED data transmission begins. The `SOF` marks the beginning of a new LED data sequence.
     */
    #define APA102_SOF() { apa102_xof(APA102_Transmission_SOF); }

    /**
     * @def APA102_EOF
     * @brief Sends the End-of-Frame (EOF) signal to the LED strip.
     *
     * @details
     * This macro transmits the predefined APA102_STOP_VALUE as an end frame delimiter using the function `apa102_xof()`. It inserts a short delay of 10 microseconds to ensure proper timing after LED data transmission ends. The `EOF` indicates the completion of the current LED data sequence.
     */
    #define APA102_EOF() { apa102_xof(APA102_Transmission_EOF); }

    void apa102_init(void);
    void apa102_xof(APA102_Transmission type);
    void apa102_led(const GFX_RGBA_Color *color);
    void apa102_leds(const GFX_RGBA_Color *color);
    void apa102_led_off(void);
    void apa102_leds_off(void);

#endif /* APA102_H_ */