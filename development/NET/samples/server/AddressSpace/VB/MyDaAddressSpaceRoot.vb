Imports System
Imports System.Collections
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace AddressSpace

	Public Class MyDaAddressSpaceRoot
		Inherits DaAddressSpaceRoot

		Public Overloads Overrides Function QueryAddressSpaceElementData(ByVal anElementID As String, ByRef anElement As AddressSpaceElement) As Integer

			Dim element As New DaAddressSpaceElement()

			If anElementID.StartsWith("stringBased") Then

				If anElementID = "stringBased" Then
					element.Name = anElementID
					element.IoMode = EnumIoMode.NONE
					element.HasChildren = True
				ElseIf anElementID = "stringBased.N1" OrElse anElementID = "stringBased.N2" Then

					element.Name = anElementID.Substring(anElementID.LastIndexOf(".") + 1)
					element.AccessRights = EnumAccessRights.READWRITEABLE
					element.Datatype = GetType(Byte)
					element.HasChildren = True
					element.IoMode = EnumIoMode.REPORT
				ElseIf anElementID = "stringBased.T1" Then

					element.Name = anElementID.Substring(anElementID.LastIndexOf(".") + 1)
					element.AccessRights = EnumAccessRights.READWRITEABLE
					element.Datatype = GetType(Byte)
					element.HasChildren = False
					element.IoMode = EnumIoMode.REPORT
				ElseIf anElementID.LastIndexOf("N1") > 0 Then

					If anElementID.Substring(anElementID.LastIndexOf("N1")).EndsWith("T1") Then

						element.Name = anElementID.Substring(anElementID.LastIndexOf(".") + 1)
						element.AccessRights = EnumAccessRights.READWRITEABLE
						element.Datatype = GetType(Byte)
						element.HasChildren = False
						element.IoMode = EnumIoMode.REPORT
					End If
				ElseIf anElementID.LastIndexOf("N2") > 0 Then

					If anElementID.Substring(anElementID.LastIndexOf("N2")).EndsWith("T1") OrElse anElementID.Substring(anElementID.LastIndexOf("N2")).EndsWith("T2") Then

						element.Name = anElementID.Substring(anElementID.LastIndexOf(".") + 1)
						element.AccessRights = EnumAccessRights.READWRITEABLE
						element.Datatype = GetType(Byte)
						element.HasChildren = False
						element.IoMode = EnumIoMode.REPORT
					End If
				End If
			ElseIf anElementID.StartsWith("syntaxBased") Then

				If anElementID = "syntaxBased" Then
					element.Name = anElementID
					element.IoMode = EnumIoMode.NONE
					element.HasChildren = True
				End If

				Dim index As Int32 = anElementID.LastIndexOf(".T")
				If index > 0 Then

					element.Name = anElementID.Substring(index + 1)
					If element.Name.Length = 2 Then

						element.HasChildren = False
						element.IoMode = EnumIoMode.POLL
						element.AccessRights = EnumAccessRights.READWRITEABLE
						element.Datatype = GetType(Int32)
					Else
						element.Name = String.Empty
					End If
				Else
					index = anElementID.LastIndexOf(".N")
					If index > 0 Then

						element.Name = anElementID.Substring(index + 1)
						If element.Name.Length = 2 Then
							element.HasChildren = True
							element.IoMode = EnumIoMode.NONE
						Else
							element.Name = String.Empty
						End If
					End If
				End If
			End If

			'  check if an element was succeeded
			If element.Name = String.Empty Then

				anElement = Nothing
				Return EnumResultCode.E_BADTYPE
			End If

			anElement = element
			Return EnumResultCode.S_OK

		End Function


		Public Overloads Overrides Function QueryAddressSpaceElementChildren(ByVal anElementID As String, ByVal aChildrenList As ArrayList) As Integer

			Dim child As MyDaAddressSpaceElement = Nothing

			If anElementID.Length = 0 Then
				child = New MyDaAddressSpaceElement()
				child.Name = "stringBased"
				child.IoMode = EnumIoMode.NONE
				child.ItemId = "stringBased"
				child.HasChildren = True
				aChildrenList.Add(child)
			ElseIf anElementID = "stringBased" Then
				child = New MyDaAddressSpaceElement()
				child.Name = "N1"
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name
				child.AccessRights = EnumAccessRights.READWRITEABLE
				child.Datatype = GetType(Byte())
				child.IoMode = EnumIoMode.REPORT
				child.HasChildren = True
				aChildrenList.Add(child)

				child = New MyDaAddressSpaceElement()
				child.Name = "N2"
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name
				child.AccessRights = EnumAccessRights.READWRITEABLE
				child.Datatype = GetType(Byte())
				child.IoMode = EnumIoMode.REPORT
				child.HasChildren = True
				aChildrenList.Add(child)

				child = New MyDaAddressSpaceElement()
				child.Name = "T1"
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name
				child.AccessRights = EnumAccessRights.READWRITEABLE
				child.Datatype = GetType(Byte)
				child.IoMode = EnumIoMode.REPORT
				child.HasChildren = False
				aChildrenList.Add(child)
			ElseIf anElementID = "stringBased.N1" Then
				child = New MyDaAddressSpaceElement()
				child.Name = "T1"
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name
				child.AccessRights = EnumAccessRights.READWRITEABLE
				child.Datatype = GetType(Byte)
				child.IoMode = EnumIoMode.REPORT
				child.HasChildren = False
				aChildrenList.Add(child)
			ElseIf anElementID = "stringBased.N2" Then

				child = New MyDaAddressSpaceElement()
				child.Name = "T1"
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name
				child.AccessRights = EnumAccessRights.READWRITEABLE
				child.Datatype = GetType(Byte)
				child.IoMode = EnumIoMode.REPORT
				child.HasChildren = False
				aChildrenList.Add(child)

				child = New MyDaAddressSpaceElement()
				child.Name = "T2"
				child.ItemId = anElementID + Application.Instance.AddressSpaceDelimiter + child.Name
				child.AccessRights = EnumAccessRights.READWRITEABLE
				child.Datatype = GetType(Byte)
				child.IoMode = EnumIoMode.REPORT
				child.HasChildren = False
				aChildrenList.Add(child)
			End If

			Return EnumResultCode.S_OK
		End Function

		''' <summary>
		''' QueryAddressSpaceElementProperties
		''' </summary>    
		Public Overloads Overrides Function QueryAddressSpaceElementProperties(ByVal anElementID As String, ByRef aPopertyList As ArrayList) As Integer

			aPopertyList = Nothing

			If anElementID.StartsWith("stringBased") Then

				aPopertyList = New ArrayList()

				Dim prop As New DaProperty()
				prop.Id = 5555
				prop.AccessRights = EnumAccessRights.READABLE
				prop.Datatype = GetType(Short)
				prop.Name = "type"
				prop.ItemId = anElementID + Application.Instance.PropertyDelimiter + "objectType"
				prop.Description = "Object Type"

				Dim prop2 As New DaProperty()
				prop2.Id = 5556
				prop2.AccessRights = EnumAccessRights.READABLE
				prop2.Datatype = GetType(Short)
				prop2.Name = "type2"
				prop2.ItemId = anElementID + Application.Instance.PropertyDelimiter + "objectType2"
				prop2.Description = "Object Type2"

				aPopertyList.Add(prop)
				aPopertyList.Add(prop2)

			End If

			Return EnumResultCode.S_OK

		End Function

	End Class

End Namespace

