Imports System
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace DAGetProperties
    Public Class OpcClient

#Region "Constructor"
        '-----------------

        Public Sub New()
        End Sub

        '--
#End Region

#Region "Private Members"
        '---------------------

#Region "Private Attributes"
        '------------------------

        Private m_daSession As MyDaSession = Nothing
        Private m_executionOptions As ExecutionOptions
        Private m_addressSpaceElementId As String
        Private m_addressSpaceElementPath As String
        Private m_operationResult As Integer

        '--
#End Region


        '--
#End Region

#Region "Public Methods"
        '---------------------

        Public Function GetApplication() As Application
            Return Application.Instance
        End Function

        Public Function Initialize() As Integer

            Dim result As Integer = EnumResultCode.S_OK
            GetApplication().VersionOtb = 447
            '  enable toolkit traces
            Dim traceGroupClient As EnumTraceGroup
            traceGroupClient = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.CLIENT)
            Dim traceGroupAll As EnumTraceGroup
            traceGroupAll = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL)
            GetApplication().EnableTracing(traceGroupAll, traceGroupAll, traceGroupClient, traceGroupClient, "Trace.txt", Convert.ToUInt32(1000000), _
               Convert.ToUInt32(0))
            '  TODO - binary license activation
            '  Fill in your binary license activation keys here
            '
            '  NOTE: you can activate one or all of the features at the same time

            '  activate the COM-DA Client feature
            '  result = Application.Instance.Activate(EnumFeature.DA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX")
            If Not ResultCode.SUCCEEDED(result) Then
                Return result
            End If

            '  activate the XML-DA Client Feature
            '  result = Application.Instance.Activate(EnumFeature.XMLDA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX")
            If Not ResultCode.SUCCEEDED(result) Then
                Return result
            End If
            '  END TODO - binary license activation

            '  proceed with the OPC Toolkit core initialization
            result = GetApplication().Initialize()

            If ResultCode.SUCCEEDED(result) Then
            End If
            Return result
        End Function

        Public Function ProcessCommandLine(ByVal commandLine As String) As Integer
            '  forward the command line arguments to the OPC Toolkit core internals
            Return Application.Instance.ProcessCommandLine(commandLine)
        End Function


        Public Sub Terminate()
            GetApplication().Terminate()
            m_daSession = Nothing
        End Sub


        Public Function InitializeDaObjects() As Integer
            Dim connectResult As Integer = EnumResultCode.E_FAIL

            m_addressSpaceElementId = "increment.I1"
            m_addressSpaceElementPath = String.Empty
            m_executionOptions = New ExecutionOptions()
            m_executionOptions.ExecutionContext = Convert.ToUInt32(0)


            Try

                '  TODO add your server URL here
                '  this is the server url for Softing OPCToolbox Demo Server
                '  first choice is COM-DA 
                Dim url As String = "opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"

                'uncomment if you need an XML-DA access
                '  Dim url As String = "http://localhost:8079/OPC/DA"

                m_daSession = New MyDaSession(url)

                connectResult = m_daSession.Connect(True, False, Nothing)
            Catch exc As Exception
                Dim traceGroupUser As EnumTraceGroup
                traceGroupUser = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER)
                GetApplication().Trace(EnumTraceLevel.ERR, traceGroupUser, "OpcClient::InitializaDaObjects", exc.ToString())
            End Try

            Return connectResult
        End Function


        Public Sub Trace(ByVal traceLevel As EnumTraceLevel, ByVal traceGroup As EnumTraceGroup, ByVal objectID As String, ByVal message As String)
            Application.Instance.Trace(traceLevel, traceGroup, objectID, message)
        End Sub

        Public Sub GetPropertiesAsync()
            m_executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS

            Dim properties As DaProperty() = Nothing
            Dim getAllPropertiesOptions As New DaGetPropertiesOptions()
            getAllPropertiesOptions.WhatPropertyData = EnumPropertyData.ALL

            m_operationResult = m_daSession.GetDaProperties(m_addressSpaceElementId, m_addressSpaceElementPath, getAllPropertiesOptions, properties, m_executionOptions)

            m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)

            System.Console.WriteLine("Get properties 1 and 5 of address space element: " + m_addressSpaceElementId)

            Dim someProperties As DaProperty() = Nothing
            Dim getPropertiesOptions As New DaGetPropertiesOptions()
            getPropertiesOptions.WhatPropertyData = EnumPropertyData.ALL
            getPropertiesOptions.PropertyIds = New Integer() {1, 5}

            m_operationResult = m_daSession.GetDaProperties(m_addressSpaceElementId, m_addressSpaceElementPath, getPropertiesOptions, someProperties, m_executionOptions)

            m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
        End Sub

        Public Sub GetPropertiesSync()
            m_executionOptions.ExecutionType = EnumExecutionType.SYNCHRONOUS
            Dim properties As DaProperty() = Nothing
            Dim getAllPropertiesOptions As New DaGetPropertiesOptions()
            getAllPropertiesOptions.WhatPropertyData = EnumPropertyData.ALL

            m_operationResult = m_daSession.GetDaProperties(m_addressSpaceElementId, m_addressSpaceElementPath, getAllPropertiesOptions, properties, m_executionOptions)

            If ResultCode.SUCCEEDED(m_operationResult) Then

                System.Console.WriteLine("Get properties of address space element:  " + m_addressSpaceElementId)
                Dim i As Integer = 0
                While i < properties.Length

                    System.Console.WriteLine("  Property Name: " + properties(i).Name)
                    System.Console.WriteLine("  Property Id: " + properties(i).Id.ToString())
                    System.Console.WriteLine("  Property Item Id: " + properties(i).ItemId)
                    System.Console.WriteLine("  Property DataType: " + properties(i).DataType.ToString())
                    System.Console.WriteLine("  Property description: " + properties(i).Description)
                    System.Console.WriteLine("  Property value: " + properties(i).ValueQT.Data.ToString() + "  ")
                    System.Console.WriteLine(" ")

                    System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
                End While
            Else

                System.Console.WriteLine("Failed to synchronously get properties of address space element: " + m_addressSpaceElementId)
            End If

            System.Console.WriteLine("Get properties 1 and 5 of address space element: " + m_addressSpaceElementId)
            Dim someProperties As DaProperty() = Nothing
            Dim getPropertiesOptions As New DaGetPropertiesOptions()
            getPropertiesOptions.PropertyIds = New Integer() {1, 5}
            getPropertiesOptions.WhatPropertyData = EnumPropertyData.ALL

            m_operationResult = m_daSession.GetDaProperties(m_addressSpaceElementId, m_addressSpaceElementPath, getPropertiesOptions, someProperties, m_executionOptions)

            If ResultCode.SUCCEEDED(m_operationResult) Then

                Dim i As Integer = 0
                While i < someProperties.Length

                    System.Console.WriteLine("  Property Name: " + someProperties(i).Name)
                    System.Console.WriteLine("  Property Id: " + someProperties(i).Id.ToString())
                    System.Console.WriteLine("  Property Item Id: " + someProperties(i).ItemId)
                    System.Console.WriteLine("  Property DataType: " + someProperties(i).DataType.ToString())
                    System.Console.WriteLine("  Property description: " + someProperties(i).Description)
                    System.Console.WriteLine("  Property value: " + someProperties(i).ValueQT.Data.ToString() + "  ")
                    System.Console.WriteLine(" ")

                    System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
                End While
            Else

                System.Console.WriteLine("Failed to synchronously get 1 and 5 properties of address space element: " + m_addressSpaceElementId)
            End If
        End Sub

        '--
#End Region

#Region "Public Properties"
        '-----------------------

        Public Property ServiceName() As String
            Get
                Return Application.Instance.ServiceName
            End Get
            Set
                Application.Instance.ServiceName = value
            End Set
        End Property

        '--
#End Region

    End Class

End Namespace

