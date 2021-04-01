package daBrowse;

import java.util.*;

import com.softing.opc.client.api.DaAddressSpaceElement;
import com.softing.opc.client.api.DaAddressSpaceElementBrowseOptions;
import com.softing.opc.client.api.DaGetPropertiesOptions;
import com.softing.opc.client.api.DaProperty;
import com.softing.opc.client.api.ExecutionOptions;
import com.softing.opc.client.enums.EnumAddressSpaceElementType;
import com.softing.opc.client.enums.EnumPropertyData;
import com.softing.opc.client.enums.EnumResultCode;
import com.softing.opc.client.enums.ResultCode;
/* COMMENT */
public class Console {

	private static OpcClient m_opcClient = null;
	
	public OpcClient getOpcClient()
	{
		return m_opcClient;
	}
	
	public void CreateOpcClient()
	{
		if (m_opcClient == null)
		{
			m_opcClient = new OpcClient();
		}
	}
	
	public static void main(String[] args) {    
		try{
			
			int result = EnumResultCode.S_OK.toInt();
			Console console = new Console();
			console.CreateOpcClient();
			OpcClient m_opcClient = console.getOpcClient();
			DaGetPropertiesOptions m_propertyGetOptions = new DaGetPropertiesOptions();
			
			ExecutionOptions m_executionOptions = m_opcClient.GetExecutionOptions();
			
			//	initialize the client instance
			if (!ResultCode.SUCCEEDED(m_opcClient.Initialize()))
			{				
				m_opcClient = null;	
				return;
			}	//	end if
			
			//	initialize the DA client simulation
			result |= m_opcClient.InitializeDaObjects();
			
			//get the session id
			MyDaSession m_session = m_opcClient.GetSession();
			
			//browse first level banches
			DaAddressSpaceElementBrowseOptions browseOptions = new DaAddressSpaceElementBrowseOptions();                    
			browseOptions.setElementTypeFilter(EnumAddressSpaceElementType.BRANCH);
			
			DaAddressSpaceElement rootDaAddressSpaceElement = new DaAddressSpaceElement();
			rootDaAddressSpaceElement.setSession(m_session);	
			ArrayList<DaAddressSpaceElement> addressSpaceElements = new ArrayList<DaAddressSpaceElement>();
			
			//elementToExpand will be used later for next operation (expand firs branch) 
			DaAddressSpaceElement elementToExpand = null;
			DaAddressSpaceElement leafForProperties = null;
			
			if (ResultCode.SUCCEEDED(m_session.browse(
					rootDaAddressSpaceElement.getItemId(), 
					browseOptions,
					addressSpaceElements,
					m_executionOptions)))
				{
					if (addressSpaceElements != null)
					{
						for (int i = 0; i < addressSpaceElements.size(); i++)
						{
							if(i==0)
							{
								elementToExpand = addressSpaceElements.get(i);
							}
							System.out.println(addressSpaceElements.get(i).getItemId() + " - Branch");
						}//end for
					}
				}//end if		
			else
			{
				System.out.println("An error occured while browsing first level branches");
			}
			
			//browse the leaves on the root
			browseOptions.setElementTypeFilter(EnumAddressSpaceElementType.LEAF);
			addressSpaceElements = new ArrayList<DaAddressSpaceElement>();
			
			if (ResultCode.SUCCEEDED(m_session.browse(
					rootDaAddressSpaceElement.getItemId(), 
					browseOptions,
					addressSpaceElements,
					m_executionOptions)))
				{
					if (addressSpaceElements != null)
					{
						for (int i = 0; i < addressSpaceElements.size(); i++)
						{
							System.out.println(addressSpaceElements.get(i).getItemId() + " - Leaf");
						}//end for
					}
				}//end if		
			else
			{
				System.out.println("An error occured while browsing first level leaves");
			}
			
			//get DA properties
			System.out.println();
			System.out.println("Print DA properties:");
			
			ArrayList<DaProperty> daProperties = new ArrayList<DaProperty>();
			m_propertyGetOptions.setWhatPropertyData(EnumPropertyData.ALL);	
			if (ResultCode.SUCCEEDED(rootDaAddressSpaceElement.getDaProperties(						
					m_propertyGetOptions,
					daProperties, 
					m_executionOptions)))
			{
				if (daProperties != null)
				{
					for (int i = 0; i < daProperties.size(); i++)
					{
						
						//add all properties except OPC-specific properties
						if (daProperties.get(i).getId() >= 100)
						{
							System.out.println(daProperties.get(i).getName());
						}
					}
				}
			}
			
			//Expand first branch
			if(elementToExpand != null)
			{
				System.out.println();
				System.out.println("Expand first branch:" + elementToExpand.getItemId());
				
				//child branches
				m_propertyGetOptions.setWhatPropertyData(EnumPropertyData.ALL);
				addressSpaceElements = new ArrayList<DaAddressSpaceElement>();
				browseOptions = new DaAddressSpaceElementBrowseOptions();
				//browseOptions.setElementNameFilter("w*");
				browseOptions.setElementTypeFilter (EnumAddressSpaceElementType.BRANCH);
				if (ResultCode.SUCCEEDED(m_session.browse(
						elementToExpand.getItemId(),
						browseOptions,
						addressSpaceElements,
						m_executionOptions)))
				{
					if (addressSpaceElements != null)
					{
						for (int i = 0; i < addressSpaceElements.size(); i++)
						{
							System.out.println(addressSpaceElements.get(i).getItemId() + " - branch child");
						}//end for
					}
				}//end if		
				
				//leaves
				browseOptions.setElementTypeFilter (EnumAddressSpaceElementType.LEAF);
				if (ResultCode.SUCCEEDED(m_session.browse(
						elementToExpand.getItemId(),
						browseOptions,
						addressSpaceElements,
						m_executionOptions)))
				{
					if (addressSpaceElements != null)
					{
						for (int i = 0; i < addressSpaceElements.size(); i++)
						{
							if(i == 0)
							{
								leafForProperties = addressSpaceElements.get(i);
							}
							System.out.println(addressSpaceElements.get(i).getItemId() + " - leaf child");                                
						}//end for
					}
				}
			}		
			
			//Print DA properties for the first encountered leaf
			if(leafForProperties != null)
			{
				System.out.println();
				System.out.println("DA properties for first leaf:" + leafForProperties.getItemId());
				
				daProperties = new ArrayList<DaProperty>();
				m_propertyGetOptions.setWhatPropertyData(EnumPropertyData.ALL);	
				if (ResultCode.SUCCEEDED(leafForProperties.getDaProperties(						
						m_propertyGetOptions,
						daProperties, 
						m_executionOptions)))
				{
					if (daProperties != null)
					{
						for (int i = 0; i < daProperties.size(); i++)
						{
							
							//add all properties except OPC-specific properties
							//if (daProperties.get(i).getId() >= 100)
							{
								System.out.println(daProperties.get(i).getDescription());
							}
						}
					}
				}
			}
			
			//terminate
			m_opcClient.Terminate();
			m_opcClient = null;
		}
		catch(Exception exc)
		{				
			//System.out.println(exc.getMessage());
			exc.printStackTrace();
		}	//	end try...catch
	}
	
}
