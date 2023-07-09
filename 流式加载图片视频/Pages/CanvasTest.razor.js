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
		this.bufferLength = this.canvas.width * this.canvas.height * 4;
		this.arrayBuffer = new ArrayBuffer(this.bufferLength);
		this.uint8Buffer = new Uint8Array(this.arrayBuffer);
		this.imageBuffer = new Uint8ClampedArray(this.arrayBuffer);
		this.imageData = new ImageData(this.imageBuffer, canvas.width, canvas.height);
	}

	/**
	 * 
	 * @param {Uint8Array} buffer
	 */
	putUint8Buffer(buffer)
	{
		let imageBuffer = new Uint8ClampedArray(buffer.buffer);
		let imageData = new ImageData(imageBuffer, this.canvas.width, this.canvas.height);
		this.context.putImageData(imageData, 0, 0);
	}

	/**将图像放置到画布中 */
	__putImage()
	{
		this.context.putImageData(this.imageData, 0, 0);
	}

	play()
	{
		let bar_width = 100;
		let left = -bar_width;
		let width = this.canvas.width;
		let bufferLength = this.bufferLength;
		let buffer = this.imageBuffer;

		setInterval(() =>
		{
			for (let i = 0; i < bufferLength; i += 4)
			{
				let col = ((i + 1) / 4) % width;
				if (col > left && col <= left + bar_width)
				{
					buffer[i] = 0;
					buffer[i + 1] = 100;
					buffer[i + 2] = 0;
				}
				else
				{
					buffer[i] = 255;
					buffer[i + 1] = 255;
					buffer[i + 2] = 255;
				}

				buffer[i + 3] = 255;
			}

			if (left < width)
			{
				left++;
			}
			else
			{
				left = -bar_width;
			}

			this.__putImage();
		},
			1 / 30);
	}
}
