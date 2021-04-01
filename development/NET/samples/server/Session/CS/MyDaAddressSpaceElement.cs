using System;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace Session
{
	public class MyDaAddressSpaceElement : DaAddressSpaceElement
	{

		#region Constructors
		//------------------------------

		public MyDaAddressSpaceElement(
			string anItemID,
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle)
			:
			base(
			anItemID,
			aName,
			anUserData,
			anObjectHandle,
			aParentHandle)
		{
		}   //	end constructor

		public MyDaAddressSpaceElement()
		{
		}   //	end constructor

		//--
		#endregion

		#region Private Attributes
		//-------------------------------

		private Hashtable m_properties = new Hashtable();

		//--
		#endregion

		#region Public Methods
		//---------------------

		/// <summary>
		/// Get elements property value data
		/// </summary>
		public void GetPropertyValue(DaRequest aRequest)
		{
			if (aRequest.PropertyId == (int)EnumPropertyId.ITEM_DESCRIPTION)
			{
				aRequest.Value = new ValueQT(
					"description",
					EnumQuality.GOOD,
					DateTime.Now);

				aRequest.Result = EnumResultCode.S_OK;
			}
			else
			{
				aRequest.Result = EnumResultCode.E_NOTFOUND;
			}   //	end if ... else
		}   //	end GetPropertyValue

		public override int QueryProperties(out ArrayList aPropertyList)
		{
			if (m_properties.Count > 0)
			{
				aPropertyList = new ArrayList();
				aPropertyList.AddRange(m_properties.Values);
			}
			else
			{
				aPropertyList = null;
			}   //	end if ... else
			return (int)(EnumResultCode.S_OK);
		}   //	end QueryProperties


		public int AddProperty(DaProperty aProperty)
		{
			if (aProperty != null)
			{
				m_properties.Add(
					aProperty.Id,
					aProperty);
				return (int)EnumResultCode.S_OK;
			}   //	end if
			else
			{
				return (int)EnumResultCode.S_FALSE;
			}   //	end if...else
		}   //	end AddProperty

		public override int QueryCacheValue(DaSession aSession, ref ValueQT aCacheValue)
		{

			if (IoMode == EnumIoMode.POLL_OWNCACHE)
			{

				MySession session = aSession as MySession;
				if (session != null)
				{
					return session.GetCacheValue(ref aCacheValue);
				}
				else
				{
					return (int)EnumResultCode.E_FAIL;
				}   //	end if ... else				
			}
			else
			{
				return base.GetCacheValue(ref aCacheValue);
			}   //	end if
		}   //	end QueryCacheValue

		public override void Change(bool isActive, int anUpdateRate)
		{
			base.Change(isActive, anUpdateRate);
			Console.m_clientChanged.Set();
		}   //	end Change

		//--
		#endregion

	}   //	end class MyAddressSpaceElement
}   //	end namespace
