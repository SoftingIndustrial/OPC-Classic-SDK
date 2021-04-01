using System;
using Softing.OPCToolbox.Server;

namespace DemoServer
{
	public class DemoCreator : Creator
	{
		#region Public Methods
		//---------------------

		public override AeAddressSpaceElement CreateInternalAeAddressSpaceElement(
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle)
		{
			return new DemoAeAddressSpaceElement(
				aName,
				anUserData,
				anObjectHandle,
				aParentHandle);
		}   //	end CreateAeAddressSpaceElement

		public override DaAddressSpaceRoot CreateDaAddressSpaceRoot()
		{
			return (DaAddressSpaceRoot)new DemoDaAddressSpaceRoot();
		}   //	end CreateDaAddressSpaceRoot

		public override DaTransaction CreateTransaction(
			EnumTransactionType transactionType,
			DaRequest[] requestList,
			uint sessionKey)
		{
			return (DaTransaction)new Transaction(
									transactionType,
									requestList,
									sessionKey);
		}   //	end CreateTransaction

		public virtual DemoDaAddressSpaceElement CreateBasicElement(String name)
		{
			return (DemoDaAddressSpaceElement)new BasicElement(name);
		}   //	end CreateBasicElement

		public virtual DemoDaAddressSpaceElement CreateBasicStaticElement(String name)
		{
			return (DemoDaAddressSpaceElement)new BasicStaticElement(name);
		}   //  end CreateBasicStaticElement

		public virtual DemoDaAddressSpaceElement CreateBasicStaticElement(System.Type type)
		{
			return (DemoDaAddressSpaceElement)new BasicStaticElement(type);
		}   //  end CreateBasicStaticElement

		public virtual DemoDaAddressSpaceElement CreateBasicDynamicElement(String name)
		{
			return (DemoDaAddressSpaceElement)new BasicDynamicElement(name);
		}   //  end CreateBasicDynamicElement

		public virtual DemoDaAddressSpaceElement CreateBasicDynamicElement(System.Type type)
		{
			return (DemoDaAddressSpaceElement)new BasicDynamicElement(type);
		}   //  end CreateBasicDynamicElement

		public virtual DemoDaAddressSpaceElement CreateSinusElement()
		{
			return (DemoDaAddressSpaceElement)new SinFunction();
		}   //  end CreateSinusElement

		public virtual DemoDaAddressSpaceElement CreateCosinusElement()
		{
			return (DemoDaAddressSpaceElement)new CosFunction();
		}   //  end CreateCosinusElement

		public virtual DemoDaAddressSpaceElement CreateTanElement()
		{
			return (DemoDaAddressSpaceElement)new TanFunction();
		}   //  end CreateTanElement

		public virtual DemoDaAddressSpaceElement CreateSimulationElement()
		{
			return (DemoDaAddressSpaceElement)new SimulationVariable();
		}   //  end CreateSimulationElement

		public virtual DemoDaAddressSpaceElement CreateSleepIntervalElement()
		{
			return (DemoDaAddressSpaceElement)new SleepIntervalVariable();
		}   //  end CreateSleepIntervalElement

		public virtual DemoDaAddressSpaceElement CreateTimeElement(String name)
		{
			return (DemoDaAddressSpaceElement)new TimeVariable(name);
		}   //  end CreateTimeElement

		public virtual DemoDaAddressSpaceElement CreateTimeElement(
			String name,
			TimeVariable.TimeType type,
			TimeVariable.TimeZone zone)
		{
			return (DemoDaAddressSpaceElement)new TimeVariable(name, type, zone);
		}   //  end CreateTimeElement

		public virtual DemoDaAddressSpaceElement CreateAlarmSimulationElement(
			String name,
			AlarmSimulation.AlarmType type)
		{
			return (DemoDaAddressSpaceElement)new AlarmSimulation(name, type);
		}   //  end CreateAlarmSimulationElement

		public virtual DemoDaAddressSpaceElement CreateKeyElement()
		{
			return (DemoDaAddressSpaceElement)new KeyVariable();
		}   //  end CreateKeyElement

		public virtual DemoAeAddressSpaceElement CreateDemoAeAddressSpaceElement()
		{
			return new DemoAeAddressSpaceElement();
		}   //  end CreateDemoAeAddressSpaceElement

		public override DaRequest CreateRequest(
			EnumTransactionType aTransactionType,
			uint aSessionHandle,
			DaAddressSpaceElement anElement,
			int aPropertyId,
			uint aRequestHandle)
		{
			return new Request(
				aTransactionType,
				aSessionHandle,
				anElement,
				aPropertyId,
				aRequestHandle);
		}   //	end CreateRequest
			//--
		#endregion

	}   //	end class MyCreator
}
