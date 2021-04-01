package daConsole;

import java.util.*;
import com.softing.opc.client.enums.EnumQuality;
import com.softing.opc.client.enums.EnumResultCode;
import com.softing.opc.client.enums.ResultCode;
import java.io.*;

public class Console {

	private static OpcClient m_opcClient = null;
	public static boolean End = false;
	
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
	public static void Usage()
	{

		System.out.println();
		System.out.println("Usage:");
		System.out.println("Use lower case letters for synchronous execution ");
		System.out.println("Use upper case letters for asynchronous execution ");
		System.out.println("Press \'a\' or \'A\' to activate the objects");
		System.out.println("Press \'c\' or \'C\' to connect the objects");
		System.out.println("Press \'d\' or \'D\' to disconnect the objects");
		System.out.println("Press \'r\' or \'R\' to read the items");
		System.out.println("Press \'w\' or \'W\' to write the items");
		System.out.println("Press \'z\' or \'Z\' to read using server object");
		System.out.println("Press \'t\' or \'T\'to write using the server object");
		System.out.println("Press \'s\' or \'S\' to get the server status");
		System.out.println("Press \'m\' to activate the connection monitor");
		System.out.println("Press \'n\' to deactivate the connection monitor");			
		System.out.println("Press \'?\' or \"u\" to display this usage information");
		System.out.println("Press \'e\' or \'q\' or Ctrl-C to exit");
		System.out.println();
	}
	
	public static void main(String[] args){
		try{
			
			int result = EnumResultCode.S_OK.toInt();
			Console console = new Console();
			console.CreateOpcClient();
			OpcClient m_opcClient = console.getOpcClient();
            
			//	initialize the client instance
			if (!ResultCode.SUCCEEDED(m_opcClient.initialize()))
			{				
				m_opcClient = null;	
				return;
			}	//	end if
			
			//	initialize the DA client simulation
			result |= m_opcClient.initializeDaObjects();
			
			Usage();
			
			boolean end = false;
			while (!end && !Console.End)
			{
				char myChar = (char) System.in.read();
				switch (myChar)
				{
					case 'A':
						m_opcClient.activateSession(false);											
						break;
					case 'a': 
						m_opcClient.activateSession(true);
						break;
					case 'C':
						m_opcClient.connectSession(false);					
						break;
					case 'c': 
						m_opcClient.connectSession(true);
						break;
					case 'D':
						m_opcClient.disconnectSession(false);				
						break;
					case 'd': 
						m_opcClient.disconnectSession(true);
						break;
					case 'R':
						m_opcClient.readItems(true);
						break;
					case 'r':
						m_opcClient.readItems(false);			
						break;
					case 'W': 
						m_opcClient.writeItems(true);
						break;
					case 'w':
						m_opcClient.writeItems(false);																
						break;
					case 'Z':
						m_opcClient.readUsingSession(true);
						break;							
					case 'z':
						m_opcClient.readUsingSession(false);							
						break;
					case 'T':
						m_opcClient.writeUsingSession(true);
						break;
					case 't':
						m_opcClient.writeUsingSession(false);
						break;
					case 'S':
						m_opcClient.getServerStatus(true);
						break;
					case 's':
						m_opcClient.getServerStatus(false);
						break;
                    case 'M':
					case 'm':
						m_opcClient.activateConnectionMonitor();
						break;
					case 'N':
					case 'n':
						m_opcClient.deactivateConnectionMonitor();					
						break;							
					case 'U':							
						break;

					case 'u':
					case '?':
						Usage();				
						break;

					case 'E':
					case 'e':
					case 'Q':
					case 'q':
						end = true;							
						break;

					default: 
						Usage();							
						break;
				}
			}// while


			m_opcClient.terminate();
			m_opcClient = null;
		}
		catch(Exception exc)
		{				
			System.out.println(exc.getMessage());
			exc.printStackTrace();
		}	//	end try...catch
	}
}
