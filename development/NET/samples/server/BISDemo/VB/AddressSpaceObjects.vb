Imports System
Imports System.Collections
Imports System.Xml
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace BISDemo

	Public Class DataPointTypeProperty
		Inherits DaProperty

#Region "//  Protected constants"

		''' <summary>
		'''  the ID for the DataPoint type property
		''' </summary>    
		Protected Const DATAPOINT_TYPE_PROPERTY_ID As Integer = 5001

		Protected Const DATAPOINT_TYPE_PROPERTY_NAME As String = "DataPointType"

#End Region

#Region "//  Protected Attributes"

		Protected m_dataPointTypeValue As Integer = 0

#End Region

#Region " //  Public Constructors"

		Public Sub New(ByVal aDataPointTypeId As Integer)
			MyClass.New(aDataPointTypeId, DATAPOINT_TYPE_PROPERTY_NAME)
		End Sub


		Public Sub New(ByVal aDataPointTypeId As Integer, ByVal aName As String)
			MyClass.New(aDataPointTypeId, aName, aName)
		End Sub


		Public Sub New(ByVal aDataPointTypeId As Integer, ByVal aName As String, ByVal aDescription As String)

			System.Diagnostics.Debug.Assert(aDataPointTypeId >= DATAPOINT_TYPE_PROPERTY_ID, "The aDataPointType is not valid")
			Me.Id = DATAPOINT_TYPE_PROPERTY_ID
			Me.Name = aName
			Me.ItemId = aName
			Me.Description = aDescription
			Me.Datatype = GetType(Integer)
			Me.AccessRights = EnumAccessRights.READABLE
			Me.m_dataPointTypeValue = aDataPointTypeId

		End Sub

#End Region

#Region "//  Public Methods"

		Public Overloads Overrides Sub HandleReadRequest(ByVal aRequest As DaRequest)

			System.Diagnostics.Debug.Assert(aRequest.PropertyId = Me.Id, "HandleRequest called with illegal request")

			'  Meant to be obverriden, just return failed
			aRequest.Result = [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.S_OK)
			aRequest.Value = New ValueQT(Me.m_dataPointTypeValue, [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now)
			aRequest.Complete()

		End Sub

#End Region

	End Class

	Public Class CommandProperty
		Inherits DataPointTypeProperty

#Region "//  ProtectedConstants"

		Protected XML_CMD_NAMESPACE As String = "file:///S3K/Proxyverwalter"

#End Region

#Region "//  Protected Attributes"

		Protected ReadOnly m_xmlDoc As XmlDocument = New XmlDocument
		Protected ReadOnly m_xmlCommandNode As XmlNode = Nothing

#End Region

#Region " //  Public Constructors"

		Public Sub New(ByVal aDataPointId As Integer, ByVal aName As String)
			MyBase.New(aDataPointId)

			Me.Id = aDataPointId
			Me.ItemId = aDataPointId.ToString
			Me.Name = aName
			Me.Description = aName
			Me.Datatype = GetType(String)
			Me.AccessRights = EnumAccessRights.READABLE

			Dim declaration As XmlDeclaration = m_xmlDoc.CreateXmlDeclaration("1.0", System.Text.Encoding.UTF8.WebName, Nothing)
			m_xmlDoc.InsertBefore(declaration, m_xmlDoc.DocumentElement)

			m_xmlCommandNode = m_xmlDoc.CreateNode(XmlNodeType.Element, "nsPV:Commands", XML_CMD_NAMESPACE)
			m_xmlDoc.AppendChild(m_xmlCommandNode)

		End Sub

#End Region

#Region "//  Public Methods"

		Public Overloads Overrides Sub HandleReadRequest(ByVal aRequest As DaRequest)

			System.Diagnostics.Debug.Assert(aRequest.PropertyId = Me.Id, "HandleRequest called with illegal request")

			aRequest.Result = [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.S_OK)
			aRequest.Value = New ValueQT(Me.CommandValue, [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now)
			aRequest.Complete()

		End Sub


		Public Sub AddCommand(ByVal aName As String, ByVal aDescription As String, ByVal anAddress As String)

			Dim innerCommand As XmlNode = Me.m_xmlDoc.CreateNode(XmlNodeType.Element, "nsPV:Command", XML_CMD_NAMESPACE)

			Dim attribute As XmlAttribute = Me.m_xmlDoc.CreateAttribute("Name")
			attribute.Value = aName
			innerCommand.Attributes.Append(attribute)

			attribute = Me.m_xmlDoc.CreateAttribute("Description")
			attribute.Value = aDescription
			innerCommand.Attributes.Append(attribute)

			attribute = Me.m_xmlDoc.CreateAttribute("Address")
			attribute.Value = anAddress
			innerCommand.Attributes.Append(attribute)

			Me.m_xmlCommandNode.AppendChild(innerCommand)

		End Sub

		Public Sub AddCommand(ByVal aName As String, ByVal aDescription As String)
			Me.AddCommand(aName, aDescription, String.Empty)
		End Sub

#End Region

#Region "//  Protected Properties"

		Protected ReadOnly Property CommandValue() As String
			Get
				Return m_xmlDoc.InnerXml
			End Get
		End Property

#End Region

	End Class


	Public Class BaseEvent
		Inherits AeEvent

#Region "//  Public Constructors"

		Public Sub New(ByVal aSource As String, ByVal aCategoryId As System.UInt32)
			MyBase.New(EnumEventType.SIMPLE, aSource, aCategoryId)

		End Sub

#End Region

	End Class

	Public Class DataEvent
		Inherits BaseEvent

#Region " //  Protected Class Attributes"

		Protected Shared CATEGORY_DATA As AeCategory = New AeCategory(EnumEventType.SIMPLE, Convert.ToUInt32(1), "Data")

#End Region

#Region "//  Public Class Methods"

		Public Shared Sub BuildCategory()

			Application.Instance.AddAeCategory(CATEGORY_DATA)
			CATEGORY_DATA.AddAttribute(Convert.ToUInt32(5550), "CV", GetType(Integer))
			CATEGORY_DATA.AddAttribute(Convert.ToUInt32(5551), "DATA", GetType(Integer))
			CATEGORY_DATA.AddAttribute(Convert.ToUInt32(5552), "ADDRESS", GetType(String))
			CATEGORY_DATA.AddAttribute(Convert.ToUInt32(5553), "TEXT", GetType(String))

		End Sub

#End Region

#Region "//  Public Constructors"
		Public Sub New(ByVal aSource As String, ByVal aCvAttribute As Object, ByVal aDataAttribute As Integer, ByVal anAddressAttribute As String, ByVal aTextAttribute As String)
			MyBase.New(aSource, CATEGORY_DATA.Id)

			Me.AttributeValueList.Add(aCvAttribute)
			Me.AttributeValueList.Add(aDataAttribute)
			Me.AttributeValueList.Add(anAddressAttribute)
			Me.AttributeValueList.Add(aTextAttribute)

		End Sub

#End Region

	End Class


	Public Class DeviceEvent
		Inherits BaseEvent

#Region " //  Protected Class Attributes"

		Protected Shared CATEGORY_DEVICE As AeCategory = New AeCategory(EnumEventType.SIMPLE, Convert.ToUInt32(2), "Device")

#End Region

#Region "//  Public Methods"

		Public Shared Sub BuildCategory()
			Application.Instance.AddAeCategory(CATEGORY_DEVICE)
			CATEGORY_DEVICE.AddAttribute(Convert.ToUInt32(5550), "CV", GetType(Integer))
		End Sub

#End Region

#Region "//  Public Constructors"

		Public Sub New(ByVal aSource As String, ByVal aCvAttribute As Object)
			MyBase.New(aSource, CATEGORY_DEVICE.Id)

			Me.AttributeValueList.Add(aCvAttribute)

		End Sub

#End Region

	End Class


	Public Class BaseAeElemenet
		Inherits AeAddressSpaceElement

#Region "// Public constructors"

		Public Sub New(ByVal aName As String, ByVal anUserData As UInt32, ByVal anObjectHandle As UInt32, ByVal aParentHandle As UInt32)
			MyBase.New(aName, anUserData, anObjectHandle, aParentHandle)

		End Sub


		Public Sub New(ByVal aName As String, ByVal hasChildren As Boolean)

			Me.m_name = aName
			Me.m_hasChildren = hasChildren

		End Sub

#End Region

#Region "//  Public Methods"

		Public Overloads Overrides Function QueryConditions(ByVal aSourcePath As String, ByRef aConditionNames As String()) As System.Int32

			aConditionNames = Nothing
			Return Convert.ToInt32(EnumResultCode.S_OK)

		End Function

#End Region

	End Class


	Public Class DaBaseElement
		Inherits DaAddressSpaceElement

#Region "//  Public Constructors"

		Public Sub New(ByVal anItemID As String, ByVal aName As String, ByVal anUserData As UInt32, ByVal anObjectHandle As UInt32, ByVal aParentHandle As UInt32)
			MyBase.New(anItemID, aName, anUserData, anObjectHandle, aParentHandle)
		End Sub


		Public Sub New()

		End Sub

#End Region

#Region "//  Public Methods"

		Public Overridable Sub HandleReadRequest(ByVal aRequest As DaRequest)

			If Not (aRequest.PropertyId = 0) Then

				Dim daProperty As DaProperty = CType(m_properties(aRequest.PropertyId), DaProperty)
				If Not (daProperty Is Nothing) Then
					daProperty.HandleReadRequest(aRequest)
				Else
					aRequest.Result = [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.E_FAIL)
					aRequest.Value = Nothing
					aRequest.Complete()
				End If
			Else
				HandleItemReadRequest(aRequest)
			End If

		End Sub


		Public Overridable Sub HandleWriteRequest(ByVal aRequest As DaRequest)

			If Not (aRequest.PropertyId = 0) Then

				Dim daProperty As DaProperty = CType(m_properties(aRequest.PropertyId), DaProperty)

				If Not (daProperty Is Nothing) Then
					daProperty.HandleWriteRequest(aRequest)
				Else
					aRequest.Result = [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.E_FAIL)
					aRequest.Complete()
				End If
			Else
				HandleItemWriteRequest(aRequest)
			End If

		End Sub


		Public Overloads Overrides Function QueryProperties(ByRef aPropertyList As ArrayList) As System.Int32

			If m_properties.Count > 0 Then
				aPropertyList = New ArrayList()
				aPropertyList.AddRange(m_properties.Values)
			Else
				aPropertyList = Nothing
			End If

			Return Convert.ToInt32(EnumResultCode.S_OK)

		End Function


		Public Overridable Function AddProperty(ByVal aProperty As DaProperty) As System.UInt32

			m_properties.Add(aProperty.Id, aProperty)
			Return Convert.ToUInt32(EnumResultCode.S_OK)

		End Function


		Public Overridable Function InitAe(ByVal anAeParent As IAddressSpaceElement) As Boolean

			Dim result As Boolean = True

			If m_aeElement Is Nothing Then
				Me.m_aeElement = New BaseAeElemenet(Me.Name, Me.HasChildren)
				anAeParent.AddChild(Me.m_aeElement)
			End If

			If Me.HasChildren Then

				Dim children As ICollection = Me.GetChildren
				For Each element As Object In children
					Dim daElement As DaBaseElement = CType(element, DaBaseElement)
					If Not (daElement Is Nothing) Then
						result = daElement.InitAe(m_aeElement) AndAlso result
					End If
				Next
			End If

			Return result

		End Function


		Public Overridable Sub Simulate()

			If Me.HasChildren Then

				Dim children As ICollection = Me.GetChildren
				For Each element As Object In children
					Dim daElement As DaBaseElement = CType(element, DaBaseElement)
					If Not (daElement Is Nothing) Then
						daElement.Simulate()
					End If
				Next
			End If

			m_simulationOn = Microsoft.VisualBasic.IIf(m_simulationOn, False, True)

		End Sub

#End Region

#Region "//  Protected Methods"

		Protected Overridable Sub HandleItemReadRequest(ByVal aRequest As DaRequest)
			Dim cacheValue As ValueQT = Nothing
			Me.GetCacheValue(cacheValue)
			aRequest.Value = cacheValue
			aRequest.Result = [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.S_OK)
			aRequest.Complete()
		End Sub

		Protected Overridable Sub HandleItemWriteRequest(ByVal aRequest As DaRequest)

			aRequest.Result = [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.E_BADRIGHTS)
			aRequest.Complete()

		End Sub

#End Region

#Region " //  Private Attributes"

		Private m_properties As Hashtable = New Hashtable

		Private m_aeElement As BaseAeElemenet = Nothing

		Protected Shared m_simulationOn As Boolean = True

#End Region

	End Class

	Public Class DataPointElement
		Inherits DaBaseElement

#Region "//  Public Constructors"

		Public Sub New(ByVal anItemID As String, ByVal aName As String, ByVal anUserData As UInt32, ByVal anObjectHandle As UInt32, ByVal aParentHandle As UInt32)
			MyBase.New(anItemID, aName, anUserData, anObjectHandle, aParentHandle)

			Me.HasChildren = False
			Me.IoMode = EnumIoMode.POLL
			Me.AccessRights = EnumAccessRights.READWRITEABLE
			Me.Datatype = GetType(Integer)

		End Sub


		Public Sub New(ByVal aName As String, ByVal aDataTypeId As Integer, ByVal aDescription As String)
			MyBase.New()

			Me.Name = aName
			Me.HasChildren = False
			Me.IoMode = EnumIoMode.POLL
			Me.AccessRights = EnumAccessRights.READWRITEABLE
			Me.Datatype = GetType(Integer)

			Dim value As ValueQT

			value = New ValueQT(aDescription, [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now)

			Me.AddProperty(New DaConstantProperty(101, "Description", "Description", value))
			Me.AddProperty(New DataPointTypeProperty(aDataTypeId, "DataPointType", "DataPointType description"))

		End Sub

#End Region

#Region "//  Public Methods"

		Public Overloads Function ValueChanged(ByVal aDataCv As Integer, ByVal anInternalData As Integer, ByVal anAddress As String, ByVal aTextDescription As String) As System.Int32

			Try

				Dim cacheValue As ValueQT = Nothing

				Me.GetCacheValue(cacheValue)
				If Not (cacheValue Is Nothing) Then
					If cacheValue.Quality.Equals(EnumQuality.GOOD) AndAlso Convert.ToInt32(cacheValue.Data) = aDataCv Then
						Return Convert.ToInt32(EnumResultCode.S_OK)
					End If
				End If

				cacheValue = New ValueQT(aDataCv, [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now)

				'  Notify value changed to the Da Element
				MyBase.ValueChanged(cacheValue)

				'  Create a Datas change event and fire it
				Dim dataEvent As DataEvent = New DataEvent(Me.ItemId, cacheValue.Data, anInternalData, anAddress, aTextDescription)
				dataEvent.Message = aDataCv.ToString
				dataEvent.Severity = Convert.ToUInt32(500)
				dataEvent.ActorId = "OPC User"
				Return dataEvent.Fire
			Catch
				Return Convert.ToInt32(EnumResultCode.E_FAIL)
			End Try
		End Function


		Public Function ExecuteCommand(ByVal anAddress As String, ByVal aCommand As String, ByVal aDescription As String) As System.Int32

			Return Me.ValueChanged(Convert.ToInt32(aCommand), 0, anAddress, aDescription)

		End Function


		Public Overloads Overrides Sub Simulate()

			Try
				If m_simulationOn Then

					Dim simulationCV As Integer = 0
					Dim cacheValue As ValueQT = Nothing
					Me.GetCacheValue(cacheValue)
					If Not (cacheValue Is Nothing) Then

						simulationCV = Convert.ToInt32(cacheValue.Data) + 1
						If simulationCV > 5 Then
							simulationCV = 0
						End If

						Me.ValueChanged(simulationCV, 0, "simulation Address", "simulation Text Description")

					End If
				End If
			Finally
				MyBase.Simulate()
			End Try

		End Sub


#End Region

	End Class


	Public Class CommandElement
		Inherits DaBaseElement

#Region "//  Protected Constants"

		Protected Const COMMAND_NAME As String = "CMD"

#End Region

#Region " // Public Constructors"

		Public Sub New()
			MyClass.New(COMMAND_NAME)
		End Sub

		Public Sub New(ByVal aName As String)
			MyBase.New()

			Me.HasChildren = False
			Me.Name = aName
			Me.AccessRights = EnumAccessRights.READWRITEABLE
			Me.IoMode = EnumIoMode.POLL
			Me.Datatype = GetType(String)

		End Sub

#End Region

#Region "//  Protected Methods"

		Protected Overloads Overrides Sub HandleItemWriteRequest(ByVal aRequest As DaRequest)


			Try
				'  start parsing the received command
				aRequest.Result = [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.E_FAIL)

				Dim doc As XmlDocument = New XmlDocument
				doc.LoadXml(CType(aRequest.Value.Data, String))

				For Each commands As XmlNode In doc.ChildNodes

					'  locate the command 
					If commands.LocalName.ToLower = "commands" Then
						For Each command As XmlNode In commands.ChildNodes
							If command.LocalName.ToLower = "command" Then
								Dim commandAddress As String = command.Attributes("Address").Value
								Dim commandName As String = command.Attributes("Name").Value
								Dim commandDescription As String = command.Attributes("Description").Value
								aRequest.Result = [Enum].ToObject(GetType(EnumResultCode), Me.ExecuteCommand(commandAddress, commandName, commandDescription))
								If ResultCode.SUCCEEDED(aRequest.Result) Then
									aRequest.Result = [Enum].ToObject(GetType(EnumResultCode), Me.ValueChanged(aRequest.Value))
								End If
								' break
							End If
						Next
					End If
				Next
			Catch
				aRequest.Result = [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.E_FAIL)
			Finally
				aRequest.Complete()
			End Try

		End Sub


		Protected Function ExecuteCommand(ByVal anAddress As String, ByVal aCommand As String, ByVal aDescription As String) As System.Int32

			Try

				'  Delegate command execution to the application
				Dim targetElement As AddressSpaceElement = Nothing
				Application.Instance.DaAddressSpaceRoot.QueryAddressSpaceElementData(anAddress, targetElement)

				Dim element As DataPointElement = CType(targetElement, DataPointElement)

				If element Is Nothing Then
					'  the targeted element was not found
					Return Convert.ToInt32(EnumResultCode.E_FAIL)
				End If

				Return element.ExecuteCommand(anAddress, aCommand, aDescription)
			Catch
				Return Convert.ToInt32(EnumResultCode.E_FAIL)
			End Try

		End Function

#End Region

	End Class

	Public Class DeviceElement
		Inherits DaBaseElement

#Region " //  Public constructors"

		Public Sub New(ByVal aName As String, ByVal aDataTypeId As Integer, ByVal aDescription As String)
			MyBase.New()

			Me.AccessRights = EnumAccessRights.READABLE
			Me.Datatype = GetType(Integer)
			Me.IoMode = EnumIoMode.REPORT
			Me.Name = aName
			Me.HasChildren = False
			Me.IoMode = EnumIoMode.POLL
			Dim value As ValueQT
			value = New ValueQT(aDescription, [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now)

			Me.AddProperty(New DaConstantProperty(101, "Description", "Description", value))
			Me.AddProperty(New DataPointTypeProperty(aDataTypeId))

		End Sub

#End Region

#Region "//  Public Methods"

		Public Property Connected() As Boolean
			Get
				Dim cacheValue As ValueQT = Nothing
				Me.GetCacheValue(cacheValue)
				If Not (cacheValue Is Nothing) AndAlso cacheValue.Quality.Equals(EnumQuality.GOOD) Then
					Return Convert.ToBoolean(cacheValue.Data)
				Else
					Return False
				End If
			End Get
			Set(ByVal Value As Boolean)

				Dim cacheValue As ValueQT = Nothing
				Me.GetCacheValue(cacheValue)
				If Not (cacheValue Is Nothing) Then
					If cacheValue.Quality.Equals(EnumQuality.GOOD) AndAlso Convert.ToBoolean(cacheValue.Data) = Value Then
						Return
					End If
				End If

				cacheValue = New ValueQT(Convert.ToInt32(Value), [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now)

				'  Notify value changed to the Da Element
				Me.ValueChanged(cacheValue)

				'  Create a State change event and fire it
				Dim deviceState As DeviceEvent = New DeviceEvent(Me.ItemId, cacheValue.Data)
				deviceState.ActorId = "OPC User"
				If Value Then
					deviceState.Message = "Connected"
					deviceState.Severity = Convert.ToUInt32(1)
				Else
					deviceState.Message = "Disconnected"
					deviceState.Severity = Convert.ToUInt32(1000)
				End If
				deviceState.Fire()
			End Set
		End Property


		Public Overloads Overrides Sub Simulate()
			Try
				If m_simulationOn Then
					Dim cacheValue As ValueQT = Nothing
					Me.GetCacheValue(cacheValue)
					If Not (cacheValue Is Nothing) Then
						Dim simulationCV As Boolean = Convert.ToBoolean(cacheValue.Data)
						simulationCV = Microsoft.VisualBasic.IIf(simulationCV, False, True)
						Connected = simulationCV
					End If
				End If
			Finally
				MyBase.Simulate()
			End Try
		End Sub

#End Region

	End Class

End Namespace

