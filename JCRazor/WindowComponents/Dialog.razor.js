export class DialogWrapper
{
	/**
	 * 静态的工厂函数
	 * @param {HTMLDialogElement} dialog_element
	 * @param {object} dotnetHelper
	 * @returns
	 */
	static create(dialog_element, close_callback_helper)
	{
		return new DialogWrapper(dialog_element, close_callback_helper);
	}

	/**
	 * 
	 * @param {HTMLDialogElement} dialog_element
	 */
	constructor(dialog_element, close_callback_helper)
	{
		this.dialog_element = dialog_element;
		this.close_callback_helper = close_callback_helper;
		this.dialog_element.onclose = () =>
		{
			this.__onclose();
		}
	}

	show()
	{
		this.dialog_element.show();
	}

	showModal()
	{
		this.dialog_element.showModal();
	}

	close()
	{
		this.dialog_element.close();
	}

	isOpened()
	{
		return this.dialog_element.open;
	}

	__onclose()
	{
		this.close_callback_helper.invokeMethodAsync("Invoke");
	}
}