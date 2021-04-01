using System;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace TestClientGui
{
	public partial class GlobalPropertiesForm : Form
	{
		public GlobalPropertiesForm()
		{
			InitializeComponent();
		}

		private void GlobalPropertiesForm_Load( object sender, EventArgs e )
		{
			if( Properties.Settings.Default.GlobalPropertiesNames != null
				&& Properties.Settings.Default.GlobalPropertiesValues != null)
			{
				for( int i = 0; i < Properties.Settings.Default.GlobalPropertiesNames.Count; i++ )
				{
					dataGridView1.Rows.Add( Properties.Settings.Default.GlobalPropertiesNames[i],
						Properties.Settings.Default.GlobalPropertiesValues[i] );
				}
			}
		}

		private void GlobalPropertiesForm_FormClosed( object sender, FormClosedEventArgs e )
		{
			if( Properties.Settings.Default.GlobalPropertiesNames != null )
				Properties.Settings.Default.GlobalPropertiesNames.Clear();

			if( Properties.Settings.Default.GlobalPropertiesValues != null )
				Properties.Settings.Default.GlobalPropertiesValues.Clear();

			foreach( DataGridViewRow row in dataGridView1.Rows )
			{
				if( row.IsNewRow )
					continue;

				if( Properties.Settings.Default.GlobalPropertiesNames == null )
					Properties.Settings.Default.GlobalPropertiesNames = new System.Collections.Specialized.StringCollection();

				if( Properties.Settings.Default.GlobalPropertiesValues == null )
					Properties.Settings.Default.GlobalPropertiesValues = new System.Collections.Specialized.StringCollection();

				string propertyName = (string) row.Cells[0].Value;
				if( !propertyName.StartsWith( "::" ) )
					propertyName = "::" + propertyName;

				Properties.Settings.Default.GlobalPropertiesNames.Add( propertyName );

				string propertyValue = (string) row.Cells[1].Value;
				if( propertyValue == null )
					propertyValue = (string) row.Cells[1].EditedFormattedValue;

				Properties.Settings.Default.GlobalPropertiesValues.Add( propertyValue );
			}

			Properties.Settings.Default.Save();
		}
	}
}
