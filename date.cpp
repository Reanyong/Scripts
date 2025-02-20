#include "pch.h"
#include "date.h"
#include "c_engine.h"

#define MIN_DATE                (-657434L)  // about year 100
#define MAX_DATE                2958465L    // about year 9999
#define HALF_SECOND  (1.0/172800.0)

static int __MonthDays[13] =
	{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

BOOL __OleDateFromTm(WORD wYear, WORD wMonth, WORD wDay,
	WORD wHour, WORD wMinute, WORD wSecond, DATE& dtDest)
{
	// Validate year and month (ignore day of week and milliseconds)
	if (wYear > 9999 || wMonth < 1 || wMonth > 12)
		return FALSE;

	//  Check for leap year and set the number of days in the month
	BOOL bLeapYear = ((wYear & 3) == 0) &&
		((wYear % 100) != 0 || (wYear % 400) == 0);

	int nDaysInMonth =
		__MonthDays[wMonth] - __MonthDays[wMonth-1] +
		((bLeapYear && wDay == 29 && wMonth == 2) ? 1 : 0);

	// Finish validating the date
	if (wDay < 1 || wDay > nDaysInMonth ||
		wHour > 23 || wMinute > 59 ||
		wSecond > 59)
	{
		return FALSE;
	}

	// Cache the date in days and time in fractional days
	long nDate;
	double dblTime;

	//It is a valid date; make Jan 1, 1AD be 1
	nDate = wYear*365L + wYear/4 - wYear/100 + wYear/400 +
		__MonthDays[wMonth-1] + wDay;

	//  If leap year and it's before March, subtract 1:
	if (wMonth <= 2 && bLeapYear)
		--nDate;

	//  Offset so that 12/30/1899 is 0
	nDate -= 693959L;

	dblTime = (((long)wHour * 3600L) +  // hrs in seconds
		((long)wMinute * 60L) +  // mins in seconds
		((long)wSecond)) / 86400.;

	dtDest = (double) nDate + ((nDate >= 0) ? dblTime : -dblTime);

	return TRUE;
}

BOOL __TmFromOleDate(DATE dtSrc, struct tm& tmDest)
{
	// The legal range does not actually span year 0 to 9999.
	if (dtSrc > MAX_DATE || dtSrc < MIN_DATE) // about year 100 to about 9999
		return FALSE;

	long nDays;             // Number of days since Dec. 30, 1899
	long nDaysAbsolute;     // Number of days since 1/1/0
	long nSecsInDay;        // Time in seconds since midnight
	long nMinutesInDay;     // Minutes in day

	long n400Years;         // Number of 400 year increments since 1/1/0
	long n400Century;       // Century within 400 year block (0,1,2 or 3)
	long n4Years;           // Number of 4 year increments since 1/1/0
	long n4Day;             // Day within 4 year block
							//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
	long n4Yr;              // Year within 4 year block (0,1,2 or 3)
	BOOL bLeap4 = TRUE;     // TRUE if 4 year block includes leap year

	double dblDate = dtSrc; // tempory serial date

	// If a valid date, then this conversion should not overflow
	nDays = (long)dblDate;

	// Round to the second
	dblDate += ((dtSrc > 0.0) ? HALF_SECOND : -HALF_SECOND);

	nDaysAbsolute = (long)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899

	dblDate = fabs(dblDate);
	nSecsInDay = (long)((dblDate - floor(dblDate)) * 86400.);

	// Calculate the day of week (sun=1, mon=2...)
	//   -1 because 1/1/0 is Sat.  +1 because we want 1-based
	tmDest.tm_wday = (int)((nDaysAbsolute - 1) % 7L) + 1;

	// Leap years every 4 yrs except centuries not multiples of 400.
	n400Years = (long)(nDaysAbsolute / 146097L);

	// Set nDaysAbsolute to day within 400-year block
	nDaysAbsolute %= 146097L;

	// -1 because first century has extra day
	n400Century = (long)((nDaysAbsolute - 1) / 36524L);

	// Non-leap century
	if (n400Century != 0)
	{
		// Set nDaysAbsolute to day within century
		nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

		// +1 because 1st 4 year increment has 1460 days
		n4Years = (long)((nDaysAbsolute + 1) / 1461L);

		if (n4Years != 0)
			n4Day = (long)((nDaysAbsolute + 1) % 1461L);
		else
		{
			bLeap4 = FALSE;
			n4Day = (long)nDaysAbsolute;
		}
	}
	else
	{
		// Leap century - not special case!
		n4Years = (long)(nDaysAbsolute / 1461L);
		n4Day = (long)(nDaysAbsolute % 1461L);
	}

	if (bLeap4)
	{
		// -1 because first year has 366 days
		n4Yr = (n4Day - 1) / 365;

		if (n4Yr != 0)
			n4Day = (n4Day - 1) % 365;
	}
	else
	{
		n4Yr = n4Day / 365;
		n4Day %= 365;
	}

	// n4Day is now 0-based day of year. Save 1-based day of year, year number
	tmDest.tm_yday = (int)n4Day + 1;
	tmDest.tm_year = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;

	// Handle leap year: before, on, and after Feb. 29.
	if (n4Yr == 0 && bLeap4)
	{
		// Leap Year
		if (n4Day == 59)
		{
			/* Feb. 29 */
			tmDest.tm_mon = 2;
			tmDest.tm_mday = 29;
			goto DoTime;
		}

		// Pretend it's not a leap year for month/day comp.
		if (n4Day >= 60)
			--n4Day;
	}

	// Make n4DaY a 1-based day of non-leap year and compute
	//  month/day for everything but Feb. 29.
	++n4Day;

	// Month number always >= n/32, so save some loop time */
	for (tmDest.tm_mon = (n4Day >> 5) + 1;
		n4Day > __MonthDays[tmDest.tm_mon]; tmDest.tm_mon++);

	tmDest.tm_mday = (int)(n4Day - __MonthDays[tmDest.tm_mon-1]);

DoTime:
	if (nSecsInDay == 0)
		tmDest.tm_hour = tmDest.tm_min = tmDest.tm_sec = 0;
	else
	{
		tmDest.tm_sec = (int)nSecsInDay % 60L;
		nMinutesInDay = nSecsInDay / 60L;
		tmDest.tm_min = (int)nMinutesInDay % 60;
		tmDest.tm_hour = (int)nMinutesInDay / 60;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// StrToDate ------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall StrToDate(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);
	if (nargs != 1)
	{
		result = INT_MIN;
		return;
	}
	result.reset();
	result.vt = VT_DATE;

	if (pargs[0]->vt != VT_BSTR)
	{
		result= INT_MIN;
		return;
	}

	HRESULT hr = VarDateFromStr(pargs[0]->bstrVal, LANG_USER_DEFAULT, 0, &result.date);
	p_engine->runtime_error(hr);
}

bool _check_StrToDate(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'StrToDate' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// FormatDateTime -------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall FormatDateTime(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1 || nargs == 2);

	int n_format = 0;
	if (nargs == 2) n_format = pargs[1]->as_integer();

	result.reset();
	result.vt = VT_BSTR;

	HRESULT hr = VarFormatDateTime(pargs[0],
									n_format,
									0,
									&result.bstrVal);

	p_engine->runtime_error(hr);
}

bool _check_FormatDateTime(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1 && n != 2)
	{
		*p_msg = "'FormatDateTime' function gets 1 or 2 arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// DateSerial -----------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall DateSerial(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 3);

	int n_year = pargs[0]->as_integer();
	int n_month = pargs[1]->as_integer();
	int n_day = pargs[2]->as_integer();

	result.vt = VT_DATE;
	__OleDateFromTm(n_year, n_month, n_day, 0, 0, 0, result.date);
}

bool _check_DateSerial(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'DateSerial' function gets 3 arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// TimeSerial -----------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall TimeSerial(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 3);

	int n_hour = pargs[0]->as_integer();
	int n_minute = pargs[1]->as_integer();
	int n_second = pargs[2]->as_integer();

	result.vt = VT_DATE;
	__OleDateFromTm(1899, 12, 30, n_hour, n_minute, n_second, result.date);
}

bool _check_TimeSerial(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 3)
	{
		*p_msg = "'TimeSerial' function gets 3 arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Now ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall Now(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 0);
	if (nargs != 0)
	{
		result = INT_MIN;
		return;
	}

	//
	//long time = ::time(0);
	//20210324
	time_t time = ::time(0);
	struct tm* t = localtime(&time);

	c_variable date;
	date.vt = VT_DATE;

	__OleDateFromTm(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, date.dblVal);

	result = date;
}

bool _check_Now(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 0)
	{
		*p_msg = "'Now' function gets no arguments.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Year -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall Year(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);	
	if (nargs != 1 || pargs[0]->vt != VT_DATE)
	{
		result = INT_MIN;
		return;
	}

	struct tm t;
	__TmFromOleDate(pargs[0]->date, t);

	result = t.tm_year;
}

bool _check_Year(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'Year' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Month ----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall Month(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);

	if (nargs != 1 || pargs[0]->vt != VT_DATE)
	{
		result = INT_MIN;
		return;
	}

	struct tm t;
	__TmFromOleDate(pargs[0]->date, t);

	result = t.tm_mon;
}

bool _check_Month(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'Month' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Day ------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall DayOfMonth(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);

	if (nargs != 1||pargs[0]->vt != VT_DATE)
	{
		result = INT_MIN;
		return;
	}

	struct tm t;
	__TmFromOleDate(pargs[0]->date, t);

	result = t.tm_mday;
}

bool _check_DayOfMonth(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'DayOfMonth' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Hour -----------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall Hour(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);

	if (nargs != 1 || pargs[0]->vt != VT_DATE)
	{
		result = INT_MIN;
		return;
	}

	struct tm t;
	__TmFromOleDate(pargs[0]->date, t);

	result = t.tm_hour;
}

bool _check_Hour(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'Hour' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Minute ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall Minute(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);

	if (nargs != 1 || pargs[0]->vt != VT_DATE)
	{
		result = INT_MIN;
		return;
	}

	struct tm t;
	__TmFromOleDate(pargs[0]->date, t);

	result = t.tm_min;
}

bool _check_Minute(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'Minute' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Second ---------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall Second(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	//_ASSERT(nargs == 1);

	if (nargs != 1 || pargs[0]->vt != VT_DATE)
	{
		result = INT_MIN;
		return;
	}

	struct tm t;
	__TmFromOleDate(pargs[0]->date, t);

	result = t.tm_sec;
}

bool _check_Second(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'Second' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// DayOfWeek ------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall DayOfWeek(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);

	if (pargs[0]->vt != VT_DATE)
	{
		result = -1;
		return;
	}

	struct tm t;
	__TmFromOleDate(pargs[0]->date, t);

	result = t.tm_wday;
}

bool _check_DayOfWeek(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'DayOfWeek' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// DayOfYear ------------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall DayOfYear(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result)
{
	_ASSERT(nargs == 1);

	if (pargs[0]->vt != VT_DATE)
	{
		result = -1;
		return;
	}

	struct tm t;
	__TmFromOleDate(pargs[0]->date, t);

	result = t.tm_yday;
}

bool _check_DayOfYear(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'DayOfYear' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// WeekOfYear -----------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall WeekOfYear(int nargs,
						  c_variable** pargs,
						  c_engine* p_engine,
						  c_variable& result)
{
	struct tm t;

	if (pargs[0]->vt != VT_DATE)
	{
		result = -1;
		return;
	}

	__TmFromOleDate(pargs[0]->date, t);

	result = t.tm_yday / 7 + 1;
}

bool _check_WeekOfYear(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'WeekOfYear' function gets 1 argument.";
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// WeekOfYear2 ----------------------------------------------------------------
//-----------------------------------------------------------------------------

void __stdcall WeekOfYear2(int nargs,
						  c_variable** pargs,
						  c_engine* p_engine,
						  c_variable& result)
{
	DATE d;
	DATE dmonday;
	struct tm t;
	struct tm tmonday;
	result.vt = VT_BOOL;

// get first monday of the year

	if (pargs[0]->vt != VT_DATE)
	{
		result.boolVal = -1;
		return;
	}

	d = pargs[0]->date;
	__TmFromOleDate(d, t);

	memcpy(&tmonday, &t, sizeof(struct tm));
	dmonday = d - t.tm_yday + 1;

	while (1)
	{
		__TmFromOleDate(dmonday, tmonday);
		if (tmonday.tm_wday == 2) break;
		dmonday ++;
	}

	if (t.tm_yday < tmonday.tm_yday)
		result = 0;
	else
		result = (t.tm_yday - tmonday.tm_yday) / 7 + 1;
}

bool _check_WeekOfYear2(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine)
{
	if (n != 1)
	{
		*p_msg = "'WeekOfYear2' function gets 1 argument.";
		return false;
	}

	return true;
}

