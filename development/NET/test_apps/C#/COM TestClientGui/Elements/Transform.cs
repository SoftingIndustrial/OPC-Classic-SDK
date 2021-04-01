using System;
using System.Xml.Serialization;
using System.Collections.Generic;
using Softing.OPCToolbox;

namespace TestClientGui.Elements
{
	public enum PropertyTransformation
	{
		ValuesQTIncrement,
		ValuesQTSetValue,
		ValuesQTSetQuality,
		ValuesQTSetTimestamp,
		NumericIncrement
	}

	public class Transform : Element
	{
		public PropertyTransformation Transformation { get; set; }

		public string Property { get; set; }

		public object Parameter { get; set; }

		public string ParameterProperty { get; set; }

		public override TestResult Execute()
		{
			switch( Transformation )
			{
				case PropertyTransformation.ValuesQTIncrement:
					return TransformValuesQTIncrement();
				case PropertyTransformation.ValuesQTSetValue:
					return TransformValuesQTSetValue();
				case PropertyTransformation.ValuesQTSetQuality:
					return TransformValuesQTSetQuality();
				case PropertyTransformation.ValuesQTSetTimestamp:
					return TransformValuesQTSetTimeStamp();
				case PropertyTransformation.NumericIncrement:
					return TransformNumericIncrement();
				default:
					return TestResult.ParameterInvalid( "Transformation" );
			}
		}

		private TestResult TransformValuesQTIncrement()
		{
			if( string.IsNullOrEmpty( Property ) )
				return TestResult.ParameterMissing( "Property" );

			ValueQT[] values = GetProperty( Property ) as ValueQT[];
			if( values == null )
			{
				ValueQT value = GetProperty( Property ) as ValueQT;
				if( value == null )
					return TestResult.ParameterInvalid( "Property" );

				values = new ValueQT[] { value };
			}

			object parameter = Parameter != null ? Parameter : GetProperty( ParameterProperty );
			if( parameter == null )
				return TestResult.ParameterMissing( "Parameter" );

			double increment;
			if( !double.TryParse( parameter.ToString(), out increment ) )
				return TestResult.ParameterInvalid( "Parameter", "Should be a numeric value representing the increment." );

			foreach( var val in values )
			{
				TestResult testResult = ValueQTIncrement( val, increment );
				if( !testResult.IsGood )
					return testResult;
			}

			return TestResult.Success();
		}

		private static TestResult ValueQTIncrement( ValueQT value, double increment )
		{
			if( value.Data is double )
				value.SetData( (double) ( (double) value.Data + increment ), value.Quality, value.TimeStamp );
			else if( value.Data is float )
				value.SetData( (float) ( (float) value.Data + increment ), value.Quality, value.TimeStamp );
			else if( value.Data is int )
				value.SetData( (int) ( (int) value.Data + increment ), value.Quality, value.TimeStamp );
			else if( value.Data is uint )
				value.SetData( (uint) ( (uint) value.Data + increment ), value.Quality, value.TimeStamp );
			else if( value.Data is short )
				value.SetData( (short) ( (short) value.Data + increment ), value.Quality, value.TimeStamp );
			else if( value.Data is ushort )
				value.SetData( (ushort) ( (ushort) value.Data + increment ), value.Quality, value.TimeStamp );
			else if( value.Data is sbyte )
				value.SetData( (sbyte) ( (sbyte) value.Data + increment ), value.Quality, value.TimeStamp );
			else if( value.Data is byte )
				value.SetData( (byte) ( (byte) value.Data + increment ), value.Quality, value.TimeStamp );
			else if( value.Data is bool )
				value.SetData( !(bool) value.Data, value.Quality, value.TimeStamp );
			else
			{
				return TestResult.ParameterInvalid( "Property", "The ValueQT's type is not supported for this operation" );
			}

			return TestResult.Success();
		}


		private TestResult TransformValuesQTSetValue()
		{
			if( string.IsNullOrEmpty( Property ) )
				return TestResult.ParameterMissing( "Property" );

			ValueQT[] values = GetProperty( Property ) as ValueQT[];
			if( values == null )
			{
				ValueQT value = GetProperty( Property ) as ValueQT;
				if( value == null )
					return TestResult.ParameterInvalid( "Property" );

				values = new ValueQT[] { value };
			}

			object parameter = Parameter != null ? Parameter : GetProperty( ParameterProperty );
			if( parameter == null )
				return TestResult.ParameterMissing( "Parameter" );

			double valueToSet;
			if( !double.TryParse( parameter.ToString(), out valueToSet ) )
				return TestResult.ParameterInvalid( "Parameter", "Should be a numeric value representing the value to set." );

			foreach( var val in values )
			{
				TestResult testResult = ValueQTSetValue( val, valueToSet );
				if( !testResult.IsGood )
					return testResult;
			}

			return TestResult.Success();
		}

