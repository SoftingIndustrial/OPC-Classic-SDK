#ifndef _VALUEQT_H_
#define _VALUEQT_H_

#include "OSCompat.h"
#include "Enums.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif


//-----------------------------------------------------------------------------
// DateTime                                                                   |
//-----------------------------------------------------------------------------
//! Represents an instant in time, typically expressed as a date and time of day.
class TBC_EXPORT DateTime
{
public:
	//! Initializes a new instance of the DateTime class.
	DateTime()
	{
		init();
	};

	//! The copy constructor of the DateTime class.
	DateTime(const DateTime& dateTime /*! Reference to the DateTime object to copy information from. */);

#ifdef TBC_OS_WINDOWS

	//! Initializes a new instance of the DateTime class to the value indicated by a pointer to a FILETIME structure.
	DateTime(const FILETIME* pFT /*! Pointer to the FILETIME structure to initialize a new instance of the DateTime class.*/)
	{
		set(pFT);
	}
	//! Assigns a reference to a FILETIME structure to a target DateTime object.
	/*!
	    \return
	    A DateTime object that has been assigned the value specified as a parameter.
	*/
	const DateTime& operator= (const FILETIME& pFT /*! Reference to the FILETIME structure to assign to the target object.*/)
	{
		set(&pFT);
		return *this;
	}
	//! Assigns a reference to a DateTime object to a target DateTime object.
	/*!
	    \return
	    A DateTime object that has been assigned the value specified as a parameter.
	*/
	const DateTime& operator= (const DateTime& pFT /*! Reference to the DateTime object to assign to the target object.*/)
	{
		m_filetime.dwLowDateTime = pFT.m_filetime.dwLowDateTime;
		m_filetime.dwHighDateTime = pFT.m_filetime.dwHighDateTime;
		return *this;
	}


	//! Sets the value of the existing object to the one indicated by a pointer to a FILETIME structure.
	void set(const FILETIME* pFT /*! Pointer to the FILETIME structure to set to the existing object.  */);

