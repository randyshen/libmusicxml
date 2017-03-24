/*

  Copyright (C) 2003-2008  Grame
  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

  This file is provided as an example of the MusicXML Library use.
*/

#ifdef VC6
# pragma warning (disable : 4786)
#endif

#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <iomanip>      // setw, set::precision, ...

#include <fstream>      // ofstream, ofstream::open(), ofstream::close()

#include <regex>

#include "msrVersion.h"
#include "msrUtilities.h"

#include "xml2Msr.h"

#include "lpsr.h"
#include "msr2Lpsr.h"
#include "lpsr2LilyPond.h"


using namespace std;
using namespace MusicXML2;

//_______________________________________________________________________________
void printUsage (int exitStatus)
{
  cerr <<
    endl <<
    "                   Welcome to xml2lilypond, " << endl <<
    "              the MusicXML to LilyPond translator" << endl <<
    "          delivered as part of the libmusicxml2 library." << endl <<
    "      https://github.com/grame-cncm/libmusicxml/tree/lilypond" << endl <<
    endl <<
    "Usage:" << endl <<
    endl <<
    "    xml2lilypond [options] [MusicXMLFile|-]" << endl <<
    endl <<
    "What it does:" << endl <<
    endl <<
    "    This multi-pass translator basically performs 4 passes:" << endl <<
    "        Pass 1: reads the contents of MusicXMLFile or stdin ('-')" << endl <<
    "                and converts it to a MusicXML tree;" << endl <<
    "        Pass 2: converts that tree to a Music Score Representation (MSR);" << endl <<
    "        Pass 3: converts the MSR to a LilyPond Score Representation (LPSR);" << endl <<
    "        Pass 4: writes the LPSR as LilyPond source code to standard output." << endl <<
    endl <<
    "    Other passes are performed according to the options, such as" << endl <<
    "    printing views of the internal data or printing a summary of the score." << endl <<
    endl <<
    "    There is a number of options to fine tune the generated LilyPond code." << endl <<
    endl <<
    "    The activity log and warning/error messages go to standard error." << endl <<
    endl <<

    "Options:" << endl <<
    endl <<

    "  General:" << endl <<
    endl <<

    "    --h, --help" << endl <<
    "          Display this help and exit." << endl <<
    "    --v, --version" << endl <<
    "          Display xml2lilypond's version number and exit." << endl <<
    endl <<

    "    --of, --outputFile fileName" << endl <<
    "          Write LilyPond code to file 'fileName' instead of standard output." << endl <<
    endl <<

//    "    --i, --interactive" << endl <<
//    "          Don't create and LilyPond output file, " << endl <<
//    "          but print the code to standard output instead." << endl <<
//    endl <<

    "    --nt, --noTrace" << endl <<
    "          Don't generate a trace of the activity to standard error." << endl <<
    endl <<
    "    --d, --debug " << endl <<
    "          Generate a trace of the activity and print additional" << endl <<
    "          debugging information to standard error." << endl <<
    "    --dd, --debugDebug " << endl <<
    "          Same as above, but print even more debugging information." << endl <<
    endl <<
    "    --fd, --forceDebug " << endl <<
    "          Force debugging information to be printed at some places." << endl <<
    "          This is less invasive that debugging everything." << endl <<
    endl <<
    "    --dm, --debugMeasures measureNumbersSet" << endl <<
    "          'measureNumbersSet' has a form such as '0,2-14,^8-10'," << endl <<
    "          where '^' excludes the corresponding numbers interval" << endl <<
    "          and 0 applies to the '<part-list>' and anacrusis if present." <<endl <<
    "          Generate a trace of the activity and print additional" << endl <<
    "          debugging information to standard error for the specified measures." << endl <<
    "    --ddm, --debugDebugMeasures <measuresSpec>" << endl <<
    "          Same as above, but print even more debugging information." << endl <<
    endl <<

    "  MSR:" << endl <<
    endl <<

    "    --mpl, --msrPitchesLanguage language" << endl <<
    "          Use 'language' to display note pitches in the MSR logs and text views." << endl <<
    "          'language' can be one of 'nederlands', 'catalan', 'deutsch', " << endl <<
    "          'english', 'espanol', 'francais', 'italiano', 'norsk', 'portugues'," << endl <<
    "          'suomi', 'svenska' or 'vlaams'. Default is 'nederlands'." << endl <<
    endl <<
    "    --srvn, --staffRelativeVoiceNumbers" << endl <<
    "          Generate voices names with numbers relative to their staff." << endl <<
    "          By default, the voice numbers found are used, " << endl <<
    "          which may be global to the score." << endl <<
    endl <<
    
    "    --noms, --dontDisplayMSRStanzas" << endl <<
    "          Don't display MSR stanzas while displaying MSR data." << endl <<
    endl <<

    "    --drd, --delayRestsDynamics" << endl <<
    "          Don't attach dynamics and wedges to rests," << endl <<
    "          but delay them until the next actual note or chord instead." << endl <<
    endl <<

    "    --msr, --displayMSR" << endl <<
    "          Write the contents of the MSR data to standard error." << endl <<
    endl <<

    "    --sum, --displayMSRSummary" << endl <<
    "          Only write a summary of the MSR to standard error." << endl <<
    "          This implies that no LilyPond code is generated." << endl <<
    endl <<

    "    --part, --partName 'originalName = newName'" << endl <<
    "    --part, --partName \"originalName = newName\"" << endl <<
    "          Rename part 'original' to 'newName', for example after " << endl <<
    "          displaying a summary of the score in a first xml2lilypond run." << endl <<
    "          There can be several occurrences of this option." << endl <<
    endl <<

    "  LPSR:" << endl <<
    endl <<

    "    --lppl, --lpsrPitchesLanguage language" << endl <<
    "          Use 'language' to display note pitches in the LPSR logs and views," << endl <<
    "          as well as in the generated LilyPond code." << endl <<
    "          The 12 LilyPond pitches languages are available:" << endl <<
    "          nederlands, catalan, deutsch, english, espanol, français, " << endl <<
    "          italiano, norsk, portugues, suomi, svenska and vlaams." << endl <<
    "          The default is to use 'nederlands'." << endl <<
    endl <<
    "    --lpcl, --lpsrChordsLanguage language" << endl <<
    "          Use 'language' to display chord names, their root and bass notes," << endl <<
    "          in the LPSR logs and views and the generated LilyPond code." << endl <<
    "          The 4 LilyPond chords languages are available:" << endl <<
    "          german, semiGerman, italian and french." << endl <<
    "          The default used by LilyPond is Ignatzek's jazz-like, english naming." << endl <<
    endl <<

    "    --lpsr, --displayLPSR" << endl <<
    "          Write the contents of the LPSR data to standard error." << endl <<
    endl <<

    "    --abs, --absolute" << endl <<
    "          Generate LilyPond absolute note octaves. " << endl <<
    "          By default, relative octaves are generated." << endl <<
    endl <<

//    "    --indent" << endl <<
    "    --sabn, --showAllBarNumbers" << endl <<
    "          Generate LilyPond code to show all bar numbers." << endl <<
    endl <<
    "    --cfbr, --compressFullBarRests" << endl <<
    "          Generate LilyPond code compress all full bar rests in the voices." << endl <<
    endl <<
    "    --noBreaks, --dontKeepLineBreaks" << endl <<
    "          Don't keep the line breaks from the MusicXML input" << endl <<
    "          and let LilyPond decide about them." << endl <<
    endl <<

    "    --numericalTime" << endl <<
    "          Don't generate non-numerical time signatures such as 'C'." << endl <<
    "    --comments" << endl <<
    "          Generate comments showing the structure of the score" << endl <<
    "          such as '% part P_POne (P1)'." << endl <<
    "    --stems" << endl <<
    "          Generate \\stemUp and \\stemDown LilyPond commands." << endl <<
    "          By default, LilyPond will take care of that by itself." << endl <<
    "    --noab, --noAutoBeaming" << endl <<
    "          Generate '\\set Voice.autoBeaming = ##f' in each voice " << endl <<
    "          to prevent LilyPond from handling beams automatically." << endl <<
    "    --iln, --generateInputLineNumbers" << endl <<
    "          Generate after each note and barline a comment containing" << endl <<
    "          its MusicXML input line number." << endl <<
    endl <<
    "    --ncfbr, --dontCompressFullBarRests" << endl <<
    "          Comment the '\\compressFullBarRests' command at the beginning of voices" << endl <<
    endl <<
    "    --blairm, --breakLinesAtIncompleteRightMeasures" << endl <<
    "          Generate a '\\break' command at the end incomplete right measures" << endl <<
    "          which is handy in popular folk dances and tunes." << endl <<
    endl <<
    "    --as, --accidentalStyle style" << endl <<
    "          Choose the LilyPond accidental styles among " << endl <<
    "          'voice', 'modern', 'modern-cautionary', 'modern-voice', " <<  endl <<
    "          'modern-voice-cautionary', 'piano', 'piano-cautionary', " << endl <<
    "          'neo-modern', 'neo-modern-cautionary', 'neo-modern-voice'," << endl <<
    "          'neo-modern-voice-cautionary', 'dodecaphonic', 'dodecaphonic-no-repeat'," << endl <<
    "          'dodecaphonic-first', 'teaching', 'no-reset forget." << endl <<
    "          The default is... 'default'." << endl <<
    endl <<
    "    --dof, --delayedOrnamentsFraction 'num/denom'" << endl <<
    "    --dof, --delayedOrnamentsFraction \"num/denom\"" << endl <<
    "          Place the delayed turn/reverseturn at the given fraction" << endl <<
    "          between the ornemented note and the next one." << endl <<
    "          The default fraction is '2/3'." << endl <<
    endl <<
    "    --mt, --midiTempo 'duration = perSecond'" << endl <<
    "    --mt, --midiTempo \"duration = perSecond\"" << endl <<
    "          Generate a '\\tempo duration = perSecond' command in the \\midi block." << endl <<
    "          'duration' is a string such as '8.', and 'perSecond' is an integer," << endl <<
    "          and spaces can be used frely in the argument." << endl <<
    "          The default is '4 = 100'." << endl <<
    endl <<
    "    --nolpc, --dontGenerateLilyPondCode" << endl <<
    "          Don't generate LilyPond code." << endl <<
    "          This can be useful if only a summary of the score is needed." << endl <<
    endl <<
    "    --nolpl, --dontGenerateLilyPondLyrics" << endl <<
    "          Don't generate lyrics in the LilyPond code." << endl <<
    endl <<
    endl <<

    endl;
    
  exit(exitStatus);
}

