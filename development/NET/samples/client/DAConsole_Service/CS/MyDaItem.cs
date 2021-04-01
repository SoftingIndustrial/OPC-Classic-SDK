using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.IO;

namespace DaConsole_Service
{
	public class MyDaItem : DaItem
	{

		#region Constructor
		//-----------------

		public MyDaItem(string itemId, MyDaSubscription parentSubscription) : base(itemId, parentSubscription)
		{
			StateChangeCompleted += new StateChangeEventHandler(HandleStateChanged);
			PerformStateTransitionCompleted += new PerformStateTransitionEventHandler(HandlePerformStateTransition);
		}

		//--
		#endregion

		#region Private Members
		//---------------------

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


		//--
		#endregion

		#region Handles

		public static void HandleStateChanged(ObjectSpaceElement sender, EnumObjectState state)
		{
			MyDaItem item = sender as MyDaItem;
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			streamWriter.WriteLine(item + " " + item.Id + " State Changed - " + state);
			streamWriter.Flush();
			streamWriter.Close();
		} // end HandleStateChanged


		public static void HandlePerformStateTransition(
			ObjectSpaceElement sender,
			uint executionContext,
			int result)
		{
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			if (ResultCode.SUCCEEDED(result))
			{
				MyDaItem item = sender as MyDaItem;
				streamWriter.WriteLine(sender + " " + item.Id + " Performed state transition - " + executionContext);
			}
			else
			{
				streamWriter.WriteLine(sender + "  Performed state transition failed! Result: " + result);
			}
			streamWriter.Flush();
			streamWriter.Close();
		} // end HandlePerformStateTransition


		//--
		#endregion

	}   // end class MyDaItem


}   //	end namespace
