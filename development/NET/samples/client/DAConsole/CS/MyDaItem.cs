using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace DaConsole
{
	public class MyDaItem : DaItem
	{

		#region Constructor
		//-----------------

		public MyDaItem(string itemId, MyDaSubscription parentSubscription) : base(itemId, parentSubscription)
		{
			ValueChanged += new ValueChangedEventHandler(HandleValueChanged);
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
			System.Console.WriteLine(item + " " + item.Id + " State Changed - " + state);
		} // end HandleStateChanged


		public static void HandleValueChanged(DaItem aDaItem, ValueQT aValue)
		{
			if (aDaItem.Id == "maths.sin")
			{
				System.Console.WriteLine("Value changed!");
				System.Console.WriteLine(String.Format("{0,-19} {1} {2,-50} ", aDaItem.Id, "-", aValue.ToString()));
			}
		} // end HandleValueChanged


		public static void HandlePerformStateTransition(
			ObjectSpaceElement sender,
			uint executionContext,
			int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				MyDaItem item = sender as MyDaItem;
				System.Console.WriteLine(sender + " " + item.Id + " Performed state transition - " + executionContext);
			}
			else
			{
				System.Console.WriteLine(sender + "  Performed state transition failed! Result: " + result);
			}
		} // end HandlePerformStateTransition


		//--
		#endregion

	}   // end class MyDaItem


}   //	end namespace