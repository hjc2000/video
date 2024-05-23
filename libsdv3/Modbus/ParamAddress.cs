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

	#region 监控数据
	/// <summary>
	///		反馈速度
	/// </summary>
	public static ushort FeedbackSpeed { get; } = 0x1000;

	/// <summary>
	///		指令速度
	/// </summary>
	public static ushort CommandSpeed { get; } = 0x1002;

	/// <summary>
	///		指令转矩
	/// </summary>
	public static ushort CommandTorque { get; } = 0x1004;

	/// <summary>
	///		峰值转矩
	/// </summary>
	public static ushort PeakTorque { get; } = 0x1006;

	/// <summary>
	///		电机电流
	/// </summary>
	public static ushort MotorCurrent { get; } = 0x1008;

	/// <summary>
	///		有效转矩
	/// </summary>
	public static ushort EffectiveTorque { get; } = 0x100A;

	/// <summary>
	///		反馈当前位置
	/// </summary>
	public static ushort FeedbackCurrentPosition { get; } = 0x100C;

	/// <summary>
	///		指令当前位置
	/// </summary>
	public static ushort CommandCurrentPosition { get; } = 0x100E;

	/// <summary>
	///		位置偏差
	/// </summary>
	public static ushort PositionDeviation { get; } = 0x1010;

	/// <summary>
	///		指令脉冲频率
	/// </summary>
	public static ushort CommandPulseFrequency { get; } = 0x1012;

	/// <summary>
	///		反馈累计脉冲数
	/// </summary>
	public static ushort TotalFeedbackPulseCount { get; } = 0x1014;

	/// <summary>
	///		指令累计脉冲数
	/// </summary>
	public static ushort TotalCommandPulseCount { get; } = 0x1016;

	/// <summary>
	///		LS-Z 之间脉冲数
	/// </summary>
	public static ushort PulseCountBetween_LS_And_Z { get; } = 0x1018;

	/// <summary>
	///		负载惯性力矩比
	/// </summary>
	public static ushort LoadInertiaTorqueRatio { get; } = 0x101A;

	/// <summary>
	///		直流中间电压（最大）
	/// </summary>
	public static ushort MaxDCIntermediateVoltage { get; } = 0x101C;

	/// <summary>
	///		直流中间电压（最小）
	/// </summary>
	public static ushort MinDCIntermediateVoltage { get; } = 0x101E;

	/// <summary>
	///		VREF 输入电压
	/// </summary>
	public static ushort VrefInputVoltage { get; } = 0x1020;

	/// <summary>
	///		TREF 输入电压
	/// </summary>
	public static ushort TrefInputVoltage { get; } = 0x1022;

	/// <summary>
	///		OL 预报值
	/// </summary>
	public static ushort OLForecast { get; } = 0x1024;
	#endregion

	public static ushort CurrentAlarm { get; } = 0x2200;

	/// <summary>
	///		报警记录的地址
	/// </summary>
	/// <param name="record_id">从 1 开始</param>
	/// <returns></returns>
	public static ushort AlarmRecords(int record_id)
	{
		return (ushort)(0x2202 + ((record_id - 1) * 2));
	}

	public static ushort Pn(int major, int minor)
	{
		if (minor is < 1 or > 99)
		{
			throw new ArgumentOutOfRangeException($"{nameof(minor)} 只能在 [1, 99] 中。");
		}

		switch (major)
		{
		case 1:
			{
				return (ushort)(0x4000 + ((minor - 1) * 2));
			}
		case 2:
			{
				return (ushort)(0x4100 + ((minor - 1) * 2));
			}
		case 3:
			{
				return (ushort)(0x4200 + ((minor - 1) * 2));
			}
		case 4:
			{
				return (ushort)(0x4300 + ((minor - 1) * 2));
			}
		}

		throw new ArgumentOutOfRangeException($"{nameof(major)} 只能在 [1, 4] 中。");
	}

	#region 立即数
	public static ushort ImmediateState { get; } = 0x5100;
	public static ushort ImmediatePosition { get; } = 0x5102;
	public static ushort ImmediateSpeed { get; } = 0x5104;
	public static ushort ImmediateAccelerationDuration { get; } = 0x5106;
	public static ushort ImmediateDecelerationDuration { get; } = 0x5108;
	#endregion

	public static ushort Speed { get; } = 0x3000;
}
