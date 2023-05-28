$(() =>
{
	console.log("是否支持" + mpegts.isSupported());
	console.log(mpegts.getFeatureList());
	if (mpegts.getFeatureList().mseLivePlayback)
	{
		var videoElement = document.getElementById('videoElement');
		var player = mpegts.createPlayer(
			{
				type: 'mpegts',  // could also be mpegts, m2ts, flv
				isLive: true,
				url: "http://localhost:8848/ts.ts"
			},
			{
				enableWorker: true,
				enableStashBuffer: false,
			});
		player.attachMediaElement(videoElement);
		player.load();
		setTimeout(() =>
		{
			player.play();
		}, 2000);
	}
});