using System;
using System.Collections.Generic;
using System.Text;

namespace TestClientGui.Elements
{
	public class MessageBox : Element
	{
		#region Public Properties
		public string Message
		{
			get;
			set;
		}
		#endregion

		public override TestResult Execute()
		{
			System.Windows.Forms.MessageBox.Show( Message );

			return TestResult.Success();
		}
	}
}