	//! Gets the value of the existing object into a FILETIME structure.
	void get(FILETIME* pFT /*! Pointer to the FILETIME structure where to set the date and time of the existing object.*/);
	//! Gets the FILETIME structure that contains the value of the existing DateTime object.
	/*!
	    \return
	    Reference to the FILETIME structure that contains the value of the existing DateTime object.
	*/
	const FILETIME& get(void)
	{
		return m_filetime;
	}
	void get(DATE& pDate);
#endif
#ifdef TBC_OS_LINUX
	DateTime(const struct timeval* pFT)
	{
		set(pFT);
	}
	const DateTime& operator=(const struct timeval& pFT)
	{
		set(&pFT);
		return *this;
	}
	const DateTime& operator= (const DateTime& pFT)
	{
		m_filetime.tv_sec = pFT.m_filetime.tv_sec;
		m_filetime.tv_usec = pFT.m_filetime.tv_usec;
		return *this;
	}
	//! Sets the value of the existing object to the one indicated by a pointer to a FILETIME structure.
	void set(const struct timeval* pFT);
	void get(struct timeval* pFT);
	const struct timeval& get(void)
	{
		return m_filetime;
	}
#endif
	//! Sets the initial value of an existing DateTime object. The initial date time value is January 1, 1601 12:00 A.M.
	void init(void);
	//! Clears the value of an existing DateTime object. The new object's value is January 1, 1601 12:00 A.M.
	void clear(void)
	{
		init();
	}
	//! Sets the value of the existing DateTime object to the current date and time.
	void now(void);
	//! Determines whether the value (date and time)of the existing DateTime object is set.
	/*!
	    \return TRUE - value set; FALSE - value not set
	*/
	int isSet(void);
	//! Returns the string representation of the existing DateTime object(Local).
	/*!
	    \return The string representation of the existing DateTime object(Local).
	*/
	tstring toString(void);
	//! Returns the string representation of the existing DateTime object(GMT).
	/*!
	    \return The string representation of the existing DateTime object(GMT).
	*/
	tstring toStringGMT(void);
	//! Gets the milliseconds component of the date represented by this instance(Local).
	/*!
	    \return
	    The millisecond component of the date(Local).
	*/
	unsigned long milliSecond(void);
	//! Gets the milliseconds component of the date represented by this instance(GMT).
	/*!
	    \return
	    The millisecond component of the date(GMT).
	*/
	unsigned long milliSecondGMT(void);
	//! Gets the seconds component of the date represented by this instance(Local).
	/*!
	    \return
	    The second component of the date(Local).
	*/
	unsigned short second(void);
	//! Gets the seconds component of the date represented by this instance(GMT).
	/*!
	    \return
	    The second component of the date(GMT).
	*/
	unsigned short secondGMT(void);
	//! Gets the minute component of the date represented by this instance(Local).
	/*!
	    \return
	    The minute component of the date(Local).
	*/
	unsigned short minute(void);
	//! Gets the minute component of the date represented by this instance(GMT).
	/*!
	    \return
	    The minute component of the date(GMT).
	*/
	unsigned short minuteGMT(void);
	//! Gets the hour component of the date represented by this instance(Local).
	/*!
	    \return
	    The hour component of the date(Local).
	*/
	unsigned short hour(void);
	//! Gets the hour component of the date represented by this instance(GMT).
	/*!
	    \return
	    The hour component of the date(GMT).
	*/
	unsigned short hourGMT(void);
	//! Gets the day of the month represented by this instance(Local).
	/*!
	    \return
	    The day component of the date(Local).
	*/
	unsigned short day(void);
	//! Gets the day of the month represented by this instance(GMT).
	/*!
	    \return
	    The day component of the date(GMT).
	*/
	unsigned short dayGMT(void);
	//! Gets the month component of the date represented by this instance(Local).
	/*!
	    \return
	    The day component of the date(Local).
	*/
	unsigned short month(void);
	//! Gets the month component of the date represented by this instance(GMT).
	/*!
	    \return
	    The day component of the date(GMT).
	*/
	unsigned short monthGMT(void);
	//! Gets the year component of the date represented by this instance(Local).
	/*!
	    \return
	    The month component of the date(Local).
	*/
	unsigned long year(void);
	//! Gets the year component of the date represented by this instance(GMT).
	/*!
	    \return
	    The month component of the date(GMT).
	*/
	unsigned long yearGMT(void);
	//! Gets the microseconds since 01.01.1601
	/*!
	    \return
	    The microsecond counter.
	*/
	ULONGLONG getMicroseconds(void) const;
	//! Sets the microseconds since 01.01.1601
	void setMicroseconds(ULONGLONG ms);
	//! Adds microseconds to the time
	void addMicroseconds(ULONGLONG ms);

protected:

#ifdef TBC_OS_WINDOWS
	//! The local operating system file time.
	FILETIME m_filetime;
#endif

#ifdef TBC_OS_LINUX
	struct timeval m_filetime;
#endif

};  //  end DateTime

// compatibility with old windows.h,
// where there was no V_I8 and V_UI8.
///

#ifndef V_I8
#define V_I8(X)          ((X)->cyVal.int64)
#endif
#ifndef V_UI8
#define V_UI8(X)         (*(ULONGLONG*)&V_I8(X))
#endif

#ifndef LPCVARIANT
typedef const VARIANT* LPCVARIANT;
#endif

//-----------------------------------------------------------------------------
// Variant                                                                    |
//-----------------------------------------------------------------------------
//! Represents the Variant data type.
class TBC_EXPORT Variant : public tagVARIANT
{

public:
	//! Initializes a new instance of the Variant class.
	Variant();

	//! The copy constructor of the Variant class.
	Variant(IN const Variant& cmnVar /*! Reference to the Variant object to copy information from.*/);

	//! Another copy constructor of the Variant class.
	Variant(IN const VARIANT& var /*! Reference to the VARIANT structure to copy information from.*/);

	//! Initializes a new instance of the Variant class to the value indicated by a pointer to a VARIANT structure.
	Variant(IN LPCVARIANT pvar /*! Pointer to the VARIANT structure to initialize a new instance of the Variant class. */);

