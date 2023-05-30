export function Log()
{
	console.log(mpegts);
}

export async function LoadVideoStream(videoElement, videoStream)
{
	const arrayBuffer = await videoStream.arrayBuffer();
	const blob = new Blob([arrayBuffer]);
	const url = URL.createObjectURL(blob);
	videoElement.onload = () =>
	{
		URL.revokeObjectURL(url);
	}

	if (mpegts.getFeatureList().mseLivePlayback)
	{
		var player = mpegts.createPlayer(
			{
				type: 'mpegts',  // could also be mpegts, m2ts, flv
				isLive: true,
				url: url,
			},
			{
				enableStashBuffer: false,
			});
		player.attachMediaElement(videoElement);
		player.load();
		player.play();
	}
}

export async function LoadVideoFromUrl(videoElement, url)
{
	if (mpegts.getFeatureList().mseLivePlayback)
	{
		var player = mpegts.createPlayer(
			{
				type: 'mpegts',  // could also be mpegts, m2ts, flv
				isLive: true,
				url: url,
			});
		player.attachMediaElement(videoElement);
		player.load();
		player.play();
	}
}

class TSPlayer
{
	constructor(videoElement, dotnetHelper)
	{
		this.videoElement = videoElement;
		this.dotnetHelper = dotnetHelper;
		this.segments = [
			"ts0.ts",
			"ts1.ts",
			"ts2.ts",
			"ts3.ts",
			"ts4.ts",
		];

		console.log(this.segments.length);
		this.mime = 'video/mp4; codecs="mp4a.40.2,avc1.64001f"';
		this.mediaSource = new MediaSource();
		this.sourceBuffer = null;
		this.transmuxer = new muxjs.mp4.Transmuxer();
		videoElement.src = URL.createObjectURL(this.mediaSource);
		this.mediaSource.addEventListener("sourceopen", () =>
		{
			this.AppendFirstSegment();
		});
	}

	/**
	 * 从 .net 中获取存放着 ts 流的 byte[] 然后推送给播放器进行播放
	 * @param {any} fileName
	 * @returns
	 */
	async FetchAndPush(fileName)
	{
		let buff = await this.dotnetHelper.invokeMethodAsync("FetchAsync", fileName);
		this.transmuxer.push(buff);
		this.transmuxer.flush();
	}

	/**
	 * 添加第一个片段到播放器
	 * @returns
	 */
	async AppendFirstSegment()
	{
		if (this.segments.length == 0)
		{
			return;
		}

		// MediaSource 对象被 video 标签打开后，MediaSource 的 URL 就
		// 没用了，可以释放了，因为 video 已经获取到 MediaSource 对象的
		// 引用了，它不需要时刻都使用 URL，只在打开 MediaSource 对象的时候
		// 需要
		URL.revokeObjectURL(this.videoElement.src);
		this.sourceBuffer = this.mediaSource.addSourceBuffer(this.mime);
		this.sourceBuffer.addEventListener('updateend', () =>
		{
			this.AppendNextSegment();
		});

		this.transmuxer.on('data', (segment) =>
		{
			let data = new Uint8Array(segment.initSegment.byteLength + segment.data.byteLength);
			data.set(segment.initSegment, 0);
			data.set(segment.data, segment.initSegment.byteLength);
			console.log(muxjs.mp4.tools.inspect(data));
			this.sourceBuffer.appendBuffer(data);
		})

		await this.FetchAndPush(this.segments.shift());
	}

	/**
	 * 添加后续的片段到播放器
	 * @returns
	 */
	async AppendNextSegment()
	{
		// reset the 'data' event listener to just append (moof/mdat) boxes to the Source Buffer
		this.transmuxer.off('data');
		this.transmuxer.on('data', (segment) =>
		{
			this.sourceBuffer.appendBuffer(new Uint8Array(segment.data));
		})

		if (this.segments.length == 0)
		{
			// notify MSE that we have no more segments to append.
			console.log("结束");
			this.mediaSource.endOfStream();
			return;
		}

		console.log("获取下一个视频");
		await this.FetchAndPush(this.segments.shift());
	}
}

export function LoadTS(videoElement, dotnetHelper)
{
	new TSPlayer(videoElement, dotnetHelper);
}