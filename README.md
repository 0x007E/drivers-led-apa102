[![Version: 1.0 Release](https://img.shields.io/badge/Version-1.0%20Release-green.svg)](https://github.com/0x007e/drivers-led-apa102) ![Build](https://github.com/0x007e/drivers-led-apa102/actions/workflows/release.yml/badge.svg) [![License GPLv3](https://img.shields.io/badge/License-GPLv3-lightgrey)](https://www.gnu.org/licenses/gpl-3.0.html)

# `APA102 RGB LED Driver`

This hardware abstracted driver can be used to interact with an [APA102](#additional-information) or any compatible type over `TWI`/`I2C`. The hardware layer is fully abstract an can be switched between different plattforms. The `SPI` library has to impelement the [spi.h](https://0x007e.github.io/drivers-led-apa102/spi_8c.html)-header used in this repository.

## File Structure

![File Structure](https://0x007e.github.io/drivers-led-apa102/apa102_8c__incl.png)

```
drivers/
└── led/
    └── apa102/
        ├── apa102.c
        └── apa102.h

hal/
├── common/
|   ├── enums/
|   |   └── SPI_enums.h
|   └── macros/
|       └── PORT_macros.h
└── avr0/
    └── spi/
        ├── spi.c
        └── spi.h

shared/
└── gfx/
    └── color.h

utils/
└──macros/
   └── stringify.h
```

> The plattform `avr0` can completely be exchanged with any other hardware abstraction library.

## Downloads

The library can be downloaded (`zip` or `tar`), cloned or used as submodule in a project.

| Type      | File               | Description              |
|:---------:|:------------------:|:-------------------------|
| Library   | [zip](https://github.com/0x007E/drivers-led-apa102/releases/latest/download/library.zip) / [tar](https://github.com/0x007E/drivers-led-apa102/releases/latest/download/library.tar.gz) | APA102 RGB LED library including all required libraries (including `hal-avr0-spi`). |

### Using with `git clone`

```sh
mkdir -p ./shared/
git clone https://github.com/0x007E/shared-gfx.git ./shared
mv ./shared/shared-gfx ./shared/gfxformat

mkdir -p ./drivers/led/
git clone https://github.com/0x007E/drivers-led-apa102.git ./drivers/led
mv ./drivers/led/drivers-led-apa102 ./drivers/led/apa102

mkdir -p ./hal/
git clone https://github.com/0x007E/hal-common.git ./hal
mv ./hal/hal-common ./hal/common

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Hardware abstraction layer of SPI (Must fit the used plattform)
mkdir -p ./hal/avr0/
git clone https://github.com/0x007E/hal-avr0-spi.git ./hal/avr0
mv ./hal/avr0/hal-avr0-spi ./hal/avr0/spi
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

mkdir -p ./utils/
git clone https://github.com/0x007E/utils-macros.git  ./utils
mv ./utils/utils-macros  ./utils/macros
```

### Using as `git submodule`

```sh
git submodule add https://github.com/0x007E/shared-gfx.git            shared/gfx

git submodule add https://github.com/0x007E/drivers-led-apa102.git    drivers/led/apa102
git submodule add https://github.com/0x007E/hal-common.git            hal/common

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Hardware abstraction layer of SPI (Must fit the used plattform)
git submodule add https://github.com/0x007E/hal-avr0-spi.git          hal/avr0/spi
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

git submodule add https://github.com/0x007E/utils-macros.git          utils/macros
```

## Programming

```c
#include "./hal/avr0/spi/spi.h"
#include "./drivers/led/apa102/apa102.h"

int main(void)
{
	spi_init(SPI_MSB, SPI_Rising, SPI_Rising);
	apa102_init();

	GFX_RGBA_Color color = {
		0x0F,
		0xF0,
		0xFF,
		APA102_MAX_INTENSITY
	}

	// To change the color independently on every led
	APA102_SOF();
	
	for (unsigned char i=0; i < APA102_NUMBER_OF_LEDS; i++)
	{
		apa102_led(&color);

		color.red++;
		color.blue--;
	}
	APA102_EOF();

	// Set the same color on all leds
	apa102_leds(&color);

	// Disable every second led
	APA102_SOF();
	
	for (unsigned char i=0; i < APA102_NUMBER_OF_LEDS; i++)
	{
		if(!((i + 1)%2))
		{
			apa102_led(&color);
			continue;
		}
		apa102_led_off();
	}
	APA102_EOF();

	// Disable all leds
	apa102_leds_off();
}
```

# Additional Information

| Type       | Link                                                                                                      | Description                                        |
|:----------:|:---------------------------------------------------------------------------------------------------------:|:---------------------------------------------------|
| APA102     | [pdf](https://cdn.sparkfun.com/datasheets/Components/LED/APA102C.pdf)                                     | RGB Full Color LED                                 |
| T3A33BRG   | [pdf](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/6794/3147_T3A33BRG-H9C0002X1U1930.pdf) | Harvatek Surface Mount PLCC IC+RGB LEDs Data Sheet |

---

R. GAECHTER