	//! Initializes a new instance of the Variant class to the value indicated by a string.
	Variant(IN const TCHAR* sz /*! Array of TCHAR-s to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by a signed character.
	Variant(IN char c /*! Character to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by an unsigned character.
	Variant(IN unsigned char b /*! Unsigned character to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by a short.
	Variant(IN short s /*! Short to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by an unsigned short.
	Variant(IN unsigned short us /*! Unsigned short to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by a long.
	Variant(IN long l /*!Long to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by an unsigned long.
	Variant(IN unsigned long ul /*! Unsigned long to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by an integer.
	Variant(IN int i /*! Integer to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by an unsigned integer.
	Variant(IN unsigned int ui /*! Unsigned integer to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by a float.
	Variant(IN float flt /*! Float to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by a double.
	Variant(IN double dbl /*! Double to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by a boolean.
	Variant(IN bool bo /*! Boolean to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by a CURRENCY.
	Variant(IN CURRENCY cy /*! CURRENCY to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by a LONGLONG.
	Variant(IN LONGLONG i8 /*! LONGLONG to initialize a new instance of the Variant class.*/);

	//! Initializes a new instance of the Variant class to the value indicated by a ULONGLONG.
	Variant(IN ULONGLONG ui8 /*! ULONGLONG to initialize a new instance of the Variant class.*/);

	//! Class destructor.
	virtual ~Variant();

	//! Assigns a Variant to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN Variant& cmnVar /*! Reference to the Variant object to assign to the target object.*/);

	//! Assigns a VARIANT to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN const VARIANT& var /*! Reference to the VARIANT structure to assign to the target object.*/);

	//! Assigns a pointer to a VARIANT to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN LPCVARIANT pvar /*!Pointer to the VARIANT structure to assign to the target object.*/);

	//! Assigns a string to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN const TCHAR* sz /*! Array of TCHAR-s to assign to the target object.*/)
	{
		setBSTR(sz);
		return *this;
	}

	//! Assigns a signed char to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN char c/*! Signed character to assign to the target object.*/)
	{
		setI1(c);
		return *this;
	}

	//! Assigns an unsigned char to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN unsigned char b /*! Unsigned character to assign to the target object.*/)
	{
		setUI1(b);
		return *this;
	}

	//! Assigns a short to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN short s/*! Signed short to assign to the target object.*/)
	{
		setI2(s);
		return *this;
	}

	//! Assigns an unsigned short to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN unsigned short us /*! Unsigned short to assign to the target object.*/)
	{
		setUI2(us);
		return *this;
	}

	//! Assigns a long to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN long l /*! Signed long to assign to the target object.*/)
	{
		setI4(l);
		return *this;
	}

	//! Assigns an unsigned long to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN unsigned long ul /*! Unsigned long to assign to the target object.*/)
	{
		setUI4(ul);
		return *this;
	}

	//! Assigns an int to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN int i/*! Signed integer to assign to the target object.*/)
	{
		setINT(i);
		return *this;
	}

	//! Assigns an unsigned int to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN unsigned int ui /*! Unsigned integer to assign to the target object.*/)
	{
		setUINT(ui);
		return *this;
	}

	//! Assigns a float to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN float flt /*! Float to assign to the target object.*/)
	{
		setR4(flt);
		return *this;
	}

	//! Assigns a double to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN double dbl /*! Double to assign to the target object.*/)
	{
		setR8(dbl);
		return *this;
	}

	//! Assigns a boolean to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN bool bo /*! Boolean to assign to the target object.*/)
	{
		setBOOL(bo);
		return *this;
	}

	//! Assigns a CURRENCY to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN CURRENCY cy /*! CURRENCY to assign to the target object.*/)
	{
		setCY(cy);
		return *this;
	}

	//! Assigns a LONGLONG to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN LONGLONG i8 /*! LONGLONG to assign to the target object.*/)
	{
		setI8(i8);
		return *this;
	}

	//! Assigns a ULONGLONG to a target Variant.
	/*!
	    \return
	    A Variant object that has been assigned the value specified as a parameter.
	*/
	const Variant& operator=(IN ULONGLONG ui8 /*! ULONGLONG to assign to the target object.*/)
	{
		setUI8(ui8);
		return *this;
	}

	//! Sets the value of the current instance to the one of a string.
	void setBSTR(IN const TCHAR* sz /*! Array of TCHAR-s to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of a signed character.
	void setI1(IN char c /*! Signed character to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of an unsigned character.
	void setUI1(IN unsigned char b /*! Unsigned character to set to the value of the current instance.*/);