		private static TestResult ValueQTSetValue( ValueQT value, double increment )
		{
			if( value.Data is double )
				value.SetData( (double) ( (double) increment ), value.Quality, value.TimeStamp );
			else if( value.Data is float )
				value.SetData( (float) ( (float) increment ), value.Quality, value.TimeStamp );
			else if( value.Data is int )
				value.SetData( (int) ( (int) increment ), value.Quality, value.TimeStamp );
			else if( value.Data is uint )
				value.SetData( (uint) ( (uint) increment ), value.Quality, value.TimeStamp );
			else if( value.Data is short )
				value.SetData( (short) ( (short) increment ), value.Quality, value.TimeStamp );
			else if( value.Data is ushort )
				value.SetData( (ushort) ( (ushort) increment ), value.Quality, value.TimeStamp );
			else if( value.Data is sbyte )
				value.SetData( (sbyte) ( (sbyte) increment ), value.Quality, value.TimeStamp );
			else if( value.Data is byte )
				value.SetData( (byte) ( (byte) increment ), value.Quality, value.TimeStamp );
			else if( value.Data is bool )
				value.SetData( (bool) (increment % 2 == 0), value.Quality, value.TimeStamp );
			else
			{
				return TestResult.ParameterInvalid( "Property", "The ValueQT's type is not supported for this operation" );
			}

			return TestResult.Success();
		}


		private TestResult TransformValuesQTSetQuality()
		{
			if( string.IsNullOrEmpty( Property ) )
				return TestResult.ParameterMissing( "Property" );

			ValueQT[] values = GetProperty( Property ) as ValueQT[];
			if( values == null )
			{
				ValueQT value = GetProperty( Property ) as ValueQT;
				if( value == null )
					return TestResult.ParameterInvalid( "Property" );

				values = new ValueQT[] { value };
			}

			object parameter = Parameter != null ? Parameter : GetProperty( ParameterProperty );
			if( parameter == null )
				return TestResult.ParameterMissing( "Parameter" );

			EnumQuality quality;
			if( !Enum.TryParse( parameter.ToString(), out quality ) )
				return TestResult.ParameterInvalid( "Parameter", "Should be a numeric value representing the value to set." );

			foreach( var val in values )
			{
				val.SetData( val.Data, quality, val.TimeStamp );
			}

			return TestResult.Success();
		}


		private TestResult TransformValuesQTSetTimeStamp()
		{
			if( string.IsNullOrEmpty( Property ) )
				return TestResult.ParameterMissing( "Property" );

			ValueQT[] values = GetProperty( Property ) as ValueQT[];
			if( values == null )
			{
				ValueQT value = GetProperty( Property ) as ValueQT;
				if( value == null )
					return TestResult.ParameterInvalid( "Property" );

				values = new ValueQT[] { value };
			}

			object parameter = Parameter != null ? Parameter : GetProperty( ParameterProperty );
			if( parameter == null )
				return TestResult.ParameterMissing( "Parameter" );

			DateTime timestamp;
			if( !DateTime.TryParse( parameter.ToString(), out timestamp ) )
				return TestResult.ParameterInvalid( "Parameter", "Should be a numeric value representing the value to set." );

			foreach( var val in values )
			{
				val.SetData( val.Data, val.Quality, timestamp );
			}

			return TestResult.Success();
		}


		private TestResult TransformNumericIncrement()
		{
			if( string.IsNullOrEmpty( Property ) )
				return TestResult.ParameterMissing( "Property" );

			object value = GetProperty( Property );

			double increment;
			if( !double.TryParse( Parameter.ToString(), out increment ) )
				return TestResult.ParameterInvalid( "Parameter", "Should be a numeric value representing the increment." );

			ValueQT valueQT = new ValueQT( value, EnumQuality.GOOD, System.DateTime.MinValue );

			TestResult testResult = ValueQTIncrement( valueQT, increment );
			if( !testResult.IsGood )
				return testResult;

			//replace property
			SetProperty( Property, valueQT.Data );

			return TestResult.Success();
		}


	}
}
