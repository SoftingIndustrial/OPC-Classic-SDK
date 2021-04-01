using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class SetProperty : Element
	{
		#region Public Properties
		public string Property { get; set; }

		public int? IntValue { get; set; }
		#endregion

		public override TestResult Execute()
		{
			if( IntValue != null )
			{
				SetProperty( Property, (int)IntValue );
				return TestResult.Success();
			}

			return TestResult.Failed( "No property to set was specified" );
		}
	}
}
