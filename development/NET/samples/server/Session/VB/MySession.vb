Imports System
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace Session
	Class MySession
		Inherits DaSession

#Region "Fields"
		Dim m_qualityGood As EnumQuality
#End Region

		Public Shared s_controlingSession As MySession = Nothing
		Private Shared s_clientCount As Int32 = 0
		Private m_clientSpecValue As ValueQT = Nothing

		Public Sub New(ByVal aType As EnumSessionType, ByVal aHandle As UInt32)
			MyBase.New(aType, aHandle)
			m_qualityGood = [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD)
			m_clientSpecValue = New ValueQT(s_clientCount, m_qualityGood, DateTime.Now)
			System.Math.Max(System.Threading.Interlocked.Increment(s_clientCount), s_clientCount - 1)
			AddHandler ClientNameChanged, AddressOf HandleClientNameChanged
		End Sub

		Public ReadOnly Property Handle() As UInt32
			Get
				Return MyBase.ObjectHandle
			End Get
		End Property

		Public Overloads Overrides Sub ConnectSession()

			MyBase.ConnectSession()

			SyncLock Console.m_sessionsJanitor
				Console.m_sessions.Add(Me)
			End SyncLock

			Console.m_clientChanged.[Set]()

		End Sub

		Public Overloads Overrides Sub DisconnectSession()

			MyBase.DisconnectSession()

			SyncLock Console.m_sessionsJanitor
				Console.m_sessions.Remove(Me)
			End SyncLock

			Console.m_clientChanged.[Set]()

		End Sub

		Private Sub HandleClientNameChanged()
			System.Console.WriteLine("New client name set for client {0:X}: {1}", Handle, ClientName)
		End Sub

		Public Overloads Overrides Function Logoff() As Integer

			If Me.Equals(s_controlingSession) Then
				s_controlingSession = Nothing
			End If

			Console.m_clientChanged.[Set]()
			Return EnumResultCode.S_OK
		End Function

		Public Overloads Overrides Function Logon(ByVal UserName As String, ByVal Password As String) As Integer

			Dim ret As Integer = EnumResultCode.E_ACCESSDENIED

			If UserName = "OPC" AndAlso Password = "opc" Then

				If Me.Equals(s_controlingSession) OrElse s_controlingSession Is Nothing Then
					s_controlingSession = Me
					ret = EnumResultCode.S_OK
				End If
			End If

			Console.m_clientChanged.[Set]()
			Return ret
		End Function

		Public Function GetCacheValue(ByRef aCacheValue As ValueQT) As Integer

			aCacheValue = New ValueQT(m_clientSpecValue.Data, m_clientSpecValue.Quality, m_clientSpecValue.TimeStamp)
			Return EnumResultCode.S_OK
		End Function

	End Class
End Namespace

