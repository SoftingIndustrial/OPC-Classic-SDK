Imports System
Imports System.Threading
Imports System.Runtime.InteropServices
Imports Microsoft.VisualBasic
Imports Softing.OPCToolbox

Namespace DAGetProperties
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
        Private Shared m_opcClient As OpcClient = Nothing
        '--
#End Region

#Region "Public Properties"
        '------------------------

        Public ReadOnly Property OpcClient() As OpcClient
            Get
                Return m_opcClient
            End Get
        End Property

        '--
#End Region

#Region "Public Methods"
        '--------------------------
        Public Sub CreateOpcClient()
            If m_opcClient Is Nothing Then
                m_opcClient = New OpcClient()
            End If
        End Sub

        '--
#End Region

        Public Shared Sub Usage()

            System.Console.WriteLine("Usage:" & Chr(10) & "")
            System.Console.Write("Press 'a' or 'A' and ENTER to asynchronously get properties" & Chr(10) & "")
            System.Console.Write("Press 's' or 'S' and ENTER to synchronously get properties" & Chr(10) & "")
            System.Console.Write("Press 'u' or 'U' and ENTER to display this usage information" & Chr(10) & "")
            System.Console.Write("Press 'e' or 'q' and ENTER to exit" & Chr(10) & "")
            System.Console.Write("" & Chr(10) & "")
        End Sub

        Public Shared Sub Main(ByVal args As String())
            Try
                Dim result As Integer = EnumResultCode.S_OK
                EndEvent = New AutoResetEvent(False)
                Dim console As New Console()

                Dim handlerRoutine As New MyWin32.HandlerRoutine(AddressOf MyWin32.Handler)
                MyWin32.SetConsoleCtrlHandler(handlerRoutine, True)

                console.CreateOpcClient()
                ' gets the OpcClient instance
                Dim client As OpcClient = console.OpcClient


                '  initialize the client instance
                If Not ResultCode.SUCCEEDED(client.Initialize()) Then
                    client = Nothing
                    Return
                End If
                '  initialize the DA client simulation
                result = result Or m_opcClient.InitializeDaObjects()

                Usage()

                Dim [end] As Boolean = False
                While Not Console.[End] AndAlso Not [end]
                    Dim read As String = System.Console.ReadLine()
                    Select Case read
                        Case "A", "a"
                            client.GetPropertiesAsync()
                        Case "S", "s"
                            client.GetPropertiesSync()
                        Case "E", "e", "Q", "q"
                            [end] = True
                        Case "U", "u", "?"
                            Usage()
                    End Select
                End While


                client.Terminate()
                client = Nothing
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

