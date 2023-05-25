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
	console.log(Hls.isSupported());
}

export function AddScript()
{
	var head = document.head;
	var script = document.createElement('script');
	script.setAttribute("src", "./hls.js");
	head.appendChild(script);
}
