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

export async function Log(stream)
{
	console.log(stream);
	stream._streamPromise.then((stream) =>
	{
		let reader = stream.getReader();
		console.log(reader);
	});
}