	//! Sets the value contained byf the current instance to the one of a short.
	void setI2(IN short s /*! Short to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of an unsigned short.
	void setUI2(IN unsigned short us /*! Unsigned short to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of a long.
	void setI4(IN long l /*! Long to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of an unsigned long.
	void setUI4(IN unsigned long ul /*! Unsigned long to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of an int.
	void setINT(IN int i /*! Signed integer to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of an unsigned int.
	void setUINT(IN unsigned int ui /*! Unsigned integer to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of a float.
	void setR4(IN float flt /*! Float to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of a double.
	void setR8(IN double dbl /*! Double to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of a boolean.
	void setBOOL(IN bool bo /*! Boolean to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of a CURRENCY.
	void setCY(IN CURRENCY cy /*! CURRENCY to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of a LONGLONG.
	void setI8(IN LONGLONG i8 /*! LONGLONG to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of a ULONGLONG.
	void setUI8(IN ULONGLONG ui8 /*! ULONGLONG to set to the value of the current instance.*/);

	//! Sets the value contained by the current instance to the one of a DATE.
	void setDATE(IN DATE date /*! DATE structure to set to the value of the current instance.*/);

	//! Converts from a Unicode string to a generic string.
	/*!
	    \return
	    Generic character string (equivalent to W when _UNICODE is defined, equivalent to A otherwise).
	*/
	TCHAR* getBSTR(TCHAR*& buffer /*! The Unicode character string to be converted.*/);


#ifdef TBC_OS_LINUX
	// under windows DATE is a double, which creates an overload conflict!
	Variant(IN DATE dt);
	const Variant& operator=(IN DATE dt)
	{
		setDATE(dt);
		return *this;
	}
#endif
	//! Sets the value the Variant contains to the one of a given type, taken from a given source that have a given size.
	/*!
	    \return
	    Reference to a Variant object that contains the newly set value.
	*/
	Variant& set(
		IN void* src /*! Source value.*/ ,
		IN VARTYPE type /*! The type of the source value.*/,
		IN unsigned short elSize /*! Source size.*/);

	//! Gets Variant's size in bytes.
	/*!
	    \return
	    The Variant's size in bytes. If an array, this is the length of the array
	    multiplied by the datatype size ( getSize(VARTYPE varType) )
	*/
	unsigned long getSize(void);

	//! Gets the size of a value that has a given data type.
	/*!
	    \return
	    The value's size same as a sizeof().
	*/
	static unsigned long getSize(VARTYPE varType /*! The value type.*/);

	//!Copies the value the Variant contains to a given memory block.
	void get(IN void* dest /*! The destination to which the value is to be copied.*/);

	//! Call this casting operator to access the underlying VARIANT structure for this Variant object.
	operator LPVARIANT();

	//! Call this casting operator to access the underlying VARIANT structure for this Variant object.
	operator LPCVARIANT() const;

	//! Tests for equality between the current Variant instance and a specified VARIANT value.
	/*!
	    \return
	    TRUE - the variants are the same; FALSE - the variants are different.
	*/
	int operator==(IN const VARIANT& var /* The VARIANT that is to be compared to the target Variant for equality. */) const;

	//! Tests for equality between the current Variant instance and a value indicated by a pointer to VARIANT.
	/*!
	    \return
	    TRUE - the variants are the same; FALSE - the variants are different.
	*/
	int operator==(IN LPCVARIANT pvar /* The pointer to the VARIANT that is to be compared to the target Variant for equality. */) const;

	//! Tests for equality between the current Variant instance and a specified Variant value.
	/*!
	    \return
	    TRUE - the variants are the same; FALSE - the variants are different.
	*/
	int operator==(IN const Variant& cmnVar /* The Variant that is to be compared to the target Variant for equality. */) const;

	//! Tests for inequality between the current Variant instance and a specified VARIANT value.
	/*!
	    \return
	    TRUE - the variants are different; FALSE - the variants are the same.
	*/
	int operator!=(IN const VARIANT& var /* The VARIANT that is to be compared to the target Variant for inequality. */) const;

	//! Tests for inequality between the current Variant instance and a specified VARIANT value.
	/*!
	    \return
	    TRUE - the variants are different; FALSE - the variants are the same.
	*/
	int operator!=(IN LPCVARIANT pvar /* The pointer to the VARIANT that is to be compared to the target Variant for equality. */) const;

	//! Tests for inequality between the current Variant instance and a specified Variant value.
	/*!
	    \return
	    TRUE - the variants are different; FALSE - the variants are the same.
	*/
	int operator!=(IN const Variant& cmnVar /* The Variant that is to be compared to the target Variant for equality. */) const;

