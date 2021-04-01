using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using TestClientGui.Elements;
using System.IO;

namespace TestClientGui
{
	partial class TestClientForm : Form
	{
		public TestList TestRoot { get; set; }

		public TestClientForm()
		{
			InitializeComponent();

			string[] args = Environment.GetCommandLineArgs();

			if( args.Length == 2 )
				OpenFile( args[1] );
		}

		private bool CompareElement( Element e1, Element e2 )
		{
			TestList l1 = e1 as TestList;
			TestList l2 = e2 as TestList;

			if( l1 != null && l2 != null )
			{
				if( l1.RunInParallel != l2.RunInParallel )
					return false;

				if( l1.StopIfOneFailed != l2.StopIfOneFailed )
					return false;

				if( l1.Children.Count != l2.Children.Count )
					return false;

				for( int i = 0; i < l1.Children.Count; i++ )
				{
					if( !CompareElement( l1.Children[i], l2.Children[i] ) )
						return false;
				}

				return true;
			}

			if( e1.GetType() == e2.GetType() )
				return true;
			else
				return false;
		}

		private void toolStripButtonRun_Click( object sender, EventArgs e )
		{
			RunAll();
		}

		private void toolStripButtonStop_Click( object sender, EventArgs e )
		{
			toolStripButtonStop.Enabled = false;
			toolStripStatusLabel1.Text = "Stopping";
			backgroundWorker1.CancelAsync();
		}


		private void toolStripButtonClearResults_Click( object sender, EventArgs e )
		{
			toolStripStatusLabel1.Text = "Ready";
			TestRoot.ClearResults();
		}

		private void RunAll()
		{
			toolStripStatusLabel1.Text = "Running";
			toolStripSplitButtonOpenFile.Enabled = toolStripSplitButtonSaveFile.Enabled = toolStripButtonRun.Enabled = toolStripButtonClearResults.Enabled = toolStripButtonShowCheckboxes.Enabled = toolStripButtonProperties.Enabled = false;
			toolStripButtonStop.Enabled = true;
			toolStripProgressBar1.Style = ProgressBarStyle.Marquee;

			TestRoot.ClearResults();
			backgroundWorker1.RunWorkerAsync();
		}

		private void treeView1_AfterSelect( object sender, TreeViewEventArgs e )
		{
			propertyGrid1.SelectedObject = e.Node.Tag;
		}

		private void toolStripButtonShowCheckboxes_CheckedChanged( object sender, EventArgs e )
		{
			Element.CheckBoxes = treeView1.CheckBoxes = toolStripButtonShowCheckboxes.Checked;

			if( toolStripButtonShowCheckboxes.Checked )
			{
				toolStripButtonShowCheckboxes.Text = "Hide checkboxes";

				if( TestRoot != null )
					CheckNode( treeView1.Nodes[0] );
			}
			else
			{
				toolStripButtonShowCheckboxes.Text = "Show checkboxes";
			}
		}

		private void CheckNode( TreeNode treeNode )
		{
			treeNode.Checked = true;

			foreach( TreeNode child in treeNode.Nodes )
				CheckNode( child );
		}

		private void toolStripSplitButtonOpenFile_ButtonClick( object sender, EventArgs e )
		{
			if( openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK )
			{
				OpenFile( openFileDialog1.FileName );
			}
		}

		private void saveTestsWithResultsToolStripMenuItem_Click( object sender, EventArgs e )
		{
			if( saveFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK )
			{
				using( Stream stream = saveFileDialog1.OpenFile() )
				{
					Element.Serializing = true;
					TestRoot.SaveToFile( stream );
					Element.Serializing = false;
				}
			}
		}


		private void saveTestsToolStripMenuItem_Click( object sender, EventArgs e )
		{
			if( saveFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK )
			{
				using( Stream stream = saveFileDialog1.OpenFile() )
				{
					Element.Serializing = Element.SerializeClean = true;
					TestRoot.SaveToFile( stream );
					Element.Serializing = Element.SerializeClean = false;
				}
			}
		}

		private void TestClientForm_DragDrop( object sender, DragEventArgs e )
		{
			if( e.Data.GetDataPresent( DataFormats.FileDrop ) )
			{
				string[] files = (string[]) e.Data.GetData( DataFormats.FileDrop );

				try
				{
					OpenFile( files[0] );
				}
				catch( Exception ex )
				{
					System.Windows.Forms.MessageBox.Show( this, ex.ToString() );
				}
			}
		}

