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

export async function log(data)
{
	let stream = await data.stream();
	console.log(stream);
	let reader = stream.getReader();
	console.log(reader);
	while (true)
	{
		let readResult = await reader.read();
		if (readResult.done)
		{
			break;
		}
		let uint8Array = readResult.value;
		console.log(uint8Array);
	}
}
