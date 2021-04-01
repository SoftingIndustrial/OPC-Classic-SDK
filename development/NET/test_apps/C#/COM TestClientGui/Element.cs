using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;
using System.ComponentModel;

namespace TestClientGui
{
	namespace Elements
	{
		public abstract class Element : ICloneable
		{
			[Description( "Integer value representing maximum repeats count" ), Category( "Common" )]
			[XmlAttribute]
			public string RepeatCount
			{
				get;
				set;
			}

			[Description( "Integer value representing maximum duration in seconds" ), Category( "Common" )]
			[XmlAttribute]
			public string RepeatDuration
			{
				get;
				set;
			}

			[Description( "Specific name" ), Category( "Common" )]
			[XmlAttribute]
			public string DisplayName
			{
				get
				{
					return m_displayName;
				}

				set
				{
					if( m_displayName != value )
					{
						if( value == null )
							TreeNode.Text = m_name;
						else
							TreeNode.Text = string.Format( "{0} ({1})", m_name, value );

						m_displayName = value;
					}
				}
			}

			[Description( "Unique element identifier" ), Category( "Common" )]
			[XmlAttribute]
			public long Id
			{
				get;
				set;
			}

			[Category( "Common" )]
			public string Description
			{
				get;
				set;
			}

			[System.ComponentModel.Browsable( false )]
			public TestResult RunResult
			{
				get
				{
					if( SerializeClean )
						return null;
					else
						return m_testResult;
				}
				set
				{
					m_testResult = value;
				}
			}

			public abstract TestResult Execute();

			protected Element()
			{
				Id = System.Threading.Interlocked.Increment( ref IdCounter );
				m_name = GetType().Name;
				TreeNode = new System.Windows.Forms.TreeNode( m_name );
				TreeNode.Tag = this;
				ResultsTreeNode = new System.Windows.Forms.TreeNode( "TestResults" );
				WaitHandle = new System.Threading.AutoResetEvent( false );
			}

			static Element()
			{
				Type[] types = System.Reflection.Assembly.GetExecutingAssembly().GetTypes();
				XmlAttributes listAttribs = new XmlAttributes();

				foreach( var type in types )
				{
					if( typeof( TestResult ).IsAssignableFrom( type ) )
					{
						listAttribs.XmlElements.Add( new XmlElementAttribute( type.Name, type ) );

						Activator.CreateInstance( type );
					}
				}

				TestList.m_aor.Add( typeof( Element ), "RunResult", listAttribs);
			}

			internal void Run( object dummy )
			{
				Run();
			}

			internal virtual void Run()
			{
				try
				{
					if( RunResult != null )
					{
						throw new ApplicationException( "Clear the results before running again" );
					}

					if( CheckBoxes && !TreeNode.Checked )
						return;

					bool bMultipleRuns = false;
					int repeatCount = int.MaxValue;

					if( RepeatCount != null )
					{
						int tmp;
						if( int.TryParse( RepeatCount, out tmp ) )
						{
							repeatCount = tmp;
							bMultipleRuns = true;
						}
					}

					double repeatDuration = double.MaxValue;

					if( RepeatDuration != null )
					{
						double tmp;
						if( double.TryParse( RepeatDuration, out tmp ) )
						{
							repeatDuration = tmp;
							bMultipleRuns = true;
						}
					}

					TestResult result = null;

					if( bMultipleRuns )
					{
						int counter = 0;
						int errors = 0, warnings = 0;
						List<TestResult> innerResults = new List<TestResult>();
						DateTime startTime = DateTime.Now;
						DateTime maxTime = repeatDuration == double.MaxValue ? DateTime.MaxValue : startTime.AddSeconds( repeatDuration );

						while( counter < repeatCount && DateTime.Now <= maxTime )
						{
							TestResult runResult;

							runResult = RunOnce();
							innerResults.Add( runResult );

							switch( runResult.Type )
							{
								case TestResults.Failed: errors++;
									break;
								case TestResults.Warning: warnings++;
									break;
							}

							counter++;
						}

						MultipleTestResult multipleResult;

						if( errors > 0 )
							multipleResult = new MultipleTestResult( TestResults.Failed );
						else if( warnings > 0 )
							multipleResult = new MultipleTestResult( TestResults.Warning );
						else
							multipleResult = new MultipleTestResult( TestResults.Success );

						multipleResult.StartTime = startTime;
						multipleResult.InnerResults = innerResults;

						ResultsTreeNode.Expand();

						result = multipleResult;
					}
					else
					{
						result = RunOnce();
					}

					RunResult = result;
					ReportFinished();
				}
				finally
				{
					WaitHandle.Set();
				}
			}

			protected TestResult RunOnce()
			{
				TestResult result = null;

				DateTime startTime = DateTime.Now;

				try
				{
					Trace( "Execute call" );
					result = Execute();
					Trace( "Execute returned" );
				}
				catch( Exception ex )
				{
					result = new TestResult( ex );
					Trace( "Execute exception" );
				}

				result.StartTime = startTime;

				return result;
			}

			protected void Trace( string text )
			{
				WriteTrace( DateTime.Now, string.Format( "{0} ({1})", m_name, Id ), text );
			}

