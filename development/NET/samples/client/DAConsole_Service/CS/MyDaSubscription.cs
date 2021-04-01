using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.IO;

namespace DaConsole_Service
{
	public class MyDaSubscription : DaSubscription
	{

		#region Constructor
		//-----------------

		public MyDaSubscription(uint updateRate, MyDaSession parentSession) : base(updateRate, parentSession)
		{
			DataChanged += new DataChangedEventHandler(HandleDataChanged);
			StateChangeCompleted += new StateChangeEventHandler(HandleStateChanged);
			ReadCompleted += new SubscriptionReadEventHandler(HandleSubscriptionReadCompleted);
			WriteCompleted += new SubscriptionWriteEventHandler(HandleSubscriptionWriteCompleted);
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
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			streamWriter.WriteLine(sender + " State Changed - " + state);
			streamWriter.Flush();
			streamWriter.Close();
		} // end HandleStateChanged


		public static void HandleDataChanged(
			DaSubscription daSubscription,
			DaItem[] items,
			ValueQT[] values,
			int[] results)
		{
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			streamWriter.WriteLine();
			streamWriter.WriteLine(daSubscription);
			for (int i = 0; i < items.Length; i++)
			{
				if (ResultCode.SUCCEEDED(results[i]))
				{
					streamWriter.WriteLine(String.Format("{0,-19} {1} {2,-50} ", items[i].Id, "-", values[i].ToString()));
				}
				else
				{
					streamWriter.WriteLine("Receiving item value failed! " + " Item: " + items[i].Id);
				}
			}
			streamWriter.WriteLine();
			streamWriter.Flush();
			streamWriter.Close();
		} // end HandleDataChanged


		public static void HandleSubscriptionReadCompleted(
			DaSubscription daSubscription,
			uint executionContext,
			DaItem[] items,
			ValueQT[] values,
			int[] results,
			int result)
		{
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			if (ResultCode.SUCCEEDED(result))
			{
				streamWriter.WriteLine(daSubscription + " asynchronous read succeeded! ");

				for (int i = 0; i < values.Length; i++)
				{
					if (ResultCode.SUCCEEDED(results[i]))
					{
						streamWriter.WriteLine(String.Format("{0,-19} {1} {2,-50} ", items[i].Id, "-", values[i].ToString()));
					}
					else
					{
						streamWriter.WriteLine(" Item read failed!" + items[i].Id + " [ResultID: " + results[i] + " ]");
					}
				}
			}
			else
			{
				streamWriter.WriteLine(" Subscription asynchronous read failed! Result: " + result);
			}
			streamWriter.Flush();
			streamWriter.Close();
		} // end HandleSubscriptionReadComplete


		public static void HandleSubscriptionWriteCompleted(
			DaSubscription daSubscription,
			uint executionContext,
			DaItem[] items,
			ValueQT[] values,
			int[] results,
			int result)
		{
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			if (ResultCode.SUCCEEDED(result))
			{
				streamWriter.WriteLine(daSubscription + " asynchronous write succeeded! ");
				for (int i = 0; i < items.Length; i++)
				{
					if (ResultCode.SUCCEEDED(results[i]))
					{
						streamWriter.WriteLine(" " + items[i].Id);
					}
					else
					{
						streamWriter.WriteLine(" aSubscription asynchronous write failed! " + items[i].Id + " [ResultID: " + results[i] + " ]");
					}
				}
			}
			else
			{
				streamWriter.WriteLine(" Subscription asynchronous write failed! Result: " + result);
			}
			streamWriter.Flush();
			streamWriter.Close();
		} // end HandleSubscriptionWriteCompleted


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
				streamWriter.WriteLine(sender + " Performed state transition - " + executionContext);
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

	}   //	end class MyDaItem

}   //	end namespace
