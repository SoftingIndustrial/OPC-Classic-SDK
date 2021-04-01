Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports System.Threading
Imports Softing.OPCToolbox
Namespace DataControl
  ''' <summary>
  ''' Summary description for OpcForm.
  ''' </summary>
  Public Class OpcForm
    Inherits System.Windows.Forms.Form
    Private groupBox1 As System.Windows.Forms.GroupBox
    Private button1 As System.Windows.Forms.Button
    Private label1 As System.Windows.Forms.Label
    Private label2 As System.Windows.Forms.Label
    Private label3 As System.Windows.Forms.Label
    Private label4 As System.Windows.Forms.Label
    Private label5 As System.Windows.Forms.Label
    Private textBox1 As System.Windows.Forms.TextBox
    Private textBox2 As System.Windows.Forms.TextBox
    Private textBox3 As System.Windows.Forms.TextBox
    Private textBox4 As System.Windows.Forms.TextBox
    Private textBox5 As System.Windows.Forms.TextBox
    Private groupBox2 As System.Windows.Forms.GroupBox
    Private groupBox3 As System.Windows.Forms.GroupBox
    Private dataControl1 As Softing.OPCToolbox.Client.Control.DataControl
    ''' <summary>
    ''' Required designer variable.
    ''' </summary>
    Private components As System.ComponentModel.Container = Nothing
    Private m_outProc As OutProc = Nothing
    Private m_opcClient As OpcClient = Nothing
    Public Sub New(ByVal anOutProc As OutProc)
      InitializeComponent()
      dataControl1.Start()

      Try
        m_outProc = anOutProc

        m_opcClient = m_outProc.OpcClient
      Catch exc As Exception
        MessageBox.Show(exc.ToString())
      End Try
    End Sub

    ''' <summary>
    ''' Clean up any resources being used.
    ''' </summary>
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
      If disposing Then
        If Not components Is Nothing Then
          components.Dispose()
        End If
      End If
      MyBase.Dispose(disposing)
    End Sub

#Region "Windows Form Designer generated code"
    ''' <summary>
    ''' Required method for Designer support - do not modify
    ''' the contents of this method with the code editor.
    ''' </summary>
    Private Sub InitializeComponent()
      Dim resources As New System.Resources.ResourceManager(GetType(OpcForm))
      Me.groupBox1 = New System.Windows.Forms.GroupBox()
      Me.groupBox3 = New System.Windows.Forms.GroupBox()
      Me.textBox4 = New System.Windows.Forms.TextBox()
      Me.label4 = New System.Windows.Forms.Label()
      Me.label5 = New System.Windows.Forms.Label()
      Me.textBox5 = New System.Windows.Forms.TextBox()
      Me.groupBox2 = New System.Windows.Forms.GroupBox()
      Me.textBox1 = New System.Windows.Forms.TextBox()
      Me.label1 = New System.Windows.Forms.Label()
      Me.label2 = New System.Windows.Forms.Label()
      Me.textBox2 = New System.Windows.Forms.TextBox()
      Me.label3 = New System.Windows.Forms.Label()
      Me.textBox3 = New System.Windows.Forms.TextBox()
      Me.button1 = New System.Windows.Forms.Button()
      Me.dataControl1 = New Softing.OPCToolbox.Client.Control.DataControl()
      Me.groupBox1.SuspendLayout()
      Me.groupBox3.SuspendLayout()
      Me.groupBox2.SuspendLayout()
      Me.SuspendLayout()
      ' 
      ' groupBox1
      ' 
      Me.groupBox1.Controls.Add(Me.groupBox3)
      Me.groupBox1.Controls.Add(Me.groupBox2)
      Me.groupBox1.Controls.Add(Me.button1)
			Me.groupBox1.Font = New System.Drawing.Font("Times New Roman", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, Convert.ToByte(0))
      Me.groupBox1.Location = New System.Drawing.Point(8, 8)
      Me.groupBox1.Name = "groupBox1"
      Me.groupBox1.Size = New System.Drawing.Size(312, 416)
      Me.groupBox1.TabIndex = 0
      Me.groupBox1.TabStop = False
      Me.groupBox1.Text = "Softing OPC Toolkit Demo OPC DA Server"
      ' 
      ' groupBox3
      ' 
      Me.groupBox3.Controls.Add(Me.textBox4)
      Me.groupBox3.Controls.Add(Me.label4)
      Me.groupBox3.Controls.Add(Me.label5)
      Me.groupBox3.Controls.Add(Me.textBox5)
			Me.groupBox3.Font = New System.Drawing.Font("Times New Roman", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, Convert.ToByte(0))
      Me.groupBox3.Location = New System.Drawing.Point(8, 208)
      Me.groupBox3.Name = "groupBox3"
      Me.groupBox3.Size = New System.Drawing.Size(296, 152)
      Me.groupBox3.TabIndex = 3
      Me.groupBox3.TabStop = False
      Me.groupBox3.Text = "Subscription2"
      ' 
      ' textBox4
      ' 
      Me.textBox4.Location = New System.Drawing.Point(16, 48)
      Me.textBox4.Name = "textBox4"
      Me.textBox4.Size = New System.Drawing.Size(256, 22)
      Me.textBox4.TabIndex = 10
      Me.textBox4.Text = ""
      ' 
      ' label4
      ' 
      Me.label4.Location = New System.Drawing.Point(8, 24)
      Me.label4.Name = "label4"
      Me.label4.TabIndex = 5
      Me.label4.Text = "maths.sin"
      ' 
      ' label5
      ' 
      Me.label5.Location = New System.Drawing.Point(8, 80)
      Me.label5.Name = "label5"
      Me.label5.Size = New System.Drawing.Size(104, 24)
      Me.label5.TabIndex = 6
      Me.label5.Text = "time.local.second"
      ' 
      ' textBox5
      ' 
      Me.textBox5.Location = New System.Drawing.Point(16, 104)
      Me.textBox5.Name = "textBox5"
      Me.textBox5.Size = New System.Drawing.Size(256, 22)
      Me.textBox5.TabIndex = 11
      Me.textBox5.Text = ""
      ' 
      ' groupBox2
      ' 
      Me.groupBox2.Controls.Add(Me.textBox1)
      Me.groupBox2.Controls.Add(Me.label1)
      Me.groupBox2.Controls.Add(Me.label2)
      Me.groupBox2.Controls.Add(Me.textBox2)
      Me.groupBox2.Controls.Add(Me.label3)
      Me.groupBox2.Controls.Add(Me.textBox3)
			Me.groupBox2.Font = New System.Drawing.Font("Times New Roman", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, Convert.ToByte(0))
      Me.groupBox2.Location = New System.Drawing.Point(8, 24)
      Me.groupBox2.Name = "groupBox2"
      Me.groupBox2.Size = New System.Drawing.Size(296, 176)
      Me.groupBox2.TabIndex = 2
      Me.groupBox2.TabStop = False
      Me.groupBox2.Text = "Subscription1"
      ' 
      ' textBox1
      ' 
      Me.textBox1.Location = New System.Drawing.Point(16, 40)
      Me.textBox1.Name = "textBox1"
      Me.textBox1.Size = New System.Drawing.Size(256, 22)
      Me.textBox1.TabIndex = 7
      Me.textBox1.Text = ""
      ' 
      ' label1
      ' 
      Me.label1.Location = New System.Drawing.Point(8, 24)
      Me.label1.Name = "label1"
      Me.label1.Size = New System.Drawing.Size(120, 16)
      Me.label1.TabIndex = 2
      Me.label1.Text = "increment.UI1"
      ' 
      ' label2
      ' 
      Me.label2.Location = New System.Drawing.Point(8, 64)
      Me.label2.Name = "label2"
      Me.label2.Size = New System.Drawing.Size(128, 23)
      Me.label2.TabIndex = 3
      Me.label2.Text = "increment.DATE"
      ' 
      ' textBox2
      ' 
      Me.textBox2.Location = New System.Drawing.Point(16, 88)
      Me.textBox2.Name = "textBox2"
      Me.textBox2.Size = New System.Drawing.Size(256, 22)
      Me.textBox2.TabIndex = 8
      Me.textBox2.Text = ""
      ' 
      ' label3
      ' 
      Me.label3.Location = New System.Drawing.Point(8, 112)
      Me.label3.Name = "label3"
      Me.label3.Size = New System.Drawing.Size(128, 23)
      Me.label3.TabIndex = 4
      Me.label3.Text = "increment.BSTR array"
      ' 
      ' textBox3
      ' 
      Me.textBox3.Location = New System.Drawing.Point(16, 136)
      Me.textBox3.Name = "textBox3"
      Me.textBox3.Size = New System.Drawing.Size(256, 22)
      Me.textBox3.TabIndex = 9
      Me.textBox3.Text = ""
      ' 
      ' button1
      ' 
			Me.button1.Font = New System.Drawing.Font("Times New Roman", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, Convert.ToByte(0))
      Me.button1.Location = New System.Drawing.Point(32, 376)
      Me.button1.Name = "button1"
      Me.button1.TabIndex = 0
      Me.button1.Text = "Bind"
      AddHandler Me.button1.Click, AddressOf Me.button1_Click
      ' 
      ' dataControl1
      ' 
      Me.dataControl1.Capacity = 16
      Me.dataControl1.BinaryLicenseDa = ""
      Me.dataControl1.BinaryLicenseXmlDa = ""
      Me.dataControl1.Session = (DirectCast((resources.GetObject("dataControl1.Session")), Softing.OPCToolbox.Client.Control.ControlDaSession))

      ' 
      ' OpcForm
      ' 
      Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
      Me.ClientSize = New System.Drawing.Size(328, 430)
      Me.Controls.Add(Me.groupBox1)
      Me.Icon = (DirectCast((resources.GetObject("$this.Icon")), System.Drawing.Icon))
      Me.Name = "OpcForm"
      Me.Text = "DataControl Demo"
      AddHandler Me.Closing, AddressOf Me.OpcForm_Closing
      Me.groupBox1.ResumeLayout(False)
      Me.groupBox3.ResumeLayout(False)
      Me.groupBox2.ResumeLayout(False)
      Me.ResumeLayout(False)
    End Sub
#End Region

    ''' <summary>
    ''' The main entry point for the application.
    ''' </summary>
    Shared Sub Main()
      Dim outProc As New OutProc()

      Try
        outProc.CreateOpcClient()
        Dim opcClient As OpcClient = outProc.OpcClient

        Dim opcForm As New OpcForm(outProc)

        Application.Run(opcForm)

        opcClient = Nothing
      Catch exc As Exception
        MessageBox.Show(exc.ToString())
      End Try

    End Sub

    Private Sub button1_Click(ByVal sender As Object, ByVal e As System.EventArgs)

      button1.Enabled = False

      Try
        textBox1.DataBindings.Add(New Binding("Text", dataControl1.GetItem("Subscription1", "increment.UI1"), "ValueToString"))
        textBox2.DataBindings.Add(New Binding("Text", dataControl1.GetItem("Subscription1", "increment.DATE"), "ValueToString"))
        textBox3.DataBindings.Add(New Binding("Text", dataControl1.GetItem("Subscription1", "increment.BSTR array"), "ValueToString"))
        textBox4.DataBindings.Add(New Binding("Text", dataControl1.GetItem("Subscription2", "maths.sin"), "ValueToString"))
        textBox5.DataBindings.Add(New Binding("Text", dataControl1.GetItem("Subscription2", "time.local.second"), "ValueToString"))
      Catch exc As Exception
        MessageBox.Show(exc.ToString())
      End Try
    End Sub

    Private Sub OpcForm_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs)
      If Not dataControl1 Is Nothing Then
        dataControl1.[Stop]()
      End If
    End Sub

  End Class
End Namespace

