using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Threading;
using Softing.OPCToolbox;

namespace DataControl
{
	/// <summary>
	/// Summary description for OpcForm.
	/// </summary>
	public class OpcForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.TextBox textBox2;
		private System.Windows.Forms.TextBox textBox3;
		private System.Windows.Forms.TextBox textBox4;
		private System.Windows.Forms.TextBox textBox5;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.GroupBox groupBox3;
		private Softing.OPCToolbox.Client.Control.DataControl dataControl1;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private OutProc m_outProc = null;
		private OpcClient m_opcClient = null;
		public OpcForm(OutProc anOutProc)
		{
			InitializeComponent();
			dataControl1.Start();

			try
			{
				m_outProc = anOutProc;

				m_opcClient = m_outProc.OpcClient;
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(OpcForm));
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.textBox4 = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.textBox5 = new System.Windows.Forms.TextBox();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.textBox2 = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.textBox3 = new System.Windows.Forms.TextBox();
			this.button1 = new System.Windows.Forms.Button();
			this.dataControl1 = new Softing.OPCToolbox.Client.Control.DataControl();
			this.groupBox1.SuspendLayout();
			this.groupBox3.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.groupBox3);
			this.groupBox1.Controls.Add(this.groupBox2);
			this.groupBox1.Controls.Add(this.button1);
			this.groupBox1.Font = new System.Drawing.Font("Times New Roman", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(312, 416);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Softing OPC Toolkit Demo OPC DA Server";
			// 
			// groupBox3
			// 
			this.groupBox3.Controls.Add(this.textBox4);
			this.groupBox3.Controls.Add(this.label4);
			this.groupBox3.Controls.Add(this.label5);
			this.groupBox3.Controls.Add(this.textBox5);
			this.groupBox3.Font = new System.Drawing.Font("Times New Roman", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.groupBox3.Location = new System.Drawing.Point(8, 208);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size(296, 152);
			this.groupBox3.TabIndex = 3;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Subscription2";
			// 
			// textBox4
			// 
			this.textBox4.Location = new System.Drawing.Point(16, 48);
			this.textBox4.Name = "textBox4";
			this.textBox4.Size = new System.Drawing.Size(256, 22);
			this.textBox4.TabIndex = 10;
			this.textBox4.Text = "";
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(8, 24);
			this.label4.Name = "label4";
			this.label4.TabIndex = 5;
			this.label4.Text = "maths.sin";
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(8, 80);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(104, 24);
			this.label5.TabIndex = 6;
			this.label5.Text = "time.local.second";
			// 
			// textBox5
			// 
			this.textBox5.Location = new System.Drawing.Point(16, 104);
			this.textBox5.Name = "textBox5";
			this.textBox5.Size = new System.Drawing.Size(256, 22);
			this.textBox5.TabIndex = 11;
			this.textBox5.Text = "";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.textBox1);
			this.groupBox2.Controls.Add(this.label1);
			this.groupBox2.Controls.Add(this.label2);
			this.groupBox2.Controls.Add(this.textBox2);
			this.groupBox2.Controls.Add(this.label3);
			this.groupBox2.Controls.Add(this.textBox3);
			this.groupBox2.Font = new System.Drawing.Font("Times New Roman", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.groupBox2.Location = new System.Drawing.Point(8, 24);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(296, 176);
			this.groupBox2.TabIndex = 2;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Subscription1";
			// 
			// textBox1
			// 
			this.textBox1.Location = new System.Drawing.Point(16, 40);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(256, 22);
			this.textBox1.TabIndex = 7;
			this.textBox1.Text = "";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 24);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(120, 16);
			this.label1.TabIndex = 2;
			this.label1.Text = "increment.UI1";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(8, 64);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(128, 23);
			this.label2.TabIndex = 3;
			this.label2.Text = "increment.DATE";
			// 
			// textBox2
			// 
			this.textBox2.Location = new System.Drawing.Point(16, 88);
			this.textBox2.Name = "textBox2";
			this.textBox2.Size = new System.Drawing.Size(256, 22);
			this.textBox2.TabIndex = 8;
			this.textBox2.Text = "";
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(8, 112);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(128, 23);
			this.label3.TabIndex = 4;
			this.label3.Text = "increment.BSTR array";
			// 
			// textBox3
			// 
			this.textBox3.Location = new System.Drawing.Point(16, 136);
			this.textBox3.Name = "textBox3";
			this.textBox3.Size = new System.Drawing.Size(256, 22);
			this.textBox3.TabIndex = 9;
			this.textBox3.Text = "";
			// 
			// button1
			// 
			this.button1.Font = new System.Drawing.Font("Times New Roman", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.button1.Location = new System.Drawing.Point(32, 376);
			this.button1.Name = "button1";
			this.button1.TabIndex = 0;
			this.button1.Text = "Bind";
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// dataControl1
			// 
			this.dataControl1.Capacity = 16;
			this.dataControl1.BinaryLicenseDa = "";
			this.dataControl1.BinaryLicenseXmlDa = "";
			this.dataControl1.Session = ((Softing.OPCToolbox.Client.Control.ControlDaSession)(resources.GetObject("dataControl1.Session")));

			// 
			// OpcForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(328, 430);
			this.Controls.Add(this.groupBox1);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "OpcForm";
			this.Text = "DataControl Demo";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.OpcForm_Closing);
			this.groupBox1.ResumeLayout(false);
			this.groupBox3.ResumeLayout(false);
			this.groupBox2.ResumeLayout(false);
			this.ResumeLayout(false);			
		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main() 
		{
			OutProc outProc = new OutProc();

			try{
				outProc.CreateOpcClient();
				OpcClient opcClient = outProc.OpcClient;

				OpcForm opcForm = new OpcForm(outProc);

				Application.Run(opcForm);

				opcClient = null;
			}
			catch(Exception exc)
			{
				MessageBox.Show(exc.ToString());
			}	//	end try...catch

		}	//	end Main

		private void button1_Click(object sender, System.EventArgs e) 
		{
			
			button1.Enabled = false;

			try
			{			
				textBox1.DataBindings.Add(new Binding("Text",dataControl1.GetItem("Subscription1","increment.UI1"),"ValueToString"));
				textBox2.DataBindings.Add(new Binding("Text",dataControl1.GetItem("Subscription1","increment.DATE"),"ValueToString"));
				textBox3.DataBindings.Add(new Binding("Text",dataControl1.GetItem("Subscription1","increment.BSTR array"),"ValueToString"));
				textBox4.DataBindings.Add(new Binding("Text",dataControl1.GetItem("Subscription2","maths.sin"),"ValueToString"));
				textBox5.DataBindings.Add(new Binding("Text",dataControl1.GetItem("Subscription2","time.local.second"),"ValueToString"));
			}
			catch (Exception exc)
			{
				MessageBox.Show(exc.ToString());
			}           
		}

		private void OpcForm_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			if (dataControl1 != null)
			{
				dataControl1.Stop();
			}   //  end if
		}  

	}	//	end class
}	//	end namespace
