/*
  MusicXML Library
  Copyright (C) Grame 2006-2013

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Grame Research Laboratory, 11, cours de Verdun Gensoul 69002 Lyon - France
  research@grame.fr
*/

#ifndef __messagesHandling__
#define __messagesHandling__

#include "iomanip" // for 'setw()'

using namespace std;

namespace MusicXML2 
{

/*!
\addtogroup msr
@{
*/

/*!
\internal
\brief A macro to exit when a condition if false
*/
// a macro to abort with code source line information,
// as assert() itself
//______________________________________________________________________________

#define msrAssert( condition, messageIfFalse ) \
{ \
  if (! condition) { \
    cerr << \
      endl << \
      "#### msrAssert failure: " << messageIfFalse << \
      endl << \
      "Aborting." << \
      endl << \
      flush; \
      \
    assert(condition); \
  } \
}

//______________________________________________________________________________
void msrWarning (
  string context,
  string inputSourceName,
  int    inputLineNumber,
  string message);

//______________________________________________________________________________
void msrMusicXMLWarning (
  int    inputLineNumber,
  string message);

//______________________________________________________________________________
void msrInternalWarning (
  int    inputLineNumber,
  string message);

//______________________________________________________________________________
void msrError (
  string context,
  string inputSourceName,
  int    inputLineNumber,
  string message);

//______________________________________________________________________________
#define msrMusicXMLError( inputLineNumber, message ) \
{ \
  msrError ( \
    "MusicXML", \
    gGeneralOptions->fInputSourceName, \
    inputLineNumber, \
    message); \
\
  if (! gMusicXMLOptions->fIgnoreMusicXMLErrors) \
    assert(false); \
}

/*!
\internal
\brief A macro to emit error messages regarding MSR and exit
*/
//______________________________________________________________________________

#define msrInternalError( inputLineNumber, message ) \
{ \
  msrError ( \
    "MSR INTERNAL", \
    gGeneralOptions->fInputSourceName, \
    inputLineNumber, \
    message); \
\
  assert(false); \
}


/*! @} */

}


#endif
