## **DateTime Class**

Represents an instant in time, typically expressed as a date and time of day.

###  Public Member Functions
#include <ValueQT.h>
DateTime ( )|Initializes a new instance of the DateTime class.
:-----:|:-----:
DateTime ( const DateTime &dateTime )|The copy constructor of the DateTime class. 
DateTime ( const FILETIME *pFT )|Initializes a new instance of the DateTime class to the value indicated by a pointer to a FILETIME structure. 
const DateTime &  operator= ( const FILETIME &pFT )|Assigns a reference to a FILETIME structure to a target DateTime object. 
const DateTime &  operator= ( const DateTime &pFT )|Assigns a reference to a DateTime object to a target DateTime object. 
void set ( const FILETIME *pFT )|Sets the value of the existing object to the one indicated by a pointer to a FILETIME structure. 
void get ( FILETIME *pFT )|Gets the value of the existing object into a FILETIME structure. 
const FILETIME & get ( void )|Gets the FILETIME structure that contains the value of the existing DateTime object. 
void init ( void )|Sets the initial value of an existing DateTime object. The initial date time value is January 1, 1601 12:00 A.M.
void clear ( void )|Clears the value of an existing DateTime object. The new object's value is January 1, 1601 12:00 A.M.
void now ( void )|Sets the value of the existing DateTime object to the current date and time.
int isSet ( void )|Determines whether the value (date and time)of the existing DateTime object is set. 
tstring toString ( void )|Returns the string representation of the existing DateTime object. 
unsigned long milliSecond ( void )|Gets the milliseconds component of the date represented by this instance. 
unsigned short second ( void )|Gets the seconds component of the date represented by this instance. 
unsigned short minute ( void )|Gets the minute component of the date represented by this instance. 
unsigned short hour ( void )|Gets the hour component of the date represented by this instance. 
unsigned short day ( void )|Gets the day of the month represented by this instance. 
unsigned short month ( void )|Gets the month component of the date represented by this instance. 
unsigned long year ( void )|Gets the year component of the date represented by this instance. 

### Constructor & Destructor Documentation
**DateTime::DateTime( const DateTime& dateTime )**|The copy constructor of the DateTime class. Parameters: dateTime  Reference to the DateTime object to copy information from. 
:-----:|:-----:
DateTime::DateTime( const FILETIME *  pFT ) [inline]|Initializes a new instance of the DateTime class to the value indicated by a pointer to a FILETIME structure. Parameters: pFT  Pointer to the FILETIME structure to initialize a new instance of the DateTime class.


### Member Function Documentation
**unsigned short DateTime::day( void ) **|**Gets the day of the month represented by this instance. Returns: The day component of the date. **
:-----:|:-----:
const FILETIME& DateTime::get( void ) [inline]|Gets the FILETIME structure that contains the value of the existing DateTime object. Returns:Reference to the FILETIME structure that contains the value of the existing DateTime object.
void DateTime::get( FILETIME * pFT )|Gets the value of the existing object into a FILETIME structure. Parameters: pFT  Pointer to the FILETIME structure where to set the date and time of the existing object. 
unsigned short DateTime::hour( void )|Gets the hour component of the date represented by this instance. Returns: The hour component of the date. 
int DateTime::isSet( void )|Determines whether the value (date and time)of the existing DateTime object is set. Returns: TRUE - value set; FALSE - value not set 
unsigned long DateTime::milliSecond( void )|Gets the milliseconds component of the date represented by this instance. Returns: The millisecond component of the date. 
unsigned short DateTime::minute( void )|Gets the minute component of the date represented by this instance. Returns: The minute component of the date. 
unsigned short DateTime::day( void )|Gets the day component of the date represented by this instance. Returns: The day component of the date. 
unsigned short DateTime::month( void )|Gets the month component of the date represented by this instance. Returns: The month component of the date.
const DateTime& DateTime::operator=( const DateTime & pFT ) [inline]|Assigns a reference to a DateTime object to a target DateTime object. Returns: A DateTime object that has been assigned the value specified as a parameter.  Parameters: pFT  Reference to the DateTime object to assign to the target object. 
const DateTime& DateTime::operator=( const FILETIME & pFT ) [inline]|Assigns a reference to a FILETIME structure to a target DateTime object. Returns: A DateTime object that has been assigned the value specified as a parameter.  Parameters: pFT  Reference to the FILETIME structure to assign to the target object. 
unsigned short DateTime::second( void )|Gets the seconds component of the date represented by this instance.  Returns: The second component of the date.
void DateTime::set( const FILETIME * pFT )|Sets the value of the existing object to the one indicated by a pointer to a FILETIME structure. Parameters: pFT  Pointer to the FILETIME structure to set to the existing object. 
tstring DateTime::toString( void )|Returns the string representation of the existing DateTime object.  Returns: The string representation of the existing DateTime object.
unsigned long DateTime::year( void )|Gets the year component of the date represented by this instance.  Returns: The year component of the date. 