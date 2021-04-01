using System;
using System.Threading;
using System.Diagnostics;
using System.Xml.Serialization;

namespace TestClientGui.Elements
{
	public class StartMonitorWatch:Element
	{
		#region Public Properties

		public string WatchName { get; set; }

		#endregion

		public override TestResult Execute()
		{
			if (string.IsNullOrEmpty(WatchName))
				return TestResult.ParameterMissing("WatchName");

			Stopwatch stopWatch = GetProperty(WatchName) as Stopwatch;
			if (stopWatch != null)
			{
				if (stopWatch.IsRunning)
				{
					return TestResult.Failed("This StopWatch is already running.");
				}
			}
			else
			{
				stopWatch = new Stopwatch();
				SetProperty(WatchName, stopWatch);
			}

			stopWatch.Start();


			return TestResult.Success();
		}

	}
}
