/*
  MusicXML Library
  Copyright (C) Grame 2006-2013

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Grame Research Laboratory, 11, cours de Verdun Gensoul 69002 Lyon - France
  research@grame.fr
*/

#include <iostream>
#include <list>
#include <algorithm>
#include <iomanip>      // std::setw, set::precision, ...

#include "lpsr.h"

using namespace std;

namespace MusicXML2 
{

//______________________________________________________________________________
// allocate the indenter static member
indenter lpsrElement::idtr;

//______________________________________________________________________________
SlpsrElement lpsrElement::create(bool debug)  {
  lpsrElement * o = new lpsrElement(debug); assert(o!=0);
  return o; 
}

lpsrElement::lpsrElement(bool debug) {
  fDebug = debug;
}
lpsrElement::~lpsrElement() {}

ostream& operator<< (ostream& os, const SlpsrElement& elt) {
  elt->printLilyPondCode(os);
  return os;
}

void lpsrElement::printLpsrStructure(ostream& os) {
  os << "\%{ lpsrElement??? \%}" << std::endl;
}

void lpsrElement::printLilyPondCode(ostream& os) {
  os << "\%{ lpsrElement??? \%}" << std::endl;
}

//______________________________________________________________________________
SlpsrDuration lpsrDuration::create(int num, int denom, int dots) {
  lpsrDuration * o = new lpsrDuration (num, denom, dots);
  assert(o!=0); 
  return o;
}

lpsrDuration::lpsrDuration (int num, int denom, int dots)
  : lpsrElement("")
{
  fNum=num; fDenom=denom; fDots=dots; 
}
lpsrDuration::~lpsrDuration() {}

void lpsrDuration::scaleNumByFraction (int num, int denom)
{
  fNum *= num/denom;
}

void lpsrDuration::sett (int num, int denom, int dots) {
  fNum=num; fDenom=denom; fDots=dots; 
}

ostream& operator<< (ostream& os, const SlpsrDuration& dur)
{
  dur->printLilyPondCode(os);
  return os;
}

void lpsrDuration::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrDuration??? \%}" << std::endl;
}

void lpsrDuration::printLilyPondCode(ostream& os)
{
  // divisions are per quater, Lpsr durations are in whole notes
  //os << "|"  << fLpsrDuration.fNum << "|" << fLpsrDuration.fDenom;

  int noteDivisions         = fNum;
  int divisionsPerWholeNote = fDenom ;
  
  if (divisionsPerWholeNote == 0)
    {
    os << 
      std::endl << 
      "%--> lpsrDuration::printLilyPondCode, noteDivisions = " << noteDivisions <<
      ", divisionsPerWholeNote = " << divisionsPerWholeNote << std::endl;
    return;
  }
  
  div_t divresult = div (noteDivisions, divisionsPerWholeNote);  
  int   div = divresult.quot;
  int   mod = divresult.rem;
  
  /*
   1024th   


512th   


256th   


128th   


64th  


32nd  


16th  


eighth  


quarter   


half  


whole   


breve   


long  


*/

  switch (div) {
    case 8:
    case 7:
    case 6:
    case 5:
      os << "\\maxima";
      break;
    case 4:
    case 3:
      os << "\\longa";
      break;
    case 2:
      os << "\\breve";
      break;
    case 1:
      os << "1";
      break;
    case 0:
      {
      // shorter than a whole note
      //os << "(shorter than a whole note) ";
      int weight = 2; // half note
      int n = noteDivisions*2;

      while (n < divisionsPerWholeNote) {
         weight *= 2;
         n *= 2;
      } // while
      os << weight;
      }
      break;
    default:
      cerr <<
        "*** ERROR, MusicXML note duration " << noteDivisions << "/" << 
        divisionsPerWholeNote << " is too large" << std::endl;
  } // switch
  
  // print the dots if any  
  if (fDots > 0) {
    while (fDots-- > 0) {
      os << ".";  
    } // while
  }
}

//______________________________________________________________________________
SlpsrNote lpsrNote::create() 
{  
  lpsrNote * o = new lpsrNote (); assert(o!=0); 
  return o;
}

lpsrNote::lpsrNote() : lpsrElement("")
{
  fDiatonicPitch         = lpsrNote::kNoDiatonicPitch;
  fAlteration           = lpsrNote::kNoAlteration;
  fOctave               = -1;
// JMI   
  fLpsrDuration = lpsrDuration::create(99, 99, 0);
  fVoice                = -1;
}
lpsrNote::~lpsrNote() {}

void lpsrNote::updateNote(
  bool              currentStepIsRest,
  DiatonicPitch     diatonicNote,
  Alteration        alteration,
  int               octave,
  SlpsrDuration     dur,
  LpsrPitch         lpsrPitch,
  int               voice,
  bool              noteBelongsToAChord)
{
  fCurrentStepIsRest = currentStepIsRest;
  fDiatonicPitch = diatonicNote;
  fAlteration = alteration;
  fOctave = octave;
  fLpsrDuration = dur;
  fLpsrPitch = lpsrPitch;
  fVoice = voice;
  fNoteBelongsToAChord = noteBelongsToAChord;
}

void lpsrNote::updateNoteDuration(int actualNotes, int normalNotes)
{
  fLpsrDuration->scaleNumByFraction(actualNotes, normalNotes);
}

void lpsrNote::setNoteBelongsToAChord () {
  fNoteBelongsToAChord = true;
}

void lpsrNote::addDynamics (SlpsrDynamics dyn) {
  fNoteDynamics.push_back(dyn);
}
void lpsrNote::addWedge (SlpsrWedge wdg) {
  fNoteWedges.push_back(wdg);
}

