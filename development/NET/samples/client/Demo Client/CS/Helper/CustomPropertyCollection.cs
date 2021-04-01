using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing.Design;

namespace DemoClient.Helper
{
    /// <summary>
    /// CustomPropertyCollection (Which is binding to property grid)
    /// </summary>
    public class CustomProperties : ICustomTypeDescriptor
    {
        #region PropertySpecCollection class definition

        /// <summary>
        /// Encapsulates a collection of PropertySpec objects.
        /// </summary>
        [Serializable]
        public class CustomPropertyCollection : IList
        {
            private ArrayList innerArray;

            /// <summary>
            /// Initializes a new instance of the CustomPropertyCollection class.
            /// </summary>
            public CustomPropertyCollection()
            {
                innerArray = new ArrayList();
            }

            /// <summary>
            /// Gets the number of elements in the CustomPropertyCollection.
            /// </summary>
            /// <value></value>
            public int Count
            {
                get { return innerArray.Count; }
            }

            /// <summary>
            /// Gets a value indicating whether the CustomPropertyCollection has a fixed size.
            /// </summary>
            /// <value></value>
            public bool IsFixedSize
            {
                get { return false; }
            }

            /// <summary>
            /// Gets a value indicating whether the CustomPropertyCollection is read-only.
            /// </summary>
            public bool IsReadOnly
            {
                get { return false; }
            }

            /// <summary>
            /// Gets a value indicating whether access to the collection is synchronized (thread-safe).
            /// </summary>
            /// <value></value>
            public bool IsSynchronized
            {
                get { return false; }
            }

            /// <summary>
            /// Gets an object that can be used to synchronize access to the collection.
            /// </summary>
            /// <value></value>
            object ICollection.SyncRoot
            {
                get { return null; }
            }

            /// <summary>
            /// Gets or sets the element at the specified index.
            ///</summary>
            /// <param name="index"></param>
            /// <value></value>
            public CustomProperty this[int index]
            {
                get { return (CustomProperty)innerArray[index]; }
                set { innerArray[index] = value; }
            }

            /// <summary>
            /// Adds a CustomProperty to the end of the CustomPropertyCollection.
            /// </summary>
            /// <param name="value"></param>
            /// <returns></returns>
            public int Add(CustomProperty value)
            {
                int index = innerArray.Add(value);

                return index;
            }

            /// <summary>
            /// Adds the elements of an array of CustomProperty objects to the end of the CustomPropertyCollection.
            /// </summary>
            /// <param name="array"></param>
            public void AddRange(CustomProperty[] array)
            {
                innerArray.AddRange(array);
            }

            /// <summary>
            /// Removes all elements from the CustomPropertyCollection.
            /// </summary>
            public void Clear()
            {
                innerArray.Clear();
            }

            /// <summary>
            /// Determines whether a CustomProperty is in the CustomPropertyCollection.
            /// </summary>
            /// <param name="item"></param>
            /// <returns></returns>
            public bool Contains(CustomProperty item)
            {
                return innerArray.Contains(item);
            }

            /// <summary>
            /// Determines whether a CustomProperty with the specified name is in the CustomPropertyCollection.
            /// </summary>
            /// <param name="name"></param>
            /// <returns></returns>
            public bool Contains(string name)
            {
                foreach (CustomProperty spec in innerArray)
                    if (spec.Name == name)
                        return true;

                return false;
            }

            /// <summary>
            /// Copies the entire CustomPropertyCollection to a compatible one-dimensional Array, starting at the
            /// beginning of the target array.
            /// </summary>
            /// <param name="array"></param>
            public void CopyTo(CustomProperty[] array)
            {
                innerArray.CopyTo(array);
            }

            /// <summary>
            /// Copies the CustomPropertyCollection or a portion of it to a one-dimensional array.
            /// </summary>
            /// <param name="array"></param>
            /// <param name="index"></param>
            public void CopyTo(CustomProperty[] array, int index)
            {
                innerArray.CopyTo(array, index);
            }

            /// <summary>
            /// Returns an enumerator that can iterate through the CustomPropertyCollection.
            /// </summary>
            /// <returns></returns>
            public IEnumerator GetEnumerator()
            {
                return innerArray.GetEnumerator();
            }

