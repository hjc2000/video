using System.Collections.Concurrent;

namespace JCNET.容器;

/// <summary>
///		* 提供数据缓存的功能。有一定容量上限，达到上限后继续添加数据会移除最旧的数据。
///		* 这适用于一个生产者生产数据，多个发送者用网络将数据发送出去的场景。当生产者以稳定的，
///		  较快的速度生产时，发送端可能来不及将数据发送出去，特别是并发连接特别多时，
///		  所有连接共用一个以太网接口，如果总的需求速率超过了带宽，则肯定会有一些连接无法及时将数据
///		  发送出去。
///		* 这里提供了缓存机制，类似于文件系统能够缓存一段时间的 ts 切片，然后定时删除那些过期的。
///		  每个发送端都需要配备一个这种缓存器，生产者会将数据添加到每个发送端的缓存器中。
///		* 因为这里是储存对数据的引用，不是储存数据本身，所以只要切片的大小不要太接近 8 字节（指针的大小）
///		  储存效率就比较高。并发连接较多时，每个连接的缓冲器储存一些指针，也不会给内存带来太多额外的压力。
///		* 考虑用 ArrayPool 储存切片，避免直接使用大型数组。大型数组需要在垃圾回收的第 3 代才能被回收。
/// </summary>
/// <typeparam name="T"></typeparam>
public class DataCache<T>
{
	public DataCache(int max_count)
	{
		if (max_count < 1)
		{
			throw new ArgumentException($"{nameof(max_count)} 必须大于等于 1");
		}

		_max_count = max_count;
		_wait_data = new SemaphoreSlim(0, max_count);
	}

	private int _max_count;
	private ConcurrentQueue<T> _data_cache_queue = [];
	private readonly object _queue_lock = new();
	private SemaphoreSlim _wait_data;

	/// <summary>
	///		添加一个元素到末尾，添加后如果元素数量达到上限，会移除最开始的那个。
	///		本方法会释放 TackOutOldestAsync 方法的等待。
	/// </summary>
	/// <param name="item"></param>
	public void PusbBack(T item)
	{
		lock (_queue_lock)
		{
			_data_cache_queue.Enqueue(item);
			if (_data_cache_queue.Count > _max_count)
			{
				// 数据过多，丢弃最旧的数据
				_ = _data_cache_queue.TryDequeue(out _);
			}

			while (_wait_data.CurrentCount < _data_cache_queue.Count)
			{
				try
				{
					_wait_data.Release();
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex);
					break;
				}
			}
		}
	}

	/// <summary>
	///		获取最旧的数据，获取后会将最旧的数据移除。如果没有数据，本方法会等待，
	/// </summary>
	/// <param name="cancellationToken"></param>
	/// <returns></returns>
	/// <exception cref="OperationCanceledException"></exception>
	public async Task<T> TackOutOldestAsync(CancellationToken cancellationToken)
	{
		while (!cancellationToken.IsCancellationRequested)
		{
			await _wait_data.WaitAsync(cancellationToken);
			lock (_queue_lock)
			{
				bool dequeue_result = _data_cache_queue.TryDequeue(out T? data);
				if (dequeue_result && data is not null)
				{
					return data;
				}
				else
				{
					continue;
				}
			}
		}

		throw new OperationCanceledException();
	}
}
