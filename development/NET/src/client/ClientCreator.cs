using System;
using Softing.OPCToolbox.Client.DA;
using System.Runtime.InteropServices;

namespace Softing.OPCToolbox.Client
{
	/// <summary>
	/// Creator class : its members are called when a certain instance of a class is created in the toolkit
	/// </summary>
    public class Creator
	{
		#region //Public Methods
		/// <summary>
		/// Creates a new instance of ServerBrowser
		/// </summary>
		virtual public ServerBrowser CreateServerBrowser(string anIpAddress){

			return new OPCToolbox.Client.ServerBrowser(anIpAddress);
		}

		/// <summary>
		/// Creates a new instance of Server
		/// </summary>
		/// <param name="x"></param>
		virtual public Session CreateDASession(){

			return new Softing.OPCToolbox.Client.DA.Session();
		}

		/// <summary>
		/// Creates a new instance of Subscription
		/// </summary>
		/// <returns></returns>
		virtual public Subscription CreateDASubscription(){

			return new Softing.OPCToolbox.Client.DA.Subscription();
		}

		/// <summary>
		/// Creates a new instance of Item
		/// </summary>
		/// <returns></returns>
		virtual public Item CreateDAItem(){

			return new Softing.OPCToolbox.Client.DA.Item();
		}
		#endregion

	}
}
