Imports System
Imports System.Collections
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace AddressSpace
	Public Class MyDaAddressSpaceElement
		Inherits DaAddressSpaceElement

		Public Sub New(ByVal anItemID As String, ByVal aName As String, ByVal anUserData As UInt32, ByVal anObjectHandle As UInt32, ByVal aParentHandle As UInt32)
			MyBase.New(anItemID, aName, anUserData, anObjectHandle, aParentHandle)
			AddHandler Me.AddedToAddressSpace, AddressOf HandleAddedToAddressSpace
			AddHandler Me.RemovedFromAddressSpace, AddressOf HandleRemovedFromAddressSpace
		End Sub

		Public Sub New()
			AddHandler Me.AddedToAddressSpace, AddressOf HandleAddedToAddressSpace
			AddHandler Me.RemovedFromAddressSpace, AddressOf HandleRemovedFromAddressSpace
		End Sub

		Public Overloads Overrides Function QueryProperties(ByRef aPropertyList As ArrayList) As Integer

			aPropertyList = New ArrayList()

			Dim prop As New DaProperty()
			prop.Id = 5555
			prop.AccessRights = EnumAccessRights.READABLE
			prop.Datatype = GetType(UInt16)
			prop.Name = "type"
			prop.Description = "Object Type"
			If ItemId Is Nothing Then
				prop.ItemId = "objectType"
			Else
				prop.ItemId = ItemId + Application.Instance.PropertyDelimiter + "objectType"
			End If

			Dim prop2 As New DaProperty()
			prop2.Id = 5556
			prop2.AccessRights = EnumAccessRights.READABLE
			prop2.Datatype = GetType(UInt16)
			prop2.Name = "type2"
			prop2.Description = "Object Type2"
			If ItemId Is Nothing Then
				prop2.ItemId = "objectType2"
			Else
				prop2.ItemId = ItemId + Application.Instance.PropertyDelimiter + "objectType2"
			End If

			aPropertyList.Add(prop)
			aPropertyList.Add(prop2)

			Return EnumResultCode.S_OK

		End Function

		Public Sub HandleAddedToAddressSpace()

			If ItemId = "stringBased" Then
				ObjectType = 20
			ElseIf (ItemId = "stringBased.N1") OrElse (ItemId = "stringBased.N2") Then
				ObjectType = 21
			ElseIf ItemId = "stringBased.T1" Then
				ObjectType = 22
			ElseIf (ItemId = "stringBased.N1.T1") OrElse (ItemId = "stringBased.N2.T1") OrElse (ItemId = "stringBased.N2.T2") Then
				ObjectType = 23
			ElseIf ItemId = "syntaxBased" Then
				ObjectType = 40
			ElseIf ItemId.StartsWith("syntaxBased") Then
				Dim index As Int32
				index = ItemId.LastIndexOf(".T")
				If index > 0 Then
					ObjectType = 42
				Else
					ObjectType = 41
				End If
			End If



			If Me.IoMode <> EnumIoMode.NONE Then
				Dim qualityGood As EnumQuality = CType([Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), EnumQuality)
				Dim initialValue As New ValueQT(0, qualityGood, DateTime.Now)
				ValueChanged(initialValue)
			End If

			OpcServer.AddressSpaceChanged.[Set]()

		End Sub

		Public Sub HandleRemovedFromAddressSpace()
			OpcServer.AddressSpaceChanged.[Set]()
		End Sub

		Private m_objectType As Byte

		Public Property ObjectType() As Byte
			Get
				Return m_objectType
			End Get
			Set(ByVal value As Byte)
				m_objectType = value
			End Set
		End Property

	End Class

End Namespace

