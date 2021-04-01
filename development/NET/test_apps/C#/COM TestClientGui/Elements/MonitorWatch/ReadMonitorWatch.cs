using System;
using System.Threading;
using System.Diagnostics;
using System.Xml.Serialization;

namespace TestClientGui.Elements
{
	public class ReadMonitorWatch : Element
	{
		#region Public Properties

		public string WatchName { get; set; }

		#endregion

		public override TestResult Execute()
		{
			if (string.IsNullOrEmpty(WatchName))
				return TestResult.ParameterMissing("WatchName");

			Stopwatch stopWatch = GetProperty(WatchName) as Stopwatch;

			if (stopWatch == null)
			{
				return TestResult.Failed("Monitor Watch does not exist");
			}


			string elapsedTime = String.Format("{0:00}:{1:00}:{2:00}.{3:000}",
			 stopWatch.Elapsed.Hours, stopWatch.Elapsed.Minutes, stopWatch.Elapsed.Seconds,
			 stopWatch.Elapsed.Milliseconds);


			return TestResult.Failed(elapsedTime);

			//return TestResult.Success();
		}

	}
}