            /// <summary>
            /// Searches for the specified CustomProperty
            /// </summary>
            /// <param name="value"></param>
            /// <returns></returns>
            public int IndexOf(CustomProperty value)
            {
                return innerArray.IndexOf(value);
            }

            /// <summary>
            /// Searches for the CustomProperty with the specified name 
            /// </summary>
            /// <param name="name"></param>
            /// <returns></returns>
            public int IndexOf(string name)
            {
                int i = 0;

                foreach (CustomProperty spec in innerArray)
                {
                    if (spec.Name == name)
                        return i;

                    i++;
                }

                return -1;
            }

            /// <summary>
            /// Inserts a CustomProperty object into the CustomPropertyCollection at the specified index.
            /// </summary>
            /// <param name="index"></param>
            /// <param name="value"></param>
            public void Insert(int index, CustomProperty value)
            {
                innerArray.Insert(index, value);
            }

            /// <summary>
            /// Removes the first occurrence of a specific object from the CustomPropertyCollection.
            /// </summary>
            /// <param name="obj"></param>
            public void Remove(CustomProperty obj)
            {
                innerArray.Remove(obj);
            }

            /// <summary>
            /// Removes the property with the specified name from the CustomPropertyCollection.
            /// </summary>
            /// <param name="name"></param>
            public void Remove(string name)
            {
                int index = IndexOf(name);
                RemoveAt(index);
            }

            /// <summary>
            /// Removes the object at the specified index of the CustomPropertyCollection.
            /// </summary>
            /// <param name="index"></param>
            public void RemoveAt(int index)
            {
                innerArray.RemoveAt(index);
            }

            /// <summary>
            /// Copies the elements of the CustomPropertyCollection to a new CustomProperty array.
            /// </summary>
            /// <returns></returns>
            public CustomProperty[] ToArray()
            {
                return (CustomProperty[])innerArray.ToArray(typeof(CustomProperty));
            }

            #region Explicit interface implementations for ICollection and IList

            void ICollection.CopyTo(Array array, int index)
            {
                CopyTo((CustomProperty[])array, index);
            }

            int IList.Add(object value)
            {
                return Add((CustomProperty)value);
            }

            bool IList.Contains(object obj)
            {
                return Contains((CustomProperty)obj);
            }

            object IList.this[int index]
            {
                get
                {
                    return ((CustomPropertyCollection)this)[index];
                }
                set
                {
                    ((CustomPropertyCollection)this)[index] = (CustomProperty)value;
                }
            }

             int IList.IndexOf(object obj)
            {
                return IndexOf((CustomProperty)obj);
            }

            void IList.Insert(int index, object value)
            {
                Insert(index, (CustomProperty)value);
            }

            void IList.Remove(object value)
            {
                Remove((CustomProperty)value);
            }
            #endregion
        }
        #endregion

		#region TypeDescriptor Implementation

		
		AttributeCollection ICustomTypeDescriptor.GetAttributes()
		{
			return TypeDescriptor.GetAttributes(this, true);
		}

		string ICustomTypeDescriptor.GetClassName()
		{
			return TypeDescriptor.GetClassName(this, true);
		}

		string ICustomTypeDescriptor.GetComponentName()
		{
			return TypeDescriptor.GetComponentName(this, true);
		}

		TypeConverter ICustomTypeDescriptor.GetConverter()
		{
			return TypeDescriptor.GetConverter(this, true);
		}

		EventDescriptor ICustomTypeDescriptor.GetDefaultEvent()
		{
			return TypeDescriptor.GetDefaultEvent(this, true);
		}

		PropertyDescriptor ICustomTypeDescriptor.GetDefaultProperty()
		{
			return TypeDescriptor.GetDefaultProperty(this, true);
		}

		object ICustomTypeDescriptor.GetEditor(Type editorBaseType)
		{
			return TypeDescriptor.GetEditor(this, editorBaseType, true);
		}

		EventDescriptorCollection ICustomTypeDescriptor.GetEvents()
		{
			return TypeDescriptor.GetEvents(this, true);
		}

		EventDescriptorCollection ICustomTypeDescriptor.GetEvents(Attribute[] attributes)
		{
			return TypeDescriptor.GetEvents(this, attributes, true);
		}

		PropertyDescriptorCollection ICustomTypeDescriptor.GetProperties()
		{
            return ((ICustomTypeDescriptor)this).GetProperties(new Attribute[0]);
		}

