#ifndef _HALREG_INPUT_
#define _HALREG_INPUT_

/* stream usb param									*/
/*--------------------------------------------------*/
#define HALREG_USB_INPUT_MODE			0x640
#define HALREG_USB_STREAM_FLAGS			0x641
#define USB_EN_ASYNCBUFFER			0x00000001
#define HALREG_USB_PCR_MODE				0x642

#define HALREG_USB_ADJUST_TICK			0x643

/* stream ts param									*/
/*--------------------------------------------------*/
#define HALREG_TSIN_MODE				0x640
#define HALREG_TSIN_FLAGS				0x641
#define TS_PIN_SERIAL               0x00000001
#define TS_PIN_NEGATIVE_CLK         0x00000002
#define TS_PIN_NEGATIVE_VALID       0x00000004
#define TS_PIN_NEGATIVE_SYNC        0x00000008

#define HALREG_TSIN_PCR_MODE			0x642

/*
	mux filter only supported in a3 mux mode
	num == 0 or no current en_tag would take all packets when actived remux at a3 chip
*/

#define HALREG_MUX_FILTER_EN			0x648				
#define MUX_FILTER_EN_TAG			0xA5F00000
#define MUX_FILTER_INV_TAG			0x00010000

#define MUX_FILTER_MASK					0xFF
#define MUX_FILTER_NAX_NUMS			16

/* pid_filter */
#define HALREG_MUX_FILTER_0				0x670
#define FILTER_EN_REPLACE			0x80000000
#define FILTER_OFFSET_SUR			16
#define FILTER_OFFSET_NEW			0
#define FILTER_NO_REPLACE(sur)		(sur << FILTER_OFFSET_SUR)
#define FILTER_REPLACE(sur,new)		(FILTER_EN_REPLACE | (sur << FILTER_OFFSET_SUR) | new)

#endif

