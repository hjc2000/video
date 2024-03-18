#pragma once

namespace jc
{
	/**
	 * @brief 输入端可以一直更新，不会影响到输出端的数据。调用 UpdateOutput 将
	 * 输入端的数据更新到输出端。直到再次更新之前，调用 GetOutput 得到的一直是上次
	 * 更新的结果。
	*/
	template<typename T>
	class Trigger
	{
	public:
		T GetOutput()
		{
			return _output;
		}

		void UpdateOutput()
		{
			_have_not_update_output = false;
			_output = _input;
		}

		void UpdateInput(T value)
		{
			_input = value;
		}

		bool HaveNotUpdateOutput()
		{
			return _have_not_update_output;
		}

	private:
		T _output{};
		T _input{};

		/**
		 * @brief 从来没有更新过输出
		*/
		bool _have_not_update_output = true;
	};
}