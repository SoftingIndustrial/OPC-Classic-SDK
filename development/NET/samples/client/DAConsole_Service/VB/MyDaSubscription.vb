Imports System
Imports System.IO
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace DaConsole_Service
	Public Class MyDaSubscription
		Inherits DaSubscription

#Region "Constructor"
		'-----------------

		Public Sub New(ByVal updateRate As UInt32, ByVal parentSession As MyDaSession)
			MyBase.New(updateRate, parentSession)
			AddHandler DataChanged, AddressOf HandleDataChanged
			AddHandler StateChangeCompleted, AddressOf HandleStateChanged
			AddHandler ReadCompleted, AddressOf HandleSubscriptionReadCompleted
			AddHandler WriteCompleted, AddressOf HandleSubscriptionWriteCompleted
			AddHandler PerformStateTransitionCompleted, AddressOf HandlePerformStateTransition
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

		Public Shared Sub HandleStateChanged(ByVal sender As ObjectSpaceElement, ByVal state As EnumObjectState)
			Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
			Dim streamWriter As New StreamWriter(fs)
			streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
			streamWriter.WriteLine(sender.ToString() + " State Changed - " + state.ToString())
			streamWriter.Flush()
			streamWriter.Close()
		End Sub


		Public Shared Sub HandleDataChanged(ByVal daSubscription As DaSubscription, ByVal items As DaItem(), ByVal values As ValueQT(), ByVal results As Integer())
			Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
			Dim streamWriter As New StreamWriter(fs)
			streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
			streamWriter.WriteLine()
			streamWriter.WriteLine(daSubscription.ToString())
			Dim i As Integer = 0
			While i < items.Length
				If ResultCode.SUCCEEDED(results(i)) Then
					streamWriter.WriteLine([String].Format("{0,-19} {1} {2,-50} ", items(i).Id.ToString(), "-", values(i).ToString()))
				Else
					streamWriter.WriteLine("Receiving item value failed! " + " Item: " + items(i).Id.ToString())
				End If
				System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
			End While
			streamWriter.WriteLine()
			streamWriter.Flush()
			streamWriter.Close()
		End Sub


		Public Shared Sub HandleSubscriptionReadCompleted(ByVal daSubscription As DaSubscription, ByVal executionContext As UInt32, ByVal items As DaItem(), ByVal values As ValueQT(), ByVal results As Integer(), ByVal result As Integer)
			Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
			Dim streamWriter As New StreamWriter(fs)
			streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
			If ResultCode.SUCCEEDED(result) Then
				streamWriter.WriteLine(daSubscription.ToString() + " asynchronous read succeeded! ")

				Dim i As Integer = 0
				While i < values.Length
					If ResultCode.SUCCEEDED(results(i)) Then
						streamWriter.WriteLine([String].Format("{0,-19} {1} {2,-50} ", items(i).Id.ToString(), "-", values(i).ToString()))
					Else
						streamWriter.WriteLine(" Item read failed!" + items(i).Id.ToString() + " [ResultID: " + results(i).ToString() + " ]")
					End If
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
			Else
				streamWriter.WriteLine(" Subscription asynchronous read failed! Result: " + result.ToString())
			End If
			streamWriter.Flush()
			streamWriter.Close()
		End Sub


		Public Shared Sub HandleSubscriptionWriteCompleted(ByVal daSubscription As DaSubscription, ByVal executionContext As UInt32, ByVal items As DaItem(), ByVal values As ValueQT(), ByVal results As Integer(), ByVal result As Integer)
			Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
			Dim streamWriter As New StreamWriter(fs)
			streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
			If ResultCode.SUCCEEDED(result) Then
				streamWriter.WriteLine(daSubscription.ToString() + " asynchronous write succeeded! ")
				Dim i As Integer = 0
				While i < items.Length
					If ResultCode.SUCCEEDED(results(i)) Then
						streamWriter.WriteLine(" " + items(i).Id)
					Else
						streamWriter.WriteLine(" Subscription asynchronous write failed! " + items(i).Id.ToString() + " [ResultID: " + results(i).ToString() + " ]")
					End If
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
			Else
				streamWriter.WriteLine(" Subscription asynchronous write failed! Result: " + result.ToString())
			End If
			streamWriter.Flush()
			streamWriter.Close()
		End Sub


		Public Shared Sub HandlePerformStateTransition(ByVal sender As ObjectSpaceElement, ByVal executionContext As UInt32, ByVal result As Integer)
			Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
			Dim streamWriter As New StreamWriter(fs)
			streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
			If ResultCode.SUCCEEDED(result) Then
				streamWriter.WriteLine(sender.ToString() + " Performed state transition - " + executionContext.ToString())
			Else
				streamWriter.WriteLine(sender.ToString() + "  Performed state transition failed! Result: " + result.ToString())
			End If
			streamWriter.Flush()
			streamWriter.Close()

		End Sub


		'--
#End Region

	End Class

End Namespace

