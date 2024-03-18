export class CssStyleOperator
{
	/**
	 * 
	 * @param {HTMLElement} element
	 * @param {string} style_name
	 * @returns
	 */
	static get_computed_style(element, style_name)
	{
		console.log(window.getComputedStyle(element));
		return window.getComputedStyle(element)[style_name];
	}

	/**
	 * 
	 * @param {HTMLElement} element
	 * @param {string} width
	 */
	static set_width(element, width)
	{
		element.style.width = width;
	}

	/**
	 * 
	 * @param {HTMLElement} element
	 * @param {string} height
	 */
	static set_height(element, height)
	{
		element.style.height = height;
	}

	/**
	 * 
	 * @param {HTMLElement} element
	 * @param {string} padding
	 */
	static set_padding(element, padding)
	{
		element.style.padding = padding;
	}

	/**
	 * 
	 * @param {HTMLElement} element
	 * @param {string} margin
	 */
	static set_margin(element, margin)
	{
		element.style.margin = margin;
	}
}