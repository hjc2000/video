namespace JCNET;
public interface IDataConsumer<T>
{
	Task SendDataAsync(T data);
}
