using System;

namespace Softing.OPCToolbox.Server
{
	/// <summary>
	/// Web template is the interface for OPC server's web interface. This class must be derived and <see cref="HandleWebTemplate"/> method must be overriden
	/// to provide customized behavior to the web server.
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="WebTemplate"]/doc/*'
	///	/>
	public class WebTemplate
	{
		#region //	Public Constructor
		//----------------------------

		/// <summary>
		/// Default constructor. No features implemented
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="WebTemplate"]/
		///		ctor[@name="WebTemplate"]/doc/*'
		///	/>
		public WebTemplate()
		{
		} //	end ctr


		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		///	 OPCToolbox callback handler for the HTTP server requests. This method must be overriden for the OPC server application usage.
		///	 The pattern is that the Web client provides a Template name with some arguments and the Template handler will build a result string
		///	 accordingly to the application's need.
		/// </summary>
		/// <param name="aTemplateName">
		/// name of the template to be resolved.
		/// </param>
		/// <param name="aNumArgs">
		/// the number of arguments of the Template.
		/// </param>
		/// <param name="anArgs">
		/// the string array with the arguments.
		/// </param>
		/// <param name="aResult">
		/// out string parameter to be filled by the method.
		/// </param>
		/// <returns>
		/// the execution result code
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="WebTemplate"]/
		///		method[@name="HandleWebTemplate"]/doc/*'
		///	/>
		public virtual int HandleWebTemplate(
			string aTemplateName,
			long aNumArgs,
			string[] anArgs,
			out string aResult)
		{
			aResult = string.Empty;
			return (int) EnumResultCode.E_NOTIMPL;
		} //	end HandleWebTemplate


		//-
		#endregion
	} //	end class WebTemplate
} //	end namespace Softing.OPCToolbox.Server