SlpsrDynamics lpsrNote::removeFirstDynamics () {
  SlpsrDynamics dyn = fNoteDynamics.front();
  fNoteDynamics.pop_front();
  return dyn;
}
SlpsrWedge lpsrNote::removeFirstWedge () {
  SlpsrWedge wdg = fNoteWedges.front();
  fNoteWedges.pop_front();
  return wdg;
}

ostream& operator<< (ostream& os, const SlpsrNote& elt)
{
  elt->printLilyPondCode(os);
  return os;
}

void lpsrNote::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrNote??? \%}" << std::endl;
}

void lpsrNote::printLilyPondCode(ostream& os)
{
  if (fCurrentStepIsRest) os << "r";
  else {
    // print the note name
    //JMI assertLpsr(fLpsrPitch != k_NoLpsrPitch, "fLpsrPitch != k_NoLpsrPitch");
    switch (fLpsrPitch) {
      
      case k_aeseh:
        os << "aeseh";
        break;
      case k_aes:
        os << "aes";
        break;
      case k_aeh:
        os << "aeh";
        break;
      case k_a:
        os << "a";
        break;
      case k_aih:
        os << "aih";
        break;
      case k_ais:
        os << "ais";
        break;
      case k_aisih:
        os << "aisih";
        break;
        
      case k_beseh:
        os << "beseh";
        break;
      case k_bes:
        os << "bes";
        break;
      case k_beh:
        os << "beh";
        break;
      case k_b:
        os << "b";
        break;
      case k_bih:
        os << "bih";
        break;
      case k_bis:
        os << "bis";
        break;
      case k_bisih:
        os << "bisih";
        break;
        
      case k_ceseh:
        os << "ceseh";
        break;
      case k_ces:
        os << "ces";
        break;
      case k_ceh:
        os << "ceh";
        break;
      case k_c:
        os << "c";
        break;
      case k_cih:
        os << "cih";
        break;
      case k_cis:
        os << "cis";
        break;
      case k_cisih:
        os << "cisih";
        break;
        
      case k_deseh:
        os << "deseh";
        break;
      case k_des:
        os << "des";
        break;
      case k_deh:
        os << "deh";
        break;
      case k_d:
        os << "d";
        break;
      case k_dih:
        os << "dih";
        break;
      case k_dis:
        os << "dis";
        break;
      case k_disih:
        os << "disih";
        break;
  
      case k_eeseh:
        os << "eeseh";
        break;
      case k_ees:
        os << "ees";
        break;
      case k_eeh:
        os << "eeh";
        break;
      case k_e:
        os << "e";
        break;
      case k_eih:
        os << "eih";
        break;
      case k_eis:
        os << "eis";
        break;
      case k_eisih:
        os << "eisih";
        break;
        
      case k_feseh:
        os << "feseh";
        break;
      case k_fes:
        os << "fes";
        break;
      case k_feh:
        os << "feh";
        break;
      case k_f:
        os << "f";
        break;
      case k_fih:
        os << "fih";
        break;
      case k_fis:
        os << "fis";
        break;
      case k_fisih:
        os << "fisih";
        break;
        
      case k_geseh:
        os << "geseh";
        break;
      case k_ges:
        os << "ges";
        break;
      case k_geh:
        os << "geh";
        break;
      case k_g:
        os << "g";
        break;
      case k_gih:
        os << "gih";
        break;
      case k_gis:
        os << "gis";
        break;
      case k_gisih:
        os << "gisih";
        break;
      default:
        os << "Note" << fLpsrPitch << "???";
    } // switch
  }
  
  if (! fNoteBelongsToAChord) {
    // print the note duration
    os << fLpsrDuration;
    
    // print the dynamics if any
    std::list<SlpsrDynamics>::const_iterator i1;
    for (i1=fNoteDynamics.begin(); i1!=fNoteDynamics.end(); i1++) {
      os << " " << (*i1);
    } // for
  
    // print the wedges if any
    std::list<SlpsrWedge>::const_iterator i2;
    for (i2=fNoteWedges.begin(); i2!=fNoteWedges.end(); i2++) {
      os << " " << (*i2);
    } // for
  }
}

/*

std::string lpsrNote::octaveRepresentation (char octave)
{
  stringstream s;
  if (octave > 0) {
    int n = octave;
    while (n > 0) {
    s << "'";
    n--;
    }
  } else if (octave < 0) {
    int n = octave;
    while (n < 0) {
      s << ",";
      n++;
    }  
  }
  return s.str();
}
  */

//______________________________________________________________________________
SlpsrSequence lpsrSequence::create(ElementsSeparator elementsSeparator)
{
  lpsrSequence* o = new lpsrSequence(elementsSeparator); assert(o!=0);
  return o;
}

lpsrSequence::lpsrSequence(ElementsSeparator elementsSeparator) : lpsrElement("")
{
  fElementsSeparator=elementsSeparator;
}
lpsrSequence::~lpsrSequence() {}

ostream& operator<< (ostream& os, const SlpsrSequence& elt)
{
  elt->printLilyPondCode(os);
  return os;
}

void lpsrSequence::printLpsrStructure(ostream& os)
{  
  os << "\%{ lpsrSequence??? \%}" << std::endl;
  
  idtr++;
  /*
  vector<SlpsrElement>::const_iterator i;
  for (i=fSequenceElements.begin(); i!=fSequenceElements.end(); i++) {
    os << (*i);
    if (fElementsSeparator == kEndOfLine) os << std::endl;
    // JMI
    else os << " ";
  } // for
  */
  
  idtr--;
}

