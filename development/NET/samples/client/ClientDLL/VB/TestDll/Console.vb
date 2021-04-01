Imports System.Runtime.InteropServices
Imports System.Threading
Imports Softing.OPCToolbox
Imports DllProject.OpcClient
Imports System

Namespace TestDll
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

        '--
#End Region

#Region "Public Properties"
        '------------------------


        '--
#End Region

#Region "Public Methods"
        '--------------------------


        '--
#End Region

        Public Shared Sub Main(ByVal args As String())
            Try
                Dim result As Integer = EnumResultCode.S_OK
                EndEvent = New AutoResetEvent(False)
                Dim console As New Console

                Dim handlerRoutine As New MyWin32.HandlerRoutine(AddressOf MyWin32.Handler)
                MyWin32.SetConsoleCtrlHandler(handlerRoutine, True)

                'create an instance of the generated DLL
                Dim usedDll As New Dll

                ' call the DLL's Start method; server will be initialized, 
                ' namespaces will be created and simulation thread will be started
                result = usedDll.Start()

                If result <> EnumResultCode.S_OK Then
                    Return
                End If

                System.Console.WriteLine("The results will be written in the ClientDll.txt file")
                System.Console.WriteLine("Press 'e' or ' q' and 'ENTER' to exit")

                Dim [end] As Boolean = False

                While Not Console.[End] AndAlso Not [end]
                    Dim read As String = System.Console.ReadLine()
                    Select Case read
                        Case "E", "e", "Q", "q"
                            [end] = True

                        Case Else
                    End Select
                End While


                'calls the DLL's Stop method which terminates the server application and closes the simulation thread
                usedDll.[Stop]()

                MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT)
            Catch exc As Exception
                System.Console.WriteLine(exc.ToString())
            End Try
        End Sub
    End Class

    Public Class MyWin32
        ' Declare the SetConsoleCtrlHandler function 
        ' as external and receiving a delegate.   
        <DllImport("Kernel32")>
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

