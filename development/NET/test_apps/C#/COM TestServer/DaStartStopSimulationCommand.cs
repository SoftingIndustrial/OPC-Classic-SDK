using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;
using System.Timers;

namespace TestServer
{
	/// <summary>
	/// Used to manage a Start/Stop Simulation command
	/// </summary>
	class DaStartStopSimulationCommand :DaAddressSpaceElement
	{
		#region Constructors

		public DaStartStopSimulationCommand(
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
		}

		public DaStartStopSimulationCommand()
		{
			Datatype = typeof(bool);
			AccessRights = EnumAccessRights.READWRITEABLE;
			IoMode = EnumIoMode.POLL;
			RemovedFromAddressSpace += new RemovedFromAddressSpace(DisposeCommand);
		}

		#endregion

		#region Public Properties

		public static string ChangeIntervalParam = "ChangeInterval";
		public static string RepeatCountParam	= "RepeatCount";
		public static string IncrementParam	  = "Increment";

		public static Type ChangeIntervalParamType = typeof(uint);
		public static Type RepeatCountParamType	= typeof(uint);
		public static Type IncrementParamType	  = typeof(uint);

		#endregion

		#region Public Methods

		public EnumResultCode StartSimulation()
		{
			try
			{
				// Get children
				ArrayList children = GetChildren();

				uint changeInterval = 0;
				uint repeatCount = 0;
				uint increment = 0;

				// get parameter values
				foreach (DaAddressSpaceElement param in children)
				{
					if (param == null)
					{
						continue;
					}

					ValueQT paramValue = null;
					param.GetCacheValue(ref paramValue);

					if (param.Name == ChangeIntervalParam)
					{
						changeInterval = (uint)paramValue.Data;
					}
					else if (param.Name == RepeatCountParam)
					{
						repeatCount = (uint)paramValue.Data;
					}
					else if (param.Name == IncrementParam)
					{
						increment = (uint)paramValue.Data;
					}
				}

				// Check if all parameters are set
				if (changeInterval != 0 && increment != 0)
				{
					// set simulation parameters
					m_simulationInterval = changeInterval;
					m_repeatCount = repeatCount;
					m_increment = increment;
					m_continousSimulation = (repeatCount == 0);

					// Set timer
					if (m_simulationTimer == null)
					{
						m_simulationTimer = new Timer();
						m_simulationTimer.Elapsed += new ElapsedEventHandler(SimulateValues);
					}

					// Start simulation
					m_simulationTimer.Stop();
					m_simulationTimer.Interval = (double)m_simulationInterval;
					m_simulationTimer.Start();
				}
				else
				{
					// Parameters not set correctly
					return EnumResultCode.E_FAIL;
				}

				return EnumResultCode.S_OK;
			}
			catch
			{
				return EnumResultCode.E_FAIL;
			}
		}

		public EnumResultCode StopSimulation()
		{
			try
			{
				// Stop items simulation
				if (m_simulationTimer != null)
				{
					// Destroy the simulation timer
					m_simulationTimer.Stop();
					m_simulationTimer.Elapsed -= new ElapsedEventHandler(SimulateValues);
					m_simulationTimer.Dispose();
					m_simulationTimer = null;

					m_continousSimulation = false;
				}

				return EnumResultCode.S_OK;
			}
			catch
			{
				return EnumResultCode.E_FAIL;
			}
		}

		#endregion

		#region Private Methods

		/// <summary>
		/// Simulate a value change for the items
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void SimulateValues(object sender, ElapsedEventArgs e)
		{
			try
			{
				if (m_repeatCount > 0 || m_continousSimulation)
				{
					// Retreive parent folder
					AddressSpaceElement parentFolder = GetParent();

					// Get children
					ArrayList simulationItems = parentFolder.GetChildren();

					// increment values
					foreach (DaAddressSpaceElement item in simulationItems)
					{
						// filter only the MyDaAddressSpaceElement items
						MyDaAddressSpaceElement daItem = item as MyDaAddressSpaceElement;

						if (daItem == null)
						{
							continue;
						}

						IncrementItemValue(daItem);
					}

					if (!m_continousSimulation)
					{
						m_repeatCount--;
					}
				}
				else
				{
					StopSimulation();
				}
			}
			catch
			{
				// stop simulation
				m_repeatCount = 0;
			}
		}

		// Return the incremented value for a ValueQT
		private int IncrementItemValue(MyDaAddressSpaceElement item)
		{
			// get actual value
			ValueQT itemValue = null;
			item.GetCacheValue(ref itemValue);
			Type dataType =  itemValue.Data.GetType();
			object newValue = null;

			if (dataType == typeof(bool))   newValue = !(bool)itemValue.Data;
			if (dataType == typeof(Byte))   newValue = (Byte)((Byte)itemValue.Data + m_increment);
			if (dataType == typeof(SByte))  newValue = (SByte)((SByte)itemValue.Data + m_increment);
			if (dataType == typeof(UInt16)) newValue = (UInt16)((UInt16)itemValue.Data + m_increment);
			if (dataType == typeof(Int16))  newValue = (Int16)((Int16)itemValue.Data + m_increment);
			if (dataType == typeof(UInt32)) newValue = (UInt32)((UInt32)itemValue.Data + m_increment);
			if (dataType == typeof(Int32))  newValue = (Int32)((Int32)itemValue.Data + m_increment);
			if (dataType == typeof(UInt64)) newValue = (UInt64)((UInt64)itemValue.Data + m_increment);
			if (dataType == typeof(Int64))  newValue = (Int64)((Int64)itemValue.Data + m_increment);
			if (dataType == typeof(Single)) newValue = (Single)((Single)itemValue.Data + m_increment);
			if (dataType == typeof(Double)) newValue = (Double)((Double)itemValue.Data + m_increment);

			// apply new value
			return item.ValueChanged(new ValueQT(newValue, EnumQuality.GOOD, DateTime.Now));
		}

		private void DisposeCommand()
		{
			// destroy simulation timer
			StopSimulation();
		}

		#endregion

		#region Private Fields

		private Timer m_simulationTimer;
		private uint m_simulationInterval = 0;
		private uint m_repeatCount = 0;
		private uint m_increment = 0;
		private bool m_continousSimulation = false;


		#endregion
	}
}