		PropertyDescriptorCollection ICustomTypeDescriptor.GetProperties(Attribute[] attributes)
		{
            ArrayList props = new ArrayList();

            foreach (CustomProperty property in properties)
            {
                ArrayList attrs = new ArrayList();

                // If a category, description, editor, or type converter are specified
                // in the PropertySpec, create attributes to define that relationship.
                if (property.Category != null)
                    attrs.Add(new CategoryAttribute(property.Category));

                if (property.Description != null)
                    attrs.Add(new DescriptionAttribute(property.Description));

                if (property.EditorTypeName != null)
                    attrs.Add(new EditorAttribute(property.EditorTypeName, typeof(UITypeEditor)));

                if (property.ConverterTypeName != null)
                    attrs.Add(new TypeConverterAttribute(property.ConverterTypeName));

                // Additionally, append the custom attributes associated with the
                // PropertySpec, if any.
                if (property.Attributes != null)
                    attrs.AddRange(property.Attributes);

                Attribute[] attrArray = (Attribute[])attrs.ToArray(typeof(Attribute));

                // Create a new property descriptor for the property item, and add
                // it to the list.
                CustomPropertyDescriptor pd = new CustomPropertyDescriptor(property,
                    this, property.Name, attrArray);
                props.Add(pd);
            }

            // Convert the list of PropertyDescriptors to a collection that the
            // ICustomTypeDescriptor can use, and return it.
            PropertyDescriptor[] propArray = (PropertyDescriptor[])props.ToArray(
                typeof(PropertyDescriptor));
            return new PropertyDescriptorCollection(propArray);

		}

		object ICustomTypeDescriptor.GetPropertyOwner(PropertyDescriptor pd)
		{
			return this;
		}

		#endregion

        private string defaultProperty;
		private CustomPropertyCollection properties;

		/// <summary>
		/// Initializes a new instance of the CustomProperties class.
		/// </summary>
        public CustomProperties()
		{
			defaultProperty = null;
			properties = new CustomPropertyCollection();
		}

		/// <summary>
		/// Gets or sets the name of the default property in the collection.
		/// </summary>
		public string DefaultProperty
		{
			get { return defaultProperty; }
			set { defaultProperty = value; }
		}

		/// <summary>
		/// Gets the collection of properties 
		/// </summary>
		public CustomPropertyCollection Properties
		{
			get { return properties; }
		}        
    }

    /// <summary>
    /// Custom property class 
    /// </summary>
    public class CustomProperty
    {
        #region Private Members

        private Attribute[] attributes;
        private string category;
        private object defaultValue;
        private string description;
        private string editor;
        private string name;
        private string type;
        private string typeConverter;

        #endregion

        #region Properties

        /// <summary>
        /// Gets or sets a collection of additional Attributes for this property
        /// </summary>
        public Attribute[] Attributes
        {
            get { return attributes; }
            set { attributes = value; }
        }

        /// <summary>
        /// Gets or sets the category name of this property.
        /// </summary>
        public string Category
        {
            get { return category; }
            set { category = value; }
        }

        /// <summary>
        /// Gets or sets the fully qualified name of the type converter
        /// type for this property.
        /// </summary>
        public string ConverterTypeName
        {
            get { return typeConverter; }
            set { typeConverter = value; }
        }

        /// <summary>
        /// Gets or sets the default value of this property.
        /// </summary>
        public object DefaultValue
        {
            get { return defaultValue; }
            set { defaultValue = value; }
        }

        /// <summary>
        /// Gets or sets the help text description of this property.
        /// </summary>
        public string Description
        {
            get { return description; }
            set { description = value; }
        }

        /// <summary>
        /// Gets or sets the fully qualified name of the editor type for
        /// this property.
        /// </summary>
        public string EditorTypeName
        {
            get { return editor; }
            set { editor = value; }
        }

        /// <summary>
        /// Gets or sets the name of this property.
        /// </summary>
        public string Name
        {
            get { return name; }
            set { name = value; }
        }

        /// <summary>
        /// Gets or sets the fully qualfied name of the type of this
        /// property.
        /// </summary>
        public string TypeName
        {
            get { return type; }
            set { type = value; }
        }

        #endregion

        #region Constructor

        public CustomProperty(string name, string type) : this(name, type, null, null, null) { }

