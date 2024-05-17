namespace libsdv3.Modbus;

public static class ParamAddress
{
	#region 硬件 EI
	public static ushort EI1 { get; } = 0x0400;
	public static ushort EI2 { get; } = (ushort)(EI1 + (2 - 1));
	public static ushort EI3 { get; } = (ushort)(EI1 + (3 - 1));
	public static ushort EI4 { get; } = (ushort)(EI1 + (4 - 1));
	public static ushort EI5 { get; } = (ushort)(EI1 + (5 - 1));
	#endregion

	#region 通信 EI
	/* - 通信 EI 可读可写。
	 * - 这里每个 EI 占用一个地址，不像功能参数那样每个数据要占用 2 个地址。所以地址是逐 1 递增的。
	 */
	public static ushort EI9 { get; } = 0x0208;
	public static ushort EI10 { get; } = (ushort)(EI9 + (10 - 9));
	public static ushort EI11 { get; } = (ushort)(EI9 + (11 - 9));
	public static ushort EI12 { get; } = (ushort)(EI9 + (12 - 9));
	public static ushort EI13 { get; } = (ushort)(EI9 + (13 - 9));
	public static ushort EI14 { get; } = (ushort)(EI9 + (14 - 9));
	public static ushort EI15 { get; } = (ushort)(EI9 + (15 - 9));
	public static ushort EI16 { get; } = (ushort)(EI9 + (16 - 9));
	public static ushort EI17 { get; } = (ushort)(EI9 + (17 - 9));
	public static ushort EI18 { get; } = (ushort)(EI9 + (18 - 9));
	public static ushort EI19 { get; } = (ushort)(EI9 + (19 - 9));
	public static ushort EI20 { get; } = (ushort)(EI9 + (20 - 9));
	public static ushort EI21 { get; } = (ushort)(EI9 + (21 - 9));
	public static ushort EI22 { get; } = (ushort)(EI9 + (22 - 9));
	public static ushort EI23 { get; } = (ushort)(EI9 + (23 - 9));
	public static ushort EI24 { get; } = (ushort)(EI9 + (24 - 9));
	#endregion

	#region 硬件 EOUT
	public static ushort EOUT1 { get; } = 0x0500;
	public static ushort EOUT2 { get; } = (ushort)(EOUT1 + (2 - 1));
	public static ushort EOUT3 { get; } = (ushort)(EOUT1 + (3 - 1));
	#endregion

	#region 通信 EOUT
	public static ushort EOUT6 { get; } = 0x0305;
	public static ushort EOUT7 { get; } = (ushort)(EOUT6 + (7 - 6));
	public static ushort EOUT8 { get; } = (ushort)(EOUT6 + (8 - 6));
	public static ushort EOUT9 { get; } = (ushort)(EOUT6 + (9 - 6));
	public static ushort EOUT10 { get; } = (ushort)(EOUT6 + (10 - 6));
	public static ushort EOUT11 { get; } = (ushort)(EOUT6 + (11 - 6));
	public static ushort EOUT12 { get; } = (ushort)(EOUT6 + (12 - 6));
	public static ushort EOUT13 { get; } = (ushort)(EOUT6 + (13 - 6));
	public static ushort EOUT14 { get; } = (ushort)(EOUT6 + (14 - 6));
	public static ushort EOUT15 { get; } = (ushort)(EOUT6 + (15 - 6));
	public static ushort EOUT16 { get; } = (ushort)(EOUT6 + (16 - 6));
	public static ushort EOUT17 { get; } = (ushort)(EOUT6 + (17 - 6));
	public static ushort EOUT18 { get; } = (ushort)(EOUT6 + (18 - 6));
	public static ushort EOUT19 { get; } = (ushort)(EOUT6 + (19 - 6));
	public static ushort EOUT20 { get; } = (ushort)(EOUT6 + (20 - 6));
	public static ushort EOUT21 { get; } = (ushort)(EOUT6 + (21 - 6));
	#endregion
}
