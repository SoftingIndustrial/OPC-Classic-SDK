using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Softing.OPCToolbox;

namespace DemoClient.DockWindows
{
    public partial class TraceWindow : ToolWindow
    {
        #region Private and Protected Methods

        private string StrTraceLevel(EnumTraceLevel traceLevel)
        {
            switch(traceLevel)
            {
                case EnumTraceLevel.DEB:
                    return "D";
                case EnumTraceLevel.ERR:
                    return "E";
                case EnumTraceLevel.INF:
                    return "I";
                case EnumTraceLevel.WRN:
                    return "W";
                default:
                    return "E";
            }
           
        }

        private string StrTraceGroup(EnumTraceGroup traceGroup)
        {
            switch (traceGroup)
            {
                case EnumTraceGroup.ALL:
                    return "ALL";
                case EnumTraceGroup.CLIENT:
                    return "";
                case EnumTraceGroup.LICENSE:
                    return "";
                case EnumTraceGroup.NOTHING:
                    return "";
                case EnumTraceGroup.OPCCLIENT:
                    return "";
                case EnumTraceGroup.OPCCLIENTADVISE:
                    return "";
                case EnumTraceGroup.OPCCLIENTCALL:
                    return "";
                case EnumTraceGroup.OPCSERVER:
                    return "";
                case EnumTraceGroup.OPCSERVERADVISE:
                    return "";
                case EnumTraceGroup.OPCSERVERCALL:
                    return "";
                default:
                    return "ALL";
            }

        }

        #endregion

        #region Public Methods

        #region Constructor

        public TraceWindow()
        {
            InitializeComponent();
        }

        #endregion

        #region Add Trace

        public void AddTrace(string traceString, EnumTraceLevel traceLevel, EnumTraceGroup traceGroup, string objectId, string message)
        {
            if (!string.IsNullOrEmpty(traceString))
            {
                richTextBoxTraces.AppendText(traceString);
            }
        }

        #endregion

        #region ClearData

        public void ClearData()
        {
            richTextBoxTraces.Clear();
        }

        #endregion

        #endregion
    }
}
