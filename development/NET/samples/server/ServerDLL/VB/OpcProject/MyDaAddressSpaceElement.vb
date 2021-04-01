Imports System
Imports System.Collections

Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace DllSample
	Public Class MyDaAddressSpaceElement
		Inherits DaAddressSpaceElement

#Region "Constructors"
		'------------------------------

		Public Sub New(ByVal anItemID As String, ByVal aName As String, ByVal anUserData As System.UInt32, ByVal anObjectHandle As UInt32, ByVal aParentHandle As UInt32)
			MyBase.New(anItemID, aName, anUserData, anObjectHandle, aParentHandle)
		End Sub

		Public Sub New()
		End Sub

		'--
#End Region

#Region "Public Static Attributes"
		'-------------------------------

		Public Shared TYPE_UNKNOWN As Byte = 0
		Public Shared TYPE_NODEMATH As Byte = 128
		Public Shared TYPE_SINE As Byte = 4
		Public Shared TYPE_ACCEPT As Byte = 8

		'--
#End Region

#Region "Private Attributes"
		'-------------------------------

		Private m_Type As Byte = TYPE_UNKNOWN
		Private m_properties As New Hashtable

		'--
#End Region

#Region "Public Property"
		'---------------------

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
				aRequest.Value = New ValueQT("description", [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now)

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
			Return CType((EnumResultCode.S_OK), Integer)
		End Function


		Public Function AddProperty(ByVal aProperty As DaProperty) As Integer
			If Not aProperty Is Nothing Then
				m_properties.Add(aProperty.Id, aProperty)
				Return CType(EnumResultCode.S_OK, Integer)
			Else
				Return CType(EnumResultCode.S_FALSE, Integer)
			End If
		End Function

		'--
#End Region

	End Class
End Namespace

