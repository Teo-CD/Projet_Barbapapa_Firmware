//
// Created by Teo-CD on 09/09/23.
//

#ifndef JIN_BARBAPAPA_PINOUT_H
#define JIN_BARBAPAPA_PINOUT_H

enum Pinout_T41 {
	pin_Serial_RX=0,
	pin_Serial_TX=1,
	pin_DBG_LED_1=2,
	pin_DBG_LED_2=3,

	pin_NFC1_CS=10,
	pin_NFC2_RST=25,
	pin_NFC1_RST=37,
	pin_NFC2_CS=38,

	pin_SPI_COPI=11,
	pin_SPI_CIPO=12,
	pin_SPI_SCK=13,
	pin_SPI1_COPI=26,
	pin_SPI1_SCK=27,
	pin_SPI1_CIPO=39,

	pin_Audio_OUT=7,
	pin_Audio_AMP_GAIN0=14,
	pin_Audio_AMP_GAIN1=15,
	pin_Audio_AMP_GAIN_EN=16,
	pin_Audio_Amp_enable=17,
	pin_Audio_LRCLK=20,
	pin_Audio_BCLK=21,

	pin_ARGB_Data=23,

	/* Power */
	pin_BAT_SNS=24,
	pin_Reg_en=30,

	pin_LCD_SCK_ALT=32,
	pin_LCD_COPI_ALT=33,
	pin_LCD_DC=34,
	pin_LCD_RST=35,
	pin_LCD_CS=36,
};

#endif //JIN_BARBAPAPA_PINOUT_H
