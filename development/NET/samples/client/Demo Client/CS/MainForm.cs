using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Client;
using System.Collections.ObjectModel;

namespace DemoClient
{
    public partial class MainForm : Form
    {      

        #region Private Members

        DockWindows.PropertyWindow propertyWindow;
        DockWindows.ServiceWindow serviceWindow;
        DockWindows.TraceWindow traceWindow;
        DockWindows.MainWindow mainWindow;

        private Softing.OPCToolbox.Client.Application clientAppInstance = Softing.OPCToolbox.Client.Application.Instance;

        #endregion

        #region Private and Protected Methods

        #region Docking

        /// <summary>
        /// Initialize dock windows
        /// </summary>
        private void InitializeDockWindows()
        {            
            propertyWindow = new DemoClient.DockWindows.PropertyWindow();
            serviceWindow = new DemoClient.DockWindows.ServiceWindow();
            traceWindow = new DemoClient.DockWindows.TraceWindow();
            mainWindow = new DemoClient.DockWindows.MainWindow();
            mainWindow.ClientAppInstance = clientAppInstance;
            mainWindow.PropertyWindow = propertyWindow;
            mainWindow.ServiceWindow = serviceWindow;
            mainWindow.MainDockPanel = dockPanel;            
            mainWindow.Show(dockPanel);           
        }

        /// <summary>
        /// Check menu items if controls are visible
        /// </summary>
        private void CheckMenuItems()
        {
            if (toolStripCommand.Visible)
            {
                toolbarToolStripMenuItem.Checked = true;
            }
        }

        #endregion    

        #region Manage Sessions

        /// <summary>
        /// Connect alll sessions
        /// </summary>
        private void ConnectSessions()
        {
            try
            {
                mainWindow.ConnectAllSessions();
            }
            catch { }
        }

        /// <summary>
        /// Activate all sessions
        /// </summary>
        private void ActivateSessions()
        {
            try
            {
                mainWindow.ActivateAllSessions();
            }
            catch { }
        }

        /// <summary>
        /// Disconnect all sessions
        /// </summary>
        private void DisconnectSessions()
        {
            try
            {
                mainWindow.DisconnectAllSessions();
            }
            catch { }
        }

        #endregion

        #region Add Last Saved Files

        private void AddLastSavedFiles()
        {
            //eliminate old items in menu
            Collection<ToolStripItem> removeList = new Collection<ToolStripItem>();
            foreach (ToolStripItem item in fileToolStripMenuItem.DropDownItems)
            {
                if (item.Tag != null)
                {
                        item.Click -= new EventHandler(OpenSavedFile);
                        removeList.Add(item);
                }
            }
            foreach (ToolStripItem item in removeList)
            {
                fileToolStripMenuItem.DropDownItems.Remove(item);
            }
            if (Helper.Options.LastSavedFiles.Count == 0)
            {
                toolStripSeparator1.Visible = false;
            }
            else
	        {
                toolStripSeparator1.Visible = true;
	        }
            for (int i = 0; i < Helper.Options.LastSavedFiles.Count; i++)
            {
                ToolStripMenuItem currentProj = new ToolStripMenuItem(string.Format("{0}. {1}", i + 1,Helper.Options.LastSavedFiles[i].FileName));
                currentProj.Tag = Helper.Options.LastSavedFiles[i].FilePath;
                currentProj.Click += new EventHandler(OpenSavedFile);
                fileToolStripMenuItem.DropDownItems.Insert(fileToolStripMenuItem.DropDownItems.IndexOf(toolStripSeparator1) + (i + 1), currentProj);
            }
        }  

        #endregion

        #endregion

        #region  Public Methods

        #region Constructor

        /// <summary>
        /// Main Form Constructor
        /// </summary>
        public MainForm()
        {
            InitializeComponent();

            //initialize client application instance
            clientAppInstance.Initialize();
            clientAppInstance.EnableTracing(
                    Helper.Options.Error,
                    Helper.Options.Warning,
                    Helper.Options.Info,
                    Helper.Options.Debug,                  
                    Helper.Options.TraceFile,
                    1000000,
                    2);
            
            clientAppInstance.TraceOutput -= new TraceEventHandler(clientAppInstance_TraceOutput);
            clientAppInstance.TraceOutput += new TraceEventHandler(clientAppInstance_TraceOutput);
        }        

        #endregion
       
        #endregion

        #region Event Handlers

        #region Loading Window

