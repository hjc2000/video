using System.Numerics;

namespace JCNET.Math;

/// <summary>
///		分数类
/// </summary>
public readonly struct Fraction
{
	public Fraction() { }

	public Fraction(BigInteger num)
	{
		_num = num;
		_den = 1;
	}

	public Fraction(BigInteger num, BigInteger den)
	{
		_num = num;
		_den = den;
	}

	/// <summary>
	///		使用字符串进行初始化。
	///		传进来的字符串可以是分数的字符串，也可以是整型或浮点的字符串，例如：
	///			"5/2", "5", "5.12"
	///		这些都是合法的。
	/// </summary>
	/// <param name="str"></param>
	public Fraction(string str)
	{
		int index = str.IndexOf('/');
		if (index == 0)
		{
			// 第 1 个字符就是 / 号
			_num = 0;
			_den = 1;
			return;
		}

		if (index > 0)
		{
			// 存在 / 号，且不是第 1 个字符
			if (str.EndsWith('/'))
			{
				throw new ArgumentException("不能以 '/' 结尾");
			}

			string num_string = str[..index];
			string den_string = str[(index + 1)..];
			_num = BigInteger.Parse(num_string);
			_den = BigInteger.Parse(den_string);
			return;
		}

		// 不包含 / 号，推测传进来的是浮点字符串或整数字符串
		index = str.IndexOf('.');
		if (index < 0)
		{
			// 传进来的是整型字符串
			_num = BigInteger.Parse(str);
			_den = 1;
			return;
		}

		// 传进来的是浮点字符串
		// 计算小数点后有多少位
		int count = str.Length - 1 - index;
		str = str.Remove(index, 1);
		_num = BigInteger.Parse(str);
		_den = BigInteger.Pow(10, count);
	}

	private readonly BigInteger _num = 0;
	private readonly BigInteger _den = 1;

	/// <summary>
	///		化简。返回化简后的新分数对象。
	/// </summary>
	/// <returns></returns>
	public Fraction Simplify()
	{
		BigInteger gcd = BigInteger.GreatestCommonDivisor(_num, _den);
		BigInteger num = _num / gcd;
		BigInteger den = _den / gcd;
		if (_den < 0)
		{
			num = -num;
			den = -den;
		}

		return new Fraction(num, den);
	}

	/// <summary>
	///		求负
	/// </summary>
	/// <param name="fraction1"></param>
	/// <returns></returns>
	public static Fraction operator -(Fraction fraction1)
	{
		Fraction ret = new(-fraction1._num, fraction1._den);
		return ret.Simplify();
	}

	/// <summary>
	///		加法
	/// </summary>
	/// <param name="fraction1"></param>
	/// <param name="fraction2"></param>
	/// <returns></returns>
	public static Fraction operator +(Fraction fraction1, Fraction fraction2)
	{
		// 两个分数的分母的最小公倍数
		BigInteger den_lcm = fraction1._den *
			fraction2._den /
			BigInteger.GreatestCommonDivisor(fraction1._den, fraction2._den);

		// 分子放大与分母相同的倍数
		BigInteger num1 = fraction1._num * (den_lcm / fraction1._den);
		BigInteger num2 = fraction2._num * (den_lcm / fraction2._den);

		Fraction ret = new(num1 + num2, den_lcm);
		return ret.Simplify();
	}

	/// <summary>
	///		减法
	/// </summary>
	/// <param name="fraction1"></param>
	/// <param name="fraction2"></param>
	/// <returns></returns>
	public static Fraction operator -(Fraction fraction1, Fraction fraction2)
	{
		Fraction ret = fraction1 + (-fraction2);
		return ret.Simplify();
	}

	/// <summary>
	///		乘法
	/// </summary>
	/// <param name="fraction1"></param>
	/// <param name="fraction2"></param>
	/// <returns></returns>
	public static Fraction operator *(Fraction fraction1, Fraction fraction2)
	{
		BigInteger num = fraction1._num * fraction2._num;
		BigInteger den = fraction1._den * fraction2._den;
		Fraction ret = new(num, den);
		return ret.Simplify();
	}

	/// <summary>
	///		除法
	/// </summary>
	/// <param name="fraction1"></param>
	/// <param name="fraction2"></param>
	/// <returns></returns>
	public static Fraction operator /(Fraction fraction1, Fraction fraction2)
	{
		Fraction ret = fraction1 * fraction2.Reciprocal;
		return ret.Simplify();
	}

	/// <summary>
	///		倒数
	/// </summary>
	public Fraction Reciprocal
	{
		get
		{
			return new Fraction(_den, _num);
		}
	}

	/// <summary>
	///		分子除以分母的结果
	/// </summary>
	public BigInteger Div
	{
		get
		{
			return _num / _den;
		}
	}
	/// <summary>
	///		分子除以分母的余数
	/// </summary>
	public BigInteger Mod
	{
		get
		{
			return _num % _den;
		}
	}

	public override string ToString()
	{
		return $"{_num} / {_den}";
	}
}
