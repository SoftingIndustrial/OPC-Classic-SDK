using System;
using System.Threading;
using System.ComponentModel;
using System.Diagnostics;
using System.Xml.Serialization;

namespace TestClientGui.Elements
{
	public class ExportMonitorWatchResult : Element
	{
		#region Public Properties

		public string WatchName { get; set; }

		public string Comment { get; set; }

		public string FilePath { get; set; }

		public string FileName { get; set; }

		#endregion


		public override TestResult Execute()
		{
			if (string.IsNullOrEmpty(WatchName))
				return TestResult.ParameterMissing("WatchName");

			if (string.IsNullOrEmpty(FileName))
			{
				FileName = WatchName + ".txt";
			}

			if (string.IsNullOrEmpty(FilePath))
			{
				FilePath = Environment.CurrentDirectory + "\\" ;
			}

			Stopwatch stopWatch = GetProperty(WatchName) as Stopwatch;

			if (stopWatch == null)
			{
				return TestResult.Failed("Monitor Watch does not exist");
			}

			string elapsedTime = String.Format("{0:00}:{1:00}:{2:00}.{3:000}",
												stopWatch.Elapsed.Hours, stopWatch.Elapsed.Minutes,
												stopWatch.Elapsed.Seconds, stopWatch.Elapsed.Milliseconds);

			WriteToFile(FilePath, FileName, "StopWatch:"+WatchName+ "	" + Comment + "		Elapsed hh:mm:ss:zzz	" + elapsedTime);

			return TestResult.Success();
		}

		public void WriteToFile(string FullFilePath, string FileName, string message)
		{
			System.IO.StreamWriter fileWriter = new System.IO.StreamWriter(FullFilePath + FileName, true);

			fileWriter.WriteLine(message);

			fileWriter.Flush();

			fileWriter.Close();

		}

	}
}