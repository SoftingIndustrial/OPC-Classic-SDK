using System;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace Console
{
	class MyWebTemplate : WebTemplate
	{

		public override int HandleWebTemplate(string aTemplateName, long aNumArgs, string[] anArgs, out string aResult)
		{

			aResult = string.Empty;

			if (aTemplateName.StartsWith("CURRENTTIME"))
			{

				bool isGerman = false;

				if (aNumArgs >= 1)
				{
					isGerman = (anArgs[0] == "GERMAN");
				}   //	end if

				DateTime now = DateTime.Now;
				System.Globalization.DateTimeFormatInfo format = null;
				if (isGerman)
				{
					format = new System.Globalization.CultureInfo("de-DE", false).DateTimeFormat;
				}
				else
				{
					format = new System.Globalization.CultureInfo("en-US", false).DateTimeFormat;
				}   //	end if ... else

				aResult = now.ToString("G", format);
				return (int)EnumResultCode.S_OK;
			}   //	end if 

			return (int)EnumResultCode.E_NOTIMPL;

		}   //	end HandleWebTemplate

	}   //	end class WebTemplate
}
