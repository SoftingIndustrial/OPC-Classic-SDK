using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Threading;
using Softing.OPCToolbox;

namespace TestServer
{
	/// <summary>
	/// Summary description for OpcForm.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private OutProc m_outProc = null;
		private OpcServer m_opcServer = null;
		public Form1(OutProc anOutProc)
		{
			InitializeComponent();

			try{

				m_outProc = anOutProc;

				m_opcServer = m_outProc.OpcServer;
			}
			catch(Exception exc)
			{
				MessageBox.Show(exc.ToString());
			}	//	end try...catch
		}	//	end constructor

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
			this.SuspendLayout();
			//
			// Form1
			//
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(360, 73);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "Form1";
			this.Text = "COM TestServer";
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main()
		{
			OutProc outProc = new OutProc();
			int result = (int)EnumResultCode.S_OK;
			try{

				//	create and initialize the OpcServer instance
				outProc.CreateOpcServer();
				OpcServer opcServer = outProc.OpcServer;
				opcServer.Initialize();

				MyCreator creator = new MyCreator();
				if (!ResultCode.SUCCEEDED(opcServer.Prepare(creator)))
				{
					opcServer.Terminate();
					opcServer = null;
					return;
				}	//	end if
				//	handle the command line arguments (register/unregister, etc)
				string commandline = Environment.CommandLine;
				result = opcServer.ProcessCommandLine(commandline);

				if (result != (uint)EnumResultCode.S_OK)
				{
					if (result == (uint)EnumResultCode.S_FALSE)
					{
						//registration operation succesful
						opcServer.Trace(
							EnumTraceLevel.INF,
							EnumTraceGroup.USER1,
							"OpcForm::Main",
							"Registration succeeded");
					}
					else
					{
						opcServer.Trace(
							EnumTraceLevel.INF,
							EnumTraceGroup.USER1,
							"OpcForm::Main",
							"Registration failed");
					}	//	end if...else

					//	no matter what close the application if
					//processCommandLine returned something different of S_OK
					opcServer.Terminate();
					opcServer = null;
					return;
				}

				//	start the OPC server's I/O internal mechanism
				if (ResultCode.SUCCEEDED(opcServer.Start()))
				{
					//	build the namespace
					opcServer.BuildAddressSpace();
					//	declare the namespaces built and the server ready for clients to connect
					opcServer.Ready();
				}	//	end if

				Form1 opcForm = new Form1(outProc);

				opcServer.MainForm = opcForm;

				Application.Run(opcForm);


				opcServer.Stop();
				opcServer.Terminate();
				opcServer = null;
			}
			catch(Exception exc)
			{
				MessageBox.Show(exc.ToString());
			}	//	end try...catch
		}	//	end Main
	}	//	ens class
}	//	end namespace
