using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Softing.OPCToolbox;
using DemoClient.Helper;

namespace DemoClient.DialogWindows
{
    public delegate void UpdateTraceWindowHandler(bool changeTraceHandler, bool sortDa, bool sortAe);

    public partial class OptionsDialog : Form
    {
        #region Private Members

        private bool canClose_ = true;
        private bool saveChanges = false;

        #endregion

        #region Properties

        private bool showTrace = true;
        public bool ShowTrace
        {
            get { return showTrace; }
            set { showTrace = value; }
        }

        private bool changeTraceOptions = false;
        public bool ChangeTraceOptions
        {
            get { return changeTraceOptions; }
            set { changeTraceOptions = value; }
        }

        #endregion

        #region Delegates

        public event UpdateTraceWindowHandler UpdateTraceWindow;

        #endregion

        #region Private and Protected Methods

        /// <summary>
        /// Load options
        /// </summary>
        private void LoadOptions()
        {
            rdBtnTimeOnly.Checked = Helper.Options.PureTime;
            rdBtnWithDate.Checked = Helper.Options.WithDateTime;
            rdBtnPure.Checked = Helper.Options.PureItemValue;
            rdBtnWithVT.Checked = Helper.Options.VTItemValue;
            rdBtnDecimal.Checked = Helper.Options.DecItemValue;
            rdBtnHex.Checked = Helper.Options.HexItemValue;
            rdBtnDaOn.Checked = Helper.Options.SortDa;
            rdBtnDaOff.Checked = !Helper.Options.SortDa;
            rdBtnAeOn.Checked = Helper.Options.SortAe;
            rdBtnAeOff.Checked = !Helper.Options.SortAe;

            rdBtnConOn.Checked = Helper.Options.ConnMonitor;
            rdBtnConOff.Checked = !Helper.Options.ConnMonitor;

            rdBtnTraceOn.Checked = Helper.Options.TraceCalls;
            rdBtnTraceOff.Checked = !Helper.Options.TraceCalls;

            //Fill Comboboxes
            cbErrors.DataSource = System.Enum.GetNames(typeof(EnumTraceGroup));
            cbErrors.SelectedItem = Helper.Options.Error.ToString();
            cbWarnings.DataSource = System.Enum.GetNames(typeof(EnumTraceGroup));
            cbWarnings.SelectedItem = Helper.Options.Warning.ToString();
            cbInfos.DataSource = System.Enum.GetNames(typeof(EnumTraceGroup));
            cbInfos.SelectedItem = Helper.Options.Info.ToString();
            cbDebug.DataSource = System.Enum.GetNames(typeof(EnumTraceGroup));
            cbDebug.SelectedItem = Helper.Options.Debug.ToString();

            txtTraceFile.Text = Helper.Options.TraceFile;            

        }

        /// <summary>
        /// Save changes
        /// </summary>
        private void SaveChanges()
        {
            Helper.Options.PureTime = rdBtnTimeOnly.Checked;
            Helper.Options.WithDateTime = rdBtnWithDate.Checked;

            Helper.Options.PureItemValue = rdBtnPure.Checked;
            Helper.Options.VTItemValue = rdBtnWithVT.Checked;

            Helper.Options.DecItemValue = rdBtnDecimal.Checked;
            Helper.Options.HexItemValue = rdBtnHex.Checked;

            bool sortDaChanged = false;
            bool sortAeChanged = false;
            if (Helper.Options.SortDa != rdBtnDaOn.Checked)
            {
                sortDaChanged = true;
                Helper.Options.SortDa = rdBtnDaOn.Checked;
            }
            if (Helper.Options.SortAe != rdBtnAeOn.Checked)
            {
                sortAeChanged = true;
                Helper.Options.SortAe = rdBtnAeOn.Checked;
            }
            
            rdBtnConOn.Checked = Helper.Options.ConnMonitor;
            rdBtnConOff.Checked = !Helper.Options.ConnMonitor;

            Helper.Options.TraceCalls = rdBtnTraceOn.Checked;
            if (Helper.Options.TraceCalls)
            {
                showTrace = true;
            }
            else
            {
                showTrace = false;
            }           


            //Fill Comboboxes
            if ((EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup), cbErrors.SelectedItem.ToString()) != Helper.Options.Error ||
                    (EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup), cbWarnings.SelectedItem.ToString()) != Helper.Options.Warning ||
                    (EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup), cbInfos.SelectedItem.ToString()) != Helper.Options.Info ||
                    (EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup), cbDebug.SelectedItem.ToString()) != Helper.Options.Debug)
            {
                changeTraceOptions = true;
                Helper.Options.Error = (EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup), cbErrors.SelectedItem.ToString());
                Helper.Options.Warning = (EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup), cbWarnings.SelectedItem.ToString());
                Helper.Options.Info = (EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup), cbInfos.SelectedItem.ToString());
                Helper.Options.Debug = (EnumTraceGroup)System.Enum.Parse(typeof(EnumTraceGroup), cbDebug.SelectedItem.ToString());                
            }
            else
            {
                changeTraceOptions = false;
            }
            if (Helper.Options.TraceFile != txtTraceFile.Text)            
            {
                changeTraceOptions = true;
                Helper.Options.TraceFile = txtTraceFile.Text;                
            }          

            if (UpdateTraceWindow != null)
            {
                UpdateTraceWindow(changeTraceOptions, sortDaChanged, sortAeChanged);
            }
        }

        #endregion

        #region Public Methods

        #region Constructor

        public OptionsDialog()
        {
            InitializeComponent();
        }

        #endregion

        #endregion

        #region Event Handlers

        /// <summary>
        /// Load Options
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OptionsDialog_Load(object sender, EventArgs e)
        {
            LoadOptions();
        }

        /// <summary>
        /// Set to true, if there were made some changes
        /// </summary>
        private void ChangesMade(object sender, EventArgs e)
        {
            saveChanges = true;
        }

        /// <summary>
        /// Save the changed options and close the window
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonOk_Click(object sender, EventArgs e)
        {
            if (saveChanges)
            {
                SaveChanges();
                saveChanges = false;
            }
            canClose_ = true;
        }

        /// <summary>
        /// SAve the changes
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnApply_Click(object sender, EventArgs e)
        {
            if (saveChanges)
            {
                SaveChanges();
                saveChanges = false;
            }
            canClose_ = false;
            this.DialogResult = DialogResult.OK;
        }
        
        /// <summary>
        /// Cloase the window
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OptionsDialog_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = !canClose_;
            canClose_ = true;
        }

        #endregion

    }
}
