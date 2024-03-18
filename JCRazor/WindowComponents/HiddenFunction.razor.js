export class HiddenFunction
{
	static create(show_hidden_function_callback_helper)
	{
		return new HiddenFunction(show_hidden_function_callback_helper);
	}

	constructor(show_hidden_function_callback_helper)
	{
		this._show_hidden_function_callback_helper = show_hidden_function_callback_helper;
		window.hf = () =>
		{
			this._show_hidden_function_callback_helper.invokeMethodAsync("Invoke");
			return "hf 开启";
		};
	}
}

