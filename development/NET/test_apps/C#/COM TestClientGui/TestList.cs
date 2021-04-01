using System;
using System.Collections.Generic;
using System.Xml.Serialization;
using System.Xml;
using System.IO;
using System.ComponentModel;
using System.Threading;
using System.Collections;

namespace TestClientGui.Elements
{
	[XmlRoot( Namespace = "http://softing-ia.com/TestClientCOM.xsd" )]
	public sealed class TestList : Element, ITestContainer
	{
		#region Public Properties
		[XmlAttribute, DefaultValue( false )]
		[Category( "List" )]
		public bool RunInParallel
		{
			get { return m_bRunInParallel; }
			set { m_bRunInParallel = value; TreeNode.ImageIndex = TreeNode.SelectedImageIndex = m_bRunInParallel ? 4 : 5; }
		}

		[XmlAttribute, DefaultValue( false )]
		[Category( "List" )]
		public bool StopIfOneFailed
		{
			get { return m_bStopIfOneFailed; }
			set { m_bStopIfOneFailed = value; }
		}

		//[XmlAttribute, DefaultValue( true )]
		//[Category( "List" )]
		//public bool GarbageCollect
		//{
		//	get { return m_bGarbageCollect; }
		//	set { m_bGarbageCollect = value; }
		//}

		[DefaultValue( false )]
		[Category( "List" )]
		public bool? DontKeepResults
		{
			get
			{
				if( m_bDontKeepResults.HasValue )
					return m_bDontKeepResults;
				else
				{
					if( Parent != null )
						return Parent.DontKeepResults;
					else
						return null;
				}
			}

			set
			{
				m_bDontKeepResults = value;
			}
		}

		[DefaultValue( false )]
		[Category( "List" )]
		public bool SaveAfterEachRun { get; set; }

		[Category( "List" )]
		public string ExternalRunFile { get; set; }

		[Category( "List" )]
		public string ExternalTracesFile { get; set; }

		[Category( "List" )]
		public List<Element> Children
		{
			get { return m_children; }
		}

		[System.ComponentModel.Browsable( false )]
		public List<string> Traces
		{
			get { return m_traces; }
		}
		#endregion

		public TestList()
		{
			//GarbageCollect = true;
			m_children = new List<Element>();
			m_properties = new Dictionary<string, object>();
			m_bRoot = false;
			TreeNode.ImageIndex = TreeNode.SelectedImageIndex = m_bRunInParallel ? 4 : 5;
		}

		static TestList()
		{
			Type[] types = System.Reflection.Assembly.GetExecutingAssembly().GetTypes();
			XmlAttributes listAttribs = new XmlAttributes();

			foreach( var type in types )
			{
				if( type.IsSubclassOf( typeof( Element ) ) )
				{
					listAttribs.XmlElements.Add( new XmlElementAttribute( type.Name, type ) );

					Activator.CreateInstance( type );
				}
			}

			m_aor.Add( typeof( TestList ), "Children", listAttribs );

			m_Global = new TestList();
		}

		public override TestResult Execute()
		{
			int errors = 0, warnings = 0;

			if( !m_bRunInParallel )
			{
				foreach( Element child in m_children )
				{
					if( IsCancelationPending() )
						break;

					if( m_bStopIfOneFailed && errors > 0 )
						break;

					child.Run();

					if( child.RunResult != null )	//it may be null if the node is unchecked
					{
						switch( child.RunResult.Type )
						{
							case TestResults.Failed: errors++;
								break;
							case TestResults.Warning: warnings++;
								break;
						}

						if( DontKeepResults.HasValue && DontKeepResults.Value )
						{
								child.RunResult = child.RunResult.Strip();
							}
						}
					}
				}
			else
			{
				List<WaitHandle> waitHandles = new List<WaitHandle>( m_children.Count );

				foreach( Element child in m_children )
				{
					waitHandles.Add( child.WaitHandle );

					ThreadPool.QueueUserWorkItem( new WaitCallback( child.Run ) );
				}

				System.Threading.WaitHandle.WaitAll( waitHandles.ToArray() );

				foreach( Element child in m_children )
				{
					if( child.RunResult != null )
					{
						switch( child.RunResult.Type )
						{
							case TestResults.Failed: errors++;
								break;
							case TestResults.Warning: warnings++;
								break;
						}

						if( DontKeepResults.HasValue && DontKeepResults.Value )
						{
								child.RunResult = child.RunResult.Strip();
							}
						}
					}
				}

			if( DontKeepResults.HasValue && DontKeepResults.Value )
			{
				this.Description = null;
			}

			//if( m_bGarbageCollect )
			//{

			//}

			TestResult result;
			if( errors > 0 )
				result = new TestResult( TestResults.Failed );
			else if( warnings > 0 )
				result = new TestResult( TestResults.Warning );
			else
				result = new TestResult( TestResults.Success );

			return result;
		}

