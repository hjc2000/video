using System.Net;
using System.Numerics;

namespace HttpServerLib.IP;
public static class IPConverter
{
	public static BigInteger ToBigInteger(IPAddress ip)
	{
		return new BigInteger(ip.GetAddressBytes().Reverse().ToArray());
	}

	public static IPAddress ToIPAddress(BigInteger ipBigInt)
	{
		return new IPAddress(ipBigInt.ToByteArray().Reverse().ToArray());
	}
}
