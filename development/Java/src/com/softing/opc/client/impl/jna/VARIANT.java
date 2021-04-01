package com.softing.opc.client.impl.jna;

import java.util.*;

import com.softing.opc.client.impl.jna.OTClient;
import com.sun.jna.*;
import com.sun.jna.ptr.ByteByReference;
import com.sun.jna.ptr.DoubleByReference;
import com.sun.jna.ptr.FloatByReference;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.LongByReference;
import com.sun.jna.ptr.NativeLongByReference;
import com.sun.jna.ptr.PointerByReference;
import com.sun.jna.ptr.ShortByReference;

import java.nio.charset.*;


public  class VARIANT extends com.sun.jna.PointerType {
		
		public Object fromNative(Object nativeValue, FromNativeContext context) 
		{
			Object o = super.fromNative(nativeValue, context);
			if (OTClient.INVALID_POINTER_VARIANT.equals(o))
				return OTClient.INVALID_POINTER_VARIANT;
			return o;
		}
		
		public static short VARIANT_TRUE = -1;
		public static short VARIANT_FALSE = 0;
		
		public static short VT_EMPTY = 0;
		public static short VT_I2 = 2;
		public static short VT_I4 = 3;
		public static short VT_R4 = 4;
		public static short VT_R8 = 5;
		public static short VT_CY = 6;
		public static short VT_DATE = 7;
		public static short VT_BSTR = 8;
		public static short VT_BOOL = 11;
		public static short VT_VARIANT = 12;
		public static short VT_I1 = 16;
		public static short VT_UI1	= 17;
		public static short VT_UI2	= 18;
		public static short VT_UI4	= 19;
		public static short VT_I8	= 20;
		public static short VT_UI8	= 21;
		public static short VT_INT	= 22;
		public static short VT_UINT	= 23;
		public static short VT_ARRAY	= 0x2000;
		
		public static short typeCodes[]={VT_EMPTY, VT_I2, VT_I4, VT_R4, VT_R8, VT_CY, VT_DATE, VT_BSTR, VT_BOOL, VT_VARIANT, VT_I1, VT_UI1, VT_UI2, VT_UI4, VT_I8, VT_UI8, VT_INT, VT_UINT, (short)(VT_ARRAY | VT_I1),(short)(VT_ARRAY | VT_UI1), (short)(VT_ARRAY | VT_I2), (short)(VT_ARRAY | VT_UI2),(short)(VT_ARRAY | VT_I4),(short)(VT_ARRAY | VT_UI4),(short)(VT_ARRAY | VT_I8),(short)(VT_ARRAY | VT_UI8),(short)(VT_ARRAY | VT_INT),(short)(VT_ARRAY | VT_UINT),(short)(VT_ARRAY | VT_R4),(short)(VT_ARRAY | VT_R8),(short)(VT_ARRAY | VT_BOOL),(short)(VT_ARRAY | VT_BSTR),(short)(VT_ARRAY | VT_DATE),(short)(VT_ARRAY | VT_CY), (short)(VT_ARRAY | VT_VARIANT)};
		public static String typeCodesToString[]={"VT_EMPTY", "VT_I2", "VT_I4", "VT_R4", "VT_R8", "VT_CY", "VT_DATE", "VT_BSTR", "VT_BOOL", "VT_VARIANT", "VT_I1", "VT_UI1", "VT_UI2", "VT_UI4", "VT_I8", "VT_UI8", "VT_INT", "VT_UINT", "VT_I1_ARRAY","VT_UI1_ARRAY","VT_I2_ARRAY","VT_UI2_ARRAY","VT_I4_ARRAY","VT_UI4_ARRAY","VT_I8_ARRAY","VT_UI8_ARRAY","VT_INT_ARRAY","VT_UINT_ARRAY","VT_R4_ARRAY","VT_R8_ARRAY","VT_BOOL_ARRAY","VT_BSTR_ARRAY","VT_DATE_ARRAY","VT_CY_ARRAY","VT_VARIANT_ARRAY"};
		