void optionError (string errorMessage)
{
  cerr <<
    endl <<
    idtr <<
    "### ERROR in the options:" <<
    endl;

  idtr++;

  cerr << idtr <<
    errorMessage <<
    endl <<
    endl;
    
  idtr--;

  exit(99);
}

//_______________________________________________________________________________
void analyzeOptions (
  int            argc,
  char*          argv[],
  string&        inputFileName,
  string&        outputFileName)
{
  // MSR options
  // -----------

  if (
    ! 
      gMsrOptions->setMsrQuartertonesPitchesLanguage ("nederlands")
    ) {
    stringstream s;

    s <<
      "INTERNAL INITIALIZATION ERROR: "
      "MSR pitches language 'nederlands' is unknown" <<
      endl <<
      "The " <<
      gQuatertonesPitchesLanguagesMap.size () <<
      " known MSR pitches languages are:" <<
      endl;

    idtr++;
  
    s <<
      existingQuartertonesPitchesLanguages ();

    idtr--;

    optionError (s.str());
  }
  
  gMsrOptions->fCreateStaffRelativeVoiceNumbers      = false;
  
  gMsrOptions->fDontDisplayMSRStanzas                = false;

  gMsrOptions->fDelayRestsDynamics                   = false;
  
  gMsrOptions->fDisplayMSR                           = false;

  gMsrOptions->fDisplayMSRSummary                    = false;

  // LPSR options
  // ------------

  if (
    ! 
      gLpsrOptions->setLpsrQuartertonesPitchesLanguage ("nederlands")
    ) {
    stringstream s;

    s <<
      "INTERNAL INITIALIZATION ERROR: "
      "LPSR pitches language 'nederlands' is unknown" <<
      endl <<
      "The " <<
      gQuatertonesPitchesLanguagesMap.size () <<
      " known LPSR pitches languages are:" <<
      endl;

    idtr++;
  
    s <<
      existingQuartertonesPitchesLanguages ();

    idtr--;

    optionError (s.str());
  }
  
  gLpsrOptions->fDisplayLPSR                         = false;

  gLpsrOptions->fDontKeepLineBreaks                  = false;
  gLpsrOptions->fShowAllBarNumbers                   = false;
  gLpsrOptions->fCompressFullBarRests                = false;
  gLpsrOptions->fBreakLinesAtIncompleteRightMeasures = false;
  
  gLpsrOptions->fKeepStaffSize                       = false;
    
  gLpsrOptions->fGenerateAbsoluteOctaves             = false;

  gLpsrOptions->fGenerateNumericalTime               = false;
  gLpsrOptions->fGenerateComments                    = false;
  gLpsrOptions->fGenerateStems                       = false;
  gLpsrOptions->fNoAutoBeaming                       = false;
  gLpsrOptions->fGenerateInputLineNumbers            = false;
  
  gLpsrOptions->fAccidentalStyle                     = "";

  gLpsrOptions->fDelayedOrnamentFractionNumerator    = 2;
  gLpsrOptions->fDelayedOrnamentFractionDenominator  = 3;

  gLpsrOptions->fDontGenerateMidiCommand             = false;
  
  gLpsrOptions->fMidiTempoDuration                   = "4";
  gLpsrOptions->fMidiTempoPerSecond                  = 100;
  
  gLpsrOptions->fGenerateMasterVoices                = true;
  
  gLpsrOptions->fDontGenerateLilyPondLyrics          = false;
  
  gLpsrOptions->fDontGenerateLilyPondCode            = false;

   // General options
  // ---------------

  int helpPresent                       = 0;
  int versionPresent                    = 0;

  int noTracePresent                    = 0;
  
  int outputFilePresent                 = 0;
  int interactivePresent                = 0;
  
  int debugPresent                      = 0;
  int debugDebugPresent                 = 0;
  int forceDebugPresent                 = 0;
  
  int debugMeasuresPresent              = 0;
  int debugdebugMeasuresPresent         = 0;
  
  // MSR options
  // -----------

  int msrPitchesLanguagePresent         = 0;

  int staffRelativeVoiceNumbersPresent  = 0;
  
  int dontDisplayMSRStanzasPresent      = 0;

  int delayRestsDynamicsPresent         = 0;
  
  int displayMSRPresent                 = 0;
  
  int displayMSRSummaryPresent          = 0;
  
  int partNamePresent                   = 0;
  
  // LPSR options
  // ------------

  int lpsrPitchesLanguagePresent        = 0;
  int lpsrChordsLanguagePresent         = 0;
  
  int displayLPSRPresent                = 0;

  int dontKeepLineBreaksPresent         = 0;
  int showAllBarNumbersPresent          = 0;
  int compressFullBarRestsPresent       = 0;
  int breakLinesAtIncompleteRightMeasuresPresent  = 0;
//  int fKeepStaffSizePresent             = 0; JMI
  
  int absolutePresent                   = 0;
  
  int numericaltimePresent              = 0;
  int commentsPresent                   = 0;
  int stemsPresent                      = 0;
  int noAutoBeamingPresent              = 0;
  int noteInputLineNumbersPresent       = 0;
  
  int accidentalStylePresent            = 0;
  
  int delayedOrnamentFractionPresent    = 0;
  
  int midiTempoPresent                  = 0;

  int dontGenerateLilyPondLyricsPresent = 0;

  int dontGenerateLilyPondCodePresent   = 0;


  static struct option long_options [] =
    {
    /* These options set a flag. */

    // General options
    // ---------------

    {
      "h",
      no_argument, &helpPresent, 1
    },
    {
      "help",
      no_argument, &helpPresent, 1
    },
    
    {
      "v",
      no_argument, &versionPresent, 1
    },
    {
      "version",
      no_argument, &versionPresent, 1
    },
    
    {
      "of",
      required_argument, &outputFilePresent, 1
    },
    {
      "outputFile",
      required_argument, &outputFilePresent, 1
    },
    
    {
      "i",
      no_argument, &interactivePresent, 1
    },
    {
      "interactive",
      no_argument, &interactivePresent, 1
    },
    
    {
      "nt",
      no_argument, &noTracePresent, 1
    },
    {
      "noTrace",
      no_argument, &noTracePresent, 1
    },
    
    {
      "d",
      no_argument, &debugPresent, 1
    },
    {
      "debug",
      no_argument, &debugPresent, 1
    },
    {
      "dd",
      no_argument, &debugDebugPresent, 1
    },
    {
      "debugDebug",
      no_argument, &debugDebugPresent, 1
    },
    
    {
      "fd",
      no_argument, &forceDebugPresent, 1
    },
    {
      "forceDebug",
      no_argument, &forceDebugPresent, 1
    },
    
    {
      "dm",
      required_argument, &debugMeasuresPresent, 1
    },
    {
      "debugMeasures",
      required_argument, &debugMeasuresPresent, 1
    },
    {
      "ddm",
      required_argument, &debugdebugMeasuresPresent, 1
    },
    {
      "debugDebugMeasures",
      required_argument, &debugdebugMeasuresPresent, 1
    },

    // MSR options
    // -----------

    {
      "mpl",
      required_argument, &msrPitchesLanguagePresent, 1
    },
    {
      "msrPitchesLanguage",
      required_argument, &msrPitchesLanguagePresent, 1
    },
    
    {
      "srvn",
      no_argument,
      &staffRelativeVoiceNumbersPresent, 1
    },
    {
      "staffRelativeVoiceNumbers",
      no_argument, &staffRelativeVoiceNumbersPresent, 1
    },
    
    {
      "noml",
      no_argument, &dontDisplayMSRStanzasPresent, 1
    },
    {
      "dontDisplayMSRStanzas",
      no_argument, &dontDisplayMSRStanzasPresent, 1
    },

    {
      "drd",
      no_argument, &delayRestsDynamicsPresent, 1
    },
    {
      "delayRestsDynamics",
      no_argument, &delayRestsDynamicsPresent, 1
    },
   
    {
      "msr",
      no_argument, &displayMSRPresent, 1},
    {
      "displayMSR",
      no_argument, &displayMSRPresent, 1
    },

    {
      "sum",
      no_argument, &displayMSRSummaryPresent, 1
    },
    {
      "displayMSRSummary",
      no_argument, &displayMSRSummaryPresent, 1
    },

    {
      "part",
      required_argument, &partNamePresent, 1
    },
    {
      "partName",
      required_argument, &partNamePresent, 1
    },

    // LPSR options
    // ------------

    {
      "lppl",
      required_argument, &lpsrPitchesLanguagePresent, 1
    },
    {
      "lpsrPitchesLanguage",
      required_argument, &lpsrPitchesLanguagePresent, 1
    },
    
    {
      "lpcl",
      required_argument, &lpsrChordsLanguagePresent, 1
    },
    {
      "lpsrChordsLanguage",
      required_argument, &lpsrChordsLanguagePresent, 1
    },
    
    {
      "lpsr",
      no_argument, &displayLPSRPresent, 1},
    {
      "displayLPSR",
      no_argument, &displayLPSRPresent, 1
    },

    {
      "abs",
      no_argument, &absolutePresent, 1
    },
    {
      "absolute",
      no_argument, &absolutePresent, 1
    },
    
    {
      "sabn",
      no_argument, &showAllBarNumbersPresent, 1
    },
    {
      "showAllBarNumbers",
      no_argument, &showAllBarNumbersPresent, 1
    },
    
    {
      "cfbr",
      no_argument, &compressFullBarRestsPresent, 1
    },
    {
      "compressFullBarRests",
      no_argument, &compressFullBarRestsPresent, 1
    },
    
    {
      "blairm",
      no_argument, &breakLinesAtIncompleteRightMeasuresPresent, 1
    },
    {
      "breakLinesAtIncompleteRightMeasures",
      no_argument, &breakLinesAtIncompleteRightMeasuresPresent, 1
    },
    
    {
      "noBreaks",
      no_argument, &dontKeepLineBreaksPresent, 1
    },
    {
      "dontKeepLineBreaks",
      no_argument, &dontKeepLineBreaksPresent, 1
    },
    
    {
      "numericalTime",
      no_argument, &numericaltimePresent, 1
    },
    
    {
      "com",
      no_argument, &commentsPresent, 1
    },
    {
      "comments",
      no_argument, &commentsPresent, 1
    },
    
    {
      "stems",
      no_argument, &stemsPresent, 1
    },
    
    {
      "noab",
      no_argument, &noAutoBeamingPresent, 1
    },
    {
      "noAutoBeaming",
      no_argument, &noAutoBeamingPresent, 1
    },
    
    {
      "iln",
      no_argument, &noteInputLineNumbersPresent, 1
    },
    {
      "generateInputLineNumbers",
      no_argument, &noteInputLineNumbersPresent, 1
    },    

    {
      "as",
      required_argument, &accidentalStylePresent, 1
    },
    {
      "accidentalStyle",
      required_argument, &accidentalStylePresent, 1
    },
    
    {
      "dof",
      required_argument, &delayedOrnamentFractionPresent, 1
    },
    {
      "delayedOrnamentFraction",
      required_argument, &delayedOrnamentFractionPresent, 1
    },
    
    {
      "mt",
      required_argument, &midiTempoPresent, 1
    },
    {
      "midiTempo",
      required_argument, &midiTempoPresent, 1
    },
    
    {
      "nolpc",
      no_argument, &dontGenerateLilyPondCodePresent, 1
    },
    {
      "dontGenerateLilyPondCode",
      no_argument, &dontGenerateLilyPondCodePresent, 1
    },

    {
      "nolpl",
      no_argument, &dontGenerateLilyPondLyricsPresent, 1
    },
    {
      "dontGenerateLilyPondLyrics",
      no_argument, &dontGenerateLilyPondLyricsPresent, 1
    },

    {0, 0, 0, 0}
    };

  /* getopt_long stores the option index here. */
  int option_index = 0;

  int c;
  while (
    (c = getopt_long (
      argc, argv,
      "hab",
      long_options, & option_index ))
      !=
    -1
    )
    {
    /*
    cerr << "c = " << c << endl;
    cerr << "option_index = " << option_index << endl;
    
    cerr <<
      "option : " << long_options[option_index].name <<
      " with flag " << long_options[option_index].flag;
    if (optarg)
      cerr << " and arg " << optarg << "\"";
    cerr << endl;

    //cerr << "debugPresent = " << debugPresent << endl;
    */
    
    switch (c)
      {
      case 0 :

        // General options
        // ------------

        {
        if (helpPresent) {
          printUsage (0);
          break;
        }

        if (versionPresent) {
          idtr++;
          
          cerr <<
            endl <<
            idtr <<
              "This is xml2lilypond version " <<
              MSRVersionNumberAsString () << "," <<
              endl <<
            idtr <<
              "an open translator of MusicXML into LilyPond." <<
              endl <<
              endl <<
            idtr <<
              "See https://github.com/dfober/libmusicxml/tree/lilypond" <<
              endl <<
            idtr <<
              "for more information and access to the source code." <<
              endl <<
            endl;

          idtr--;
          
          exit (0);
          break;
        }

        if (outputFilePresent) {
          outputFileName = optarg;

          stringstream s;

          s <<
            "--outputFile" << " " << outputFileName;
          gGeneralOptions->fCommandLineOptions +=
            s.str();
          outputFilePresent = false;
        }

        if (interactivePresent) {
          gGeneralOptions->fInteractive = false;
          gGeneralOptions->fCommandLineOptions +=
            "--interactive ";
          interactivePresent = false;
        }
        
        if (noTracePresent) {
          gGeneralOptions->fTrace = false;
          gGeneralOptions->fCommandLineOptions +=
            "--noTrace ";
          noTracePresent = false;
        }
        
        if (debugPresent) {
          gGeneralOptions->fTrace = true;
          gGeneralOptions->fDebug = true;
          gGeneralOptions->fCommandLineOptions +=
            "--debug ";
          debugPresent = false;
        }
        if (debugDebugPresent) {
          gGeneralOptions->fTrace = true;
          gGeneralOptions->fDebug = true;
          gGeneralOptions->fDebugDebug = true;
          gGeneralOptions->fCommandLineOptions +=
            "--debugDebug ";
          debugDebugPresent = false;
        }
        if (forceDebugPresent) {
          gGeneralOptions->fTrace = true;
          gGeneralOptions->fForceDebug = true;
          gGeneralOptions->fCommandLineOptions +=
            "--forceDebug ";
          forceDebugPresent = false;
        }
        
        if (debugMeasuresPresent) {
          // optarg contains the measure numbers set specification
          gGeneralOptions->fTrace = true;
          gGeneralOptions->fDebug = true;
          
          char*        measuresSpec = optarg;
          stringstream s;

          s <<
            "--debugMeasures" << " " << measuresSpec << " ";
          gGeneralOptions->fCommandLineOptions +=
            s.str();
            
          gGeneralOptions->fDebugMeasureNumbersSet =
            decipherNumbersSetSpecification (
              measuresSpec, false); // 'true' to debug it
          debugMeasuresPresent = false;
        }
        if (debugdebugMeasuresPresent) {
          gGeneralOptions->fTrace = true;
          gGeneralOptions->fDebugDebug = true;
          
          char*        measuresSpec = optarg;
          stringstream s;

          s <<
            "--debugDebugMeasures" << " " << measuresSpec;
          gGeneralOptions->fCommandLineOptions +=
            s.str();
            
          gGeneralOptions->fDebugMeasureNumbersSet =
            decipherNumbersSetSpecification (
              measuresSpec, false); // 'true' to debug it
          debugdebugMeasuresPresent = false;
        }

        // MSR options
        // -----------
        
        if (msrPitchesLanguagePresent) {
          // optarg contains the language name
          string optargAsString;
          {
            stringstream s;
            s << optarg;
            optargAsString = s.str();
          }
          
          if (! gMsrOptions->setMsrQuartertonesPitchesLanguage (
            optargAsString)) {
            stringstream s;
        
            s <<
              "MSR pitches language " << optargAsString <<
              " is unknown" <<
              endl <<
              "The " <<
              gQuatertonesPitchesLanguagesMap.size () <<
              " known MSR pitches languages are:" <<
              endl;
        
            idtr++;
          
            s <<
              existingQuartertonesPitchesLanguages ();
        
            idtr--;
        
            optionError (s.str());
          }
          
          gGeneralOptions->fCommandLineOptions +=
            "--msrPitchesLanguage " +
            optargAsString +
            " ";
          msrPitchesLanguagePresent = false;
          }
             
        if (staffRelativeVoiceNumbersPresent) {
          gMsrOptions->fCreateStaffRelativeVoiceNumbers = true;
          gGeneralOptions->fCommandLineOptions +=
            "--staffRelativeVoiceNumbers ";
          staffRelativeVoiceNumbersPresent = false;
        }
        
        if (dontDisplayMSRStanzasPresent) {
          gMsrOptions->fDontDisplayMSRStanzas = true;
          gGeneralOptions->fCommandLineOptions +=
            "--dontGenerateMSRStanzas ";
          dontDisplayMSRStanzasPresent = false;
        }
        
        if (delayRestsDynamicsPresent) {
          gMsrOptions->fDelayRestsDynamics = true;
          gGeneralOptions->fCommandLineOptions +=
            "--delayRestsDynamics ";
          delayRestsDynamicsPresent = false;
        }
        
        if (displayMSRPresent) {
          gMsrOptions->fDisplayMSR = true;
          gGeneralOptions->fCommandLineOptions +=
            "--displayMSR ";
          displayMSRPresent = false;
        }

        if (displayMSRSummaryPresent) {
          gMsrOptions->fDisplayMSRSummary = true;
          gLpsrOptions->fDontGenerateLilyPondCode = true;
          gGeneralOptions->fCommandLineOptions +=
            "--displayScoreSummary ";
          displayMSRSummaryPresent = false;
        }
        
        if (partNamePresent) {
          // optarg contains the part name
          char*        partNameSpec = optarg;
          stringstream s;

          s <<
            "--partName" << " \"" << partNameSpec << "\" ";

          gGeneralOptions->fCommandLineOptions +=
            s.str();

          std::pair<string, string>
            pair =
              extractNamesPairFromString (
                partNameSpec,
                '=',
                false); // 'true' to debug it
          /*
          cerr <<
            "--> pair.first = \"" << pair.first << "\", " <<
            "--> pair.second = \"" << pair.second << "\"" <<
            endl;
          */

          // is this part name in the part renaming map?
          map<string, string>::iterator
            it =
              gMsrOptions->fPartsRenaming.find (pair.first);
                
          if (it != gMsrOptions->fPartsRenaming.end ()) {
            // yes, issue error message
            stringstream s;

            s <<
              "Part \"" << pair.first << "\" occurs more that one" <<
              "in the '--partName' option";
              
            optionError (s.str());
          }
          else
            gMsrOptions->fPartsRenaming [pair.first] =
              pair.second;

          partNamePresent = false;
        }
        
        // LPSR options
        // ------------

        if (lpsrPitchesLanguagePresent) {
          // optarg contains the language name
          string optargAsString;
          {
            stringstream s;
            s << optarg;
            optargAsString = s.str();
          }
          
          if (! gLpsrOptions->setLpsrQuartertonesPitchesLanguage (
            optargAsString)) {
            stringstream s;

            s <<
              "LPSR pitches language name '" << optargAsString <<
              "' is unknown" <<
              endl <<
              "The " <<
              gQuatertonesPitchesLanguagesMap.size () <<
              " known LPSR pitches languages are:" <<
              endl;
        
            idtr++;
          
            s <<
              existingQuartertonesPitchesLanguages ();
        
            idtr--;
        
            optionError (s.str());
          }
          
          gGeneralOptions->fCommandLineOptions +=
            "--lpsrPitchesLanguage " +
            optargAsString +
            " ";
          lpsrPitchesLanguagePresent = false;
          }
             
        if (lpsrChordsLanguagePresent) {
          // optarg contains the language name
          string optargAsString;
          {
            stringstream s;
            s << optarg;
            optargAsString = s.str();
          }
          
          if (! gLpsrOptions->setLpsrChordsLanguage (
            optargAsString)) {
            stringstream s;

            s <<
              "LPSR chords language name '" << optargAsString <<
              "' is unknown" <<
              endl <<
              "The " <<
              gQuatertonesPitchesLanguagesMap.size () <<
              " known LPSR chords languages are:" <<
              endl;
        
            idtr++;
          
            s <<
              existingLpsrChordsLanguages ();
        
            idtr--;
              
            optionError (s.str());
          }
          
          gGeneralOptions->fCommandLineOptions +=
            "--lpsrChordsLanguage " +
            optargAsString +
            " ";
          lpsrChordsLanguagePresent = false;
          }
             
        if (displayLPSRPresent) {
          gLpsrOptions->fDisplayLPSR = true;
          gGeneralOptions->fCommandLineOptions +=
            "--displayLPSR ";
          displayLPSRPresent = false;
        }

        if (absolutePresent) {
          gLpsrOptions->fGenerateAbsoluteOctaves = true;
          gGeneralOptions->fCommandLineOptions +=
            "--absolute ";
          absolutePresent = false;
        }

        if (showAllBarNumbersPresent) {
          gLpsrOptions->fShowAllBarNumbers = true;
          gGeneralOptions->fCommandLineOptions +=
            "--showAllBarNumbers ";
          showAllBarNumbersPresent = false;
        }

        if (compressFullBarRestsPresent) {
          gLpsrOptions->fCompressFullBarRests = true;
          gGeneralOptions->fCommandLineOptions +=
            "--compressFullBarRests ";
          compressFullBarRestsPresent = false;
        }

        if (breakLinesAtIncompleteRightMeasuresPresent) {
          gLpsrOptions->fBreakLinesAtIncompleteRightMeasures = true;
          gGeneralOptions->fCommandLineOptions +=
            "--breakLinesAtIncompleteRightMeasures ";
          breakLinesAtIncompleteRightMeasuresPresent = false;
        }

        if (dontKeepLineBreaksPresent) {
          gLpsrOptions->fDontKeepLineBreaks = true;
          gGeneralOptions->fCommandLineOptions +=
            "--dontKeepLineBreaks ";
          dontKeepLineBreaksPresent = false;
        }

        if (numericaltimePresent) {
          gLpsrOptions->fGenerateNumericalTime = true;
          gGeneralOptions->fCommandLineOptions +=
            "--numericalTime ";
          numericaltimePresent = false;
        }
        if (commentsPresent) {
          gLpsrOptions->fGenerateComments = true;
          gGeneralOptions->fCommandLineOptions +=
            "--comments ";
          commentsPresent = false;
        }
        if (stemsPresent) {
          gLpsrOptions->fGenerateStems = true;
          gGeneralOptions->fCommandLineOptions +=
            "--stems ";
          stemsPresent = false;
        }
        if (noAutoBeamingPresent) {
          gLpsrOptions->fNoAutoBeaming = true;
          gGeneralOptions->fCommandLineOptions +=
            "--noAutoBeaming ";
          noAutoBeamingPresent = false;
        }
        if (noteInputLineNumbersPresent) {
          gLpsrOptions->fGenerateInputLineNumbers = true;
          gGeneralOptions->fCommandLineOptions +=
            "--generateInputLineNumbers ";
          noteInputLineNumbersPresent = false;
        }
        
        if (accidentalStylePresent) {
          // optarg contains the accidental style name
          stringstream s;

          if (! gLpsrOptions->setAccidentalStyle (optarg)) {
            s <<
              "--accidentalStyle argument '" << optarg <<
              "' is not known";
              
            optionError (s.str());
          }

          s <<
            "--accidentalStyle " << optarg << " ";
          
          gGeneralOptions->fCommandLineOptions +=
            s.str();
          accidentalStylePresent = false;
        }

        if (delayedOrnamentFractionPresent) {
          // optarg contains the midi tempo specification
          // decipher it to extract numerator and denominator values
          string optargAsString;
          {
            stringstream s;
            s << optarg;
            optargAsString = s.str();
          }
          
          string regularExpression (
            "[[:space:]]*([[:digit:]]+)[[:space:]]*"
            "/"
            "[[:space:]]*([[:digit:]]+)[[:space:]]*");
            
          regex  e (regularExpression);
          smatch sm;

          regex_match (optargAsString, sm, e);

          /*
          cout <<
            "There are " << sm.size() << " matches" <<
            " for string '" << optargAsString <<
            "' with regex '" << regularExpression <<
            "'" <<
            endl;
          */
        
          if (sm.size ()) {
            for (unsigned i = 0; i < sm.size (); ++i) {
              cout << "[" << sm [i] << "] ";
            } // for
            cout << endl;
          }
          
          else {
            stringstream s;

            s <<
              "--delayedOrnamentFraction argument '" << optarg <<
              "' is ill-formed";
              
            optionError (s.str());
          }
          
          {
            stringstream s;
            s << sm [1];
            s >> gLpsrOptions->fDelayedOrnamentFractionNumerator;
          }
          {
            stringstream s;
            s << sm [2];
            s >> gLpsrOptions->fDelayedOrnamentFractionDenominator;
          }
          /*
          cerr <<
            "gLpsrOptions->fDelayedOrnamentFractionNumerator = " <<
            gLpsrOptions->fDelayedOrnamentFractionNumerator <<
            endl <<
            "gLpsrOptions->fDelayedOrnamentFractionDenominator = " <<
            gLpsrOptions->fDelayedOrnamentFractionDenominator <<
            endl;
          */

          stringstream s;

          s <<
            "--delayedOrnamentFraction '" << optargAsString << "' ";
          
          gGeneralOptions->fCommandLineOptions +=
            s.str();
          midiTempoPresent = false;
        }

        if (midiTempoPresent) {
          // optarg contains the midi tempo specification
          // decipher it to extract duration and perSecond values
          string optargAsString;
          {
            stringstream s;
            s << optarg;
            optargAsString = s.str();
          }
          
          string regularExpression (
            "[[:space:]]*([[:digit:]]+\\.*)[[:space:]]*"
            "="
            "[[:space:]]*([[:digit:]]+)[[:space:]]*");
            
          regex  e (regularExpression);
          smatch sm;

          regex_match (optargAsString, sm, e);

          /*
          cout <<
            "There are " << sm.size() << " matches" <<
            " for string '" << optargAsString <<
            "' with regex '" << regularExpression <<
            "'" <<
            endl;
          */
        
          if (sm.size ()) {
            for (unsigned i = 0; i < sm.size (); ++i) {
              cout << "[" << sm [i] << "] ";
            } // for
            cout << endl;
          }
          
          else {
            stringstream s;

            s <<
              "--midiTempo argument '" << optarg <<
              "' is ill-formed";
              
            optionError (s.str());
          }
          
          gLpsrOptions->fMidiTempoDuration =  sm [1];

          {
            stringstream s;
            s << sm [2];
            s >> gLpsrOptions->fMidiTempoPerSecond;
          }
          /*
          cerr <<
            "gLpsrOptions->fMidiTempoDuration = " <<
            gLpsrOptions->fMidiTempoDuration <<
            endl <<
            "gLpsrOptions->fMidiTempoPerSecond = " <<
            gLpsrOptions->fMidiTempoPerSecond <<
            endl;
          */

          stringstream s;

          s <<
            "--midiTempo '" << optargAsString << "' ";
          
          gGeneralOptions->fCommandLineOptions +=
            s.str();
          midiTempoPresent = false;
        }
        
        if (dontGenerateLilyPondLyricsPresent) {
          gLpsrOptions->fDontGenerateLilyPondLyrics = true;
          gGeneralOptions->fCommandLineOptions +=
            "--dontGenerateLilyPondLyrics ";
          dontGenerateLilyPondLyricsPresent = false;
        }
        
        if (dontGenerateLilyPondCodePresent) {
          gLpsrOptions->fDontGenerateLilyPondCode = true;
          gGeneralOptions->fCommandLineOptions +=
            "--dontGenerateLilyPondCode ";
          dontGenerateLilyPondCodePresent = false;
        }

        }
        break;
        
      default:
        printUsage (1);
        break;
      } // switch
    } // while
 
  int nonOptionArgsNumber = argc-optind;

  switch (nonOptionArgsNumber)
    {
    case 1 :
      inputFileName = argv [optind];
      break;

    default:
      printUsage (1);
      break;
    } //  switch
}

