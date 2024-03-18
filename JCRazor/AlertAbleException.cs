namespace JCRazor;

/// <summary>
///		此异常的消息会通过 alert 通知用户。这用来告知用户它的操作没有达到想要的结果。
///		此异常由底层抛出，组件收到后应该 alert。
/// </summary>
public class AlertAbleException(string? message) : Exception(message)
{
}