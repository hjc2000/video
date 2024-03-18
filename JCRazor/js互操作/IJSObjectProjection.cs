using Microsoft.JSInterop;

namespace JCRazor.js互操作;

/// <summary>
///		js 对象在 .NET 侧的投影类。
/// </summary>
public interface IJSObjectProjection
{
	/// <summary>
	///		作为 js 对象在 .NET 侧的投影类，反过来，那个 js 对象也是本类对象在 js 侧的投影。
	///		
	///		本属性引用本类对象位于 js 侧的投影。每一个本类的派生类的对象都需要在 js 侧创建一个同名的类，作为投影。
	/// </summary>
	public IJSObjectReference Projection { get; }
}
