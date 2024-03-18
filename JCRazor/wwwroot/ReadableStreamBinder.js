/**
 * 投影类
 */
export class ReadableStreamBinder
{
	static create(readable_stream)
	{
		return new ReadableStreamBinder(readable_stream);
	}

	/**
	 * 
	 * @param {ReadableStream} readable_stream
	 */
	constructor(readable_stream)
	{
		this.readable_stream = readable_stream;
		this.reader = this.readable_stream.getReader();
	}

	async read()
	{
		let result = await this.reader.read();
		console.log(result);
		if (result.done)
		{
			throw "流结束";
		}

		return result.value;
	}

	get_stream_size()
	{
		let size = this.readable_stream.size;
		if (size != undefined)
		{
			return size;
		}
		else
		{
			return -1;
		}
	}

	get_file_name()
	{
		if (this.readable_stream.fileName != undefined)
		{
			return this.readable_stream.fileName;
		}
		else
		{
			return "";
		}
	}
}