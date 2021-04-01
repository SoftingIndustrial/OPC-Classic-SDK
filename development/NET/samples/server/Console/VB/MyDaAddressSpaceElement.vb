Imports System
Imports System.Collections
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace Console
	Public Class MyDaAddressSpaceElement
		Inherits DaAddressSpaceElement

#Region "Fields"
		Dim m_qualityGood As EnumQuality
#End Region

#Region "Constructors"
		'------------------------------

		Public Sub New(ByVal anItemID As String, ByVal aName As String, ByVal anUserData As UInt32, ByVal anObjectHandle As UInt32, ByVal aParentHandle As UInt32)
			MyBase.New(anItemID, aName, anUserData, anObjectHandle, aParentHandle)
			m_qualityGood = [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD)
		End Sub

		Public Sub New()
		End Sub

		'--
#End Region

#Region "Private Attributes"
		'-------------------------------

		Private m_properties As New Hashtable
		Private m_Type As Byte = TYPE_UNKNOWN

		'--
#End Region

#Region "Public Constants"
		'-------------------------------

		Public Shared TYPE_UNKNOWN As Byte = 0
		Public Shared TYPE_NODEMATH As Byte = 128
		Public Shared TYPE_NODERANDOM As Byte = 64
		Public Shared TYPE_RANDOM_100MS As Byte = 1
		Public Shared TYPE_RANDOM_1S As Byte = 2
		Public Shared TYPE_SINE As Byte = 4
		Public Shared TYPE_ACCEPT As Byte = 8

		'--
#End Region

#Region "Public Attributes"
		'-------------------------------

		Public Overridable Property Type() As Byte
			Get
				Return m_Type
			End Get
			Set(ByVal Value As Byte)
				m_Type = Value
			End Set
		End Property

		'--
#End Region

#Region "Public Methods"
		'---------------------

		''' <summary>
		''' Get elements property value data
		''' </summary>

		Public Sub GetPropertyValue(ByVal aRequest As DaRequest)
			If aRequest.PropertyId = 101 Then

				If Type = MyDaAddressSpaceElement.TYPE_RANDOM_100MS Then
					aRequest.Value = New ValueQT("Random value changing every 100 ms", m_qualityGood, DateTime.Now)
					aRequest.Result = EnumResultCode.S_OK
				ElseIf Type = MyDaAddressSpaceElement.TYPE_RANDOM_1S Then
					aRequest.Value = New ValueQT("Random value changing every second", m_qualityGood, DateTime.Now)
					aRequest.Result = EnumResultCode.S_OK
				ElseIf Type = MyDaAddressSpaceElement.TYPE_NODERANDOM Then
					aRequest.Value = New ValueQT("Random values", m_qualityGood, DateTime.Now)
					aRequest.Result = EnumResultCode.S_OK
				End If
			ElseIf aRequest.PropertyId = 6020 Then

				aRequest.Value = New ValueQT(OpcServer.g_angle, m_qualityGood, DateTime.Now)
				aRequest.Result = EnumResultCode.S_OK
			Else

				If m_properties.ContainsKey(aRequest.PropertyId) Then

					Dim [property] As DaConstantProperty = CType(m_properties(aRequest.PropertyId), DaConstantProperty)
					If Not [property] Is Nothing Then

						aRequest.Value = [property].Value
						aRequest.Result = EnumResultCode.S_OK
					End If
				End If

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

		'--
#End Region

	End Class
End Namespace