void lpsrSequence::printLilyPondCode(ostream& os)
{
  std::list<SlpsrElement>::const_iterator i;
  for (i=fSequenceElements.begin(); i!=fSequenceElements.end(); i++) {
    os << idtr << (*i);
    if (fElementsSeparator == kEndOfLine) os << std::endl;
  } // for
}

//______________________________________________________________________________
SlpsrParallel lpsrParallel::create(ElementsSeparator elementsSeparator)
{
  lpsrParallel* o = new lpsrParallel(elementsSeparator); assert(o!=0);
  return o;
}

lpsrParallel::lpsrParallel(ElementsSeparator elementsSeparator) : lpsrElement("")
{
  fElementsSeparator=elementsSeparator;
}
lpsrParallel::~lpsrParallel() {}

ostream& operator<< (ostream& os, const SlpsrParallel& elt)
{
  elt->printLilyPondCode(os);
  return os;
}

void lpsrParallel::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrParallel??? \%}" << std::endl;
}

void lpsrParallel::printLilyPondCode(ostream& os)
{      
  os << idtr << "<<" << std::endl;
  
  idtr++;
  
  int size = fParallelElements.size();
  
  for (int i = 0; i < size; i++ ) {
    os << idtr << fParallelElements[i];
  } // for
  
  idtr--;
  
  os << idtr << ">>" << std::endl;
}

//______________________________________________________________________________
SlpsrChord lpsrChord::create(SlpsrDuration chordduration)
{
  lpsrChord* o = new lpsrChord(chordduration); assert(o!=0);
  return o;
}

lpsrChord::lpsrChord (SlpsrDuration chordduration)
  : lpsrElement("")
{
  fChordDuration = chordduration;
}
lpsrChord::~lpsrChord() {}

void lpsrChord::addDynamics (SlpsrDynamics dyn) {
  fChordDynamics.push_back(dyn);
}
void lpsrChord::addWedge (SlpsrWedge wdg) {
  fChordWedges.push_back(wdg);
}

ostream& operator<< (ostream& os, const SlpsrChord& chrd)
{
  chrd->printLilyPondCode(os);
  return os;
}

void lpsrChord::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrChord??? \%}" << std::endl;
}

void lpsrChord::printLilyPondCode(ostream& os)
{
  std::vector<SlpsrNote>::const_iterator
    iBegin = fChordNotes.begin(),
    iEnd   = fChordNotes.end(),
    i      = iBegin;
  os << "<";
  for ( ; ; ) {
    os << (*i);
    if (++i == iEnd) break;
    os << " ";
  } // for
  os << ">";
  
  // print the chord duration
  os << fChordDuration;

  // print the dynamics if any
  std::list<SlpsrDynamics>::const_iterator i1;
  for (i1=fChordDynamics.begin(); i1!=fChordDynamics.end(); i1++) {
    os << " " << (*i1);
  } // for

  // print the wedges if any
  std::list<SlpsrWedge>::const_iterator i2;
  for (i2=fChordWedges.begin(); i2!=fChordWedges.end(); i2++) {
    os << " " << (*i2);
  } // for
}

//______________________________________________________________________________
SlpsrBarLine lpsrBarLine::create(int nextBarNumber)
{
  lpsrBarLine* o = new lpsrBarLine(nextBarNumber); assert(o!=0);
  return o;
}

lpsrBarLine::lpsrBarLine(int nextBarNumber) : lpsrElement("")
{
  fNextBarNumber=nextBarNumber; 
}
lpsrBarLine::~lpsrBarLine() {}

ostream& operator<< (ostream& os, const SlpsrBarLine& elt)
{
  elt->printLilyPondCode(os);
  return os;
}

void lpsrBarLine::printLpsrStructure(ostream& os)
{
  os << idtr << "lpsrBarLine " << fNextBarNumber << std::endl;
}

void lpsrBarLine::printLilyPondCode(ostream& os)
{
  os << "| % " << fNextBarNumber << std::endl;
}

//______________________________________________________________________________
SlpsrComment lpsrComment::create(std::string contents, GapKind gapKind)
{
  lpsrComment* o = new lpsrComment(contents, gapKind); assert(o!=0);
  return o;
}

lpsrComment::lpsrComment(std::string contents, GapKind gapKind)
  : lpsrElement("")
{
  fContents=contents;
  fGapKind=gapKind;
}
lpsrComment::~lpsrComment() {}

ostream& operator<< (ostream& os, const SlpsrComment& elt)
{
  elt->printLilyPondCode(os);
  return os;
}

void lpsrComment::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrComment??? \%}" << std::endl;
}

void lpsrComment::printLilyPondCode(ostream& os)
{
  os << "% " << fContents;
  if (fGapKind == kGapAfterwards) os << std::endl;
}

//______________________________________________________________________________
SlpsrBreak lpsrBreak::create(int nextBarNumber)
{
  lpsrBreak* o = new lpsrBreak(nextBarNumber); assert(o!=0);
  return o;
}

lpsrBreak::lpsrBreak(int nextBarNumber) : lpsrElement("")
{
  fNextBarNumber=nextBarNumber; 
}
lpsrBreak::~lpsrBreak() {}

ostream& operator<< (ostream& os, const SlpsrBreak& elt)
{
  elt->printLilyPondCode(os);
  return os;
}

void lpsrBreak::printLpsrStructure(ostream& os)
{
  os << "lpsrBreak " << fNextBarNumber << std::endl;
}