			protected virtual void WriteTrace( DateTime dateTime, string name, string text )
			{
				Parent.WriteTrace( dateTime, name, text );
			}

			protected virtual object GetProperty( string name )
			{
				return Parent.GetProperty( name );
			}

			protected virtual void SetProperty( string name, object o )
			{
				if( string.IsNullOrEmpty( name ) )
				{
					name = "PropertyOf." + Id;
				}

				Parent.SetProperty( name, o );
			}

			protected virtual void UnsetProperty( string name )
			{
				Parent.UnsetProperty( name );
			}

			protected string ExpandProperty( string value )
			{
				string s = value;
				int start = -1;

				do
				{
					start = s.IndexOf( '{', start + 1 );

					if( start == -1 )
						break;

					int end = s.IndexOf( '}', start );

					if( end != -1 )
					{
						object inner = GetProperty( s.Substring( start + 1, end - start - 1 ) );

						if( inner != null )
							s = s.Substring( 0, start ) + inner.ToString() + s.Substring( end + 1 );
					}

				} while( true );

				return s;
			}

			public virtual void ClearResults()
			{
				RunResult = null;
				ResultsTreeNode.Nodes.Clear();
				ResultsTreeNode.Remove();
				WaitHandle.Reset();
			}

			protected virtual void ReportFinished()
			{
				Parent.ReportFinished( this );
			}

			internal virtual bool IsCancelationPending()
			{
				return Parent.IsCancelationPending();
			}

			internal virtual void RefreshView()
			{
				if( RunResult != null )
				{
					switch( RunResult.Type )
					{
						case TestResults.Success: ResultsTreeNode.ImageIndex = ResultsTreeNode.SelectedImageIndex = 1; break;
						case TestResults.Failed: ResultsTreeNode.ImageIndex = ResultsTreeNode.SelectedImageIndex = 2; break;
						case TestResults.Warning: ResultsTreeNode.ImageIndex = ResultsTreeNode.SelectedImageIndex = 3; break;
						case TestResults.Executing: ResultsTreeNode.ImageIndex = ResultsTreeNode.SelectedImageIndex = 6; break;
					}

					if( ResultsTreeNode.Tag != RunResult )
					{
						ResultsTreeNode.Tag = RunResult;

						MultipleTestResult multipleResult = RunResult as MultipleTestResult;
						if( multipleResult != null && multipleResult.InnerResults != null )
						{
							for( int i = 0; i < multipleResult.InnerResults.Count; i++ )
							{
								System.Windows.Forms.TreeNode runNode = new System.Windows.Forms.TreeNode( string.Format( "Run #{0}", i + 1 ) );
								TestResult runResult = multipleResult.InnerResults[i];

								switch( runResult.Type )
								{
									case TestResults.Success: runNode.ImageIndex = runNode.SelectedImageIndex = 1;
										break;
									case TestResults.Failed: runNode.ImageIndex = runNode.SelectedImageIndex = 2;
										break;
									case TestResults.Warning: runNode.ImageIndex = runNode.SelectedImageIndex = 3;
										break;
									case TestResults.Executing: runNode.ImageIndex = runNode.SelectedImageIndex = 6;
										break;
								}

								runNode.Tag = runResult;
								ResultsTreeNode.Nodes.Add( runNode );
							}
						}

						if( !TreeNode.Nodes.Contains( ResultsTreeNode ) && TreeNode.Tag != ResultsTreeNode.Tag )
							TreeNode.Nodes.Insert( 0, ResultsTreeNode );

						if( RunResult.Type != TestResults.Success )
							TreeNode.Expand();
						else if( !( this is ITestContainer ) )
							TreeNode.Collapse();
					}
				}
			}

			public virtual object Clone()
			{
				Element clone = (Element) MemberwiseClone();

				clone.Id = System.Threading.Interlocked.Increment( ref IdCounter );
				clone.WaitHandle = new System.Threading.AutoResetEvent( false );

				clone.TreeNode = new System.Windows.Forms.TreeNode( string.IsNullOrEmpty( clone.DisplayName ) ? m_name : string.Format( "{0} ({1})", m_name, clone.DisplayName ) );
				clone.TreeNode.Tag = clone;
				clone.TreeNode.Checked = this.TreeNode.Checked;
				clone.ResultsTreeNode = new System.Windows.Forms.TreeNode( "TestResults" );
				clone.RunResult = null;

				return clone;
			}

			[XmlIgnore]
			[System.ComponentModel.Browsable(false)]
			public TestList Parent { get; set; }

			[XmlIgnore]
			[System.ComponentModel.Browsable( false )]
			public string Name
			{
				get
				{
					return m_name;
				}
			}

			public static bool SerializeClean{ get; set; }
			public static bool Serializing { get; set; }
			public static bool CheckBoxes { get; set; }

			private static int IdCounter = 0;

			#region Fields
			private string m_name;
			private string m_displayName;
			private TestResult m_testResult;
			#endregion

			internal System.Windows.Forms.TreeNode TreeNode { get; set; }
			internal System.Windows.Forms.TreeNode ResultsTreeNode { get; set; }
			internal System.Threading.AutoResetEvent WaitHandle { get; set; }
		}
	}
}
