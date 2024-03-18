export class DialogWrapper
{
	/**
	 * 
	 * @param {any} dialogElement
	 * @param {object} dotnetHelper
	 * @returns
	 */
	static create(dialogElement, close_callback_helper)
	{
		return new DialogWrapper(dialogElement, close_callback_helper);
	}

	/**
	 * 
	 * @param {HTMLDialogElement} dialogElement
	 */
	constructor(dialogElement, close_callback_helper)
	{
		this.dialogElement = dialogElement;
		this.close_callback_helper = close_callback_helper;
		this.dialogElement.onclose = () =>
		{
			this.__onclose();
		}
	}

	show()
	{
		this.dialogElement.show();
	}

	showModal()
	{
		this.dialogElement.showModal();
	}

	close()
	{
		this.dialogElement.close();
	}

	isOpened()
	{
		return this.dialogElement.open;
	}

	__onclose()
	{
		this.close_callback_helper.invokeMethodAsync("Invoke");
	}
}