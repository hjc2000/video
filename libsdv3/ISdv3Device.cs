namespace libsdv3;

/// <summary>
///		本接口的方法、属性需要实现为线程安全，允许多个线程同时使用同一个方法、属性，
///		或者同时使用不同的方法、属性。
/// </summary>
public interface ISdv3Device
{
	#region 硬件 EI
	public bool EI1 { get; }
	public bool EI2 { get; }
	public bool EI3 { get; }
	public bool EI4 { get; }
	public bool EI5 { get; }
	#endregion

	#region 通信 EI
	public bool EI9 { get; set; }
	public bool EI10 { get; set; }
	public bool EI11 { get; set; }
	public bool EI12 { get; set; }
	public bool EI13 { get; set; }
	public bool EI14 { get; set; }
	public bool EI15 { get; set; }
	public bool EI16 { get; set; }
	public bool EI17 { get; set; }
	public bool EI18 { get; set; }
	public bool EI19 { get; set; }
	public bool EI20 { get; set; }
	public bool EI21 { get; set; }
	public bool EI22 { get; set; }
	public bool EI23 { get; set; }
	public bool EI24 { get; set; }
	#endregion

	#region EOUT
	public bool EOUT1 { get; }
	public bool EOUT2 { get; }
	public bool EOUT3 { get; }
	public bool EOUT6 { get; }
	public bool EOUT7 { get; }
	public bool EOUT8 { get; }
	public bool EOUT9 { get; }
	public bool EOUT10 { get; }
	public bool EOUT11 { get; }
	public bool EOUT12 { get; }
	public bool EOUT13 { get; }
	public bool EOUT14 { get; }
	public bool EOUT15 { get; }
	public bool EOUT16 { get; }
	public bool EOUT17 { get; }
	public bool EOUT18 { get; }
	public bool EOUT19 { get; }
	public bool EOUT20 { get; }
	public bool EOUT21 { get; }
	#endregion

	#region 监控数据
	/// <summary>
	///		反馈速度
	/// </summary>
	public uint FeedbackSpeed { get; }

	/// <summary>
	///		指令速度
	/// </summary>
	public uint CommandSpeed { get; }

	/// <summary>
	///		指令转矩
	/// </summary>
	public uint CommandTorque { get; }

	/// <summary>
	///		峰值转矩
	/// </summary>
	public uint PeakTorque { get; }

	/// <summary>
	///		电机电流
	/// </summary>
	public uint MotorCurrent { get; }

	/// <summary>
	///		有效转矩
	/// </summary>
	public uint EffectiveTorque { get; }

	/// <summary>
	///		反馈当前位置
	/// </summary>
	public int FeedbackCurrentPosition { get; }

	/// <summary>
	///		指令当前位置
	/// </summary>
	public uint CommandCurrentPosition { get; }

	/// <summary>
	///		位置偏差
	/// </summary>
	public uint PositionDeviation { get; }

	/// <summary>
	///		指令脉冲频率
	/// </summary>
	public uint CommandPulseFrequency { get; }

	/// <summary>
	///		反馈累计脉冲数
	/// </summary>
	public uint TotalFeedbackPulseCount { get; }

	/// <summary>
	///		指令累计脉冲数
	/// </summary>
	public uint TotalCommandPulseCount { get; }

	/// <summary>
	///		LS-Z 之间脉冲数
	/// </summary>
	public uint PulseCountBetween_LS_And_Z { get; }

	/// <summary>
	///		负载惯性力矩比
	/// </summary>
	public uint LoadInertiaTorqueRatio { get; }

	/// <summary>
	///		直流中间电压（最大）
	/// </summary>
	public uint MaxDCIntermediateVoltage { get; }

	/// <summary>
	///		直流中间电压（最小）
	/// </summary>
	public uint MinDCIntermediateVoltage { get; }

	/// <summary>
	///		VREF 输入电压
	/// </summary>
	public uint VrefInputVoltage { get; }

	/// <summary>
	///		TREF 输入电压
	/// </summary>
	public uint TrefInputVoltage { get; }

	/// <summary>
	///		OL 预报值
	/// </summary>
	public uint OLForecast { get; }
	#endregion

	#region 报警
	/// <summary>
	///		当前报警
	/// </summary>
	public uint CurrentAlarm { get; }

	/// <summary>
	///		报警记录
	/// </summary>
	/// <param name="record_id">从 1 开始。</param>
	/// <returns></returns>
	public uint AlarmRecords(int record_id);

	/// <summary>
	///		AlarmRecords 的数量
	/// </summary>
	/// <returns></returns>
	public int AlarmRecordCount { get; }
	#endregion

	public uint Pn(int major, int minor);
	public void SetPn(int major, int minor, uint[] value);
	public void SetPn(int major, int minor, uint value);
}