		private void OpenFile( string filePath )
		{
			TestList tl;

			try
			{
				tl = TestList.LoadFromFile( filePath );

				if( Properties.Settings.Default.RecentFilesList == null )
				{
					Properties.Settings.Default.RecentFilesList = new System.Collections.Specialized.StringCollection();
				}

				if( !Properties.Settings.Default.RecentFilesList.Contains( filePath ) )
				{
					Properties.Settings.Default.RecentFilesList.Insert( 0, filePath );
					Properties.Settings.Default.Save();

					ReloadRecentFilesListMenu();
				}
				else
				{//move to top
					int ix = Properties.Settings.Default.RecentFilesList.IndexOf( filePath );

					if( ix != -1 )
					{
						Properties.Settings.Default.RecentFilesList.RemoveAt( ix );
						Properties.Settings.Default.RecentFilesList.Insert( 0, filePath );
						Properties.Settings.Default.Save();

						ReloadRecentFilesListMenu();
					}
				}
			}
			catch( IOException ex )
			{ //remove the file entry
				int ix = Properties.Settings.Default.RecentFilesList.IndexOf( filePath );

				if( ix != -1 )
				{
					Properties.Settings.Default.RecentFilesList.RemoveAt( ix );
					Properties.Settings.Default.Save();

					ReloadRecentFilesListMenu();
				}

				System.Windows.Forms.MessageBox.Show( ex.ToString() );
				return;
			}
			catch( Exception ex )
			{
				System.Windows.Forms.MessageBox.Show( ex.ToString() );
				return;
			}

			tl.SaveToFile( "out.xml" );

			TestRoot = tl;

			treeView1.Nodes.Clear();
			tl.RefreshView();
			tl.TreeNode.Expand();
			treeView1.Nodes.Add( tl.TreeNode );
		}

		private void TestClientForm_DragEnter( object sender, DragEventArgs e )
		{
			if( e.Data.GetDataPresent( DataFormats.FileDrop ) )
			{
				string[] files = (string[]) e.Data.GetData( DataFormats.FileDrop );

				if( files.Length == 1 )
				{
					e.Effect = DragDropEffects.Copy;
					return;
				}
			}

			e.Effect = DragDropEffects.None;
		}

		private void backgroundWorker1_DoWork( object sender, DoWorkEventArgs e )
		{
			TestRoot.RunAll( backgroundWorker1 );
		}

		private void backgroundWorker1_ProgressChanged( object sender, ProgressChangedEventArgs e )
		{
			Element el = (Element) e.UserState;
			el.RefreshView();
		}

		private void backgroundWorker1_RunWorkerCompleted( object sender, RunWorkerCompletedEventArgs e )
		{
			if( e.Error != null )
			{
				System.Windows.Forms.MessageBox.Show( e.Error.ToString() );
			}

			toolStripProgressBar1.Value = 100;
			toolStripProgressBar1.Style = ProgressBarStyle.Continuous;
			toolStripStatusLabel1.Text = "Finished";
			toolStripButtonStop.Enabled = false;
			toolStripSplitButtonOpenFile.Enabled = toolStripSplitButtonSaveFile.Enabled = toolStripButtonRun.Enabled = toolStripButtonClearResults.Enabled = toolStripButtonShowCheckboxes.Enabled = toolStripButtonProperties.Enabled = true;
		}

		private void insertCopyToolStripMenuItem_Click( object sender, EventArgs e )
		{
			TreeView tree = (TreeView) contextMenuStrip1.SourceControl;
			TreeNode node = tree.SelectedNode;
			Element el = node != null ? node.Tag as Element : null;

			if( el != null && System.Windows.Forms.MessageBox.Show( string.Format( "Are you sure you want to insert a copy of {0}", el.Name ), "Confirm insert copy", MessageBoxButtons.YesNo ) == System.Windows.Forms.DialogResult.Yes )
			{
				if( el.Parent != null )
				{
					el.Parent.InsertAfter( el, (Element) el.Clone() );
				}
			}
		}