	//! Clears the Variant.
	void clear(void);

	//! Copies the current Variant to a VARIANT.
	/*!
	    \return
	    TRUE - copy succeeded; FALSE - copy failed.
	*/
	int copyTo(OUT LPVARIANT destination /* Pointer to the VARIANT that contains the copied information. */) const;

	//! Initializes the value of the current Variant instance with a given one.
	/*!
	    \return
	    TRUE - initialization succeeded; FALSE - initialization failed.
	*/
	int initFrom(IN LPCVARIANT source  /* Pointer to the VARIANT that contains the information to initialize. */);

	//! Initializes a Variant to a value with a given datatype.
	void init(IN VARTYPE datatype  /* Pointer to the VARIANT that contains the information to initialize. */);

	//! Initializes the value a Variant contains to an array value of a given size. The values in the array have a given datatype.
	void initArray(
		VARTYPE datatype, /*!< The base type of the array (the VARTYPE of each element of the array).*/
		unsigned long arraySize, /*!< The number of elements in the array.*/
		unsigned long lBound = 0 /*!< The lower bound for the array. */);

	//! Returns the string representation of the existing Variant object.
	/*!
	    \return The string representation of the existing Variant object.
	*/
	tstring toString();

	//! Returns whether a given datatype is supported.
	/*!
	    \return
	    TRUE  - variant copied and datatype changed;
	    FALSE - can't change datatype
	*/
	static int supportDatatype(IN VARTYPE dataType);

	//! Change the datatype of the value the Variant contains.
	/*!
	    \return
	    TRUE  - datatype changed;
	    FALSE - can't change datatype
	*/
	int changeDatatype(IN VARTYPE type);

	//! Change the datatype of a value a VARIANT contains with the one given as parameter.
	/*!
	    \return
	    TRUE  - variant copied and datatype changed;
	    FALSE - can't change datatype
	*/
	static int changeDatatype(
		IN VARIANT* value,/*!< The source  value. */
		IN VARTYPE type,/*!< The wanted datatype.  */
		OUT VARIANT* changedValue, /*!< The destination value. */
		long* pError = NULL /*!< The conversion error. */
	);
	//! Change a value's datatype.
	/*!
	    \return
	    TRUE  - variant copied and datatype changed;
	    FALSE - can't change datatype
	*/
	static int changeDatatype(
		IN VARIANT* value,/*!< The source value. */
		IN VARTYPE type,/*!< The wanted datatype.  */
		IN unsigned long localeID,/*!< A locale id. */
		VARIANT* changedValue,/*!< The destination value. */
		long* pError = NULL /*!< The conversion error. */
	);
	//! Tests for equality between two VARIANT-s.
	/*!
	    \return
	    TRUE  - the variants are equal;
	    FALSE - the variants are not equal
	*/
	static int areEqual(
		LPCVARIANT pvar1,/*!<First LPCVARIANT to compare */
		LPCVARIANT pvar2 /*!<Second LPCVARIANT to compare */
	);
	//! Tests for equality between two VARIANT-s that contain array values.
	/*!
	    \return
	    TRUE  - the variants are equal;
	    FALSE - the variants are not equal
	*/
	static int areEqualArrays(
		LPCVARIANT pvar1, /*!<First LPCVARIANT to compare */
		LPCVARIANT pvar2  /*!<Second LPCVARIANT to compare */
	);
	//! Copies a source VARIANT into a destination VARIANT.
	/*!
	    \return
	    TRUE  - the copy was made;
	    FALSE - can't copy
	*/
	static long variantCopy(
		LPVARIANT pvargDest,  /*!<Pointer to the variant to copy to. */
		LPVARIANT pvargSrc); /*!<Pointer to the variant to copy from. */
	//! Clears a given variant.
	/*!
	    \return
	    TRUE  - the clear was made;
	    FALSE - can't clear
	*/
	static long variantClear(
		LPVARIANT pvarg/*!<Pointer to the variant to clear. */
	);
	//! Creates a new array descriptor, allocates and initializes the data for the array, and returns a pointer to the new array descriptor.
	/*!
	    \return
	    Points to the array descriptor, or Null if the array could not be created.
	*/
	static LPSAFEARRAY safeArrayCreate(
		VARTYPE vt, /*!< The base type of the array (the VARTYPE of each element of the array).*/
		unsigned short cDims, /*!<Number of dimensions in the array. The number cannot be changed after the array is created. */
		LPSAFEARRAYBOUND rgsabound/* Pointer to a vector of bounds (one for each dimension) to allocate for the array.*/
	);
	//! Creates a one-dimensional array whose lower bound is always zero.
	/*!
	    \return
	    Points to the array descriptor, or Null if the array could not be created.
	*/
	static LPSAFEARRAY safeArrayCreateVector(
		VARTYPE vt, /*!< The base type of the array (the VARTYPE of each element of the array).*/
		long lLbound, /*!< The lower bound for the array. */
		unsigned long cElements /*!< The number of elements in the array.*/
	);

