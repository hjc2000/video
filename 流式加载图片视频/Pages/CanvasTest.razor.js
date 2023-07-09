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
		this.imageData = new ImageData(canvas.width, canvas.height);
	}

	play()
	{
		let bar_width = 100;
		let left = -bar_width;
		let col_num = this.imageData.data.length / 4 / this.canvas.height;

		setInterval(() =>
		{
			for (let i = 0; i < this.imageData.data.length; i += 4)
			{
				let col = ((i + 1) / 4) % this.canvas.width;
				if (col > left && col <= left + bar_width)
				{
					this.imageData.data[i] = 0;
					this.imageData.data[i + 1] = 0;
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

			if (left < col_num)
			{
				left++;
			}
			else
			{
				left = -bar_width;
			}

			this.context.putImageData(this.imageData, 0, 0);
		},
			1 / 60);
		return this.imageData;
	}
}
