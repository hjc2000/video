#pragma once

namespace FFmpeg
{
	template <class T>
	class Wrapper
	{
		// 访问器
	public:
		/**
		 * @brief 获取包装器包装的基础对象
		 *
		 * @return T* const
		 */
		virtual T *const get_Base(void)
		{
			return _baseObj;
		}

	protected:
		T *_baseObj = nullptr;
	};
}