void lpsrBreak::printLilyPondCode(ostream& os)
{
  os <<
    "\\myBreak | % " << fNextBarNumber << std::endl <<
    std::endl;
}

//______________________________________________________________________________
SlpsrBarNumberCheck lpsrBarNumberCheck::create(int nextBarNumber)
{
  lpsrBarNumberCheck* o = new lpsrBarNumberCheck(nextBarNumber); assert(o!=0);
  return o;
}

lpsrBarNumberCheck::lpsrBarNumberCheck(int nextBarNumber) : lpsrElement("")
{
  fNextBarNumber=nextBarNumber; 
}
lpsrBarNumberCheck::~lpsrBarNumberCheck() {}

ostream& operator<< (ostream& os, const SlpsrBarNumberCheck& elt)
{
  elt->printLilyPondCode(os);
  return os;
}

void lpsrBarNumberCheck::printLpsrStructure(ostream& os)
{
  os << "lpsrBarNumberCheck " << fNextBarNumber << std::endl;
}

void lpsrBarNumberCheck::printLilyPondCode(ostream& os)
{
  os << "\\barNumberCheck #" << fNextBarNumber << std::endl;
}

//______________________________________________________________________________
SlpsrTuplet lpsrTuplet::create()
{
  lpsrTuplet* o = new lpsrTuplet(); assert(o!=0);
  return o;
}

lpsrTuplet::lpsrTuplet() : lpsrElement("")
{
  fTupletNumber = k_NoTuplet;
  
  fActualNotes = -1;
  fNormalNotes = -1;
}
lpsrTuplet::~lpsrTuplet() {}

void lpsrTuplet::updateTuplet (int number, int actualNotes, int normalNotes)
{
  fTupletNumber = number;
  
  fActualNotes = actualNotes;
  fNormalNotes = normalNotes;  
}

ostream& operator<< (ostream& os, const SlpsrTuplet& elt)
{
  elt->printLilyPondCode(os);
  return os;
}

void lpsrTuplet::printLpsrStructure(ostream& os)
{
  os <<
    idtr << "lpsrTuplet " << 
    fActualNotes << "/" << fNormalNotes << std::endl;

  idtr++;
  
  std::vector<SlpsrElement>::const_iterator
    iBegin = fTupletContents.begin(),
    iEnd   = fTupletContents.end(),
    i      = iBegin;
    for ( ; ; ) {
      os << (*i);
      if (++i == iEnd) break;
      os << std::endl;
    } // for

  idtr--;
}

void lpsrTuplet::printLilyPondCode(ostream& os)
{
  os << "\\tuplet " << fActualNotes << "/" << fNormalNotes << " { ";

  idtr++;
  
  std::vector<SlpsrElement>::const_iterator
    iBegin = fTupletContents.begin(),
    iEnd   = fTupletContents.end(),
    i      = iBegin;
    for ( ; ; ) {
      os << (*i);
      if (++i == iEnd) break;
      os << " ";
    } // for
  
  idtr--;
  
  os << " }" << std::endl;
}

//______________________________________________________________________________
SlpsrBeam lpsrBeam::create(int number, BeamKind beamKind)
{
  lpsrBeam* o = new lpsrBeam(number, beamKind); assert(o!=0);
  return o;
}

lpsrBeam::lpsrBeam(int number, BeamKind beamKind)
  : lpsrElement("")
{
  fBeamNumber = number;
  fBeamKind   = beamKind; 
}
lpsrBeam::~lpsrBeam() {}

ostream& operator<< (ostream& os, const SlpsrBeam& dyn)
{
  dyn->printLilyPondCode(os);
  return os;
}

void lpsrBeam::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrBeam??? \%}" << std::endl;
}

void lpsrBeam::printLilyPondCode(ostream& os)
{
  switch (fBeamKind) {
    case kBeginBeam:
      os << "[ ";
      break;
    case kContinueBeam:
      break;
    case kEndBeam:
      os << "] ";
      break;
    default:
      os << "Beam " << fBeamKind << "???";
  } // switch
}

//______________________________________________________________________________
SlpsrDynamics lpsrDynamics::create(DynamicsKind dynamicsKind)
{
  lpsrDynamics* o = new lpsrDynamics(dynamicsKind); assert(o!=0);
  return o;
}

lpsrDynamics::lpsrDynamics(DynamicsKind dynamicsKind) : lpsrElement("")
{
  fDynamicsKind = dynamicsKind; 
}
lpsrDynamics::~lpsrDynamics() {}

ostream& operator<< (ostream& os, const SlpsrDynamics& dyn)
{
  dyn->printLilyPondCode(os);
  return os;
}

void lpsrDynamics::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrDynamics??? \%}" << std::endl;
}

void lpsrDynamics::printLilyPondCode(ostream& os)
{
  switch (fDynamicsKind) {
    case kFDynamics:
      os << "\\f";
      break;
    case kPDynamics:
      os << "\\p";
      break;
    default:
      os << "Dynamics " << fDynamicsKind << "???";
  } // switch
}

//______________________________________________________________________________
SlpsrWedge lpsrWedge::create(WedgeKind wedgeKind)
{
  lpsrWedge* o = new lpsrWedge(wedgeKind); assert(o!=0);
  return o;
}

lpsrWedge::lpsrWedge(WedgeKind wedgeKind) : lpsrElement("")
{
  fWedgeKind=wedgeKind; 
}
lpsrWedge::~lpsrWedge() {}

ostream& operator<< (ostream& os, const SlpsrWedge& wdg)
{
  wdg->printLilyPondCode(os);
  return os;
}

void lpsrWedge::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrWedge??? \%}" << std::endl;
}

