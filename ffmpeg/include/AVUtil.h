/**
 * @file FFmpegEnum.htmlinclude [block] file-name
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 * typedef 一些 ffmpeg 中的数据类型，或者将 ffmpeg 中的宏定义转化为枚举类型
 */
#pragma once
#include<include_ffmpeg.h>
#include <string>
#include<iostream>
#include<sstream>

using std::string;
using std::cout;
using std::endl;
using std::stringstream;

namespace FFmpeg
{
	typedef ::AVMediaType AVMediaType;
	typedef ::AVDictionaryEntry AVDictionaryEntry;
	typedef ::AVCodecID AVCodecID;
	typedef ::AVCodecParameters AVCodecParameters;
	typedef ::AVPixelFormat AVPixelFormat;
	typedef ::AVRational AVRational;
	typedef ::AVCodecParameters AVCodecParameters;
	typedef ::AVCodecID AVCodecID;

	/// <summary>
	/// 字典操作的枚举类型
	/// </summary>
	enum AVDictionaryFlag
	{
		none = 0,

		match_case = 1, /**< Only get an entry with exact-case key match. Only relevant in av_dict_get(). */

		ignore_suffix = 2,	 /**< Return first entry in a dictionary whose first part corresponds to the search key,
												ignoring the suffix of the found key string. Only relevant in av_dict_get(). */
		dont_strdup_key = 4, /**< Take ownership of a key that's been
												  allocated with av_malloc() or another memory allocation function. */
		dont_strdup_val = 8, /**< Take ownership of a value that's been
												  allocated with av_malloc() or another memory allocation function. */
		dont_overwrite = 16, /**< Don't overwrite existing entries. */

		append = 32,  /**< If the entry already exists, append to it.  Note that no
										delimiter is added, the strings are simply concatenated. */
		multikey = 64 /**< Allow to store several equal keys in the dictionary */
	};

	#pragma region AVMediaType 的相关操作
	std::string media_type_to_string(FFmpeg::AVMediaType type);
	std::string &operator<<(std::string &str, FFmpeg::AVMediaType const type);
	std::ostream &operator<<(std::ostream &ostream, FFmpeg::AVMediaType const type);
	void operator>>(std::string const &str, FFmpeg::AVMediaType &type);
	#pragma endregion

	#pragma region AVRational 的相关操作
	std::string &operator<<(std::string &str, FFmpeg::AVRational const rational);
	std::ostream &operator<<(std::ostream &ostream, FFmpeg::AVRational const rational);
	double operator*(int64_t const duration, FFmpeg::AVRational const rational);
	double operator*(FFmpeg::AVRational const rational, int64_t const duration);
	#pragma endregion

	#pragma region AVPixelFormat 相关操作
	/// <summary>
	/// 获取像素的名称。如果找不到指定的像素名称，返回空字符串
	/// </summary>
	/// <param name="pf"></param>
	/// <returns></returns>
	std::string get_pix_fmt_name(FFmpeg::AVPixelFormat pf);
	std::string &operator<<(std::string &str, FFmpeg::AVPixelFormat pf);
	std::ostream &operator<<(std::ostream &ostream, FFmpeg::AVPixelFormat pf);
	#pragma endregion

}

