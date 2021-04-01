Imports System
Imports Microsoft.VisualBasic
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace OpcClient
    Public Class OpcClient

#Region "Fields"
        Dim m_traceGroupUser As EnumTraceGroup
#End Region

#Region "Constructor"
        '-----------------

        Public Sub New()
            m_traceGroupUser = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER)
        End Sub

        '--
#End Region

#Region "Private Members"
        '---------------------

#Region "Private Attributes"
        '------------------------

        Private m_daSession As DaSession = Nothing
        Private m_daSubscription As DaSubscription = Nothing
        Private m_daItem As DaItem = Nothing
        Private m_aeSession As AeSession = Nothing
        Private m_aeSubscription As AeSubscription = Nothing
        '--
#End Region

#Region "Private Methods"
        '---------------------

        Private Function GetStateToString(ByVal state As EnumConditionState) As String
            Dim stateToString As String = String.Empty

            If (state And EnumConditionState.ACTIVE) = EnumConditionState.ACTIVE Then
                stateToString &= " ACT"
            End If
            If (state And EnumConditionState.ENABLED) = EnumConditionState.ENABLED Then
                stateToString &= " ENA"
            End If
            If (state And EnumConditionState.ACKED) = EnumConditionState.ACKED Then
                stateToString &= " ACK"
            End If
            If state = 0 Then
                stateToString &= " DIS"
            End If
            Return stateToString
        End Function

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
            GetApplication().VersionOtb = 447
            Dim result As Integer = GetApplication().Initialize()
            Dim traceGroupClient As EnumTraceGroup
            Dim traceGroupAll As EnumTraceGroup
            traceGroupClient = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.CLIENT)
            traceGroupAll = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL)
            GetApplication().EnableTracing(traceGroupAll, traceGroupAll, traceGroupClient, traceGroupClient, "Trace.txt", Convert.ToUInt32(1000000), _
         Convert.ToUInt32(2))

            Return result
        End Function

        Public Function ProcessCommandLine(ByVal commandLine As String) As Integer
            Return Application.Instance.ProcessCommandLine(commandLine)
        End Function


        Public Sub Terminate()
            GetApplication().Terminate()
            m_daSession = Nothing
            m_daSubscription = Nothing
            m_daItem = Nothing
            m_aeSession = Nothing
            m_aeSubscription = Nothing
        End Sub


        Public Function GetConditionState() As String
            If m_aeSession Is Nothing Then
                Return ""
            End If

            Dim message As String = [String].Empty

            Try
                Dim conditionStateToString As String = [String].Empty
                Dim conditionState As AeConditionState = Nothing

                Dim result As Integer = m_aeSession.GetAeConditionState("computer.clock.time slot 1", "between", Nothing, conditionState, Nothing)

                If ResultCode.SUCCEEDED(result) Then

                    message = "The condition state is: " & Chr(10) & ""
                    message &= " Source Path: computer.clock.time slot 1 " & Chr(10) & ""
                    message &= " Condition Name: between " & Chr(10) & ""
                    message &= " State: " + GetStateToString(conditionState.State) + "" & Chr(10) & ""
                    message &= " Quality: " + conditionState.Quality.ToString() + "" & Chr(10) & ""
                    message &= " Active Time: " + conditionState.ConditionActiveTime + "" & Chr(10) & ""
                    message &= " Inactive Time: " + conditionState.ConditionInactiveTime + "" & Chr(10) & ""
                    message &= " AcknowledgeTime: " + conditionState.ConditionAckTime + "" & Chr(10) & ""
                    message &= " AcknowledgerComment: " + conditionState.AcknowledgerComment + "" & Chr(10) & ""
                    message &= " AcknowledgerID: " + conditionState.AcknowledgerId + "" & Chr(10) & ""
                    message &= " Active subcondition time: " + conditionState.SubConditionActiveTime + "" & Chr(10) & ""
                    message &= " Active subcondition name: " + conditionState.ActiveSubConditionName + "" & Chr(10) & ""
                    message &= " Active subcondition definition: " + conditionState.ActiveSubConditionDefinition + "" & Chr(10) & ""
                    message &= " Active subcondition description: " + conditionState.ActiveSubConditionDescription + "" & Chr(10) & ""
                    message &= " Active subcondition severity: " + conditionState.ActiveSubConditionSeverity.ToString() + "" & Chr(10) & ""
                    message &= " Number of subconditions: " + conditionState.SubConditionsNames.Length + "" & Chr(10) & ""
                    Dim i As Integer = 0
                    While i < conditionState.SubConditionsNames.Length
                        message &= "  Subcondition name: " + conditionState.SubConditionsNames(i) + "" & Chr(10) & ""
                        message &= "  Subcondition definition: " + conditionState.SubConditionsDefinitions(i) + "" & Chr(10) & ""
                        message &= "  Subcondition description: " + conditionState.SubConditionsDescriptions(i) + "" & Chr(10) & ""
                        message &= "  Subcondition severity: " + conditionState.SubConditionsSeverities(i).ToString() + "" & Chr(10) & ""
                        System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
                    End While
                Else
                    message = "Get condition state failed!" & Chr(10) & ""
                End If
            Catch exc As Exception
                GetApplication().Trace(EnumTraceLevel.ERR, m_traceGroupUser, "OpcClient::GetConditionState", exc.ToString())
            End Try
            Return message
        End Function

        Public Function InitializeAeObjects() As Integer
            Dim connectResult As Integer = EnumResultCode.E_FAIL
            Try

                m_aeSession = New AeSession("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}")
                m_aeSubscription = New AeSubscription(m_aeSession)

                connectResult = m_aeSession.Connect(True, True, Nothing)

                m_aeSubscription.RefreshAeConditions(Nothing)
            Catch exc As Exception
                GetApplication().Trace(EnumTraceLevel.ERR, m_traceGroupUser, "OpcClient::InitializeAeObjects", exc.ToString())
            End Try

            Return connectResult
        End Function
        Public Function InitializeDaObjects() As Integer
            Dim connectResult As Integer = EnumResultCode.E_FAIL

            Try
                m_daSession = New DaSession("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}")
                m_daSubscription = New DaSubscription(Convert.ToUInt32(1000), m_daSession)
                m_daItem = New DaItem("maths.sin", m_daSubscription)

                connectResult = m_daSession.Connect(True, False, Nothing)
            Catch exc As Exception
                GetApplication().Trace(EnumTraceLevel.ERR, m_traceGroupUser, "OpcClient::InitializaDaObjects", exc.ToString())
            End Try

            Return connectResult
        End Function

        Public Function ReadItem() As String
            Dim message As String = [String].Empty

            Try
                Dim readCount As UInt32 = Convert.ToUInt32(0)
                Dim itemList As DaItem() = New DaItem(Convert.ToInt32(readCount)) {}
                itemList(0) = m_daItem

                Dim values As ValueQT() = Nothing
                Dim results As Integer() = Nothing

                If ResultCode.SUCCEEDED(m_daSubscription.Read(Convert.ToUInt32(0), itemList, values, results, Nothing)) Then
                    message &= " " & Chr(10) & "Read item synchronously using subscription " & Chr(10) & ""

                    Dim i As Integer = 0
                    While i < values.Length
                        If ResultCode.SUCCEEDED(results(i)) Then
                            message &= " " + itemList(i).Id + " - "
                            message += values(i).ToString() + "" & Chr(10) & "" & Chr(10) & ""
                        Else
                            message &= "Read failed for item " + itemList(i).Id + "" & Chr(10) & "" & Chr(10) & ""
                        End If
                        System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
                    End While
                Else
                    message &= " Subscription synchronous read failed!" + "" & Chr(10) & "" & Chr(10) & ""
                End If
            Catch exc As Exception
                GetApplication().Trace(EnumTraceLevel.ERR, m_traceGroupUser, "OpcClient::ReadItem", exc.ToString())
            End Try
            Return message
        End Function


        Public Sub Trace(ByVal traceLevel As EnumTraceLevel, ByVal traceGroup As EnumTraceGroup, ByVal objectID As String, ByVal message As String)
            Application.Instance.Trace(traceLevel, traceGroup, objectID, message)
        End Sub

        '--
#End Region

#Region "Public Properties"
        '-----------------------

        Public Property ServiceName() As String
            Get
                Return Application.Instance.ServiceName
            End Get
            Set(ByVal Value As String)
                Application.Instance.ServiceName = Value
            End Set
        End Property

        '--
#End Region

    End Class

End Namespace

