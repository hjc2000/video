namespace JCNET;
public class IndexOperator
{
	/// <summary>
	///		限制索引的范围，防止越界。
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="list"></param>
	/// <param name="current_index"></param>
	/// <returns>集合为空时会返回 -1. 因为没办法，任何值都是越界的。</returns>
	public static int LimitCurrentIndex<T>(IList<T>? list, int current_index)
	{
		if (list is null)
		{
			return -1;
		}

		if (list.Count == 0)
		{
			return -1;
		}

		// 第一个过滤器，防止上溢
		if (current_index >= list.Count)
		{
			current_index = list.Count - 1;
		}

		/* 经过第一个过滤器后，如果列表长度为 0，有可能产生下溢，所以要第二个过滤器，防止下溢。
		 * 此外，有可能传进来的 current_index 本来就是下溢的。
		 */
		if (current_index < 0)
		{
			current_index = 0;
		}

		return current_index;
	}
}
