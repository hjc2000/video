// 这里的代码用来禁用 鼠标滚轮缩放、双指滑动缩放
export function DisableZoom()
{
	window.addEventListener('mousewheel', function (event)
	{
		if (event.ctrlKey === true || event.metaKey)
		{
			event.preventDefault();
		}
	},
		{ passive: false });

	window.addEventListener('touchstart', function (event)
	{
		if (event.touches.length >= 2)
		{
			event.preventDefault();
		}
	});

	window.addEventListener('touchmove', function (event)
	{
		if (event.touches.length >= 2)
		{
			event.preventDefault();
		}
	});

	window.addEventListener('touchend', function (event)
	{
		if (event.touches.length >= 2)
		{
			event.preventDefault();
		}
	});
}