		//public static int VT_RESERVED	= 0x8000,
		//public static int VT_ILLEGAL	= 0xffff,
		//public static int VT_ILLEGALMASKED	= 0xfff,
		//public static int VT_TYPEMASK	= 0xfff
	
	public VARIANT(com.sun.jna.Pointer pointer) {
		super(pointer);
	}
	
	
	public VARIANT(Pointer p, Object o)
	{
		//p pointer allocation is outside the method call
		//a reference to it should be kept in order to avoid garbage collection
		//(if not allocated in vative heap)
		
		super(p);
		this.setPointer(p);
		
		OTClient.INSTANCE.OTVariantInit(this);
		OTClient.INSTANCE.OTVariantClear(this);
		
		short type = VARIANT.getType(o.getClass());
		this.getPointer().setShort(0, type);
		if(type == VT_I1|| type == VT_UI1)
		{
			this.getPointer().setByte(8, ((Byte)o).byteValue());
		}
		else if(type == VT_I2 || type == VT_UI2)
		{
			this.getPointer().setShort(8, ((Short)o).shortValue());		
		}
		else if(type == VT_BOOL)
		{
			short bov = ((Boolean)o).booleanValue() ? VARIANT_TRUE : VARIANT_FALSE;
			this.getPointer().setShort(8, bov);
		}
		else if(type == VT_I4 || type == VT_UI4 || type == VT_INT || type == VT_UINT)
		{
			this.getPointer().setInt(8, ((Integer)o).intValue());	
		}
		else if(type == VT_BSTR)
		{
			Pointer strPointer = OTClient.INSTANCE.OTSysAllocString(new WString(o.toString()));
			this.getPointer().setPointer(8, strPointer);	
		}
		else if (type == VT_R4)
		{
			this.getPointer().setFloat(8, ((Float)o).floatValue());
		}
		else if(type == VT_R8)
		{
			this.getPointer().setDouble(8, ((Double)o).doubleValue());
		}
		else if(type == VT_CY)
		{
			this.getPointer().setLong(8, ((Long)o).longValue());
		}
		else if(type == VT_DATE)
		{
			FILETIME ft = new FILETIME((Date)o);
			long longDate = ft.getCLongDate();
			this.getPointer().setLong(8, longDate);
		}
		else if(type == VT_I8
			|| type == VT_UI8)
		{
			this.getPointer().setLong(8, ((Long)o).longValue());
		}
		else if(type == VT_VARIANT)
		{
			//TODO:
		}
		else if((type & VT_ARRAY) == VT_ARRAY)
		{
			Object[] objArray = (Object[])o;
			SAFEARRAYBOUND sab = new SAFEARRAYBOUND(new NativeLong(objArray.length), new NativeLong(0));
			Pointer ptrSa = OTClient.INSTANCE.OTSafeArrayCreate((short)(type & ~VT_ARRAY) , (short)1, sab);
			//SAFEARRAY sa = new SAFEARRAY(ptrSa);
			//SAFEARRAY sa = OTClient.INSTANCE.OTSafeArrayCreate((short)(type & ~VT_ARRAY) , (short)1, sab);

			for(int i = 0 ; i < objArray.length; i++)
			{
				Pointer pData = null;
				Pointer pStr = null;
				if(type == (VT_ARRAY | VT_I1))
				{					
					pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(Byte.SIZE));
					pData.setByte(0, (Byte)objArray[i]);
				}
				else if(type == (VT_ARRAY | VT_I2))
				{
					pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(Short.SIZE));
					pData.setShort(0, (Short)objArray[i]);
				}
				else if(type == (VT_ARRAY | VT_BOOL))
				{
					pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(Short.SIZE));
					short bov = ((Boolean)objArray[i]).booleanValue() ? VARIANT_TRUE : VARIANT_FALSE;
					pData.setShort(0, bov);
				}
				else if(type == (VT_ARRAY | VT_I4))
				{
					pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(NativeLong.SIZE));
					pData.setInt(0, (Integer)objArray[i]);
				}
				else if(type == (VT_ARRAY | VT_I8))
				{
					pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(8));
					pData.setLong(0, (Long)objArray[i]);
				}
				else if(type == (VT_ARRAY | VT_R4))
				{
					pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(4));
					pData.setFloat(0, (Float)objArray[i]);
				}
				else if(type == (VT_ARRAY | VT_R8))
				{
					pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(8));
					pData.setDouble(0, (Double)objArray[i]);
				}
				else if(type == (VT_ARRAY | VT_DATE))
				{
					FILETIME ft = new FILETIME((Date)objArray[i]);
					long longDate = ft.getCLongDate();
					
					pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(8));
					pData.setLong(0, longDate);
				}
				else if(type == (VT_ARRAY | VT_CY))
				{
					pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(8));
					pData.setLong(0, (Long)objArray[i]);
				}
				else if(type == (VT_ARRAY | VT_BSTR))
				{
					pStr = OTClient.INSTANCE.OTSysAllocString(new WString((String)objArray[i]));
					pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(Integer.SIZE));
					pData.setPointer(0, pStr);
				}
				NativeLongByReference nlbr = new NativeLongByReference(new NativeLong(i));
				OTClient.INSTANCE.OTSafeArrayPutElement(ptrSa, nlbr, pData);
				OTClient.INSTANCE.OTFreeMemory(pData);
				if(type == (VT_ARRAY | VT_BSTR))
				{
					OTClient.INSTANCE.OTFreeMemory(pStr);
				}
			}
			//this.getPointer().setPointer(8, sa.pvData);
			this.getPointer().setPointer(8, ptrSa);
		}	
	}
	
	public short getType()
	{
		return this.getPointer().getShort(0);
	}
	
	public Object getObject()
	{
		short vartype = getType();
		if(vartype == VT_I1
			|| vartype == VT_UI1)
		{
			return new Byte (this.getPointer().getByte(8));
		}
		else if(vartype == VT_I2
			|| vartype == VT_UI2)
		{
			return new Short(this.getPointer().getShort(8));	
		}
		else if(vartype == VT_BOOL)
		{
			Short s = new Short(this.getPointer().getShort(8));
			return new Boolean(s.equals(VARIANT_TRUE) ? true:false);
		}	
		else if(vartype == VT_I4
			|| vartype == VT_UI4
			|| vartype == VT_INT
			|| vartype == VT_UINT)
		{
			return new Integer(this.getPointer().getInt(8));
		}
		else if(vartype == VT_BSTR)
		{
			return getStringFromBSTRPointer(this.getPointer().getPointer(8));
		}
		else if (vartype == VT_R4)
		{
			return new Float(this.getPointer().getFloat(8));
		}
		else if(vartype == VT_R8)
		{
			return new Double(this.getPointer().getDouble(8));
		}
		else if(vartype == VT_CY)
		{
			return new Long(this.getPointer().getLong(8));
		}
		else if(vartype == VT_DATE)
		{
			double d = this.getPointer().getDouble(8);
			return FILETIME.TimeValToDate(d);
		}
		if(vartype == VT_I8
			|| vartype == VT_UI8)
		{
			return new Long(this.getPointer().getLong(8));
		}
		else if(vartype == VT_VARIANT)
		{
			return null; // TODO: to implement
		}
		else if((vartype & VT_ARRAY) == VT_ARRAY) 
		{
			Object result = null;
			
			SAFEARRAY sa = new SAFEARRAY(this.getPointer().getPointer(8));
			//int size = OTClient.INSTANCE.OTSafeArrayGetElemsize(sa).intValue();
			
			NativeLongByReference nlbrUBound = new NativeLongByReference();
			NativeLongByReference nlbrLBound = new NativeLongByReference();
			
			int nDim = OTClient.INSTANCE.OTSafeArrayGetDim(sa).intValue();
			
			OTClient.INSTANCE.OTSafeArrayGetLBound(sa, (short)nDim, nlbrLBound);
			OTClient.INSTANCE.OTSafeArrayGetUBound(sa, (short)nDim, nlbrUBound);
			
			int upperBound = nlbrUBound.getValue().intValue();
			int lowerBound = nlbrLBound.getValue().intValue();
			int size = upperBound - lowerBound;
			
			if(size >= 0)
			{	
				int arrayIndex = 0;
				if(vartype == (VT_ARRAY | VT_I1)
					|| vartype == (VT_ARRAY | VT_UI1))
				{
					ByteByReference bbr = new ByteByReference();
					Pointer pValue = bbr.getPointer();
					
					Byte[] objArray = new Byte[size];
					for(int i=lowerBound; i < upperBound; i++)
					{
						OTClient.INSTANCE.OTSafeArrayGetElement(sa, new NativeLongByReference(new NativeLong(i)), pValue);
						objArray[arrayIndex++] = pValue.getByte(0);
					}
					result = objArray;
				}
				else if(vartype == (VT_ARRAY | VT_I2)
						|| vartype == (VT_ARRAY | VT_UI2))
				{			
					ShortByReference sbr = new ShortByReference();
					Pointer pValue = sbr.getPointer();
					
					Short[] objArray = new Short[size];
					for(int i=lowerBound; i < upperBound; i++)
					{
						OTClient.INSTANCE.OTSafeArrayGetElement(sa, new NativeLongByReference(new NativeLong(i)), pValue);
						objArray[arrayIndex++] = pValue.getShort(0);
					}
					result = objArray;
				}
				else if(vartype == (VT_ARRAY | VT_I4)
						|| vartype == (VT_ARRAY | VT_UI4)
						|| vartype == (VT_ARRAY | VT_INT)
						|| vartype == (VT_ARRAY | VT_UINT))
				{			
					IntByReference ibr = new IntByReference();
					Pointer pValue = ibr.getPointer();
					
					Integer[] objArray = new Integer[size];
					for(int i=lowerBound; i < upperBound; i++)
					{
						OTClient.INSTANCE.OTSafeArrayGetElement(sa, new NativeLongByReference(new NativeLong(i)), pValue);
						objArray[arrayIndex++] = pValue.getInt(0);
					}
					result = objArray;
				}
				else if(vartype == (VT_ARRAY | VT_I8)
						|| vartype == (VT_ARRAY | VT_UI8)
						|| vartype == (VT_ARRAY | VT_CY))
				{	
					LongByReference lbr = new LongByReference();
					Pointer pValue = lbr.getPointer();
					
					Long[] objArray = new Long[size];
					for(int i=lowerBound; i < upperBound; i++)
					{
						OTClient.INSTANCE.OTSafeArrayGetElement(sa, new NativeLongByReference(new NativeLong(i)), pValue);
						objArray[arrayIndex++] = pValue.getLong(0);
					}
					result = objArray;
				}
				else if(vartype == (VT_ARRAY | VT_R4))
				{	
					FloatByReference fbr = new FloatByReference();
					Pointer pValue = fbr.getPointer();
					
					Float[] objArray = new Float[size];
					for(int i=lowerBound; i < upperBound; i++)
					{
						OTClient.INSTANCE.OTSafeArrayGetElement(sa, new NativeLongByReference(new NativeLong(i)), pValue);
						objArray[arrayIndex++] = pValue.getFloat(0);
					}
					result = objArray;
				}
				else if(vartype == (VT_ARRAY | VT_R8))
				{			
					DoubleByReference dbr = new DoubleByReference();
					Pointer pValue = dbr.getPointer();
					
					Double[] objArray = new Double[size];
					for(int i=lowerBound; i < upperBound; i++)
					{
						OTClient.INSTANCE.OTSafeArrayGetElement(sa, new NativeLongByReference(new NativeLong(i)), pValue);
						objArray[arrayIndex++] = pValue.getDouble(0);
					}
					result = objArray;
				}
				else if(vartype == (VT_ARRAY | VT_DATE))
				{			
					DoubleByReference dbr = new DoubleByReference();
					Pointer pValue = dbr.getPointer();
					
					Date[] objArray = new Date[size];
					for(int i=lowerBound; i < upperBound; i++)
					{
						OTClient.INSTANCE.OTSafeArrayGetElement(sa, new NativeLongByReference(new NativeLong(i)), pValue);
						objArray[arrayIndex++] =  FILETIME.TimeValToDate(pValue.getDouble(0));
					}
					result = objArray;
				}
				else if(vartype == (VT_ARRAY | VT_BSTR))
				{		
					PointerByReference pbr = new PointerByReference();
					Pointer pValue = pbr.getPointer();
					
					String[] objArray = new String[size];
					for(int i=lowerBound; i < upperBound; i++)
					{
						OTClient.INSTANCE.OTSafeArrayGetElement(sa, new NativeLongByReference(new NativeLong(i)), pValue);
						//Pointer pStr = OTClient.INSTANCE.OTSysBstrToString(pValue);
						objArray[arrayIndex++] = getStringFromBSTRPointer(pValue.getPointer(0));
					}
					result = objArray;
				}
				else if(vartype == (VT_ARRAY | VT_BOOL))
				{
					ShortByReference sbr = new ShortByReference();
					Pointer pValue = sbr.getPointer();
					
					Boolean[] objArray = new Boolean[size];
					for(int i=lowerBound; i < upperBound; i++)
					{
						OTClient.INSTANCE.OTSafeArrayGetElement(sa, new NativeLongByReference(new NativeLong(i)), pValue);
						Short s = pValue.getShort(0);
						objArray[arrayIndex++] = new Boolean(s.equals(VARIANT_TRUE) ? true:false);
					}
					result = objArray;	
				}
			}
			return result;
		}
		else
		{
			return null;
		}
	}
	
	private static String getStringFromBSTRPointer(Pointer pBstr)
	{
		if(pBstr != null)
		{
			NativeLongByReference bstrLength = new NativeLongByReference();
			bstrLength.setPointer(pBstr.share(-4));
			
			if(bstrLength.getValue().intValue() >0)
			{
				byte[] bArray = pBstr.getByteArray(0, bstrLength.getValue().intValue());
				String sResult = new String(bArray, Charset.forName("UTF-16LE"));
				return sResult;
			}
			else
			{
				return "";
			}
		}
		else
		{
			return "";
		}
	}
	
	public String getTypeToString()
	{
		String unidentified = "UNIDENTIFIED TYPE";
		short typ = getType();
		for (int i = 0; i< typeCodes.length; i++)
		{
			if(typ == typeCodes[i])
			{
				return typeCodesToString[i];	
			}
		}
		return unidentified;
	}
	
	public String toString()
	{
		return "Type:" + getTypeToString() + ",Value:" + getValueToString();
	}
	
	public String getValueToString()
	{
		Object o = this.getObject();
		short vartype = getType();
		if((vartype & VT_ARRAY) == VT_ARRAY)
		{
			StringBuffer strBuf = new StringBuffer();
			strBuf.append("{");
			if(o != null && (o instanceof Object[]))
			{
				Object[] objArray = (Object[])o;
				boolean bExists = false;
				for(int i = 0; i < objArray.length; i++)
				{
					if(bExists)
					{
						strBuf.append(",");
					}
					strBuf.append(objArray[i].toString());
					bExists = true;
				}
			}
			strBuf.append("}");
			return strBuf.toString();
		}
		else
		{
			return o.toString();
		}
	}
	
	
	
	
	public VARIANT() {
		super();
	}
	
	
	//before calling setBSTR allways call SysAllocString();
	public void setBSTR(Pointer p)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_BSTR);
		this.getPointer().setPointer(8, p);
	}
	public void setI1(byte b)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_I1);
		this.getPointer().setByte(8, b);
	}
	public void setUI1(byte b)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_UI1);
		this.getPointer().setByte(8, b);
	}
	public void setI2(short s)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_I2);
		this.getPointer().setShort(8, s);
	}
	public void setUI2(short us)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_UI2);
		this.getPointer().setShort(8, us);
	}
	public void setI4(int i)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_I4);
		this.getPointer().setInt(8, i);
	}
	public void setUI4(int i)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_UI4);
		this.getPointer().setInt(8, i);
	}
	public void setINT(int i)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_INT);
		this.getPointer().setInt(8, i);
	}
	public void setUINT(int i)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_UINT);
		this.getPointer().setInt(8, i);
	}
	public void setR4(float flt)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_R4);
		this.getPointer().setFloat(8, flt);
	}
	public void setR8(double dbl)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_R8);
		this.getPointer().setDouble(8, dbl);
	}
	/*public void setBOOL(short bo)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_BOOL);
		this.getPointer().setShort(8, bo);
	}*/
	
	public void setBOOL(boolean b)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_BOOL);
		short bov = (b) ? VARIANT_TRUE : VARIANT_FALSE;
		this.getPointer().setShort(8, bov);
	}
	
	public void setCY(long cy)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_CY);
		this.getPointer().setLong(8, cy);
	}
	public void setI8(long i8)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_I8);
		this.getPointer().setLong(8, i8);
	}
	public void setUI8(long ui8)
	{
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_UI8);
		this.getPointer().setLong(8, ui8);
	}
	public void setDATE(Date dt)
	{
		FILETIME ft = new FILETIME((Date)dt);
		long longDate = ft.getCLongDate();
			
		OTClient.INSTANCE.OTVariantClear(this);
		this.getPointer().setShort(0, VT_DATE); 
		this.getPointer().setLong(8, longDate);
	}
	
	public void setByteArray(Byte[] objArray)
	{
		if(objArray != null)
		{
			SAFEARRAYBOUND sab = new SAFEARRAYBOUND(new NativeLong(objArray.length), new NativeLong(0));
			Pointer ptrSa = OTClient.INSTANCE.OTSafeArrayCreate( VT_I1 , (short)1, sab);
			//SAFEARRAY sa = new SAFEARRAY(ptrSa);	
			for (int i = 0; i < objArray.length; i++)
			{
				Pointer pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(Byte.SIZE));
				pData.setByte(0, objArray[i]);
				
				NativeLongByReference nlbr = new NativeLongByReference(new NativeLong(i));
				OTClient.INSTANCE.OTSafeArrayPutElement(ptrSa, nlbr, pData);
				OTClient.INSTANCE.OTFreeMemory(pData);
			}
		}
	}
	
	public void setShortArray(Short[] objArray)
	{
		if(objArray != null)
		{
			SAFEARRAYBOUND sab = new SAFEARRAYBOUND(new NativeLong(objArray.length), new NativeLong(0));
			Pointer ptrSa = OTClient.INSTANCE.OTSafeArrayCreate( VT_I2 , (short)1, sab);
			//SAFEARRAY sa = new SAFEARRAY(ptrSa);	
			for (int i = 0; i < objArray.length; i++)
			{
				Pointer pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(Short.SIZE));
				pData.setShort(0, objArray[i]);
				
				NativeLongByReference nlbr = new NativeLongByReference(new NativeLong(i));
				OTClient.INSTANCE.OTSafeArrayPutElement(ptrSa, nlbr, pData);
				OTClient.INSTANCE.OTFreeMemory(pData);
			}
		}
	}
	
	public void setIntArray(Integer[] objArray)
	{
		if(objArray != null)
		{
			SAFEARRAYBOUND sab = new SAFEARRAYBOUND(new NativeLong(objArray.length), new NativeLong(0));
			Pointer ptrSa = OTClient.INSTANCE.OTSafeArrayCreate( VT_I4 , (short)1, sab);
			//SAFEARRAY sa = new SAFEARRAY(ptrSa);	
			for (int i = 0; i < objArray.length; i++)
			{
				Pointer pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(NativeLong.SIZE));
				pData.setInt(0, objArray[i]);
				
				NativeLongByReference nlbr = new NativeLongByReference(new NativeLong(i));
				OTClient.INSTANCE.OTSafeArrayPutElement(ptrSa, nlbr, pData);
				OTClient.INSTANCE.OTFreeMemory(pData);
			}
		}
	}
	
	public void setLongArray(Long[] objArray)
	{
		if(objArray != null)
		{
			SAFEARRAYBOUND sab = new SAFEARRAYBOUND(new NativeLong(objArray.length), new NativeLong(0));
			Pointer ptrSa = OTClient.INSTANCE.OTSafeArrayCreate( VT_I8 , (short)1, sab);
			//SAFEARRAY sa = new SAFEARRAY(ptrSa);	
			for (int i = 0; i < objArray.length; i++)
			{
				Pointer pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(8));
				pData.setLong(0, objArray[i]);
				
				NativeLongByReference nlbr = new NativeLongByReference(new NativeLong(i));
				OTClient.INSTANCE.OTSafeArrayPutElement(ptrSa, nlbr, pData);
				OTClient.INSTANCE.OTFreeMemory(pData);
			}
		}
	}
	
	public void setFloatArray(Float[] objArray)
	{
		if(objArray != null)
		{
			SAFEARRAYBOUND sab = new SAFEARRAYBOUND(new NativeLong(objArray.length), new NativeLong(0));
			Pointer ptrSa = OTClient.INSTANCE.OTSafeArrayCreate( VT_R4 , (short)1, sab);
			//SAFEARRAY sa = new SAFEARRAY(ptrSa);	
			for (int i = 0; i < objArray.length; i++)
			{
				Pointer pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(4));
				pData.setFloat(0, objArray[i]);
				
				NativeLongByReference nlbr = new NativeLongByReference(new NativeLong(i));
				OTClient.INSTANCE.OTSafeArrayPutElement(ptrSa, nlbr, pData);
				OTClient.INSTANCE.OTFreeMemory(pData);
			}
		}
	}
	
	public void setDoubleArray(Double[] objArray)
	{
		if(objArray != null)
		{
			SAFEARRAYBOUND sab = new SAFEARRAYBOUND(new NativeLong(objArray.length), new NativeLong(0));
			Pointer ptrSa = OTClient.INSTANCE.OTSafeArrayCreate( VT_R8 , (short)1, sab);
			//SAFEARRAY sa = new SAFEARRAY(ptrSa);	
			for (int i = 0; i < objArray.length; i++)
			{
				Pointer pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(8));
				pData.setDouble(0, objArray[i]);
				
				NativeLongByReference nlbr = new NativeLongByReference(new NativeLong(i));
				OTClient.INSTANCE.OTSafeArrayPutElement(ptrSa, nlbr, pData);
				OTClient.INSTANCE.OTFreeMemory(pData);
			}
		}
	}
	
	public void setDateArray(Date[] objArray)
	{
		if(objArray != null)
		{
			SAFEARRAYBOUND sab = new SAFEARRAYBOUND(new NativeLong(objArray.length), new NativeLong(0));
			Pointer ptrSa = OTClient.INSTANCE.OTSafeArrayCreate( VT_DATE , (short)1, sab);
			//SAFEARRAY sa = new SAFEARRAY(ptrSa);	
			for (int i = 0; i < objArray.length; i++)
			{
				FILETIME ft = new FILETIME(objArray[i]);
				long longDate = ft.getCLongDate();
				
				Pointer pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(8));
				pData.setLong(0, longDate);
				
				NativeLongByReference nlbr = new NativeLongByReference(new NativeLong(i));
				OTClient.INSTANCE.OTSafeArrayPutElement(ptrSa, nlbr, pData);
				OTClient.INSTANCE.OTFreeMemory(pData);
			}
		}
	}
	
	public void setBSTRArray(String[] objArray)
	{
		if(objArray != null)
		{
			SAFEARRAYBOUND sab = new SAFEARRAYBOUND(new NativeLong(objArray.length), new NativeLong(0));
			Pointer ptrSa = OTClient.INSTANCE.OTSafeArrayCreate( VT_BSTR , (short)1, sab);
			//SAFEARRAY sa = new SAFEARRAY(ptrSa);	
			for (int i = 0; i < objArray.length; i++)
			{			
				Pointer pStr = OTClient.INSTANCE.OTSysAllocString(new WString((String)objArray[i]));
				Pointer pData = OTClient.INSTANCE.OTAllocateMemory(new NativeLong(4));
				pData.setPointer(0, pStr);
				
				NativeLongByReference nlbr = new NativeLongByReference(new NativeLong(i));
				OTClient.INSTANCE.OTSafeArrayPutElement(ptrSa, nlbr, pData);
				
				OTClient.INSTANCE.OTFreeMemory(pStr);
				OTClient.INSTANCE.OTFreeMemory(pData);
			}
		}
	}
	public static short getType(Object o) {	
		return getType(o.getClass());
	}
	
	public static short getType(Class input) {	
		if (input == null)               return VT_EMPTY;
		else if (input == Byte.class)	  	 return VT_I1;
		else if (input == Short.class)        return VT_I2;
		else if (input == Integer.class)      return VT_I4;
		else if (input == Long.class)         return VT_I8;
		else if (input == Float.class)        return VT_R4;
		else if (input == Double.class)       return VT_R8;
		else if (input == Boolean.class)      return VT_BOOL;
		else if (input == Date.class)		 return VT_DATE;
		else if (input == String.class)       return VT_BSTR;
		//else if (input == Object.class)       return VT_EMPTY;
		else if (input == Byte[].class)       return (short)(VT_ARRAY | VT_I1);
		else if (input == Short[].class)	     return (short)(VT_ARRAY | VT_I2);
		else if (input == Integer[].class)    return (short)(VT_ARRAY | VT_I4);
		else if (input == Long[].class)       return (short)(VT_ARRAY | VT_I8);
		else if (input == Float[].class)    	 return (short)(VT_ARRAY | VT_R4);
		else if (input == Double[].class)   	 return (short)(VT_ARRAY | VT_R8);
		else if (input == Boolean[].class)    return (short)(VT_ARRAY | VT_BOOL);
		else if (input == Date[].class)       return (short)(VT_ARRAY | VT_DATE);
		else if (input == String[].class)     return (short)(VT_ARRAY | VT_BSTR);
		else if (input == Object[].class)   	 return (short)(VT_ARRAY | VT_VARIANT);
		// check for special types.
		else if (input == Class.class)         return VT_I2;
		
		return VT_EMPTY;
	}
	
	public static Class getSysType(short input) {

		if (input == VT_I1) return	Byte.class;
		if (input == VT_UI1) return	Byte.class;
		if (input == VT_I2) return	Short.class;
		if (input == VT_UI2) return	Short.class;
		if (input == VT_I4) return	Integer.class;
		if (input == VT_INT) return Integer.class;
		if (input == VT_UI4) return	Integer.class;
		if (input == VT_UINT) return Integer.class;
		if (input == VT_I8) return	Long.class;
		if (input == VT_UI8) return	Long.class;
		if (input == VT_R4) return	Float.class;
		if (input == VT_R8) return	Double.class;
		if (input == VT_CY) return	Long.class;
		if (input == VT_BOOL) return	Boolean.class;
		if (input == VT_DATE) return	Date.class;
		if (input == VT_BSTR) return	String.class;
		if (input == VT_EMPTY) return	Object.class;
		if (input == (VT_ARRAY | VT_I1)) return	Byte[].class;
		if (input == (VT_ARRAY | VT_UI1)) return	Byte[].class;
		if (input == (VT_ARRAY | VT_I2)) return	Short[].class;
		if (input == (VT_ARRAY | VT_UI2)) return	Short[].class;
		if (input == (VT_ARRAY | VT_I4)) return	Integer[].class;
		if (input == (VT_ARRAY | VT_INT)) return Integer[].class;
		if (input == (VT_ARRAY | VT_UI4)) return	Integer[].class;
		if (input == (VT_ARRAY | VT_UINT)) return	Integer[].class;
		if (input == (VT_ARRAY | VT_I8)) return	Long[].class;
		if (input == (VT_ARRAY | VT_UI8)) return Long[].class;
		if (input == (VT_ARRAY | VT_R4)) return	Float[].class;
		if (input == (VT_ARRAY | VT_R8)) return	Double[].class;
		if (input == (VT_ARRAY | VT_CY)) return	Long[].class;
		if (input == (VT_ARRAY | VT_BOOL)) return	Boolean[].class;
		if (input == (VT_ARRAY | VT_DATE)) return	Date[].class;
		if (input == (VT_ARRAY | VT_BSTR)) return	String[].class;
		if (input == (VT_ARRAY | VT_VARIANT)) return Object[].class;
		
		return null;
	}		
	
}