        private void MainForm_Load(object sender, EventArgs e)
        {
            //docking the windows
            InitializeDockWindows();

            //check menu items if controls visible
            CheckMenuItems();

            //disable DaWrite Bar
            toolStripDaWrite.Enabled = false;
            toolStripButtonDelete.Enabled = false;
            toolStripButtonProperties.Enabled = false;
            propertyWindowToolStripMenuItem.Enabled = false;
            deleteToolStripMenuItem.Enabled = false;
            refreshToolStripMenuItem.Enabled = false;
            groupToolStripMenuItem.Enabled = false;
            itemToolStripMenuItem.Enabled = false;

            //Add Lass Saved Files
            AddLastSavedFiles();

            //assign to the events
            if (mainWindow != null)
            {
                mainWindow.EnableWriteBar -= new DemoClient.DockWindows.EnableWriteBarHandler(mainWindow_EnableWriteBar);
                mainWindow.EnableWriteBar += new DemoClient.DockWindows.EnableWriteBarHandler(mainWindow_EnableWriteBar);

                mainWindow.EnableButtons -= new DemoClient.DockWindows.EnableButtonsHandler(mainWindow_EnableButtons);
                mainWindow.EnableButtons += new DemoClient.DockWindows.EnableButtonsHandler(mainWindow_EnableButtons);

                mainWindow.RefreshWindow += new DemoClient.DockWindows.RefreshWindowHandler(mainWindow_RefreshWindow);
            }
        }                       

        #endregion

        #region Menu Events

        #region View menuItems  events

        /// <summary>
        /// Show the Property Window
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void propertyWindowToolStripMenuItem_Click(object sender, EventArgs e)
        {
            mainWindow.ShowProperties();
        }

        /// <summary>
        /// Show the trace window
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void traceWindowToolStripMenuItem_Click(object sender, EventArgs e)
        {
            traceWindow.Show(dockPanel);
        }

        /// <summary>
        /// Show/hide Command toolbar
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void toolbarToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (toolbarToolStripMenuItem.Checked)
            {
                toolStripCommand.Visible = true;
            }
            else 
            {
                toolStripCommand.Visible = false;
            }
        }

        /// <summary>
        /// Show/hide DaWrite Bar
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void toolStripMenuItemDaWrite_Click(object sender, EventArgs e)
        {

            if (toolStripMenuItemDaWrite.Checked)
            {
                toolStripDaWrite.Visible = true;
            }
            else
            {
                toolStripDaWrite.Visible = false;
            }
        }