		private void removeToolStripMenuItem_Click( object sender, EventArgs e )
		{
			TreeView tree = (TreeView)contextMenuStrip1.SourceControl;
			TreeNode node = tree.SelectedNode;
			Element el = node != null ? node.Tag as Element : null;

			if( el != null && System.Windows.Forms.MessageBox.Show( string.Format( "Are you sure you want to remove {0}", el.Name ), "Confirm remove", MessageBoxButtons.YesNo ) == System.Windows.Forms.DialogResult.Yes )
			{
				if( el.Parent != null )
				{
					el.Parent.Remove( el );
				}
			}
		}


		private void collapseChildrenToolStripMenuItem_Click( object sender, EventArgs e )
		{
			TreeView tree = (TreeView) contextMenuStrip1.SourceControl;
			TreeNode node = tree.SelectedNode;

			if( node != null )
			{
				foreach( TreeNode child in node.Nodes )
				{
					child.Collapse();
				}
			}
		}

		private void collapseSuccededToolStripMenuItem_Click( object sender, EventArgs e )
		{
			TreeView tree = (TreeView) contextMenuStrip1.SourceControl;
			TreeNode node = tree.SelectedNode;
			TestList el = node != null ? node.Tag as TestList : null;

			if( el != null )
			{
				CollapseSucceded( el );
			}
		}

		private void CollapseSucceded( TestList el )
		{
			foreach( Element child in el.Children )
			{
				if( child.RunResult != null && child.RunResult.Type == TestResults.Success )
				{
					child.TreeNode.Collapse();
					continue;
				}

				TestList list = child as TestList;

				if( list != null )
				{
					CollapseSucceded( list );
				}
			}
		}

		private void expandChildrenToolStripMenuItem_Click( object sender, EventArgs e )
		{
			TreeView tree = (TreeView) contextMenuStrip1.SourceControl;
			TreeNode node = tree.SelectedNode;

			if( node != null )
			{
				foreach( TreeNode child in node.Nodes )
				{
					child.ExpandAll();
				}
			}
		}

		private void debugToolStripMenuItem_Click( object sender, EventArgs e )
		{
			TreeView tree = (TreeView) contextMenuStrip1.SourceControl;
			TreeNode node = tree.SelectedNode;
			Element el = node != null ? node.Tag as Element : null;

			if( el != null /*&& MessageBox.Show( string.Format( "Are you sure you want to remove {0}", el.Name ), "Confirm remove", MessageBoxButtons.YesNo ) == System.Windows.Forms.DialogResult.Yes*/ )
			{
				/*if( el.Parent != null )
				{
					el.Parent.Remove( el );
				}*/
				el.RefreshView();
			}
		}

		private void treeView1_NodeMouseClick( object sender, TreeNodeMouseClickEventArgs e )
		{
			if( e.Button == System.Windows.Forms.MouseButtons.Right )
			{
				treeView1.SelectedNode = e.Node;
			}
		}

		private void toolStripButtonProperties_Click( object sender, EventArgs e )
		{
			new GlobalPropertiesForm().ShowDialog();

			TestList.SetGlobalProperties( Properties.Settings.Default.GlobalPropertiesNames, Properties.Settings.Default.GlobalPropertiesValues );
		}

		private void TestClientForm_Load( object sender, EventArgs e )
		{
			ReloadRecentFilesListMenu();

			TestList.SetGlobalProperties( Properties.Settings.Default.GlobalPropertiesNames, Properties.Settings.Default.GlobalPropertiesValues );
		}

		private void ReloadRecentFilesListMenu()
		{
			if( Properties.Settings.Default.RecentFilesList != null )
			{
				toolStripSplitButtonOpenFile.DropDownItems.Clear();

				foreach( var file in Properties.Settings.Default.RecentFilesList )
				{
					ToolStripItem tsi = toolStripSplitButtonOpenFile.DropDownItems.Add( file );
					tsi.Click += ToolStripRecentFilesList_Click;
				}
			}
		}

		void ToolStripRecentFilesList_Click( object sender, EventArgs e )
		{
			OpenFile( ( (ToolStripMenuItem) sender ).Text );
		}

		private void treeView1_AfterCheck( object sender, TreeViewEventArgs e )
		{
			if( e.Node.Tag is Element )
			{
				if( e.Node.Checked && e.Node.Parent != null && !e.Node.Parent.Checked )
				{
					e.Node.Parent.Checked = true;
				}
			}
		}
	}
}
