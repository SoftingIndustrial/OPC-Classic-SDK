namespace OPCEnumerate
{
	/// <summary>
	/// Summary description for OutProc.
	/// </summary>
	public class OutProc
	{
		#region Private Attributes
		//-------------------------
		private static OpcClient m_opcClient = null;

		//--
		#endregion

		#region Public Properties
		//------------------------

		public OpcClient OpcClient
		{
			get
			{
				return m_opcClient;
			}   //	end get
		}   //	end OpcClient
			//--
		#endregion

		#region Public Methods
		//--------------------------
		public void CreateOpcClient()
		{
			if (m_opcClient == null)
			{
				m_opcClient = new OpcClient();
			}   //	end if
		}   //	end CreateOpcClient
			//--
		#endregion

	}   //	end class Outproc
}   //	end namespace
