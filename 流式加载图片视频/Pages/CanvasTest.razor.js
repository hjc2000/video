export class Canvas
{
	/**
	 * 工厂函数
	 * @param {any} canvas
	 * @param {any} context
	 * @returns
	 */
	static create(canvas, context)
	{
		return new Canvas(canvas, context);
	}

	/**
	 * 构造函数
	 * @param {HTMLCanvasElement} canvas
	 * @param {string} context
	 */
	constructor(canvas, context)
	{
		this.canvas = canvas;
		this.context = this.canvas.getContext(context);
		this.buffer = new Uint8ClampedArray(this.canvas.width * this.canvas.height * 4);
		this.imageData = new ImageData(this.buffer, canvas.width, canvas.height);
	}

	/**获取画布高度 */
	get height()
	{
		return this.canvas.height;
	}

	/**获取画布宽度 */
	get width()
	{
		return this.canvas.width;
	}

	/**获取缓冲区长度 */
	get bufferLength()
	{
		return this.buffer.length;
	}

	play()
	{
		let bar_width = 100;
		let left = -bar_width;

		setInterval(() =>
		{
			for (let i = 0; i < this.bufferLength; i += 4)
			{
				let col = ((i + 1) / 4) % this.width;
				if (col > left && col <= left + bar_width)
				{
					this.imageData.data[i] = 0;
					this.imageData.data[i + 1] = 100;
					this.imageData.data[i + 2] = 0;
				}
				else
				{
					this.imageData.data[i] = 255;
					this.imageData.data[i + 1] = 255;
					this.imageData.data[i + 2] = 255;
				}

				this.imageData.data[i + 3] = 255;
			}

			if (left < this.width)
			{
				left++;
			}
			else
			{
				left = -bar_width;
			}

			this.context.putImageData(this.imageData, 0, 0);
		},
			1 / 30);

		return this.imageData;
	}
}
