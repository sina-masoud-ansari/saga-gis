
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    api_string.cpp                     //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <math.h>

#include <wx/string.h>
#include <wx/datetime.h>

#include "api_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String::CSG_String(void)
{
	m_pString	= new wxString;
}

CSG_String::CSG_String(const CSG_String &String)
{
	m_pString	= new wxString(*String.m_pString);
}

CSG_String::CSG_String(const char *String)
{
	m_pString	= new wxString(String);
}

CSG_String::CSG_String(char Character)
{
	m_pString	= new wxString(Character);
}

//---------------------------------------------------------
CSG_String::~CSG_String(void)
{
	delete(m_pString);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char * CSG_String::c_str(void) const
{
	return( m_pString->c_str() );
}

//---------------------------------------------------------
size_t CSG_String::Length(void)	const
{
	return( m_pString->Length() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_String::Clear(void)
{
	m_pString->Clear();
}

//---------------------------------------------------------
int CSG_String::Printf(const char *Format, ...)
{
	va_list	argptr;

	va_start(argptr, Format);

	m_pString->PrintfV(Format, argptr);

	va_end(argptr);

	return( Length() );
}

//---------------------------------------------------------
CSG_String CSG_String::Format(const char *Format, ...)
{
	va_list		argptr;
	CSG_String	s;

	va_start(argptr, Format);

	s.m_pString->PrintfV(Format, argptr);

	va_end(argptr);

	return( s );
}

//---------------------------------------------------------
CSG_String & CSG_String::Append(const char *String)
{
	m_pString->Append(String);

	return( *this );
}

CSG_String & CSG_String::Append(char Character)
{
	m_pString->Append(Character);

	return( *this );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String & CSG_String::operator = (const CSG_String &String)
{
	*m_pString	= *String.m_pString;

	return( *this );
}

CSG_String & CSG_String::operator = (const char *String)
{
	*m_pString	= String;

	return( *this );
}

CSG_String & CSG_String::operator = (char Character)
{
	*m_pString	= Character;

	return( *this );
}

//---------------------------------------------------------
CSG_String CSG_String::operator + (const CSG_String &String) const
{
	CSG_String	s(*this);

	s	+= String;

	return( s );
}

CSG_String CSG_String::operator + (const char *String) const
{
	CSG_String	s(*this);

	s	+= String;

	return( s );
}

CSG_String CSG_String::operator + (char Character) const
{
	CSG_String	s(*this);

	s	+= Character;

	return( s );
}

CSG_String		operator + (const char *A, const CSG_String &B)
{
	CSG_String	s(A);

	s	+= B;

	return( s );
}

CSG_String		operator + (char A, const CSG_String &B)
{
	CSG_String	s(A);

	s	+= B;

	return( s );
}

//---------------------------------------------------------
void CSG_String::operator += (const CSG_String &String)
{
	*m_pString	+= *String.m_pString;
}

void CSG_String::operator += (const char *String)
{
	*m_pString	+= String;
}

void CSG_String::operator += (char Character)
{
	*m_pString	+= Character;
}

//---------------------------------------------------------
char & CSG_String::operator [] (int i)
{
	return( m_pString->GetWritableChar(i) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_String::Cmp(const char *String)	const
{
	return( m_pString->Cmp(String) );
}

//---------------------------------------------------------
int CSG_String::CmpNoCase(const char *String) const
{
	return( m_pString->CmpNoCase(String) );
}

//---------------------------------------------------------
CSG_String & CSG_String::Make_Lower(void)
{
	m_pString->MakeLower();

	return( *this );
}

//---------------------------------------------------------
CSG_String & CSG_String::Make_Upper(void)
{
	m_pString->MakeUpper();

	return( *this );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSG_String::Replace(const char *sOld, const char *sNew, bool replaceAll)
{
	return( m_pString->Replace(sOld, sNew, replaceAll) );
}

//---------------------------------------------------------
CSG_String & CSG_String::Remove(size_t pos)
{
	m_pString->Remove(pos);

	return( *this );
}

//---------------------------------------------------------
CSG_String & CSG_String::Remove(size_t pos, size_t len)
{
	m_pString->Remove(pos, len);

	return( *this );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_String::Remove_WhiteChars(bool fromEnd)
{
	int		n, c;

	if( fromEnd )
	{
		for(n=Length()-1; n>=0; n--)
		{
			c	= m_pString->GetChar(n);

			if( c == ' ' || c == '\t' )
			{
				break;
			}
		}

		if( n < (int)Length() - 1 )
		{
			Remove(n);
		}

		n	= Length() - n;
	}
	else
	{
		for(n=0; n<(int)Length(); n++)
		{
			c	= m_pString->GetChar(n);

			if( c == ' ' || c == '\t' )
			{
				break;
			}
		}

		if( n > 0 )
		{
			Remove(0, n);
		}
	}

	return( n );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_String::Find(char Character, bool fromEnd)
{
	return( m_pString->Find(Character, fromEnd) );
}

//---------------------------------------------------------
int CSG_String::Find(const char *String)
{
	return( m_pString->Find(String) );
}

//---------------------------------------------------------
bool CSG_String::Contains(const char *String)
{
	return( m_pString->Contains(String) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_String::AfterFirst(char Character) const
{
	return( CSG_String(m_pString->AfterFirst(Character).c_str()) );
}

//---------------------------------------------------------
CSG_String CSG_String::AfterLast(char Character) const
{
	return( CSG_String(m_pString->AfterLast(Character).c_str()) );
}

//---------------------------------------------------------
CSG_String CSG_String::BeforeFirst(char Character) const
{
	return( CSG_String(m_pString->BeforeFirst(Character).c_str()) );
}

//---------------------------------------------------------
CSG_String CSG_String::BeforeLast(char Character) const
{
	return( CSG_String(m_pString->BeforeLast(Character).c_str()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_String::Right(size_t count) const
{
	return( CSG_String(m_pString->Right(count).c_str()) );
}

//---------------------------------------------------------
CSG_String CSG_String::Mid(size_t first, size_t count) const
{
	return( CSG_String(m_pString->Mid(first, count).c_str()) );
}

//---------------------------------------------------------
CSG_String CSG_String::Left(size_t count) const
{
	return( CSG_String(m_pString->Left(count).c_str()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_String::asInt(void) const
{
	int		Value	= 0;

	asInt(Value);

	return( Value );

//	return( asInt(Value) ? Value : 0 );
}

bool CSG_String::asInt(int &Value) const
{
	long	lValue;

	if( m_pString->ToLong(&lValue) )
	{
		Value	= (int)lValue;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
double CSG_String::asDouble(void) const
{
	double	Value	= 0.0;

	asDouble(Value);

	return( Value );

//	return( asDouble(Value) ? Value : 0.0 );
}

bool CSG_String::asDouble(double &Value) const
{
	return( m_pString->ToDouble(&Value) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Strings::CSG_Strings(void)
{
	m_nStrings	= 0;
	m_Strings	= NULL;
}

//---------------------------------------------------------
CSG_Strings::CSG_Strings(const CSG_Strings &Strings)
{
	m_nStrings	= 0;
	m_Strings	= NULL;

	Assign(Strings);
}

//---------------------------------------------------------
CSG_Strings::CSG_Strings(int nStrings, const char **Strings)
{
	m_nStrings	= 0;
	m_Strings	= NULL;

	for(int i=0; i<m_nStrings; i++)
	{
		Add(Strings[i]);
	}
}

//---------------------------------------------------------
CSG_Strings::~CSG_Strings(void)
{
	Clear();
}

//---------------------------------------------------------
void CSG_Strings::Clear(void)
{
	for(int i=0; i<m_nStrings; i++)
	{
		delete(m_Strings[i]);
	}

	if( m_Strings )
	{
		SG_Free(m_Strings);
	}

	m_nStrings	= 0;
	m_Strings	= NULL;
}

//---------------------------------------------------------
bool CSG_Strings::Assign(const CSG_Strings &Strings)
{
	Clear();

	for(int i=0; i<m_nStrings; i++)
	{
		Add(Strings[i]);
	}

	return( true );
}

//---------------------------------------------------------
CSG_Strings & CSG_Strings::operator  = (const CSG_Strings &Strings)
{
	Assign(Strings);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Strings::Add(const CSG_String &String)
{
	m_Strings	= (CSG_String **)SG_Realloc(m_Strings, (m_nStrings + 1) * sizeof(CSG_String *));
	m_Strings[m_nStrings]	= new CSG_String(String);
	m_nStrings++;

	return( true );
}

//---------------------------------------------------------
bool CSG_Strings::Set_Count(int nStrings)
{
	Clear();

	for(int i=0; i<m_nStrings; i++)
	{
		Add("");
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String		SG_Get_CurrentTimeStr(bool bWithDate)
{
	CSG_String	s;
	wxDateTime	t;

	t.SetToCurrent();

	if( bWithDate )
	{
		s.Append(t.FormatISODate());
		s.Append("/");
	}

	s.Append(t.FormatISOTime());

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String		SG_Double_To_Degree(double Value)
{
	char		c;
	int			d, h;
	double		s;
	CSG_String	String;

	if( Value < 0.0 )
	{
		Value	= -Value;
		c		= '-';
	}
	else
	{
		c		= '+';
	}

	Value	= fmod(Value, 360.0);
	d		= (int)Value;
	Value	= 60.0 * (Value - d);
	h		= (int)Value;
	Value	= 60.0 * (Value - h);
	s		= Value;

	String.Printf("%c%03d�%02d'%02f''", c, d, h, s);

	return( String );
}

//---------------------------------------------------------
double			SG_Degree_To_Double(const char *String)
{
	double		d, h, s, sig;
	CSG_String	sVal(String);

	sig	= 1.0;
	d	= h	= s	= 0.0;

	if( sVal.BeforeFirst('�').asDouble(d) )
	{
		if( d < 0.0 )
		{
			sig	= -1.0;
			d	= -d;
		}

		sVal.AfterFirst('�' ).asDouble(h);
		sVal.AfterFirst('\'').asDouble(s);
	}
	else
	{
		sVal.asDouble(d);
	}

	return( sig * (d + h / 60.0 + s / (60.0 * 60.0)) );
}

//---------------------------------------------------------
CSG_String		SG_Double_To_Date(double Value)
{
	int		y, m, d;

	y	= (int)(Value / 10000);	Value	-= y * 10000;
	m	= (int)(Value / 100);	Value	-= m * 100;
	d	= (int)(Value / 1);

	return( CSG_String::Format("%02d.%02d.%04d", d, m, y) );
}

//---------------------------------------------------------
double			SG_Date_To_Double(const char *String)
{
	int			d, m, y;

	if( String )
	{
		CSG_String	s(String), sValue;

		sValue	= s.AfterLast	('.');
		y		= sValue.asInt();
		sValue	= s.BeforeLast	('.');	s		= sValue;

		sValue	= s.AfterLast	('.');
		m		= sValue.asInt();
		sValue	= s.BeforeLast	('.');	s		= sValue;
		d		= sValue.asInt();

		if( d < 1 )	d	= 1;	else if( d > 31 )	d	= 31;
		if( m < 1 )	m	= 1;	else if( m > 12 )	d	= 12;

		return( 10000 * y + 100 * m + 1 * d );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int				SG_Get_Significant_Decimals(double Value, int maxDecimals)
{
	int		Decimals;
	double	Reminder;

	Value	= fabs(Value);

	for(Decimals=0; Decimals<maxDecimals; Decimals++)
	{
		Reminder	= Value - (int)(Value);

		if( Reminder == 0.0 )
		{
			return( Decimals );
		}

		Value	= 10.0 * Value;
	}

	return( maxDecimals );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String		SG_Get_String(double Value, int Precision, bool bScientific)
{
	CSG_String	s;

	s.Printf("%.*f", Precision, Value);

	s.Replace(",", ".");

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
