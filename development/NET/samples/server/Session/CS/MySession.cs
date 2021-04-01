using System;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace Session
{
	class MySession : DaSession
	{

		public static MySession s_controlingSession = null;
		private static Int32 s_clientCount = 0;
		private ValueQT m_clientSpecValue = null;

		public MySession(EnumSessionType aType, uint aHandle) : base(aType, aHandle)
		{
			m_clientSpecValue = new ValueQT(s_clientCount, EnumQuality.GOOD, DateTime.Now);
			ClientNameChanged += new SessionClientNameChanged(HandleClientNameChanged);
			s_clientCount++;
		}   //	end ctr

		public uint Handle
		{
			get
			{
				return base.ObjectHandle;
			}   //	end get
		}   //	end Handle

		public override void ConnectSession()
		{

			base.ConnectSession();

			lock (Console.m_sessionsJanitor)
			{
				Console.m_sessions.Add(this);
			}
			Console.m_clientChanged.Set();

		}   //	end ConnectSession

		public override void DisconnectSession()
		{

			base.DisconnectSession();

			lock (Console.m_sessionsJanitor)
			{
				Console.m_sessions.Remove(this);
			}
			Console.m_clientChanged.Set();

		}   //	end DisconnectSession

		private void HandleClientNameChanged()
		{
			System.Console.WriteLine("New client name set for client {0:X}: {1}", Handle, ClientName);
		}   //	end HandleClientNameChanged

		public override int Logoff()
		{

			if (this.Equals(s_controlingSession))
			{
				s_controlingSession = null;
			}   //	end if

			Console.m_clientChanged.Set();
			return (int)EnumResultCode.S_OK;

		}   //	end Logoff

		public override int Logon(System.String UserName, System.String Password)
		{

			int ret = (int)EnumResultCode.E_ACCESSDENIED;

			if (UserName == "OPC" && Password == "opc")
			{

				if (this.Equals(s_controlingSession) || s_controlingSession == null)
				{
					s_controlingSession = this;
					ret = (int)EnumResultCode.S_OK;
				}   //	end if
			}   //	end if

			Console.m_clientChanged.Set();
			return ret;
		}   //	end Logon

		public int GetCacheValue(ref ValueQT aCacheValue)
		{

			aCacheValue = new ValueQT(m_clientSpecValue.Data, m_clientSpecValue.Quality, m_clientSpecValue.TimeStamp);
			return (int)EnumResultCode.S_OK;
		}   //	end GetCacheValue

	}   //	end class MySession
}
