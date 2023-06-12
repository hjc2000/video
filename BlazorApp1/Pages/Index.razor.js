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
	let readResult = await reader.read();
	console.log(readResult);
}


export async function get_stream(data)
{
	let stream = await data.stream();
	return stream;
}

export class InputFileElement
{
	static create(element)
	{
		return new InputFileElement(element);
	}

	constructor(element)
	{
		this.element = element;
		this.fileList = element.files;
	}

	get_file_as_stream(index)
	{
		let file = this.fileList[index];
		console.log(file.stream());
	}
}