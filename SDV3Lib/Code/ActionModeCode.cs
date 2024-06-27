namespace SDV3Lib.Code;
public enum ActionModeCode
{
	ServoOff,
	ServoOn,

	/// <summary>
	///		速度零停止
	/// </summary>
	ZeroSpeedStop,

	/// <summary>
	///		手动运行
	/// </summary>
	ManualOperation,

	/// <summary>
	///		脉冲列运行
	/// </summary>
	PulseOperation,

	/// <summary>
	///		+OT
	/// </summary>
	PositiveOT,

	/// <summary>
	///		-OT
	/// </summary>
	NegativeOT,

	/// <summary>
	///		电压不足
	/// </summary>
	LowVoltage,

	/// <summary>
	///		自动运行
	/// </summary>
	AutomaticOperation,

	/// <summary>
	///		原点复归
	/// </summary>
	OriginRestoration,

	/// <summary>
	///		插入运行
	/// </summary>
	InsertRun
}