//_______________________________________________________________________________
void printOptions ()
{
  if (gGeneralOptions->fTrace)
    cerr << idtr <<
      "The command line options and arguments have been analyzed" <<
      endl;

  idtr++;
  
  cerr << idtr <<
    "The command line is:" <<
    endl;

  idtr++;

  cerr <<
    idtr <<
      gGeneralOptions->fProgramName << " " <<
      gGeneralOptions->fCommandLineOptions <<
      gGeneralOptions->fInputSourceName <<
      endl;

  idtr--;

  // the option name field width
  int const fieldWidth = 31;
  
  // General options
  // ---------------

  cerr << idtr <<
    "The general options are:" <<
    endl;

  idtr++;

  cerr << left <<
    idtr <<
      setw(fieldWidth) << "input source name" << " : " <<
      gGeneralOptions->fInputSourceName <<
      endl <<
      
    idtr <<
      setw(fieldWidth) << "translation date" << " : " <<
      gGeneralOptions->fTranslationDate <<
      endl <<
      
    idtr <<
      setw(fieldWidth) << "interactive" << " : " <<
      booleanAsString (gGeneralOptions->fInteractive) <<
      endl <<
        
    idtr <<
      setw(fieldWidth) << "trace" << " : " <<
      booleanAsString (gGeneralOptions->fTrace) <<
      endl <<
        
    idtr <<
      setw(fieldWidth) << "debug" << " : " <<
      booleanAsString (gGeneralOptions->fDebug) <<
      endl <<
    idtr <<
      setw(fieldWidth) << "debugDebug" << " : " <<
      booleanAsString (gGeneralOptions->fDebugDebug) <<
      endl <<
    idtr <<
      setw(fieldWidth) << "forceDebug" << " : " <<
      booleanAsString (gGeneralOptions->fForceDebug) <<
      endl <<
    idtr <<
      setw(fieldWidth) << "debugMeasureNumbersSet" << " : ";

  if (gGeneralOptions->fDebugMeasureNumbersSet.empty ())
    cerr << "none";
  else
    for (
      set<int>::const_iterator i =
        gGeneralOptions->fDebugMeasureNumbersSet.begin();
      i != gGeneralOptions->fDebugMeasureNumbersSet.end();
      i++) {
        cerr << (*i) << " ";
    } // for
  
  cerr << endl;

  idtr--;

  // MSR options
  // -----------

  cerr << idtr <<
    "The MSR options are:" <<
    endl;

  idtr++;
  
  cerr << left <<
    idtr << setw(fieldWidth) << "msrPitchesLanguage" << " : \"" <<
      msrQuatertonesPitchesLanguageAsString (
        gMsrOptions->fMsrQuatertonesPitchesLanguage) <<
        "\"" <<
        endl <<
    
    idtr << setw(fieldWidth) << "createStaffRelativeVoiceNumbers" << " : " <<
      booleanAsString (gMsrOptions->fCreateStaffRelativeVoiceNumbers) <<
      endl <<

    idtr << setw(fieldWidth) << "dontDisplayMSRStanzas" << " : " <<
      booleanAsString (gMsrOptions->fDontDisplayMSRStanzas) <<
      endl <<

    idtr << setw(fieldWidth) << "delayRestsDynamics" << " : " <<
      booleanAsString (gMsrOptions->fDelayRestsDynamics) <<
      endl <<

    idtr << setw(fieldWidth) << "displayMSR" << " : " <<
      booleanAsString (gMsrOptions->fDisplayMSR) <<
      endl <<
    
    idtr << setw(fieldWidth) << "displayMSRSummary" << " : " <<
      booleanAsString (gMsrOptions->fDisplayMSRSummary) <<
      endl <<
    
    idtr << setw(fieldWidth) << "partRenamingSpecifications" << " : ";
    
  if (gMsrOptions->fPartsRenaming.empty ())
    cerr << "none";
  else
    for (
      map<string, string>::const_iterator i =
        gMsrOptions->fPartsRenaming.begin();
      i != gMsrOptions->fPartsRenaming.end();
      i++) {
        cerr << "\"" << ((*i).first) << " = " << ((*i).second) << "\" ";
    } // for
  
  cerr << endl;

  idtr--;

  // LPSR options
  // ------------

  cerr << idtr <<
    "The LPSR options are:" <<
    endl;

  idtr++;
  
  cerr << left <<
    idtr << setw(fieldWidth) << "lpsrPitchesLanguage" << " : \"" <<
      msrQuatertonesPitchesLanguageAsString (
        gLpsrOptions->fLpsrQuatertonesPitchesLanguage) <<
        "\"" <<
        endl <<
    idtr << setw(fieldWidth) << "lpsrChordsLanguage" << " : \"" <<
      lpsrChordsLanguageAsString (
        gLpsrOptions->fLpsrChordsLanguage) <<
        "\"" <<
        endl <<

    idtr << setw(fieldWidth) << "displayLPSR" << " : " <<
      booleanAsString (gLpsrOptions->fDisplayLPSR) <<
      endl <<

    idtr << setw(fieldWidth) << "generateAbsoluteOctaves" << " : " <<
      booleanAsString (gLpsrOptions->fGenerateAbsoluteOctaves) <<
      endl <<
    
    idtr << setw(fieldWidth) << "dontKeepLineBreaks" << " : " <<
      booleanAsString (gLpsrOptions->fDontKeepLineBreaks
        ? "true" : "false") << endl <<
    
    idtr << setw(fieldWidth) << "generateNumericalTime" << " : " <<
      booleanAsString (gLpsrOptions->fGenerateNumericalTime) <<
      endl <<
    idtr << setw(fieldWidth) << "generateComments" << " : " <<
      booleanAsString (gLpsrOptions->fGenerateComments) <<
      endl <<
    idtr << setw(fieldWidth) << "generateStems" << " : " <<
      booleanAsString (gLpsrOptions->fGenerateStems) <<
      endl <<
    idtr << setw(fieldWidth) << "noAutoBeaming" << " : " <<
      booleanAsString (gLpsrOptions->fNoAutoBeaming) <<
      endl <<
     idtr << setw(fieldWidth) << "generateInputLineNumbers" << " : " <<
      booleanAsString (gLpsrOptions->fGenerateInputLineNumbers) <<
      endl <<

    idtr << setw(fieldWidth) << "dontGenerateLilyPondLyrics" << " : " <<
      booleanAsString (gLpsrOptions->fDontGenerateLilyPondLyrics) <<
      endl <<

    idtr << setw(fieldWidth) << "dontGenerateLilyPondCode" << " : " <<
      booleanAsString (gLpsrOptions->fDontGenerateLilyPondCode) <<
      endl;

  idtr--;

  idtr--;
}

