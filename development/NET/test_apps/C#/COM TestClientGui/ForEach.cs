using System.Xml.Serialization;
using System.Collections.Generic;
using System.Threading;
using System.ComponentModel;
using System;
using System.Collections;

namespace TestClientGui.Elements
{
	public sealed class ForEach : Element, ITestContainer
	{
		#region Public Properties
		[XmlAttribute]
		public string Property { get; set; }

		[XmlAttribute]
		public string inProperty { get; set; }

		[Category( "List" )]
		public List<Element> Children
		{
			get { return m_children; }
		}
		#endregion

		public ForEach()
		{
			m_children = new List<Element>();
			TreeNode.ImageIndex = TreeNode.SelectedImageIndex = 5;
		}

		static ForEach()
		{
			Type[] types = System.Reflection.Assembly.GetExecutingAssembly().GetTypes();
			XmlAttributes listAttribs = new XmlAttributes();

			foreach( var type in types )
			{
				if( type.IsSubclassOf( typeof( Element ) ) )
				{
					listAttribs.XmlElements.Add( new XmlElementAttribute( type.Name, type ) );
				}
			}

			TestList.m_aor.Add( typeof( ForEach ), "Children", listAttribs );
		}

		public override TestResult Execute()
		{
			int errors = 0, warnings = 0;

			foreach( Element child in m_children )
			{
				if( IsCancelationPending() )
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
				}
			}

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

				int repeatCount = int.MaxValue;

				if( RepeatCount != null )
				{
					int tmp;
					if( int.TryParse( RepeatCount, out tmp ) )
					{
						repeatCount = tmp;
					}
				}

				double repeatDuration = double.MaxValue;

				if( RepeatDuration != null )
				{
					double tmp;
					if( double.TryParse( RepeatDuration, out tmp ) )
					{
						repeatDuration = tmp;
					}
				}


				TestResult result = null;
				DateTime startTime = DateTime.Now;

				int counter = 0;
				int errors = 0, warnings = 0;
				List<ForEach> innerLists = new List<ForEach>();
				DateTime maxTime = repeatDuration == double.MaxValue ? DateTime.MaxValue : startTime.AddSeconds( repeatDuration );

				//set a temporary result to be able to refresh the view of partial results
				RunResult = new ForEachTestResult( TestResults.Executing );
				RunResult.StartTime = startTime;
				( (ForEachTestResult) RunResult ).Steps = innerLists;

				IEnumerable collection = GetProperty( inProperty ) as IEnumerable;

				foreach( var o in collection )
				{
					if( counter >= repeatCount || DateTime.Now > maxTime || IsCancelationPending() )
						break;

					SetProperty( Property, o );

					ForEach clone = (ForEach) Clone();

					lock( innerLists )
					{
						innerLists.Add( clone );
					}

					ReportFinished();

					TestResult runResult = clone.RunOnce();
					clone.RunResult = new ForCursorTestResult( runResult, o );

					switch( runResult.Type )
					{
						case TestResults.Failed: errors++;
							break;
						case TestResults.Warning: warnings++;
							break;
					}

					counter++;
				}

				ForEachTestResult forResult;

				if( errors > 0 )
					forResult = new ForEachTestResult( TestResults.Failed );
				else if( warnings > 0 )
					forResult = new ForEachTestResult( TestResults.Warning );
				else
					forResult = new ForEachTestResult( TestResults.Success );

				forResult.StartTime = startTime;
				forResult.Steps = innerLists;
				result = forResult;


				RunResult = result;
				ReportFinished();
			}
			finally
			{
				WaitHandle.Set();
			}
		}

		public override object Clone()
		{
			ForEach clone = (ForEach) base.Clone();
			clone.m_children = new List<Element>();
			clone.TreeNode.ImageIndex = clone.TreeNode.SelectedImageIndex = 5;

			foreach( var child in m_children )
			{
				clone.Add( (Element) child.Clone() );
			}

			return clone;
		}

		public void Add( Element el )
		{
			m_children.Add( el );
			el.Parent = Parent;
			TreeNode.Nodes.Add( el.TreeNode );
		}

		public void Remove( Element el )
		{
			m_children.Remove( el );
			el.Parent = null;
			TreeNode.Nodes.Remove( el.TreeNode );
		}

		public override void ClearResults()
		{
			base.ClearResults();

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
			Parent.ReportFinished( el );
		}

		internal override bool IsCancelationPending()
		{
			return Parent.IsCancelationPending();
		}

		internal override void RefreshView()
		{
			base.RefreshView();

			foreach( Element el in m_children )
			{
				el.RefreshView();
			}

			if( RunResult is ForEachTestResult )
			{
				int counter = 1;
				var steps = ( (ForEachTestResult) RunResult ).Steps;

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

		public void FixChildren()
		{
			foreach( Element el in Children )
			{
				if( el.Parent != Parent )
					el.Parent = Parent;

				if( !this.TreeNode.Nodes.Contains( el.TreeNode ) )
					this.TreeNode.Nodes.Add( el.TreeNode );

				ITestContainer c = el as ITestContainer;
				if( c != null )
					c.FixChildren();
			}
		}

		#region Private Fields
		List<Element> m_children;

		internal static XmlAttributeOverrides m_aor = new XmlAttributeOverrides();
		#endregion
	}

	public class ForEachTestResult : TestResult
	{
		[Description( "List containing a copy of the sets of tests for each step" ), Category( "List" )]
		public List<ForEach> Steps { get; set; }

		public ForEachTestResult()
		{
		}

		public ForEachTestResult( TestResults type )
			: base( type )
		{
		}
	}

	public class ForCursorTestResult : TestResult
	{
		[XmlIgnore]
		[Description( "The value stored in the property for this step" )]
		[TypeConverter( typeof( ExpandableObjectConverter ) )]
		public object Cursor { get; set; }

		public ForCursorTestResult()
		{
		}

		public ForCursorTestResult( TestResult initial, object o )
			: base( initial.Type )
		{
			this.Details = initial.Details;
			this.EndTime = initial.EndTime;
			this.Error = initial.Error;
			this.StartTime = initial.StartTime;

			Cursor = o;
		}
	}
}
