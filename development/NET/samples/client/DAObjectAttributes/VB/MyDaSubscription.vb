Imports System
Imports Softing.OPCToolbox.Client

Namespace DAObjectAttributes
	Public Class MyDaSubscription
		Inherits DaSubscription

#Region "Constructor"
		'-----------------

		Public Sub New(ByVal updateRate As UInt32, ByVal parentSession As MyDaSession)
			MyBase.New(updateRate, parentSession)
		End Sub

		'--
#End Region

#Region "Private Members"
		'---------------------

#Region "Private Attributes"
		'------------------------


		'--
#End Region


		'--
#End Region

#Region "Public Methods"
		'---------------------


		'--
#End Region

#Region "Public Properties"
		'-----------------------


		'--
#End Region

#Region "Handles"


		'--
#End Region

	End Class

End Namespace