void lpsrWedge::printLilyPondCode(ostream& os)
{
  switch (fWedgeKind) {
    case lpsrWedge::kCrescendoWedge:
      cout << "\\<";
      break;
    case lpsrWedge::kDecrescendoWedge:
      cout << "\\>";
      break;
    case lpsrWedge::kStopWedge:
      cout << "\\!";
      break;
  } // switch
}

//______________________________________________________________________________
SlpsrLyrics lpsrLyrics::create(std::string name, std::string contents)
{
  lpsrLyrics* o = new lpsrLyrics(name, contents); assert(o!=0);
  return o;
}

lpsrLyrics::lpsrLyrics(std::string name, std::string contents) : lpsrElement("")
{
  fLyricsName = name;
  fLyricsContents=contents; 
}
lpsrLyrics::~lpsrLyrics() {}

void lpsrLyrics::printLpsrStructure(ostream& os)
{  
  os << idtr << "lpsrLyrics " << fLyricsName << std::endl;
  idtr++;
  os << fLyricsContents << std::endl;
  idtr--;
}

void lpsrLyrics::printLilyPondCode(ostream& os)
{  
  os << idtr << fLyricsName << " = \\lyricmode {" << std::endl;
  idtr++;
  os << idtr << fLyricsContents << std::endl;
  idtr--;
  os << idtr << "}" << std::endl;
}

//______________________________________________________________________________
SlpsrPart lpsrPart::create(std::string name, bool absoluteCode, bool generateNumericalTime)
{
  lpsrPart* o = new lpsrPart(name, absoluteCode, generateNumericalTime); assert(o!=0);
  return o;
}

lpsrPart::lpsrPart(std::string name, bool absoluteCode, bool generateNumericalTime)
  : lpsrElement("")
{
  fPartName = name;
  fPartAbsoluteCode = absoluteCode;
  fGenerateNumericalTime = generateNumericalTime;
  
  // create the implicit lpsrSequence element
  fPartLpsrSequence = lpsrSequence::create(lpsrSequence::kSpace);
  
  // add the default 4/4 time signature
  SlpsrTime time = lpsrTime::create(4, 4, fGenerateNumericalTime);
  SlpsrElement t = time;
  fPartLpsrSequence->appendElementToSequence (t);

}
lpsrPart::~lpsrPart() {}

void lpsrPart::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrPart??? \%}" << std::endl;
}

void lpsrPart::printLilyPondCode(ostream& os)
{
  /*
  lpsrElement cmd = lpsrcmd::create("set Staff.instrumentName ="); // USER
  stringstream s1, s2;
  string instr = header.fPartName->getValue();
  */
  
  os << idtr << fPartName << " = ";
  if (! fPartAbsoluteCode) os << "\\relative ";
  os << "{" << std::endl;
  idtr++;
  os << fPartLpsrSequence << std::endl;
  idtr--;
  os << idtr << "}" << std::endl;
}

//______________________________________________________________________________
SlpsrPaper lpsrPaper::create() {
  lpsrPaper* o = new lpsrPaper(); assert(o!=0);
  return o;
}

lpsrPaper::lpsrPaper() : lpsrElement("") {
  fPaperWidth = -1.0;
  fPaperHeight = -1.0;
  fTopMargin = -1.0;
  fBottomMargin = -1.0;
  fLeftMargin = -1.0;
  fRightMargin = -1.0;
    
  fBetweenSystemSpace = -1.0;
  fPageTopSpace = -1.0;
}
lpsrPaper::~lpsrPaper() {}

ostream& operator<< (ostream& os, const SlpsrPaper& pap) {
  pap->printLilyPondCode(os);
  return os;
}

void lpsrPaper::printLpsrStructure(ostream& os) {
  os << "\%{ lpsrPaper??? \%}" << std::endl;
}

void lpsrPaper::printLilyPondCode(ostream& os)
{  
  os << "\\paper {" << std::endl;

  idtr++;
  
  if (fPaperWidth > 0) {
    os << 
      idtr << "paper-width = " << std::setprecision(4) << fPaperWidth << "\\cm" << std::endl;
  }
  if (fPaperHeight > 0) {
    os <<
      idtr << "paper-height = " << std::setprecision(4) << fPaperHeight << "\\cm" << std::endl;
  }
  if (fTopMargin > 0) {
    os <<
      idtr << "top-margin = " << std::setprecision(4) << fTopMargin << "\\cm" << std::endl;
  }
  if (fBottomMargin > 0) {
    os <<
      idtr << "bottom-margin = " << std::setprecision(4) << fBottomMargin << "\\cm" << std::endl;
  }
  if (fLeftMargin > 0) {
    os <<
      idtr << "left-margin = " << std::setprecision(4) << fLeftMargin << "\\cm" << std::endl;
  }

  if (fRightMargin > 0) {
    os << idtr << "right-margin = " << std::setprecision(4) << fRightMargin << "\\cm" << std::endl;
  }

/*
  if (fBetweenSystemSpace > 0) {
    os << idtr << "between-system-space = " << std::setprecision(4) << fBetweenSystemSpace << "\\cm" << std::endl;
  }

  if (fPageTopSpace > 0) {
    os << idtr << "page-top-space = " << std::setprecision(4) << fPageTopSpace << "\\cm" << std::endl;
  }
*/

  idtr--;
  
  os << "}" << std::endl;
}

//______________________________________________________________________________
SlpsrHeader lpsrHeader::create() {
  lpsrHeader* o = new lpsrHeader(); assert(o!=0);
  return o;
}

