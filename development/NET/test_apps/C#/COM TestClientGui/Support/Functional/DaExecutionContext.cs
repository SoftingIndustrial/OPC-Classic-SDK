using System.Threading;

namespace TestClientGui.Elements
{
	public class DaExecutionContext
	{
		public static uint Next
		{
			get
			{
				return (uint) Interlocked.Increment( ref m_current );
			}
		}

		private static int m_current = 0;
	}
}
