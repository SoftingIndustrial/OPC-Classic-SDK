using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Xml.Serialization;

namespace TestClientGui.Elements
{
	public enum TestResults
	{
		Success,
		Warning,
		Failed,
		Executing
	}

	public class TestResult
	{
		#region Public Properties
		[Category( "Common" ), ReadOnly( true )]
		public TestResults Type { get; set; }

		[Category( "Common" )]
		public string Details { get; set; }

		[Description( "The exception object that was raised" ), Category( "Common" )]
		[XmlIgnore]
		public Exception Error { get; set; }

		[System.ComponentModel.Browsable( false )]
		public string ErrorDetail
		{
			get
			{
				if( Error != null )
					return Error.ToString();
				else
					return null;
			}

			set
			{
				Error = new Exception( value );
			}
		}

		//try to use a TypeConverter
		[Category( "Common" ), ReadOnly( true )]
		[TypeConverter( typeof( ExpandableObjectConverter ) )]
		public DateTime StartTime { get; set; }

		[Category( "Common" ), ReadOnly( true )]
		[TypeConverter( typeof( ExpandableObjectConverter ) )]
		public DateTime EndTime { get; set; }

		[Category( "Common" ), ReadOnly( true )]
		[TypeConverter( typeof( ExpandableObjectConverter ) )]
		public object OutValue { get; set; }

		/// <summary>
		/// Gets a value indicating whether this instance is good.
		/// </summary>
		/// <value>
		///   <c>true</c> if this instance is good; otherwise, <c>false</c>.
		/// </value>
		[XmlIgnore]
		public bool IsGood
		{
			get
			{
				if( Type == TestResults.Success )
					return true;
				return false;
			}
		}
		#endregion

		#region Ctors
		public TestResult()
		{
		}

		public TestResult( TestResults type )
		{
			Type = type;

			if( type != TestResults.Executing )
			{
				EndTime = DateTime.Now;
			}
		}

		public TestResult( TestResults type, string message )
		{
			Type = type;
			Details = message;

			if( type != TestResults.Executing )
			{
				EndTime = DateTime.Now;
			}
		}

		public TestResult( Exception exception )
		{
			Type = TestResults.Failed;
			Details = "Unhandled exception";
			Error = exception;
			EndTime = DateTime.Now;
		}
		#endregion

		#region Convenience constructors
		public static TestResult Success()
		{
			return new TestResult( TestResults.Success );
		}

		public static TestResult Failed( string details )
		{
			return new TestResult( TestResults.Failed, details );
		}

		public static TestResult ParameterInvalid( string parameter, string details = null )
		{
			if( details == null )
				return TestResult.Failed( string.Format( "({0}) parameter is invalid", parameter ) );
			else
				return TestResult.Failed( string.Format( "({0}) parameter is invalid.\r\n{1}", parameter, details ) );
		}

		public static TestResult ParameterMissing( string parameter )
		{
			return TestResult.Failed( string.Format( "({0}) parameter is missing", parameter ) );
		}
		#endregion

		public TestResult Strip()
		{
			Type thisType = this.GetType();
			if( ( thisType == typeof( TestResult ) ) || ( thisType == typeof( MultipleTestResult ) ) || ( thisType == typeof( TestListTestResult ) )
				 || ( thisType == typeof( ForEachTestResult ) ) )
			{
				return this;
			}

			TestResult clone = new TestResult( this.Type, this.Details );
			clone.StartTime = this.StartTime;
			clone.EndTime = this.EndTime;
			clone.Error = this.Error;

			return clone;
		}
	}

	public class MultipleTestResult : TestResult
	{
		[Description( "List containing the TestResults of each test run" ), Category( "Common" )]
		public List<TestResult> InnerResults { get; set; }

		public MultipleTestResult()
		{
		}

		public MultipleTestResult( TestResults type )
			: base( type )
		{
		}

		static MultipleTestResult()
		{
			Type[] types = System.Reflection.Assembly.GetExecutingAssembly().GetTypes();
			XmlAttributes listAttribs = new XmlAttributes();

			foreach( var type in types )
			{
				if( typeof( TestResult ).IsAssignableFrom( type ) )
				{
					listAttribs.XmlArrayItems.Add( new XmlArrayItemAttribute( type ) );
				}
			}

			TestList.m_aor.Add( typeof( MultipleTestResult ), "InnerResults", listAttribs );
		}
	}
}
