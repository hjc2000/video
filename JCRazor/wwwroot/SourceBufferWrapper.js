/**
 * SourceBuffer 对象的包装器
 */
export class SourceBufferWrapper
{
	/**
	 * 
	 * @param {SourceBuffer} source_buffer
	 * @returns
	 */
	static Create(
		source_buffer,
		update_end_callback_helper
	)
	{
		return new SourceBufferWrapper(source_buffer, update_end_callback_helper);
	}

	/**
	 * 
	 * @param {SourceBuffer} source_buffer
	 */
	constructor(
		source_buffer,
		update_end_callback_helper
	)
	{
		this.source_buffer = source_buffer;
		this.update_end_callback_helper = update_end_callback_helper;

		this.__on_update_end = () =>
		{
			this.OnUpdateEnd();
		};
		this.source_buffer.addEventListener("updateend", this.__on_update_end);
	}

	Dispose()
	{
		this.source_buffer.removeEventListener("updateend", this.__on_update_end);
	}

	/**
	 * 将流中的数据添加到播放缓冲区中。
	 * 禁忌：不要将很长的流甚至是无限长度的流传给本函数。因为本函数会将流中的所有数据都
	 * 复制到 arrayBuffer 中后才会将这个 arrayBuffer 追加到播放缓冲区。
	 * @param {any} dotnet_stream_ref
	 */
	async AppendBufferByStream(dotnet_stream_ref)
	{
		const arrayBuffer = await dotnet_stream_ref.arrayBuffer();
		this.source_buffer.appendBuffer(arrayBuffer);
	}

	/**
	 * 
	 * @param {Uint8Array} uin8_arr
	 */
	async AppendBufferByArray(uin8_arr)
	{
		this.source_buffer.appendBuffer(uin8_arr.buffer);
	}

	OnUpdateEnd()
	{
		this.update_end_callback_helper.invokeMethodAsync("Invoke");
	}

	/**
	 * 当前缓冲的数据的头部所对应的播放时间。单位：秒。
	 * @returns
	 */
	BufferStartTime()
	{
		return this.source_buffer.buffered.start(0);
	}

	/**
	 * 当前缓冲的数据的尾部所对应的播放时间。单位：秒。
	 * @returns
	 */
	BufferEndTime()
	{
		return this.source_buffer.buffered.end(0)
	}

	RemoveData(start_time, end_time)
	{
		this.source_buffer.remove(start_time, end_time);
	}
}