//_______________________________________________________________________________
int main (int argc, char *argv[]) 
{
 /*
  cerr << "argc = " << argc << endl;
  for (int i = 0; i < argc ; i++ ) {
    cerr << "argv[ " << i << "] = " << argv[i] << endl;
  }
  */

  // analyze the pitches and chords languages variables
  // ------------------------------------------------------

  initializePitchesLanguages ();
  initializeLpsrChordsLanguages ();
  
  // analyze the command line options
  // ------------------------------------------------------

  gGeneralOptions = msrGeneralOptions::create ();
  assert(gGeneralOptions != 0);

  gMsrOptions = msrOptions::create ();
  assert(gMsrOptions != 0);

  gLpsrOptions = lpsrOptions::create();
  assert(gLpsrOptions != 0);
  
  string    inputFileName;
  string    outputFileName;
  
  analyzeOptions (
    argc, argv,
    inputFileName, outputFileName);

  // program name
  // ------------------------------------------------------
  
  gGeneralOptions->fProgramName = argv [0];
  
  // input source name
  // ------------------------------------------------------

  gGeneralOptions->fInputSourceName = inputFileName;
  
  // translation date
  // ------------------------------------------------------

  time_t      translationRawtime;
  struct tm*  translationTimeinfo;
  char buffer [80];

  time (&translationRawtime);
  translationTimeinfo = localtime (&translationRawtime);

  strftime (buffer, 80, "%A %F @ %T %Z", translationTimeinfo);
  gGeneralOptions->fTranslationDate = buffer;
  
  // trace
  // ------------------------------------------------------

  if (gGeneralOptions->fTrace) {
    cerr <<  idtr <<
      "This is xml2Lilypond v" << MSRVersionNumberAsString () << 
      " from libmusicxml2 v" << musicxmllibVersionStr () <<
      endl;

    cerr <<  idtr <<
      "Launching conversion of ";

    if (gGeneralOptions->fInputSourceName == "-")
      cerr << "standard input";
    else
      cerr << "\"" << gGeneralOptions->fInputSourceName << "\"";

    cerr <<
      " to LilyPond" <<
      endl;

    cerr << idtr <<
      "LilyPond code will be written to ";
    if (outputFileName.size())
      cerr << outputFileName;
    else
      cerr << "standard output";
    cerr << endl;
    
    printOptions ();
  }

  // open output file if need be
  // ------------------------------------------------------

  ofstream outStream;

  if (outputFileName.size()) {
    if (gGeneralOptions->fDebug)
      cerr << idtr <<
        "Opening file '" << outputFileName << "' for writing" <<
        endl;
        
    outStream.open (outputFileName.c_str(), ofstream::out);
  }
      
  // create MSR from MusicXML contents
  // ------------------------------------------------------

  S_msrScore mScore;

  if (inputFileName == "-") {
    // input comes from standard input
    if (outputFileName.size())
      mScore =
        musicxmlFd2Msr (stdin, gMsrOptions, outStream);
    else
      mScore =
        musicxmlFd2Msr (stdin, gMsrOptions, outStream);
  }
  
  else {
    // input comes from a file
    if (outputFileName.size()) // ??? JMI
      mScore =
        musicxmlFile2Msr (
          inputFileName.c_str(), gMsrOptions, outStream);
    else
      mScore =
        musicxmlFile2Msr (
          inputFileName.c_str(), gMsrOptions, outStream);
  }
    
  if (! mScore) {
    cerr <<
      "### Conversion from MusicCML to MSR failed ###" <<
      endl <<
      endl;
    return 1;
  }

  // create LPSR from MSR
  // ------------------------------------------------------

  S_lpsrScore lpScore;
        
  if (! gLpsrOptions->fDontGenerateLilyPondCode) {
    if (outputFileName.size()) // ??? JMI
      lpScore =
        msr2Lpsr (mScore, gMsrOptions, gLpsrOptions, outStream);
    else
      lpScore =
        msr2Lpsr (mScore, gMsrOptions, gLpsrOptions, outStream);
    
    if (! lpScore) {
      cerr <<
        "### Conversion from MSR to LPSR failed ###" <<
        endl <<
        endl;
      return 1;
    }
  }

  // generate LilyPond code from LPSR
  // ------------------------------------------------------

  if (! gLpsrOptions->fDontGenerateLilyPondCode) {
    if (outputFileName.size()) // ??? JMI
      lpsr2LilyPond (lpScore, gMsrOptions, gLpsrOptions, cout);
    else
      lpsr2LilyPond (lpScore, gMsrOptions, gLpsrOptions, cout);
    
    if (outputFileName.size()) {
      if (gGeneralOptions->fDebug)
        cerr << idtr <<
          "Closing file '" << outputFileName << "'" <<
          endl;
          
      outStream.close ();
    }
  }

  // print timing information
  // ------------------------------------------------------

  timing::gTiming.print (cerr);
  

  if (! true) { // JMI
    cerr <<
      "### Conversion from LPSR to LilyPond code failed ###" <<
      endl <<
      endl;
    return 1;
  }

  return 0;
}