lpsrHeader::lpsrHeader() : lpsrElement("") {}
lpsrHeader::~lpsrHeader() {}

void lpsrHeader::printLpsrStructure(ostream& os) {
  os << "\%{ lpsrHeader??? \%}" << std::endl;
}

void lpsrHeader::printLilyPondCode(ostream& os) {
  idtr++;
  
  os << "\\header {" << std::endl;
  
  if (fScorePartwise) {
    std::string source = fScorePartwise->getAttributeValue("version");
    std::string dest;
    std::for_each( source.begin(), source.end(), stringQuoteEscaper(dest));
    os << idtr << "%MusicXMl_version = \"" << dest << "\"" << std::endl;
  }
  
  if (fWorkNumber) {
    std::string source = fWorkNumber->getValue();
    std::string dest;
    std::for_each( source.begin(), source.end(), stringQuoteEscaper(dest));
    os << idtr << "%work_number = \""  << dest << "\"" << std::endl;
  }
  
  if (fWorkTitle) {
    std::string source = fWorkTitle->getValue();
    std::string dest;
    std::for_each( source.begin(), source.end(), stringQuoteEscaper(dest));
    os << idtr << "%work_title = \""  << dest << "\"" << std::endl;
  }
    
  if (fMovementNumber) {
    os << idtr << "%movement_number = \""  << fMovementNumber->getValue() << "\"" << std::endl;
  }
    
  if (fMovementTitle) {
    std::string source = fMovementTitle->getValue();
    std::string dest;
    std::for_each( source.begin(), source.end(), stringQuoteEscaper(dest));
    os << idtr << "%movement_title = \""  << dest << "\"" << std::endl;
    os << idtr << "title = \""  << dest << "\"" << std::endl;
  }
    
  if (!fCreators.empty()) {
    vector<S_creator>::const_iterator i1;
    for (i1=fCreators.begin(); i1!=fCreators.end(); i1++) {
      string type = (*i1)->getAttributeValue("type");
      std::transform(type.begin(), type.end(), type.begin(), ::tolower);
      if (type == "composer" || type == "arranger")
        os
          << idtr << type << " = \""  << (*i1)->getValue() << "\"" << std::endl;
      else
        os <<
          idtr << "%" << type << " = \""  << (*i1)->getValue() << "\"" << std::endl;
    } // for
  }
    
  if (fRights) {
    std::string source = fRights->getValue();
    std::string dest;
    std::for_each( source.begin(), source.end(), stringQuoteEscaper(dest));
//    os << idtr << "%rights = \""  << dest << "\"" << std::endl;
    os << idtr << "copyright = \""  << dest << "\"" << std::endl;
  }
    
  if (!fSoftwares.empty()) {
    vector<S_software>::const_iterator i2;
    for (i2=fSoftwares.begin(); i2!=fSoftwares.end(); i2++) {
      std::string source = (*i2)->getValue();
      std::string dest;
      std::for_each( source.begin(), source.end(), stringQuoteEscaper(dest));
//      os << idtr << "%software = \""  << dest << "\"" << std::endl;
      os << idtr << "encodingsoftware = \""  << dest << "\"" << std::endl;
    } // for
  }
    
  if (fEncodingDate) {
    std::string source = fEncodingDate->getValue();
    std::string dest;
    std::for_each( source.begin(), source.end(), stringQuoteEscaper(dest));
//    os << idtr << "%encoding_date = \""  << dest << "\"" << std::endl;
    os << idtr << "encodingdate = \""  << dest << "\"" << std::endl;
  }
  
  os << "}" << std::endl;
  
  idtr--;
}

//______________________________________________________________________________
SlpsrLilypondVarValAssoc lpsrLilypondVarValAssoc::create(
      std::string     variableName,
      std::string     value, 
      VarValSeparator varValSeparator,
      QuotesKind      quotesKind,
      CommentedKind   commentedKind,
      std::string     unit)
{
  lpsrLilypondVarValAssoc* o =
    new lpsrLilypondVarValAssoc(
    variableName, value, varValSeparator, 
    quotesKind, commentedKind, unit);
  assert(o!=0);
  return o;
}

lpsrLilypondVarValAssoc::lpsrLilypondVarValAssoc(
      std::string     variableName,
      std::string     value, 
      VarValSeparator varValSeparator,
      QuotesKind      quotesKind,
      CommentedKind   commentedKind,
      std::string     unit)
  : lpsrElement("")
{
  fVariableName=variableName;
  fVariableValue=value;
  fVarValSeparator=varValSeparator;
  fQuotesKind=quotesKind;
  fCommentedKind=commentedKind;
  fUnit = unit;
}

lpsrLilypondVarValAssoc::~lpsrLilypondVarValAssoc() {}

void lpsrLilypondVarValAssoc::changeAssoc (std::string value) {
  fVariableValue=value;
}

ostream& operator<< (ostream& os, const SlpsrLilypondVarValAssoc& assoc) {
  assoc->printLilyPondCode(os);
  return os;
}

void lpsrLilypondVarValAssoc::printLpsrStructure(ostream& os) {
  os << "\%{ lpsrLilypondVarValAssoc??? \%}" << std::endl;
}

void lpsrLilypondVarValAssoc::printLilyPondCode(ostream& os) {
  if (fCommentedKind == kCommented) os << "\%";
  os << fVariableName;
  if (fVarValSeparator == kEqualSign) os << " = ";
  else os << " ";
  if (fQuotesKind == kQuotesAroundValue) os << "\"";
  os << fVariableValue << fUnit;
  if (fQuotesKind == kQuotesAroundValue) os << "\"";
  os << std::endl;
}

