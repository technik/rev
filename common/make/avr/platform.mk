#-----------------------------------------------------------------------------------------------------------------------
# Revolution SDK
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 30th, 2012
#-----------------------------------------------------------------------------------------------------------------------
# Avr platform configuration

PLATFORM := avr

AVR_DEFINITIONS := -DATMEGA
ifeq(atmega328p, $(MCU))
    PLATFORM_DEFINITIONS := $(AVR_DEFINITIONS) -DATMEGA328P -DF_CPU=16000000L
    PLATFORM_FLAGS := -mmcu=m328p
endif
ifeq(atmega2560, $(MCU))
    PLATFROM_DEFINITIONS := -DATMEGA -DATMEGA2560 -DF_CPU=16000000L
    PLATFORM_FLAGS := -mmcu=atmega2560
endif