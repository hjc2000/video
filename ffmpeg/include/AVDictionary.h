/**
 * @file AVDictionary.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-17
 *
 * @copyright Copyright (c) 2023
 *
 * @code {.cpp} 示例

	FFmpeg::AVDictionary dic;
	dic.av_dict_set("name", "hjc");
	cout << dic.av_dict_count() << endl;
	auto entry = dic.av_dict_get("name", nullptr);
	cout << entry->key << endl;
	cout << entry->value << endl;

 * @endcode
 *
 */

#pragma once
#include <AVUtil.h>
extern "C"
{
#define __STDC_CONSTANT_MACROS
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
}

namespace FFmpeg
{
	/**
	 * @brief 对 ::AVDictionary 的封装
	 * 本类对象实例化后，不会立刻创建一个 ::AVDictionary 对象，要调用
	 * av_dict_set 方法之后才会创建。本类对象销毁的时候会自动在析构函数中调用 ffmpeg
	 * 的 API 来清理字典占用的资源，当然，也可以手动调用本类的 av_dict_free 方法
	 * 来清理资源。清理完了之后，再次调用 av_dict_set 方法会创建一个新的 ::AVDictionary
	 * 对象
	 */
	class AVDictionary
	{
	public:
		~AVDictionary()
		{
			av_dict_free();
		}

#pragma region 对 ::AVDictionary 的实现
	public:
		/**
		 * @brief 获取字典中记录的条数
		 *
		 * @return int
		 */
		inline int av_dict_count()
		{
			return ::av_dict_count(m_dic);
		}

		/**
		 * @brief 向字典中写一条记录
		 *
		 * @param key
		 * @param value
		 * @param flags
		 * @return int
		 */
		inline int av_dict_set(const char *key, const char *value, AVDictionaryFlag flags = AVDictionaryFlag::none)
		{
			return ::av_dict_set(&m_dic, key, value, flags);
		}

		/**
		 * @brief 获取字典中的记录
		 *
		 * @param key
		 * @param previous_entry 指定上一条记录，如果指定，则从这里开始往后查找，
		 * 如果传入 nullprt 则从头开始查找
		 * @param flags
		 * @return FFmpeg::AVDictionaryEntry*
		 */
		AVDictionaryEntry *av_dict_get(const char *key, const AVDictionaryEntry *previous_entry, AVDictionaryFlag flags = AVDictionaryFlag::none)
		{
			return ::av_dict_get(m_dic, key, previous_entry, flags);
		}

		/**
		 * @brief 释放字典使用的所有资源
		 *
		 */
		inline void av_dict_free()
		{
			::av_dict_free(&m_dic);
		}
#pragma endregion

	public:
		::AVDictionary *m_dic = nullptr;
	};
}