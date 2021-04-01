Imports System
Imports System.Threading
Imports System.IO
Imports Softing.OPCToolbox

Namespace OpcClient
    ''' <summary>
    ''' Summary description for Class1.
    ''' </summary>
    Public Class Dll
#Region "Private Attributes"
        '-------------------------
        Private m_simulationEnd As Boolean = False
        Private Shared m_mutex As New Mutex
        Private Shared m_opcClient As OpcClient = Nothing

        '--
#End Region

#Region "Private Methods"
        '----------------------

        Private Sub Simulate()
            Dim fileStream As New FileStream("ClientDll.txt", FileMode.Create, FileAccess.Write)
            Dim streamWriter As New StreamWriter(fileStream)
            streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
            ' Wait until it is safe to enter.
            m_mutex.WaitOne()

            While Not m_simulationEnd
                'Simulate the DA Client read operation
                Dim readValue As String = m_opcClient.ReadItem()

                'Log the read value to the file
                streamWriter.WriteLine(readValue)
                'Simulate the AE Client condition state request
                Dim state As String = m_opcClient.GetConditionState()

                'Log the condition state to the file
                streamWriter.WriteLine(state)
                Thread.Sleep(1000)
            End While
            streamWriter.Flush()
            streamWriter.Close()
            ' Release the Mutex
            m_mutex.ReleaseMutex()

        End Sub

        Private Sub StartSimulationThread()
            Dim simulationThread As New Thread(New ThreadStart(AddressOf Simulate))
            simulationThread.Start()
        End Sub
        '--
#End Region

#Region "Public Methods"
        '--------------------

        Public Function Start() As Integer
            Try
                Dim result As Integer = EnumResultCode.S_OK
                Dim registrationResult As Integer = EnumResultCode.S_OK
                ' create the OpcClient instance
                m_opcClient = New OpcClient
                '  TODO - binary license activation
                '  Fill in your binary license activation keys here
                '
                '  NOTE: you can activate one or all of the features at the same time
                '  firstly activate the COM-DA Client feature
                ' result = m_opcClient.GetApplication().Activate(EnumFeature.DA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX")
                If Not ResultCode.SUCCEEDED(result) Then
                    Return result
                End If

                '  activate the COM-AE Client Feature
                ' result = m_opcClient.GetApplication().Activate(EnumFeature.AE_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX")
                If Not ResultCode.SUCCEEDED(result) Then
                    Return result
                End If

                '  activate the XML-DA Client Feature
                '  result = m_opcClient.GetApplication().Activate(EnumFeature.XMLDA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX")
                If Not ResultCode.SUCCEEDED(result) Then
                    Return result
                End If
                '  END TODO - binary license activation

                '  initialize the client instance
                If Not ResultCode.SUCCEEDED(m_opcClient.Initialize()) Then
                    m_opcClient = Nothing
                    Return EnumResultCode.E_FAIL
                End If
                '  initialize the DA client simulation
                registrationResult = registrationResult Or m_opcClient.InitializeDaObjects()
                '  initialize the AE client simulation
                registrationResult = registrationResult Or m_opcClient.InitializeAeObjects()
                '  start the simulation thread 
                If ResultCode.SUCCEEDED(registrationResult) Then
                    StartSimulationThread()
                End If
                Return registrationResult
            Catch exc As Exception
                Console.WriteLine(exc.ToString())
                Return EnumResultCode.E_FAIL
            End Try
        End Function

        Public Sub [Stop]()
            '  terminate the simulation
            m_simulationEnd = True
            m_mutex.WaitOne()
            m_mutex.ReleaseMutex()
            m_opcClient.Terminate()
            m_opcClient = Nothing
        End Sub

        '--
#End Region

    End Class
End Namespace

