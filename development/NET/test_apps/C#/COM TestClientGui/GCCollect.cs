using System.Collections.Generic;
using System.Xml.Serialization;
using System;

namespace TestClientGui.Elements
{
	public class GCCollect : Element
	{
		public override TestResult Execute()
		{
			long before = GC.GetTotalMemory( false );
			GC.Collect();
			long after = GC.GetTotalMemory( true );

			return new GCCollectTestResult( before, after );
		}
	}

	public class GCCollectTestResult : TestResult
	{
		public long Before { get; set; }
		public long After { get; set; }

		public GCCollectTestResult()
		{
		}

		public GCCollectTestResult( long before, long after )
			: base( TestResults.Success )
		{
			Before = before;
			After = after;
		}
	}
}
