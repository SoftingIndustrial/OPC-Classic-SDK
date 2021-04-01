Imports System
Imports System.Collections
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace Session
	Public Class MyDaAddressSpaceElement
		Inherits DaAddressSpaceElement

#Region "Constructors"
		'------------------------------

		Public Sub New(ByVal anItemID As String, ByVal aName As String, ByVal anUserData As UInt32, ByVal anObjectHandle As UInt32, ByVal aParentHandle As UInt32)
			MyBase.New(anItemID, aName, anUserData, anObjectHandle, aParentHandle)
		End Sub

		Public Sub New()
		End Sub

		'--
#End Region

#Region "Private Attributes"
		'-------------------------------

		Private m_properties As New Hashtable

		'--
#End Region

#Region "Public Methods"
		'---------------------

		''' <summary>
		''' Get elements property value data
		''' </summary>
		Public Sub GetPropertyValue(ByVal aRequest As DaRequest)
			Dim qualityGood As EnumQuality
			qualityGood = [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD)

			If aRequest.PropertyId = EnumPropertyId.ITEM_DESCRIPTION Then
				aRequest.Value = New ValueQT("description", qualityGood, DateTime.Now)

				aRequest.Result = EnumResultCode.S_OK
			Else
				aRequest.Result = EnumResultCode.E_NOTFOUND
			End If
		End Sub

		Public Overloads Overrides Function QueryProperties(ByRef aPropertyList As ArrayList) As Integer
			If m_properties.Count > 0 Then
				aPropertyList = New ArrayList
				aPropertyList.AddRange(m_properties.Values)
			Else
				aPropertyList = Nothing
			End If
			Return EnumResultCode.S_OK
		End Function


		Public Function AddProperty(ByVal aProperty As DaProperty) As Integer
			If Not aProperty Is Nothing Then
				m_properties.Add(aProperty.Id, aProperty)
				Return EnumResultCode.S_OK
			Else
				Return EnumResultCode.S_FALSE
			End If
		End Function

		Public Overloads Overrides Function QueryCacheValue(ByVal aSession As DaSession, ByRef aCacheValue As ValueQT) As Integer

			If IoMode = EnumIoMode.POLL_OWNCACHE Then

				Dim session As MySession = CType(aSession, MySession)
				If Not session Is Nothing Then
					Return session.GetCacheValue(aCacheValue)
				Else
					Return EnumResultCode.E_FAIL
				End If
			Else
				Return MyBase.GetCacheValue(aCacheValue)
			End If
		End Function

		Public Overloads Overrides Sub Change(ByVal isActive As Boolean, ByVal anUpdateRate As Integer)
			MyBase.Change(isActive, anUpdateRate)
			Console.m_clientChanged.[Set]()
		End Sub

		'--
#End Region

	End Class
End Namespace

