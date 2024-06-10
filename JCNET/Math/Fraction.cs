using System.Numerics;

namespace JCNET.Math;

/// <summary>
///		分数类
/// </summary>
public struct Fraction
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

	private BigInteger _num = 0;
	private BigInteger _den = 1;

	/// <summary>
	///		化简。返回化简后的新分数对象。
	/// </summary>
	/// <returns></returns>
	public Fraction Simplify()
	{
		BigInteger gcd = BigInteger.GreatestCommonDivisor(_num, _den);
		_num /= gcd;
		_den /= gcd;
		if (_den < 0)
		{
			_num = -_num;
			_den = -_den;
		}

		return new Fraction(_num, _den);
	}

	public static Fraction operator -(Fraction fraction1)
	{
		Fraction ret = new(-fraction1._num, fraction1._den);
		return ret.Simplify();
	}

	/// <summary>
	///		两个分数相加
	/// </summary>
	/// <param name="fraction1"></param>
	/// <param name="fraction2"></param>
	/// <returns></returns>
	public static Fraction operator +(Fraction fraction1, Fraction fraction2)
	{
		// 两个分数的分母的最小公倍数
		BigInteger den_lcm = fraction1._den * fraction2._den
			/ BigInteger.GreatestCommonDivisor(fraction1._den, fraction2._den);

		// 分子放大与分母相同的倍数
		BigInteger new_num1 = fraction1._num * (den_lcm / fraction1._den);
		BigInteger new_num2 = fraction2._num * (den_lcm / fraction2._den);

		Fraction ret = new(new_num1 + new_num2, den_lcm);
		return ret.Simplify();
	}

	public static Fraction operator -(Fraction fraction1, Fraction fraction2)
	{
		Fraction ret = fraction1 + (-fraction2);
		return ret.Simplify();
	}

	public static Fraction operator *(Fraction fraction1, Fraction fraction2)
	{
		BigInteger num = fraction1._num * fraction2._num;
		BigInteger den = fraction1._den * fraction2._den;
		Fraction ret = new(num, den);
		return ret.Simplify();
	}

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
}
