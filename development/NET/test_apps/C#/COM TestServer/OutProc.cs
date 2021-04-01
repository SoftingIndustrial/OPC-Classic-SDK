using System;
using System.Threading;
using System.IO;

namespace TestServer
{
	/// <summary>
	/// Summary description for OutProc.
	/// </summary>
	public class OutProc
	{
		#region Private Attributes
		//-------------------------
		private static OpcServer m_opcServer = null;

		//--
		#endregion

		#region Public Properties
		//------------------------

		public OpcServer OpcServer
		{
			get
			{
				return m_opcServer;
			}	//	end get
		}	//	end OpcServer
		//--
		#endregion

		#region Public Methods
		//--------------------------
		public void CreateOpcServer()
		{
			if (m_opcServer == null)
			{
				m_opcServer = new OpcServer();
			}	//	em=nd if
		}	//	end CreateOpcClient
		//--
		#endregion

	}	//	end class Outproc
}	//	end namespace