        public CustomProperty(string name, Type type)
            :
            this(name, type.AssemblyQualifiedName, null, null, null) { }

        public CustomProperty(string name, string type, string category) : this(name, type, category, null, null) { }

        public CustomProperty(string name, Type type, string category)
            :
            this(name, type.AssemblyQualifiedName, category, null, null) { }


        public CustomProperty(string name, string type, string category, string description)
            :
            this(name, type, category, description, null) { }


        public CustomProperty(string name, Type type, string category, string description)
            :
            this(name, type.AssemblyQualifiedName, category, description, null) { }

        public CustomProperty(string name, string type, string category, string description, object defaultValue)
        {
            this.name = name;
            this.type = type;
            this.category = category;
            this.description = description;
            this.defaultValue = defaultValue;
            this.attributes = null;
        }

        public CustomProperty(string name, Type type, string category, string description, object defaultValue)
            :
            this(name, type.AssemblyQualifiedName, category, description, defaultValue) { }

        public CustomProperty(string name, string type, string category, string description, object defaultValue,
            string editor, string typeConverter)
            : this(name, type, category, description, defaultValue)
        {
            this.editor = editor;
            this.typeConverter = typeConverter;
        }

        public CustomProperty(string name, Type type, string category, string description, object defaultValue,
            string editor, string typeConverter)
            :
            this(name, type.AssemblyQualifiedName, category, description, defaultValue, editor, typeConverter) { }


        public CustomProperty(string name, string type, string category, string description, object defaultValue,
            Type editor, string typeConverter)
            :
            this(name, type, category, description, defaultValue, editor.AssemblyQualifiedName,
            typeConverter) { }

        public CustomProperty(string name, Type type, string category, string description, object defaultValue,
            Type editor, string typeConverter)
            :
            this(name, type.AssemblyQualifiedName, category, description, defaultValue,
            editor.AssemblyQualifiedName, typeConverter) { }

        public CustomProperty(string name, string type, string category, string description, object defaultValue,
            string editor, Type typeConverter)
            :
            this(name, type, category, description, defaultValue, editor, typeConverter.AssemblyQualifiedName) { }

        public CustomProperty(string name, Type type, string category, string description, object defaultValue,
            string editor, Type typeConverter)
            :
            this(name, type.AssemblyQualifiedName, category, description, defaultValue, editor,
            typeConverter.AssemblyQualifiedName) { }

        public CustomProperty(string name, string type, string category, string description, object defaultValue,
            Type editor, Type typeConverter)
            :
            this(name, type, category, description, defaultValue, editor.AssemblyQualifiedName,
            typeConverter.AssemblyQualifiedName) { }

        public CustomProperty(string name, Type type, string category, string description, object defaultValue,
            Type editor, Type typeConverter)
            :
            this(name, type.AssemblyQualifiedName, category, description, defaultValue,
            editor.AssemblyQualifiedName, typeConverter.AssemblyQualifiedName) { }

        #endregion
    }

    /// <summary>
    /// Custom PropertyDescriptor
    /// </summary>
    public class CustomPropertyDescriptor : PropertyDescriptor
    {
            private CustomProperties customProperties;

			private CustomProperty customProperty;

			public CustomPropertyDescriptor(CustomProperty item, CustomProperties items, string name, Attribute[] attrs) :
				base(name, attrs)
			{
				this.customProperties = items;
				this.customProperty = item;
			}

			public override Type ComponentType
			{
				get { return customProperty.GetType(); }
			}

			public override bool IsReadOnly
			{
				get { return (Attributes.Matches(ReadOnlyAttribute.Yes)); }
			}

			public override Type PropertyType
			{
				get { return Type.GetType(customProperty.TypeName); }
			}

			public override bool CanResetValue(object component)
			{
				if(customProperty.DefaultValue == null)
					return false;
				else
					return !this.GetValue(component).Equals(customProperty.DefaultValue);
			}
                
            public override object GetValue(object component)
            {
                return customProperty.DefaultValue;
            }

			public override void ResetValue(object component)
			{
                SetValue(component, customProperty.DefaultValue);
			}

			public override void SetValue(object component, object value)
			{
				customProperty.DefaultValue = value;
			}

			public override bool ShouldSerializeValue(object component)
			{
				return false;
			}
		}    
   
}