		internal override void Run()
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
				DateTime startTime = DateTime.Now;

				if( bMultipleRuns )
				{
					int runCounter = 0;
					int errors = 0, warnings = 0;
					List<TestList> innerLists = new List<TestList>();
					DateTime maxTime = repeatDuration == double.MaxValue ? DateTime.MaxValue : startTime.AddSeconds( repeatDuration );

					XmlWriter externalRunFileWriter = null;

					if( SaveAfterEachRun )
					{
						string timestamp = DateTime.Now.ToString( "yyyy-MM-dd-HH-mm-ss", System.Globalization.DateTimeFormatInfo.InvariantInfo );
						ExternalRunFile = timestamp + "Results.xml";
						ExternalTracesFile = timestamp + "Traces.xml";

						externalRunFileWriter = XmlWriter.Create( ExternalRunFile );
						externalRunFileWriter.WriteStartElement( "TestList", "http://softing-ia.com/TestClient.xsd" );

						m_externalTracesFileWriter = XmlWriter.Create( ExternalTracesFile );
						m_externalTracesFileWriter.WriteStartElement( "TestList", "http://softing-ia.com/TestClient.xsd" );
						m_externalTracesFileWriter.WriteStartElement( "Traces" );
					}

					//set a temporary result to be able to refresh the view of partial results
					RunResult = new TestListTestResult( TestResults.Executing );
					RunResult.StartTime = startTime;
					( (TestListTestResult) RunResult ).TestLists = innerLists;

					while( runCounter < repeatCount && DateTime.Now <= maxTime && !IsCancelationPending() )
					{
						TestResult runResult;

						TestList clone = (TestList) Clone();

						if( !SaveAfterEachRun )
						{
							lock( innerLists )
							{
								innerLists.Add( clone );
							}
							ReportFinished();
						}

						runResult = clone.RunOnce();
						clone.RunResult = runResult;

						if( SaveAfterEachRun )
						{
							lock( m_serializer )
							{
								Element.Serializing = true;
								m_serializer.Serialize( externalRunFileWriter, clone );
								Element.Serializing = false;
							}
						}

						switch( runResult.Type )
						{
							case TestResults.Failed: errors++;
								break;
							case TestResults.Warning: warnings++;
								break;
						}

						runCounter++;
					}

					if( SaveAfterEachRun )
					{
						externalRunFileWriter.WriteEndElement();
						externalRunFileWriter.Close();

						m_externalTracesFileWriter.WriteEndElement();
						m_externalTracesFileWriter.WriteEndElement();
						m_externalTracesFileWriter.Close();
					}

					TestListTestResult listResult = (TestListTestResult) RunResult;

					if( errors > 0 )
						listResult.Type = TestResults.Failed;
					else if( warnings > 0 )
						listResult.Type = TestResults.Warning;
					else
						listResult.Type = TestResults.Success;

					listResult.EndTime = DateTime.Now;
					listResult.StartTime = startTime;
					result = listResult;
				}
				else
				{
					SaveAfterEachRun = false;
					RunResult = new TestResult( TestResults.Executing );
					RunResult.StartTime = startTime;
					ReportFinished();

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

		public void RunAll( BackgroundWorker worker )
		{
			m_bRoot = true;
			m_traces = new List<string>();
			m_backgroundWorker = worker;
			Run();
			m_bRoot = false;
		}

		public override object Clone()
		{
			TestList clone = (TestList) base.Clone();
			clone.Parent = this;
			clone.m_children = new List<Element>();
			clone.m_properties = new Dictionary<string, object>();
			clone.m_bRoot = false;
			clone.m_traces = null;
			clone.ExternalRunFile = null;
			clone.ExternalTracesFile = null;
			clone.TreeNode.ImageIndex = clone.TreeNode.SelectedImageIndex = clone.m_bRunInParallel ? 4 : 5;

			foreach( var child in m_children )
			{
				clone.Add( (Element) child.Clone() );
			}

			return clone;
		}

		public void Add( Element el )
		{
			m_children.Add( el );
			el.Parent = this;
			TreeNode.Nodes.Add( el.TreeNode );
		}

		public void Remove( Element el )
		{
			m_children.Remove( el );
			el.Parent = null;
			TreeNode.Nodes.Remove( el.TreeNode );
		}

		public void InsertAfter( Element el, Element elToInsert )
		{
			m_children.Insert( m_children.IndexOf( el ) + 1, elToInsert );
			elToInsert.Parent = this;
			TreeNode.Nodes.Insert( TreeNode.Nodes.IndexOf( el.TreeNode ) + 1, elToInsert.TreeNode );
		}

		protected override object GetProperty( string name )
		{
			name = ExpandProperty( name );

			object o;
			if( !m_properties.TryGetValue( name, out o ) )
			{
				if( Parent != null )
					return Parent.GetProperty( name );
				else
					if( this != m_Global )
						return m_Global.GetProperty( name );
			}

			return o;
		}

		protected override void SetProperty( string name, object o )
		{
			m_properties[ExpandProperty( name )] = o;
		}

		protected override void UnsetProperty( string name )
		{
			m_properties.Remove( ExpandProperty( name ) );
		}

		protected override void WriteTrace( DateTime dateTime, string name, string text )
		{
			if( m_bRoot )
			{
				string line = string.Format( "{0}\t{1}\t{2}", dateTime, name, text );

				lock( m_traces )
				{
					if( SaveAfterEachRun )
					{
						m_externalTracesFileWriter.WriteStartElement( "string" );
						m_externalTracesFileWriter.WriteValue( line );
						m_externalTracesFileWriter.WriteEndElement();
					}
					else
					m_traces.Add( line );
				}
			}
			else
				Parent.WriteTrace( dateTime, name, text );
		}

		public override void ClearResults()
		{
			base.ClearResults();
			m_properties.Clear();

			foreach( Element el in m_children )
			{
				el.ClearResults();
			}
		}

		protected override void ReportFinished()
		{
			ReportFinished( this );
		}

		internal void ReportFinished( Element el )
		{
			if( m_bRoot )
			{
				if( m_backgroundWorker != null )
					m_backgroundWorker.ReportProgress( 0, el );
			}
			else
				Parent.ReportFinished( el );
		}

		internal override bool IsCancelationPending()
		{
			if( m_bRoot )
			{
				if( m_backgroundWorker != null )
					return m_backgroundWorker.CancellationPending;
				else
					return false;
			}
			else
				return Parent.IsCancelationPending();
		}

		internal override void RefreshView()
		{
			base.RefreshView();

			foreach( Element el in m_children )
			{
				el.RefreshView();
			}

			if( RunResult is TestListTestResult )
			{
				int counter = 1;
				var steps = ( (TestListTestResult) RunResult ).TestLists;

				lock( steps )
				{
					foreach( var list in steps )
					{
						list.FixChildren();
						list.RefreshView();

						System.Windows.Forms.TreeNode runNode = list.TreeNode;

						if( runNode.Tag != list.RunResult )
						{
							runNode.Tag = list.RunResult;
							runNode.Text = string.Format( "Run #{0}", counter );

							if( list.RunResult != null )
							{
								switch( list.RunResult.Type )
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
							}

							if( !ResultsTreeNode.Nodes.Contains( runNode ) )
								ResultsTreeNode.Nodes.Add( runNode );

							if( list.ResultsTreeNode.Tag == runNode.Tag )
								list.ResultsTreeNode.Remove();
						}

						counter++;
					}
				}
			}
		}

		public static TestList LoadFromFile( string filePath )
		{
			using( var stream = new FileStream( filePath, FileMode.Open ) )
			{
				TestList tl = LoadFromFile( stream, Path.GetDirectoryName( filePath ) );
				tl.ImportedFile = new ImportFile( filePath );
				return tl;
			}
		}

		public static TestList LoadFromFile( Stream stream, string path = null )
		{
			if( m_serializer == null )
				CreateSerializer();

			TestList tl;

			lock( m_serializer )
			{
				tl = (TestList) m_serializer.Deserialize( stream );
			}

			tl.LoadExternalResults();
			tl.LoadImportedFiles( path );
			tl.FixChildren();

			return tl;
		}

		private static void CreateSerializer()
		{
			m_serializer = new XmlSerializer( typeof( TestList ), m_aor, null, null, "http://softing-ia.com/TestClientCOM.xsd" ); ;
		}

		private void LoadExternalResults()
		{
			if( RunResult != null && RunResult is TestListTestResult && SaveAfterEachRun )
			{
				if( !string.IsNullOrEmpty( ExternalRunFile ) )
				{
					TestList tl = TestList.LoadFromFile( ExternalRunFile );

					foreach( Element el in tl.Children )
					{
						( (TestListTestResult) RunResult ).TestLists.Add( (TestList) el );
					}
				}

				/*if( !string.IsNullOrEmpty( ExternalTracesFile ) )
				{
					TestList tl = TestList.LoadFromFile( ExternalTracesFile );

					Traces.AddRange( tl.Traces );
				}*/
			}
		}

		private void LoadImportedFiles( string path )
		{
			for( int i = 0; i < Children.Count; i++ )
			{
				Element el = Children[i];
				ImportFile imf = el as ImportFile;

				if( imf != null )
				{
					TestList tl = TestList.LoadFromFile( Path.Combine( path, imf.FilePath ) );
					tl.ImportedFile = imf;
					tl.Parent = this;
					Children[i] = tl;
				}

				TestList il = el as TestList;
				if( il != null )
					il.LoadImportedFiles( path );
			}
		}

		public void FixChildren()
		{
			foreach( Element el in Children )
			{
				if( el.Parent != this )
					el.Parent = this;

				if( !this.TreeNode.Nodes.Contains( el.TreeNode ) )
					this.TreeNode.Nodes.Add( el.TreeNode );

				ITestContainer c = el as ITestContainer;
				if( c != null )
					c.FixChildren();
			}
		}

		public void SaveToFile( string filePath )
		{
			using( FileStream fs = new FileStream( "out.xml", FileMode.Create ) )
			{
				SaveToFile( fs );
			}
		}

		internal void SaveToFile( Stream stream )
		{
			if( m_serializer == null )
				CreateSerializer();

			XmlWriterSettings settings = new XmlWriterSettings();
			settings.Indent = true;
			settings.CloseOutput = true;

			using( XmlWriter writer = XmlWriter.Create( stream, settings ) )
			{
				lock( m_serializer )
				{
					m_serializer.Serialize( writer, this );
				}
			}
		}

		internal static void SetGlobalProperties( System.Collections.Specialized.StringCollection names, System.Collections.Specialized.StringCollection values )
		{
			m_Global.m_properties.Clear();

			if( names != null && values != null )
			{
				for( int i = 0; i < names.Count; i++ )
				{
					m_Global.m_properties.Add( names[i], values[i] );
				}
			}
		}

		[XmlIgnore]
		[Description( "Integer value representing maximum duration in seconds" )]
		public Dictionary<string, object> Properties
		{
			get
			{
				return m_properties;
			}
		}

		[XmlIgnore]
		public ImportFile ImportedFile
		{
			get
			{
				return m_importedFile;
			}

			set
			{
				m_importedFile = value;

				if( string.IsNullOrEmpty( DisplayName ) && m_importedFile != null )
					DisplayName = m_importedFile.FilePath;
			}
		}

		#region Private Fields
		bool m_bRunInParallel;
		bool m_bStopIfOneFailed;
		//bool m_bGarbageCollect;
		bool? m_bDontKeepResults;
		List<Element> m_children;
		Dictionary<string, object> m_properties;
		bool m_bRoot;
		ImportFile m_importedFile;
		List<string> m_traces;
		BackgroundWorker m_backgroundWorker;
		XmlWriter m_externalTracesFileWriter;

		static XmlSerializer m_serializer;
		internal static XmlAttributeOverrides m_aor = new XmlAttributeOverrides();
		static TestList m_Global;

		#endregion
	}

	public class TestListTestResult : TestResult
	{
		[Description( "List containing a copy of the sets of tests for each test run in this list" ), Category( "List" )]
		public List<TestList> TestLists { get; set; }

		public TestListTestResult()
		{
		}

		public TestListTestResult( TestResults type )
			: base( type )
		{
		}
	}

	public sealed class ImportFile : Element
	{
		[Category( "List" )]
		public string FilePath
		{
			get;
			set;
		}

		public ImportFile()
		{
		}

		public ImportFile( string filePath )
		{
			FilePath = filePath;
		}

		public override TestResult Execute()
		{
			throw new NotImplementedException();
		}

		public override string ToString()
		{
			return FilePath;
		}
	}
}
