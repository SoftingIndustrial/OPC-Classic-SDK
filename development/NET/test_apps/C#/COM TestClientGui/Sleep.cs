using System.Threading;

namespace TestClientGui.Elements
{
	public class Sleep : Element
	{
		#region Public Properties
		public int Duration
		{
			get;
			set;
		}
		#endregion


		public override TestResult Execute()
		{
			Thread.Sleep( Duration );

			return TestResult.Success();
		}
	}
}
