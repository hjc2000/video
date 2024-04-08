#ifndef _OUTPUT_RFMIXER_
#define _OUTPUT_RFMIXER_

#include<vatek_base.h>
#include<halreg_rfmixer.h>
#include<jccpp/Json.h>

enum rfmixer_status
{
	rfmixer_unsupport = -2,
	rfmixer_fail = -1,
	rfmixer_idle = 0,
	rfmixer_powered = 1,
};

#define R2_TUNE_HDR_TAG				0xFF001934
#define R2_TUNE_ITEM_TAG			0x47001934
#define R2_TUNE_STORAGE_TYPE		0xF1F10122
#define R2_TUNE_NUMS				0x40

#define R2_RANGE_I          0
#define R2_RANGE_Q          1
#define R2_RANGE_IMG        2
#define R2_RANGE_PHASE      3
#define R2_RANGE_PAGAIN     4
#define R2_RANGE_POWER      5
#define R2_RANGE_RC         6
#define R2_RANGE_NUMS       7

#define R2_VALIDX_MIN       0
#define R2_VALIDX_MAX       1
#define R2_VALIDX_SIGN      2
#define R2_VALIDX_MASK      3
#define R2_VALIDX_NUMS		4

struct r2_tune
{
	uint16_t _ioffset{};
	uint16_t _qoffset{};
	uint16_t _imgoffset{};
	uint16_t _phaseoffset{};

	std::string ToString()
	{
		return ToJson().dump(4);
	}

	Json ToJson()
	{
		return Json{
			{"_ioffset",_ioffset},
			{"_qoffset",_qoffset},
			{"_imgoffset",_imgoffset},
			{"_phaseoffset",_phaseoffset},
		};
	}
};

enum r2_cntl_mode
{
	r2_cntl_unknown = -1,
	r2_cntl_path_0 = 0,
	r2_cntl_path_1 = 1,
	r2_cntl_param = 2,
};

inline std::string ToString(r2_cntl_mode value)
{
	switch (value)
	{
	case r2_cntl_path_0:
		{
			return "r2_cntl_path_0";
		}
	case r2_cntl_path_1:
		{
			return "r2_cntl_path_1";
		}
	case r2_cntl_param:
		{
			return "r2_cntl_param";
		}
	default:
		{
			return "Unknown R2 control mode";
		}
	}
}

struct r2_param
{
	r2_cntl_mode _r2_control_mode{};
	uint32_t r2_flags{};
	r2_tune _tune{};
	uint16_t _pagain{};
	uint16_t _gpiocntl{};
};

#endif
