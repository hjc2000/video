export class InputFileElementBinder
{
	/**
	 * 构造一个 InputFileElementBinder 实例。因为 .net 无法直接使用构造函数，所以只能使用
	 * 这个工厂函数来创建一个对象
	 * 
	 * @param {string} accept 可接受的文件扩展名拼接成的字符串。每种扩展名使用逗号分隔，例如：
	 * ".jpg,.png"
	 * 
	 * @param {boolean} multiple
	 * @param {any} file_load_callback_helper
	 * @returns
	 */
	static create(accept, multiple, file_load_callback_helper)
	{
		return new InputFileElementBinder(accept, multiple, file_load_callback_helper);
	}

	/**
	 * 
	 * @param {string} accept
	 * @param {any} file_load_callback_helper
	 * @param {boolean} multiple
	 */
	constructor(accept, multiple, file_load_callback_helper)
	{
		this.inputFileElement = document.createElement("input");
		this.inputFileElement.type = "file";
		if (accept)
		{
			this.inputFileElement.accept = accept;
		}

		this.file_load_callback_helper = file_load_callback_helper;
		if (multiple)
		{
			this.inputFileElement.multiple = true;
		}

		// 设置样式
		this.inputFileElement.style.display = "block";
		this.inputFileElement.style.width = "0";
		this.inputFileElement.style.height = "0";
		this.inputFileElement.style.margin = "0";
		this.inputFileElement.style.padding = "0";
		this.inputFileElement.style.borderStyle = "none";

		// 追加到 DOM 中
		document.body.appendChild(this.inputFileElement);
	}

	/**
	 * 触发文件输入标签的点击事件
	 */
	click()
	{
		// 先聚焦
		this.inputFileElement.focus();

		// 订阅聚焦事件
		this.inputFileElement.onfocus = () =>
		{
			setTimeout(() =>
			{
				this.file_load_callback_helper.invokeMethodAsync("Invoke", this.inputFileElement.files.length);
			}, 200);
		};

		// 点击输入标签，打开文件选取窗口。关闭文件选取窗口后焦点会回到输入标签身上，从而触发 onfocus 事件。
		this.inputFileElement.click();
	}

	/**
	 * 以 js 流的方式读取文件
	 * 
	 * @param {number} index 要读取第几个文件
	 * @param {*} slice_start 切片开始的位置。
	 * @returns
	 */
	get_file_as_stream(index, slice_start)
	{
		let file = this.inputFileElement.files[index];
		if (slice_start > 0)
		{
			file = file.slice(slice_start);
		}

		let readable_stream = file.stream();

		// 将 file 的 size 赋值给 readable_stream
		// js 的 readable_stream 本来没有 size 属性，但是文件有大小
		readable_stream.size = file.size;
		readable_stream.fileName = file.name;
		return readable_stream;
	}

	/**
	 * 移除文件输入标签
	 */
	remove()
	{
		this.inputFileElement.remove();
	}
}