//______________________________________________________________________________
SlpsrSchemeVarValAssoc lpsrSchemeVarValAssoc::create(
      std::string     variableName,
      std::string     value, 
      CommentedKind   commentedKind )
{
  lpsrSchemeVarValAssoc* o = new lpsrSchemeVarValAssoc(
    variableName, value, commentedKind);
  assert(o!=0);
  return o;
}

lpsrSchemeVarValAssoc::lpsrSchemeVarValAssoc(
      std::string     variableName,
      std::string     value, 
      CommentedKind   commentedKind )
  : lpsrElement("")
{
  fVariableName=variableName;
  fVariableValue=value;
  fCommentedKind=commentedKind;
}

lpsrSchemeVarValAssoc::~lpsrSchemeVarValAssoc() {}

void lpsrSchemeVarValAssoc::changeAssoc (std::string value)
{
  fVariableValue=value;
}

ostream& operator<< (ostream& os, const SlpsrSchemeVarValAssoc& assoc)
{
  assoc->printLilyPondCode(os);
  return os;
}

void lpsrSchemeVarValAssoc::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrSchemeVarValAssoc??? \%}" << std::endl;
}

void lpsrSchemeVarValAssoc::printLilyPondCode(ostream& os)
{
  if (fCommentedKind == kCommented) os << "\%";
  os << "#(" << fVariableName << " " << fVariableValue << ")" << std::endl;
}

//______________________________________________________________________________
SlpsrLayout lpsrLayout::create() {
  lpsrLayout* o = new lpsrLayout(); assert(o!=0);
  return o;
}

lpsrLayout::lpsrLayout() : lpsrElement("") {}
lpsrLayout::~lpsrLayout() {}

ostream& operator<< (ostream& os, const SlpsrLayout& lay) {
  lay->printLilyPondCode(os);
  return os;
}

void lpsrLayout::printLpsrStructure(ostream& os) {
  os << idtr << "\%{ lpsrLayout??? \%}" << std::endl;
}

void lpsrLayout::printLilyPondCode(ostream& os) {  
  os << idtr << "\\layout {" << std::endl;

  idtr++;

  int n1 = fLpsrLilypondVarValAssocs.size();
  for (int i = 0; i < n1; i++ ) {
    os << idtr << fLpsrLilypondVarValAssocs[i];
  } // for
    
  int n2 = fLpsrSchemeVarValAssocs.size();
  for (int i = 0; i < n2; i++ ) {
    os << idtr << fLpsrSchemeVarValAssocs[i];
  } // for
  
  idtr--;

  os << idtr << "}" << std::endl;
}

//______________________________________________________________________________
SlpsrTime lpsrTime::create(int numerator, int denominator, bool generateNumericalTime)
{
  lpsrTime* o = new lpsrTime(numerator, denominator, generateNumericalTime); assert(o!=0);
  return o;
}

lpsrTime::lpsrTime(int numerator, int denominator, bool generateNumericalTime)
  : lpsrElement("")
{
  fRational = rational(numerator, denominator);
// JMI  fNumerator=numerator; 
  //fDenominator=denominator;
  fGenerateNumericalTime = generateNumericalTime;
}
lpsrTime::~lpsrTime() {}

void lpsrTime::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrTime??? \%}" << std::endl;
}

void lpsrTime::printLilyPondCode(ostream& os)
{
//  os << fName << "\\time \"" << fNumerator << "/" << fDenominator << "\"" << std::endl;
  if (fGenerateNumericalTime)
    os << "\\numericTimeSignature ";
  os <<
    "\\time " <<
    fRational.getNumerator() << "/" << fRational.getDenominator() <<
    std::endl;
}

//______________________________________________________________________________
SlpsrClef lpsrClef::create(std::string clefName)
{
  lpsrClef* o = new lpsrClef(clefName); assert(o!=0);
  return o;
}

lpsrClef::lpsrClef(std::string clefName) : lpsrElement("")
{
  fClefName=clefName;
}
lpsrClef::~lpsrClef() {}

void lpsrClef::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrClef??? \%}" << std::endl;
}

void lpsrClef::printLilyPondCode(ostream& os)
{
  os << "\\clef \"" << fClefName << "\"" << std::endl;
}

//______________________________________________________________________________
SlpsrKey lpsrKey::create(std::string tonic, KeyMode keyMode)
{
  lpsrKey* o = new lpsrKey(tonic, keyMode); assert(o!=0);
  return o;
}

lpsrKey::lpsrKey(std::string tonic, KeyMode keyMode) : lpsrElement("")
{
  fTonic=tonic;
  fKeyMode=keyMode; 
}
lpsrKey::~lpsrKey() {}

ostream& operator<< (ostream& os, const SlpsrKey& key)
{
  key->printLilyPondCode(os);
  return os;
}

void lpsrKey::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrKey??? \%}" << std::endl;
}

void lpsrKey::printLilyPondCode(ostream& os)
{
  os << "\\key " << fTonic << " ";
  if (fKeyMode == kMajor) os << "\\major";
  else os << "\\minor";
  os << std::endl;
}

//______________________________________________________________________________
SlpsrMidi lpsrMidi::create()
{
  lpsrMidi* o = new lpsrMidi(); assert(o!=0);
  return o;
}

lpsrMidi::lpsrMidi() : lpsrElement("")
{}
lpsrMidi::~lpsrMidi() {}

ostream& operator<< (ostream& os, const SlpsrMidi& mid)
{
  mid->printLilyPondCode(os);
  return os;
}

