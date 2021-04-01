'-----------------------------------------------------------------------------
'                                                                            |
'                   Softing Industrial Automation GmbH                       |
'                        Richard-Reitzner-Allee 6                            |
'                           85540 Haar, Germany                              |
'                                                                            |
'                 This is a part of the Softing OPC Toolkit                  |
'        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
'                           All Rights Reserved                              |
'                                                                            |
'-----------------------------------------------------------------------------
'-----------------------------------------------------------------------------
'                             OPC Toolkit NET                                |
'                                                                            |
'  Filename    : OutProc.vb                                                  |
'  Version     : 4.47.1                                                      |
'  Date        : 27-July-2020                                                |
'                                                                            |
'  Description : OPC OutProc main class implementation                       |
'                                                                            |
'-----------------------------------------------------------------------------
Imports System

Namespace OPCEnumerate
	''' <summary>
	''' Summary description for OutProc.
	''' </summary>
	Public Class OutProc
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
				m_opcClient = New OpcClient
			End If
		End Sub
		'--
#End Region

	End Class
End Namespace