        /// <summary>
        /// Refresh treer
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void refreshToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                mainWindow.RefreshTree();
            }
            catch
            { }
        }

        #endregion       

        #region Help menuItem events

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            About.AboutDialog aboutDlg = new DemoClient.About.AboutDialog();
            aboutDlg.ShowDialog();

        }


        #endregion

        #region Manage Sessions

        private void activateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ActivateSessions();

        }

        private void connectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ConnectSessions();
        }

        private void disconnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DisconnectSessions();
        }

        #endregion

        #region Edit Session

        /// <summary>
        /// Delete selected object
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                mainWindow.DeleteAction();
            }
            catch 
            { }
        }

        /// <summary>
        /// Add Subscription
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void groupToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                mainWindow.AddSubscription();
            }
            catch
            { }
        }

        /// <summary>
        /// Add DaItem
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void itemToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                mainWindow.AddItem();
            }
            catch
            { }
        }

        /// <summary>
        /// Options dialog
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void optionsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                DialogWindows.OptionsDialog optionDlg = new DemoClient.DialogWindows.OptionsDialog();
                optionDlg.UpdateTraceWindow -= new DemoClient.DialogWindows.UpdateTraceWindowHandler(optionDlg_UpdateTraceWindow);
                optionDlg.UpdateTraceWindow += new DemoClient.DialogWindows.UpdateTraceWindowHandler(optionDlg_UpdateTraceWindow);
                optionDlg.ShowDialog();
            }
            catch
            { }

        }

        /// <summary>
        /// Update trace window
        /// </summary>
        void optionDlg_UpdateTraceWindow(bool changeTraceHandler, bool sortDaChanged, bool sortAeChanged)
        {
            try
            {
                if (Helper.Options.TraceCalls)
                {
                    traceWindowToolStripMenuItem.Enabled = true;

                }
                else
                {
                    traceWindowToolStripMenuItem.Enabled = false;
                    traceWindow.Hide();
                }
                if (changeTraceHandler)
                {
                    clientAppInstance.EnableTracing(
                    Helper.Options.Error,
                    Helper.Options.Warning,
                    Helper.Options.Info,
                    Helper.Options.Debug,
                    Helper.Options.TraceFile,                    
                    1000000,
                    2);
                }
                if (sortAeChanged || sortDaChanged)
                {
                    mainWindow.FillAddressSpace(); 
                }          
                
            }
            catch { }
        }

        #endregion

        #region Disable/Enable Buttons

        void mainWindow_EnableButtons(bool enableDelete, bool enableProperties, bool enableAddItem, bool enableAddSubscription, bool enableRefresh)
        {
            toolStripButtonDelete.Enabled = enableDelete;
            toolStripButtonProperties.Enabled = enableProperties;
            propertyWindowToolStripMenuItem.Enabled = enableProperties;
            deleteToolStripMenuItem.Enabled = enableDelete;
            refreshToolStripMenuItem.Enabled = enableRefresh;
            groupToolStripMenuItem.Enabled = enableAddSubscription;
            itemToolStripMenuItem.Enabled = enableAddItem;

        }     

        #endregion

        #region Open/Save/New File

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                mainWindow.NewFile(true);
                traceWindow.ClearData();
            }
            catch
            { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                mainWindow.OpenFile();
            }
            catch
            { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                mainWindow.SaveFile();
            }
            catch
            { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                mainWindow.SaveAsFile();
            }
            catch
            { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        #endregion

        #endregion

        #region Toolbar Events

        #region Enable/Disable DaWrite Bar

        void mainWindow_EnableWriteBar(bool enable)
        {
            toolStripDaWrite.Enabled = enable;
        }

        #endregion

        #region Show Properties

        /// <summary>
        /// Show Properties Window
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void toolStripButtonProperties_Click(object sender, EventArgs e)
        {
            try
            {
                mainWindow.ShowProperties();
            }
            catch
            { }
        }

        #endregion

        #region Manage Sessions

        private void toolStripButtonDisconnect_Click(object sender, EventArgs e)
        {
            DisconnectSessions();
        }

        private void toolStripButtonConnect_Click(object sender, EventArgs e)
        {
            ConnectSessions();
        }

        private void toolStripButtonActivate_Click(object sender, EventArgs e)
        {
            ActivateSessions();
        }

        private void toolStripButtonDelete_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                mainWindow.DeleteAction();
            }
            catch
            { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }       

        #endregion

        #region Write 

        /// <summary>
        /// Write inserted value
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void toolStripButtonWrite_Click(object sender, EventArgs e)
        {
            mainWindow.WriteValue(toolStripTextBoxDaWrite.Text);
            toolStripTextBoxDaWrite.Text = "";
        }

        #endregion

        #region Open/Save/New File

        private void newToolStripButton_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                mainWindow.NewFile(true);
                traceWindow.ClearData();
            }
            catch
            { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        private void openToolStripButton_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                mainWindow.OpenFile();
            }
            catch
            { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        private void saveToolStripButton_Click(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                mainWindow.SaveFile();
            }
            catch
            { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        #endregion

        #endregion

        #region Register Trace

        void clientAppInstance_TraceOutput(string traceString, EnumTraceLevel traceLevel, EnumTraceGroup traceGroup, string objectId, string message)
        {
            if (this.InvokeRequired)
            {
                Softing.OPCToolbox.TraceEventHandler callback = new TraceEventHandler(clientAppInstance_TraceOutput);
                this.BeginInvoke(callback, new object[] { traceString, traceLevel, traceGroup, objectId, message });
                return;
            }
            if (traceWindow == null) return;
            traceWindow.AddTrace(traceString, traceLevel, traceGroup, objectId, message);
        }

        #endregion

        #region OpenSavedFiles

        private void OpenSavedFile(object sender, EventArgs e)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;
                ToolStripMenuItem menuItem = sender as ToolStripMenuItem;
                if (menuItem.Tag == null) return;
                string filePath = menuItem.Tag.ToString();
                mainWindow.OpenSavedFile(filePath);
            }
            catch
            { }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        #endregion

        #region Refresh Window

        void mainWindow_RefreshWindow(bool refreshMenu)
        {
            this.Refresh();
            if (refreshMenu)
            {
                AddLastSavedFiles();
            }
        }  

        #endregion

        #region Closing Window

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.Cursor = Cursors.WaitCursor;
            if (DockWindows.MainWindow.AnyChange)
            {
                DialogResult result = MessageBox.Show("Save changes?", "Demo Client", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
                if (result == DialogResult.Yes)
                {
                     mainWindow.SaveFile();
                }
                if (result == DialogResult.Cancel)
                {                    
                    e.Cancel = true;
                    this.Cursor = Cursors.Default;
                    return;
                }
            }
            this.Refresh();
            propertyWindow.Close();
            traceWindow.Close();
            serviceWindow.Close();
            mainWindow.Close();
            clientAppInstance.Terminate();
        }

        #endregion                                    

        #endregion
    }
}
