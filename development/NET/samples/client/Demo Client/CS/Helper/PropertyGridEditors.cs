using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Design;
using System.Windows.Forms.Design;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Drawing;

namespace DemoClient.Helper
{
    public class LocaleId
    {

        private static string selectedItem = "";
        public static string SelectedItem
        {
            get { return selectedItem; }
            set { selectedItem = value; }
        }

        private static string[] localeIdList = null;
        public static string[] LocaleIdList
        {
            get { return LocaleId.localeIdList; }
            set { LocaleId.localeIdList = value; }
        }
    }

    public class DataTypeEditor : UITypeEditor
    {
        #region Private Members

        IWindowsFormsEditorService wfes;

        #endregion

        #region Private and Protected Methods

        private void CloseDropdown()
        {
            if (wfes != null)
            {
                wfes.CloseDropDown();
            }
        }

        #endregion

        #region Public Methods

        public override UITypeEditorEditStyle
               GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.DropDown;
        }

        public override object EditValue(ITypeDescriptorContext context,
                                IServiceProvider provider, object value)
        {
            Helper.DataTypes dataTypeHelper = new DataTypes();

            wfes = provider.GetService(typeof(IWindowsFormsEditorService)) as
              IWindowsFormsEditorService;

            if (wfes != null)
            {
                int index;
                System.Windows.Forms.ListBox listBox = new System.Windows.Forms.ListBox();
                foreach (string dictEntry in dataTypeHelper.RequiredDataTypes.Values)
                {
                    index = listBox.Items.Add(dictEntry);
                    if (dictEntry == dataTypeHelper.NativeDataType)
                    {
                        listBox.SelectedIndex = index;
                    }
                }

                listBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
                listBox.DoubleClick += new EventHandler(listBox_DoubleClick);
                listBox.Click += new EventHandler(listBox_Click);

                wfes.DropDownControl(listBox);
                if (listBox.SelectedItem != null)
                {
                    value = listBox.SelectedItem;
                }
            }

            return value;
        }

        #endregion

        #region Event Handlers

        void listBox_Click(object sender, EventArgs e)
        {
            CloseDropdown();
        }

        void listBox_DoubleClick(object sender, EventArgs e)
        {
            CloseDropdown();
        }

        #endregion
    }

    public class LCIDEditor : UITypeEditor
    {
        #region Private Members

        IWindowsFormsEditorService wfes;
        
        #endregion       

        #region Private and Protected Methods

        private void CloseDropdown()
        {
            if (wfes != null)
            {
                wfes.CloseDropDown();
            }
        }

        #endregion

        #region Public Methods

        public override UITypeEditorEditStyle
               GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.DropDown;
        }

        public override object EditValue(ITypeDescriptorContext context,
                                IServiceProvider provider, object value)
        {
            LocaleId localeId = new LocaleId();
            wfes = provider.GetService(typeof(IWindowsFormsEditorService)) as
              IWindowsFormsEditorService;

            if (wfes != null)
            {
                int index;
                System.Windows.Forms.ListBox listBox = new System.Windows.Forms.ListBox();
                foreach (string lcid in LocaleId.LocaleIdList)
                {
                    index = listBox.Items.Add(lcid);
                    if (lcid == LocaleId.SelectedItem)
                    {
                        listBox.SelectedIndex = index;
                    }
                }

                listBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
                listBox.DoubleClick += new EventHandler(listBox_DoubleClick);
                listBox.Click += new EventHandler(listBox_Click);

                wfes.DropDownControl(listBox);
                if (listBox.SelectedItem != null)
                {
                    value = listBox.SelectedItem;
                }
            }

            return value;
        }

        #endregion

        #region Event Handlers

        void listBox_Click(object sender, EventArgs e)
        {
            CloseDropdown();
        }

        void listBox_DoubleClick(object sender, EventArgs e)
        {
            CloseDropdown();
        }

        #endregion
    }
    
}
