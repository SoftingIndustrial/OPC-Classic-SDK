using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TestClientGui.Elements
{
	public enum AssertType
	{
		Equal,
		NotEqual
	}

	public class Assert : Element
	{
		#region Public Properties
		public AssertType Type { get; set; }
		#endregion

		public override TestResult Execute()
		{
			switch( Type )
			{
				case AssertType.Equal:
					return AssertIsEqual();
				case AssertType.NotEqual:
					return AssertNotIsEqual();
				default:
					return TestResult.ParameterInvalid( "Type" );
			}
		}

		private TestResult AssertIsEqual()
		{
			throw new NotImplementedException();
		}

		private TestResult AssertNotIsEqual()
		{
			throw new NotImplementedException();
		}
	}
}
