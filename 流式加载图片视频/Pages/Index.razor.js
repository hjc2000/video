export async function setImage(imageElement, imageStream)
{
	const arrayBuffer = await imageStream.arrayBuffer();
	const blob = new Blob([arrayBuffer]);
	const url = URL.createObjectURL(blob);
	imageElement.onload = () =>
	{
		URL.revokeObjectURL(url);
	}
	imageElement.src = url;
}

export function Log()
{
	console.log(mpegts);
}

export async function LoadVideo(videoElement, videoStream)
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

export function Load_ts()
{
	// Create array of TS files to play
	segments = [
		"http://localhost:8848/ts0.ts",
		"http://localhost:8848/ts1.ts",
		"http://localhost:8848/ts2.ts",
		"http://localhost:8848/ts3.ts",
		"http://localhost:8848/ts4.ts",
	];

	// Replace this value with your files codec info
	mime = 'video/mp4; codecs="mp4a.40.2,avc1.64001f"';

	let mediaSource = new MediaSource();
	let transmuxer = new muxjs.mp4.Transmuxer();

	video = document.querySelector('video');
	video.src = URL.createObjectURL(mediaSource);
	mediaSource.addEventListener("sourceopen", appendFirstSegment);

	function appendFirstSegment()
	{
		if (segments.length == 0)
		{
			return;
		}

		URL.revokeObjectURL(video.src);
		sourceBuffer = mediaSource.addSourceBuffer(mime);
		sourceBuffer.addEventListener('updateend', appendNextSegment);

		transmuxer.on('data', (segment) =>
		{
			let data = new Uint8Array(segment.initSegment.byteLength + segment.data.byteLength);
			data.set(segment.initSegment, 0);
			data.set(segment.data, segment.initSegment.byteLength);
			console.log(muxjs.mp4.tools.inspect(data));
			sourceBuffer.appendBuffer(data);
		})

		fetch(segments.shift()).then((response) =>
		{
			return response.arrayBuffer();
		}).then((response) =>
		{
			transmuxer.push(new Uint8Array(response));
			transmuxer.flush();
		})
	}

	function appendNextSegment()
	{
		// reset the 'data' event listener to just append (moof/mdat) boxes to the Source Buffer
		transmuxer.off('data');
		transmuxer.on('data', (segment) =>
		{
			sourceBuffer.appendBuffer(new Uint8Array(segment.data));
		})

		if (segments.length == 0)
		{
			// notify MSE that we have no more segments to append.
			console.log("结束");
			mediaSource.endOfStream();
		}
		console.log("获取下一个视频");
		fetch(segments.shift()).then((response) =>
		{
			return response.arrayBuffer();
		}).then((response) =>
		{
			transmuxer.push(new Uint8Array(response));
			transmuxer.flush();
		})
	}
}