void lpsrMidi::printLpsrStructure(ostream& os)
{
  os << idtr << "\%{ lpsrMidi??? \%}" << std::endl;
}

void lpsrMidi::printLilyPondCode(ostream& os)
{  
  os << idtr << "\\midi {" << std::endl;
  
  idtr++;
  
  os << idtr << "% to be completed" << std::endl;
  
  idtr--;
  
  os << idtr << "}" << std::endl;
}

//______________________________________________________________________________
SlpsrScore lpsrScore::create()
{
  lpsrScore* o = new lpsrScore(); assert(o!=0);
  return o;
}

lpsrScore::lpsrScore() : lpsrElement("")
{
  // create the parallel music element
  fScoreParallelMusic = lpsrParallel::create(lpsrParallel::kEndOfLine);
  
  // create the layout element
  fScoreLayout = lpsrLayout::create();
  
  // create the midi element
  fScoreMidi = lpsrMidi::create();
  
  // add the "indent" association to the layout
  /*

  SlpsrComment com =
    lpsrComment::create("uncomment the following to keep original scores global size");
  fLpsrSeq->prependElementToSequence (com);

  stringstream s;
  std::string globalSfaffSizeAsString;

  s << fGlobalStaffSize;
  s >> globalSfaffSizeAsString;

  */

  SlpsrSchemeVarValAssoc staffSize =
    lpsrSchemeVarValAssoc::create (
      "layout-set-staff-size", "14",
      lpsrSchemeVarValAssoc::kCommented);
  fScoreLayout->addLpsrSchemeVarValAssoc (staffSize);  
}
lpsrScore::~lpsrScore() {}

ostream& operator<< (ostream& os, const SlpsrScore& scr)
{
  scr->printLilyPondCode(os);
  return os;
}

void lpsrScore::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrScore??? \%}" << std::endl;
}

void lpsrScore::printLilyPondCode(ostream& os) {  
  os << "\\score {" << std::endl;

  idtr++;

  os << fScoreParallelMusic << std::endl;

  os << fScoreLayout << std::endl;

  os << fScoreMidi;

  idtr--;

  os << "}" << std::endl;
  
}

//______________________________________________________________________________
SlpsrNewstaffCommand lpsrNewstaffCommand::create()
{
  lpsrNewstaffCommand* o = new lpsrNewstaffCommand(); assert(o!=0);
  return o;
}

lpsrNewstaffCommand::lpsrNewstaffCommand() : lpsrElement("")
{}
lpsrNewstaffCommand::~lpsrNewstaffCommand() {}

ostream& operator<< (ostream& os, const SlpsrNewstaffCommand& nstf)
{
  nstf->printLilyPondCode(os);
  return os;
}

void lpsrNewstaffCommand::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrNewstaffCommand??? \%}" << std::endl;
}

void lpsrNewstaffCommand::printLilyPondCode(ostream& os)
{      
  os << "\\new Staff <<" << std::endl;
  
  idtr++;
  
  int size = fNewStaffElements.size();

  for (int i = 0; i < size; i++ ) {
    os << idtr << fNewStaffElements[i];
  } // for
  
  idtr--;
  
  os << idtr << ">>" << std::endl;  
}

//______________________________________________________________________________
SlpsrNewlyricsCommand lpsrNewlyricsCommand::create()
{
  lpsrNewlyricsCommand* o = new lpsrNewlyricsCommand(); assert(o!=0);
  return o;
}

lpsrNewlyricsCommand::lpsrNewlyricsCommand() : lpsrElement("")
{}
lpsrNewlyricsCommand::~lpsrNewlyricsCommand() {}

ostream& operator<< (ostream& os, const SlpsrNewlyricsCommand& nstf)
{
  nstf->printLilyPondCode(os);
  return os;
}

void lpsrNewlyricsCommand::printLpsrStructure(ostream& os)
{
  os << "\%{ lpsrNewlyricsCommand??? \%}" << std::endl;
}

void lpsrNewlyricsCommand::printLilyPondCode(ostream& os)
{  
  os << "\\new Lyrics <<" << std::endl;
  
  idtr++;
  
  if (fNewStaffElements.empty()) {
    cerr <<
      "%ERROR, fNewStaffElements is empty" << std::endl;
    cout <<
      "%ERROR, fNewStaffElements is empty" << std::endl;
  } else {
    vector<SlpsrElement>::const_iterator
    iBegin = fNewStaffElements.begin(),
    iEnd   = fNewStaffElements.end(),
    i      = iBegin;
    for ( ; ; ) {
      os << idtr << (*i);
      if (++i == iEnd) break;
      os << std::endl;
    } // for
  }
    
  idtr--;
  
  os << ">>" << std::endl;
}

//______________________________________________________________________________
SlpsrVariableUseCommand lpsrVariableUseCommand::create(std::string variableName)
{
  lpsrVariableUseCommand* o = new lpsrVariableUseCommand(variableName); assert(o!=0);
  return o;
}

lpsrVariableUseCommand::lpsrVariableUseCommand(std::string variableName)
  : lpsrElement("")
{ fVariableName = variableName; }
lpsrVariableUseCommand::~lpsrVariableUseCommand() {}

ostream& operator<< (ostream& os, const SlpsrVariableUseCommand& nstf)
{
  nstf->printLilyPondCode(os);
  return os;
}

void lpsrVariableUseCommand::printLpsrStructure(ostream& os)
{
  os << "lpsrVariableUseCommand" << fVariableName << std::endl;
}

void lpsrVariableUseCommand::printLilyPondCode(ostream& os)
{
  os << "\\" << fVariableName << std::endl;
}


}
