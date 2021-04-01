using Softing.OPCToolbox.Client;

namespace DABrowse
{
	public class MyDaSession : DaSession
	{

		#region Constructor
		//-----------------

		public MyDaSession(string url) : base(url)
		{
			StateChangeCompleted += new StateChangeEventHandler(HandleStateChanged);
		}

		//--
		#endregion

		#region Private Members
		//---------------------
		bool m_connected = false;

		#region Private Attributes
		//------------------------


		//--
		#endregion

		//--
		#endregion

		#region Public Methods
		//---------------------

		//--
		#endregion

		#region Public Properties
		//-----------------------
		public bool isConnected()
		{
			return m_connected;
		}

		//--
		#endregion

		#region Handles
		//---------------------
		public void HandleStateChanged(ObjectSpaceElement sender, EnumObjectState state)
		{
			if (state == EnumObjectState.CONNECTED)
			{
				m_connected = true;
			}
			else
			{
				m_connected = false;
			}
		}


		//--
		#endregion

	}   //	end class MyDaSession

}   //	end namespace
