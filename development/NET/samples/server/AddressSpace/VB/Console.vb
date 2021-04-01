Imports System
Imports System.Runtime.InteropServices
Imports System.Threading
Imports Softing.OPCToolbox

Namespace AddressSpace
    Public Class Console
#Region "Public Attributes"
        '-----------------------------------

        ' signals application should terminate
        Public Shared EndEvent As AutoResetEvent
        Public Shared [End] As Boolean = False
        '--
#End Region

#Region "Private Attributes"
        '-------------------------

        Private m_simulationEnd As Boolean = False
        Private Shared m_mutex As New Mutex()
        Private m_count As Integer = 0
        Private Shared m_opcServer As OpcServer = Nothing

        '--
#End Region

#Region "Public Properties"
        '------------------------
        Public Property SimulationEnd() As Boolean
            Get
                Return m_simulationEnd
            End Get
            Set(ByVal value As Boolean)
                m_simulationEnd = value
            End Set
        End Property
        Public ReadOnly Property Mutex() As Mutex
            Get
                Return m_mutex
            End Get
        End Property

        Public ReadOnly Property OpcServer() As OpcServer
            Get
                Return m_opcServer
            End Get
        End Property
        '--
#End Region

#Region "Public Methods"
        '--------------------------
        Public Sub CreateOpcServer()
            If m_opcServer Is Nothing Then
                m_opcServer = New OpcServer()
            End If
        End Sub

        Public Sub Simulate()
            ' Wait until it is safe to enter.
            m_mutex.WaitOne()

            Dim iRet As Integer = WaitHandle.WaitTimeout

            While Not m_simulationEnd
                iRet = System.Threading.WaitHandle.WaitAny(New AutoResetEvent() {OpcServer.AddressSpaceChanged}, 1000, True)
                If iRet = WaitHandle.WaitTimeout Then

                    If m_count > 30 Then

                        OpcServer.ReconfigHandles(0).RemoveChild(OpcServer.ReconfigHandles(2))
                        OpcServer.ObjectBased.RemoveChild(OpcServer.ReconfigHandles(0))
                        OpcServer.ObjectBased.RemoveChild(OpcServer.ReconfigHandles(1))
                        m_opcServer.RebuildRandomAddressSpace(OpcServer.ObjectBased)
                        OpcServer.AddressSpaceChanged.[Set]()
                        m_count = 0
                    Else
                        System.Math.Max(System.Threading.Interlocked.Increment(m_count), m_count - 1)
                    End If
                ElseIf iRet = 0 Then
                    OpcServer.ShowObjectTree()
                End If

            End While
            ' Release the Mutex
            m_mutex.ReleaseMutex()

        End Sub

        Public Sub StartSimulationThread()
            Dim simulationThread As Thread = New Thread(AddressOf Simulate)
            simulationThread.Start()
        End Sub

        '--
#End Region

        Public Shared Sub Main(ByVal args As String())
            Try

                Dim result As Integer = EnumResultCode.S_OK
                EndEvent = New AutoResetEvent(False)
                Dim console As New Console()

                Dim handlerRoutine As MyWin32.HandlerRoutine = Nothing
                handlerRoutine = AddressOf MyWin32.Handler
                MyWin32.SetConsoleCtrlHandler(handlerRoutine, True)

                '  create and initialize the OpcServer instance
                console.CreateOpcServer()
                Dim server As OpcServer = console.OpcServer

                server.Initialize()

                Dim creator As New MyCreator()
                If Not ResultCode.SUCCEEDED(server.Prepare(creator)) Then
                    server.Terminate()
                    MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT)
                    server = Nothing
                    Return
                End If

                '  handle the command line arguments (register/unregister, etc)
                Dim commandline As String = Environment.CommandLine
                result = server.ProcessCommandLine(commandline)
                Dim traceGroup As EnumTraceGroup
                traceGroup = CType([Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), EnumTraceGroup)

                If Not (result = EnumResultCode.S_OK) Then
                    If result = EnumResultCode.S_FALSE Then
                        'registration operation succesful
                        server.Trace(EnumTraceLevel.INF, traceGroup, "Console::Main", "Registration succeeded")
                    Else
                        server.Trace(EnumTraceLevel.INF, traceGroup, "Console::Main", "Registration failed")
                    End If

                    '  no matter what close the application if
                    'processCommandLine returned something different of S_OK
                    server.Terminate()
                    server = Nothing
                    Return
                End If
                server.ReconfigHandles = New MyDaAddressSpaceElement(2) {}
                AddressSpace.OpcServer.AddressSpaceChanged = New System.Threading.AutoResetEvent(True)

                '  start the OPC server's I/O internal mechanism 
                If ResultCode.SUCCEEDED(server.Start()) Then
                    '  build the namespace
                    server.BuildAddressSpace()
                    '  declare the namespaces built and the server ready for clients to connect
                    server.Ready()
                End If

                server.ShowObjectTree()

                '  start the simulation thread 
                If ResultCode.SUCCEEDED(result) Then
                    console.StartSimulationThread()
                End If

                System.Console.WriteLine("Press Ctrl-C to exit" & Microsoft.VisualBasic.Chr(10) & "")

                While Not Console.[End]
                    '  TODO: place your cyclic code here            
                    Thread.Sleep(1000)
                End While

                '  terminate the simulation
                console.SimulationEnd = True
                console.Mutex.WaitOne()
                console.Mutex.ReleaseMutex()

                server.[Stop]()
                server.Terminate()
                server = Nothing
                MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT)
            Catch exc As Exception
                System.Console.WriteLine(exc.ToString())
            End Try
        End Sub
    End Class

    Public Class MyWin32
        ' Declare the SetConsoleCtrlHandler function 
        ' as external and receiving a delegate.   
        <DllImport("Kernel32")> _
        Public Shared Function SetConsoleCtrlHandler(ByVal Handler As HandlerRoutine, ByVal Add As Boolean) As Boolean
        End Function

        ' A delegate type to be used as the handler routine 
        ' for SetConsoleCtrlHandler.
        Public Delegate Function HandlerRoutine(ByVal CtrlType As CtrlTypes) As Boolean

        ' An enumerated type for the control messages 
        ' sent to the handler routine.
        Public Enum CtrlTypes
            CTRL_C_EVENT = 0
            CTRL_BREAK_EVENT
            CTRL_CLOSE_EVENT
            CTRL_LOGOFF_EVENT = 5
            CTRL_SHUTDOWN_EVENT
        End Enum

        ' A private static handler function.
        Public Shared Function Handler(ByVal CtrlType As MyWin32.CtrlTypes) As Boolean
            Dim message As String = String.Empty

            ' A switch to handle the event type.
            Select Case CtrlType
                Case MyWin32.CtrlTypes.CTRL_C_EVENT, MyWin32.CtrlTypes.CTRL_BREAK_EVENT, MyWin32.CtrlTypes.CTRL_CLOSE_EVENT, MyWin32.CtrlTypes.CTRL_LOGOFF_EVENT, MyWin32.CtrlTypes.CTRL_SHUTDOWN_EVENT
                    message = "Stop execution, since CTRL command!"
                    Console.EndEvent.[Set]()
                    Console.[End] = True
            End Select
            ' Use interop to display a message for the type of event.
            System.Console.WriteLine(message)
            Return True
        End Function
    End Class
End Namespace

