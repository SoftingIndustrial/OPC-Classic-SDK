using System;
using System.Threading;
using System.Diagnostics;
using System.Xml.Serialization;

namespace TestClientGui.Elements
{
	public class StopMonitorWatch : Element
	{
		#region Public Properties

		public string WatchName { get; set; }

		#endregion

		public override TestResult Execute()
		{
			if (string.IsNullOrEmpty( WatchName ))
				return TestResult.ParameterMissing("WatchName");

			Stopwatch stopWatch = GetProperty( WatchName ) as Stopwatch;

			if (stopWatch == null)
			{
				return TestResult.Failed("Monitor Watch does not exist");
			}

			if (!stopWatch.IsRunning)
			{
				return TestResult.Failed("This StopWatch is not running.");
			}

			stopWatch.Stop();

			return TestResult.Success();
		}

	}
}