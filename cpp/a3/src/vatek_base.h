#ifndef _VATEK_BASE_
#define _VATEK_BASE_

#include<cstdint>
#include<stddef.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<chrono>
#include<DateTime.h>

using namespace std::chrono;

#define CHIP_RAISE_COMMAND_TIMEOUT      10000
#define CHIP_TS_PACKET_LEN              188
#define CHIP_STREAM_SLICE_LEN			24064

#define VATEK_VERSION 30900

class DeviceFinder;
class A3Device;

/**
 * @ingroup core_props
 * vatek sdk result define
 */
enum vk_result
{
	vatek_keilc_struct = 0x7FFFFFFF,
	vatek_bufferempty = -14,
	vatek_sizeerr = -13,				/*!< size error (value not support)*/
	vatek_crcfail = -12,				/*!< check crc fail	*/
	vatek_memfail = -11,				/*!< memory alloc fail or overflow */
	vatek_format = -10,					/*!< format not current */
	vatek_nodevice = -9,				/*!< device not exists */
	vatek_busy = -8,					/*!< system is busy */
	vatek_timeout = -7,					/*!< wait operation timeout */
	vatek_hwfail = -6,					/*!< send command to device fail or call system api fail */
	vatek_badstatus = -5,				/*!< can not call at this device status */
	vatek_bufoverflow = -4,				/*!< memory size limited overflow */
	vatek_badparam = -3,				/*!< parameter set not supported */
	vatek_unsupport = -2,				/*!< function not supported */
	vatek_unknown = -1,					/*!< unknown fail */
	vatek_success = 0,					/*!< bigger than 0 success */
	vatek_quitcli = 100,
};

#define is_vatek_success(a)		(a >= vatek_success)

enum vatek_debug_level
{
	debug_level_all = 0,
	debug_level_warning = 1,
	debug_level_error = 2,
};

/* compiler time assert check */
#define _VASSERT(COND,MSG) typedef char static_assertion_##MSG[(!!(COND))*2-1];

vk_result vatek_version();

void vatek_print(const char *fmt, ...);

void vatek_debug_set(vatek_debug_level level);

/// <summary>
///		将数组转换为 uint32_t，使用的是大端序，即数组的低字节变成 uint32_t 中的高字节。
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
static inline uint32_t vatek_buffer_2_uint32(uint8_t *pbuf)
{
	return (pbuf[0] << 24) | (pbuf[1] << 16) | (pbuf[2] << 8) | pbuf[3];
}

/// <summary>
///		将 uint32_t 转换为字节数组。转换出来的字节数组是大端序，即 uint32_t 的低字节
///		变成字节数组的高字节。
/// </summary>
/// <param name="pbuf">转换结果</param>
/// <param name="val">要被转换成字节数组的 uint32_t</param>
static inline void vatek_uint32_2_buffer(uint8_t *pbuf, uint32_t val)
{
	*pbuf++ = (uint8_t)(val >> 24);
	*pbuf++ = (uint8_t)(val >> 16);
	*pbuf++ = (uint8_t)(val >> 8);
	*pbuf++ = (uint8_t)(val & 0xFF);
}

#define VERR(fmt,...)   vatek_debug_print(debug_level_error,"[%08x:%-8s] - %-24s - " fmt "\r\n",DateTime::GetSteadyTimeInMilliseconds(),"error",__func__,##__VA_ARGS__)
#define VWAR(fmt,...)   vatek_debug_print(debug_level_warning,"[%08x:%-8s] - %-24s - " fmt "\r\n",DateTime::GetSteadyTimeInMilliseconds(),"warning",__func__,##__VA_ARGS__)
#define VDBG(fmt,...)   vatek_debug_print(debug_level_all,"[%08x:%-8s] - %-24s - " fmt "\r\n",DateTime::GetSteadyTimeInMilliseconds(),"info",__func__,##__VA_ARGS__)

#endif
