using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace TestClientGui.Elements
{
    public class DaSubscribeChildItemsDA2 : Element
    {
        #region Private Fields
        DaAddressSpaceElementBrowseOptions m_browseOptions;
        System.Threading.AutoResetEvent m_waitHandle;
        private uint m_asyncExecutionContext;
        Softing.OPCToolbox.Client.DaAddressSpaceElement[] m_browseAsyncResults = null;
        int m_browseAsyncResult;
        #endregion

        #region Public Properties

        public string Session { get; set; }

        public string RootID { get; set; }

        public string RootPath { get; set; }

        [TypeConverter(typeof(ExpandableObjectConverter))]
        public DaAddressSpaceElementBrowseOptions BrowseOptions
        {
            get
            {
                return m_browseOptions;
            }

            set
            {
                m_browseOptions = value;
            }
        }

        //[DefaultValue(1000)]
        public uint updateRate { get; set; }

        public string Property { get; set; }

        [DefaultValue(true)]
        public bool ExpectedSuccess { get; set; }
        #endregion

        public DaSubscribeChildItemsDA2()
        {
            ExpectedSuccess = true;
            updateRate = 1000;
        }

        public override TestResult Execute()
        {
            if (string.IsNullOrEmpty(Session))
                return TestResult.ParameterMissing("Session");
            if (string.IsNullOrEmpty(RootID))
                return TestResult.ParameterMissing("RootID");
            //	if (string.IsNullOrEmpty(RootPath))
            //	return TestResult.ParameterMissing("RootPath");


            Softing.OPCToolbox.Client.DaSession session = GetProperty(Session) as DaSession;
            if (session == null)

                return TestResult.Failed("Session non existent");

            try
            {
                ExecutionOptions executionOptions = new ExecutionOptions(EnumExecutionType.SYNCHRONOUS, 0);
                session.Connect(false, false, executionOptions);
                
                Softing.OPCToolbox.Client.DaAddressSpaceElement[] elements;

                Softing.OPCToolbox.Client.DaAddressSpaceElement rootDaAddressSpaceElement = new Softing.OPCToolbox.Client.DaAddressSpaceElement(
                EnumAddressSpaceElementType.BRANCH, String.Empty, string.Empty, string.Empty, 0, null);
                rootDaAddressSpaceElement.Session = session;


                int result = rootDaAddressSpaceElement.Browse(new Softing.OPCToolbox.Client.DaAddressSpaceElementBrowseOptions(), out elements, new ExecutionOptions(EnumExecutionType.SYNCHRONOUS, m_asyncExecutionContext));

                if (!ResultCode.SUCCEEDED(result))
                {
                    EnumResultCode resCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), result.ToString());

                    return TestResult.Failed(string.Format("Call returned: {0}  ({1})", resCode.ToString(), result));
                }

                Softing.OPCToolbox.Client.DaAddressSpaceElement addressSpaceElement = null;
                for (int i = 0; i < elements.Length; i++)
                {
                    if (elements[i].ItemId == RootID)
                    {
                        addressSpaceElement = elements[i];
                        break;
                    }
                }

                if (addressSpaceElement == null)
                    return TestResult.Failed(string.Format("Address space element {0} not found!", RootID));

                if (BrowseOptions == null)
                    BrowseOptions = new DaAddressSpaceElementBrowseOptions();


                
                
                
                
                                
                
                if (ResultCode.SUCCEEDED(addressSpaceElement.Browse( BrowseOptions.Wrapped, out elements, new ExecutionOptions( EnumExecutionType.SYNCHRONOUS , 0 ) )))
                {
                    //Softing.OPCToolbox.Client.DaAddressSpaceElement daElement;
                    Softing.OPCToolbox.Client.DaSubscription workerSubscription = new Softing.OPCToolbox.Client.DaSubscription(updateRate, session);

                    foreach (Softing.OPCToolbox.Client.DaAddressSpaceElement daElement in elements)
                    {
                        DaItem newItem = new DaItem(daElement.ItemId, workerSubscription);
                        //	result =  Connect(true, true, executionOptions);
                    }

                    result = workerSubscription.Connect(true, true, executionOptions);
                    if (ResultCode.FAILED(result))
                    {
                        EnumResultCode resCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), result.ToString());

                        return TestResult.Failed(string.Format("Subscribe Call returned: {0}  ({1})", resCode.ToString(), result));
                    }

                    SetProperty(Property, workerSubscription);

                }//end if
                else
                {
                    EnumResultCode resCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), result.ToString());

                    return TestResult.Failed(string.Format(" Browse Call returned: {0}  ({1})", resCode.ToString(), result));
                }


            }
            catch (Exception ex)
            {
                if (ExpectedSuccess)
                    return new TestResult(ex);
            }

            return TestResult.Success();
        }
    }
}
