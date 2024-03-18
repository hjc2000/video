namespace JCNET.流;

public class SpliceByte
{
	public static ushort SpliceIntoUint16(byte high, byte low)
	{
		return (ushort)((high << 8) | low);
	}

	public static uint SpliceIntoUint32(byte[] bytes, int offset = 0, bool littleEndian = false)
	{
		uint result = 0;
		if (littleEndian)
		{
			for (int i = 0; i < 4; i++)
			{
				result |= (uint)bytes[offset + i] << (8 * i);
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				result <<= 8;
				result |= bytes[offset + i];
			}
		}

		return result;
	}
}
