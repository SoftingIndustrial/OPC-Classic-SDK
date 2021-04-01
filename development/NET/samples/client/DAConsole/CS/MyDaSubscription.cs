using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace DaConsole
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
			System.Console.WriteLine(sender + " State Changed - " + state);
		} // end HandleStateChanged


		public static void HandleDataChanged(
			DaSubscription daSubscription,
			DaItem[] items,
			ValueQT[] values,
			int[] results)
		{
			System.Console.WriteLine();
			System.Console.WriteLine(daSubscription);
			for (int i = 0; i < items.Length; i++)
			{
				if (ResultCode.SUCCEEDED(results[i]))
				{
					System.Console.WriteLine(String.Format("{0,-19} {1} {2,-50} ", items[i].Id, "-", values[i].ToString()));
				}
				else
				{
					System.Console.WriteLine("Receiving item value failed! " + " Item: " + items[i].Id);
				}
			}
			System.Console.WriteLine();
		} // end HandleDataChanged


		public static void HandleSubscriptionReadCompleted(
			DaSubscription daSubscription,
			uint executionContext,
			DaItem[] items,
			ValueQT[] values,
			int[] results,
			int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine(daSubscription + " asynchronous read succeeded! ");

				for (int i = 0; i < values.Length; i++)
				{
					if (ResultCode.SUCCEEDED(results[i]))
					{
						System.Console.WriteLine(String.Format("{0,-19} {1} {2,-50} ", items[i].Id, "-", values[i].ToString()));
					}
					else
					{
						System.Console.WriteLine(" Item read failed!" + items[i].Id + " [ResultID: " + results[i] + " ]");
					}
				}
			}
			else
			{
				System.Console.WriteLine(" Subscription asynchronous read failed! Result: " + result);
			}
		} // end HandleSubscriptionReadComplete


		public static void HandleSubscriptionWriteCompleted(
			DaSubscription daSubscription,
			uint executionContext,
			DaItem[] items,
			ValueQT[] values,
			int[] results,
			int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine(daSubscription + " asynchronous write succeeded! ");
				for (int i = 0; i < items.Length; i++)
				{
					if (ResultCode.SUCCEEDED(results[i]))
					{
						System.Console.WriteLine(" " + items[i].Id);
					}
					else
					{
						System.Console.WriteLine(" aSubscription asynchronous write failed! " + items[i].Id + " [ResultID: " + results[i] + " ]");
					}
				}
			}
			else
			{
				System.Console.WriteLine(" Subscription asynchronous write failed! Result: " + result);
			}
		} // end HandleSubscriptionWriteCompleted


		public static void HandlePerformStateTransition(
			ObjectSpaceElement sender,
			uint executionContext,
			int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine(sender + " Performed state transition - " + executionContext);
			}
			else
			{
				System.Console.WriteLine(sender + "  Performed state transition failed! Result: " + result);
			}
		} // end HandlePerformStateTransition

		//--
		#endregion

	}   //	end class MyDaItem

}   //	end namespace