	static long SafeArrayAccessData(
		LPSAFEARRAY psa,
		void** ppvData
	);

	static long SafeArrayUnaccessData(
		LPSAFEARRAY psa
	);

	static long SafeArrayGetUBound(
		LPSAFEARRAY psa,
		unsigned short nDim,
		long* plUbound
	);

	static long SafeArrayGetLBound(
		LPSAFEARRAY psa,
		unsigned short nDim,
		long* plLbound
	);

}; // Variant

inline Variant::operator LPVARIANT()
{
	return this;
}

inline Variant::operator LPCVARIANT() const
{
	return this;
}

inline int Variant::changeDatatype(IN VARTYPE type)
{
	return changeDatatype(this, type, this);
}

inline int Variant::operator==(IN const VARIANT& var) const
{
	return areEqual(this, &var);
}

inline int Variant::operator==(IN LPCVARIANT pvar) const
{
	return areEqual(this, pvar);
}

inline int Variant::operator==(IN const Variant& cmnVar) const
{
	return areEqual(this, &cmnVar);
}


inline int Variant::operator!=(IN const VARIANT& var) const
{
	return !areEqual(this, &var);
}

inline int Variant::operator!=(IN LPCVARIANT pvar) const
{
	return !areEqual(this, pvar);
}

inline int Variant::operator!=(IN const Variant& cmnVar) const
{
	return !areEqual(this, &cmnVar);
}



//-----------------------------------------------------------------------------
// ValueData                                                                  |
//-----------------------------------------------------------------------------
class TBC_EXPORT ValueData
{

protected:
	Variant m_data;

public:
	ValueData(const VARIANT* aData):
		m_data(aData)
	{
	}   //  end ctr

	ValueData(const VARIANT& aData):
		m_data(aData)
	{
	}   //  end ctr

	ValueData(void)
	{
		m_data.init(VT_EMPTY);
	}   //  end ctr

	virtual ~ValueData(void)
	{
	}   //  end destructor

	const Variant& getData(void) const
	{
		return m_data;
	}
	
	tstring toString(void);

};  //  end class ValueData



//-----------------------------------------------------------------------------
// ValueQT                                                                    |
//-----------------------------------------------------------------------------
class TBC_EXPORT ValueQT : public ValueData
{

protected:
	EnumQuality m_quality;
	DateTime m_timeStamp;

public:

	ValueQT():
		m_quality(EnumQuality_BAD_WAITING_FOR_INITIAL_DATA)
	{
		m_timeStamp.init();
	}   //  end ctr

	ValueQT(VARIANT& aData,
			EnumQuality aQuality,
			DateTime aTimeStamp) :
		ValueData(aData)
	{
		m_quality = aQuality;
		m_timeStamp = aTimeStamp;
	}   //  end ctr

	ValueQT(ValueQT& aValue):
		ValueData(aValue.getData()),
		m_quality(aValue.getQuality()),
		m_timeStamp(aValue.getTimeStamp())
	{
	}   //  end ctr

	virtual ~ValueQT() {}

	EnumQuality getQuality(void) const
	{
		return m_quality;
	}
	
	DateTime getTimeStamp(void) const
	{
		return m_timeStamp;
	}

	void setData(VARIANT& aData, EnumQuality aQuality, DateTime aTimeStamp)
	{
		m_data = aData;
		m_quality = aQuality;
		m_timeStamp = aTimeStamp;
	}   //  end set

	int isQualitySet()
	{
		return (m_quality != EnumQuality_BAD_WAITING_FOR_INITIAL_DATA);
	}
	int isTimeStampSet()
	{
		return m_timeStamp.isSet();
	}

	tstring toString();

};  //  end class ValueQT


#pragma pack(pop)
#endif

