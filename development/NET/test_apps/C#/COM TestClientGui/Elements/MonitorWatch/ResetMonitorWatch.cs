using System;
using System.Threading;
using System.Diagnostics;
using System.Xml.Serialization;

namespace TestClientGui.Elements
{
	public class ResetMonitorWatch : Element
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

			stopWatch.Reset();

			return TestResult.Success();
		}

	}
}