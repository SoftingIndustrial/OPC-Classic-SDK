using System;
using System.Windows.Forms;
using Softing.OPCToolbox;

namespace TestClientGui
{
	static class Program
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault( false );

			if( !ResultCode.SUCCEEDED (TestClient.Initialize() ) )
			{
				return;
			}

			Application.Run( new TestClientForm() );

			TestClient.ApplicationInstance.Terminate();
		}
	}
}
