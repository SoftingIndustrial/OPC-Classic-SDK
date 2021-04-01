using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TestServer
{
	/// <summary>
	/// Uses the Pseudo random generator as a source.
	/// </summary>
	public class RandomSource
	{
		/// <summary>
		/// Initializes the source with a time dependent seed.
		/// </summary>
		public RandomSource()
		{
			m_random = new Random();
		}

		/// <summary>
		/// Initializes the source with a seed.
		/// </summary>
		/// <param name="seed">The number used to initialize the Pseudo random sequence.</param>
		public RandomSource(int seed)
		{
			m_random = new Random(seed);
		}

		/// <summary>
		/// Fills a range in array of bytes with random numbers.
		/// </summary>
		/// <param name="bytes">The array to update.</param>
		/// <param name="offset">The start of the range generate.</param>
		/// <param name="count">The number of bytes to generate.</param>
		/// <exception cref="ArgumentNullException">Thrown if the bytes parameter is null.</exception>
		/// <exception cref="ArgumentOutOfRangeException">Thrown if the offset or count parameters do not specify a valid range within the bytes parameter.</exception>
		public void NextBytes(byte[] bytes, int offset, int count)
		{
			if (bytes == null) throw new ArgumentNullException("bytes");
			if (offset < 0 || (offset != 0 && offset >= bytes.Length)) throw new ArgumentOutOfRangeException("offset");
			if (count < 0 || offset + count > bytes.Length) throw new ArgumentOutOfRangeException("count");

			if (bytes.Length == 0)
			{
				return;
			}

			if (offset == 0 && count == bytes.Length)
			{
				m_random.NextBytes(bytes);
			}
			else
			{
				byte[] buffer = new byte[count];
				m_random.NextBytes(buffer);
				Array.Copy(buffer, 0, bytes, offset, count);

			}
		}

		/// <summary>
		/// Returns a random non-negative integer which does not exeed the specified maximum.
		/// </summary>
		/// <param name="max">The maximum value to return.</param>
		/// <returns>A random value greater than 0 but less than or equal to max.</returns>
		/// <exception cref="ArgumentOutOfRangeException">Thrown if the max parameter is less than zero.</exception>
		public int NextInt32(int max)
		{
			if (max < 0) throw new ArgumentOutOfRangeException("max");

			if (max < Int32.MaxValue)
			{
				max++;
			}

			return m_random.Next(max);
		}

		private Random m_random;
	}
}
