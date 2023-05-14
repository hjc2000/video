$(() =>
{
	const video = document.createElement('video');
	const player = new NPlayer.Player(
		{
			video,
			videoProps: { autoplay: 'true' },
			videoSources: [{ src: './hk.mp4', type: 'video/mp4' }],
		}
	);
	player.mount(document.body);
});