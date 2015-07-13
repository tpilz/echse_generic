#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>    // for time types and functions

#include "omp.h"

// Utility code from dklib
#include "except/except.h"
#include "cmdline/cmdline.h"
#include "typeconv/typeconv.h"
#include "logfile/logfile.h"
#include "table/table.h"
#include "settings/settings.h"
#include "fixedZoneTime/fixedZoneTime.h"

// Generated code (bundled in a single include file)
#include "AUTOechse_includeFiles.h"

// Options (preprocessor directives)
#include "echse_options.h"

// Global constants
#include "echse_globalConst.h"

// Core classes
#include "echse_coreClass_abstractObject.h"
#include "echse_coreClass_abstractObjectGroup.h"
#include "echse_coreClass_templateObjectGroup.h"
#include "echse_coreClass_spaceTimeDataCollection.h"
#include "echse_coreClass_multiState.h"

// Core functions
#include "echse_coreFunct_instantiateObjects.h"
#include "echse_coreFunct_saveState.h" 
#include "echse_coreFunct_setObjectLevels.h"
#include "echse_coreFunct_util.h"

using namespace std;

int main (const int argc, const char* argv[]) {

  // Main variables
  string file_control, file_log, file_err;
  string format_err;
  string input_colsep, output_colsep;
  string input_commentchar, output_commentchar;
  string outdir, outfmt;

  vector<abstractObjectGroup*> objectGroups;
  vector<abstractObject*> objects;
  spaceTimeDataCollection externalInputs;
  vector<fixedZoneTime> times_stateOutput;

  unsigned int number_of_threads, singlethread_if_less_than;

  bool silent=false;
  bool trap_fpe;

  bool saveFinalState;

  // Vector controlling the order of processing
  // Outer vector: Levels
  // Inner vectors: Indices of the objects of a particular level
  vector< vector<unsigned int> > processingTree;

  struct t_timeInfo {
    // User input
    fixedZoneTime simStart, simEnd;
    unsigned int delta_t;   
    // Internal
    fixedZoneTime stepStart;
    int stepCounter, numberOfSteps;
    string stepEnd_asString;
  } simtime;
  struct t_comptime {
    time_t appStart, appEnd;
    int appSeconds;
    time_t ini, now;
    int seconds_remain;
    unsigned int progressStepIndex;
  } comptime;


 
  //////////////////////////////////////////////////////////////////////////////
  // Start of program
  //////////////////////////////////////////////////////////////////////////////

  comptime.appStart= time(0);

  logfile lg;
  try {

    ////////////////////////////////////////////////////////////////////////////
    // Get command line infos
    cmdline cmdargs(argc, argv);
    try {
      file_control= cmdargs.get_value("file_control", "=");
      file_log= cmdargs.get_value("file_log", "=");
      file_err= cmdargs.get_value("file_err", "=");
      format_err= cmdargs.get_value("format_err", "=");
      silent= as_logical(cmdargs.get_value("silent", "="));
    } catch (except) {
      except e(__PRETTY_FUNCTION__,"Failed to retrieve info from command line.",
        __FILE__,__LINE__);
      throw(e);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Open logfile
    try {
      lg.init(file_log, false);
    } catch (except) {
      except e(__PRETTY_FUNCTION__,"Cannot open log file.",__FILE__,__LINE__);
      throw(e);
    }
    lg.add(silent, "Logging started.");

    ////////////////////////////////////////////////////////////////////////////
    // Report important preprocessor options
    #ifndef CHECK_RANGE
    #  error "Macro CHECK_RANGE is undefined."
    #endif
    #if CHECK_RANGE 
      lg.add(silent, "Range checks in data access functions: ACTIVE");
    #else 
      lg.add(silent, "Range checks in data access functions: DISABLED");
    #endif

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Reading configuration data");
    settings control;
    try {
      control.read_fileAndArgs(file_control, "=", "#",cmdargs,"=");
    } catch (except) {
      stringstream errmsg;
      errmsg << "Failed to read the configuration data.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Querying frequently used control data");
    try {
      input_colsep= control["input_columnSeparator"];
      input_commentchar= control["input_lineComment"];
      output_colsep= control["output_columnSeparator"];
      output_commentchar= control["output_lineComment"];
      outfmt= control["outputFormat"];
      outdir= control["outputDirectory"];
      saveFinalState= as_logical(control["saveFinalState"]);
      number_of_threads= max(as_unsigned_integer(1), as_unsigned_integer(control["number_of_threads"]));
      singlethread_if_less_than= max(as_unsigned_integer(0), as_unsigned_integer(control["singlethread_if_less_than"]));
      trap_fpe= as_logical(control["trap_fpe"]); 
    } catch (except) {
      stringstream errmsg;
      errmsg << "Missing or bad setting(s) in control file '" << file_control << "'.";
      except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
      throw(e);
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Setting number of threads");
    number_of_threads= min(number_of_threads, as_unsigned_integer(omp_get_max_threads()));
    omp_set_num_threads(number_of_threads);
    lg.add(silent, "Number of threads (if >=" + as_string(singlethread_if_less_than) +
      " parallel objects): " + as_string(number_of_threads));

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Instantiating object groups");
    // Function from generated code!
    instantiateObjectGroups(objectGroups);

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Instantiating objects");
    try {
      instantiateObjects(control["table_objectDeclaration"], input_colsep, input_commentchar,
        objectGroups, objects);
    } catch (except) {
      except e(__PRETTY_FUNCTION__,"Failed to instantiate object(s).",__FILE__,__LINE__);
      throw(e);
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Initializing object outputs");
    try {
      table tab;
	    tab.read(control["table_selectedOutput"], true, input_colsep, input_commentchar);
      for (unsigned int i=0; i<objects.size(); i++) {
        objects[i]->init_outputs(tab);
      }
      tab.clear();
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Failed to initialize object outputs.", __FILE__, __LINE__);
      throw(e);
    }
    lg.add(silent, "Switching on debug mode for selected objects");
    try {
      table tab;
	    tab.read(control["table_debugOutput"], true, input_colsep, input_commentchar);
      for (unsigned int i=0; i<objects.size(); i++) {
        objects[i]->set_debugMode(tab);
      }
      tab.clear();
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Failed to set debug modes.", __FILE__, __LINE__);
      throw(e);
    }
    lg.add(silent, "Setting times of state output");
    try {
      table tab;
      // We expect a one-column table with times in ISO format --> don't use space as colsep
	    tab.read(control["table_stateOutput"], true, "\t", input_commentchar);
      unsigned int colindex;
      try {
        colindex= tab.colindex(globalConst::colNames.time);
      } catch (except) {
        stringstream errmsg;
        errmsg << "Missing column '" << globalConst::colNames.time << "' in table of state output times.";
        except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
        throw(e);
      }
      times_stateOutput.resize(tab.nrow());
      if (tab.nrow() > 0) {
        for (unsigned int i=1; i<=tab.nrow(); i++) {
          times_stateOutput[i-1].set(tab.get_element(i,colindex));
        }
      }
      tab.clear();
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Failed to set times of state output.", __FILE__, __LINE__);
      throw(e);
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Assigning simulated inputs to objects (object linkage)");
    try {
      table tab;
      tab.read(control["table_inputOutputRelations"], true, input_colsep, input_commentchar);
      for (unsigned int i=0; i<objects.size(); i++) {
        objects[i]->assign_inputsSim(tab, objects);
      }
      tab.clear();
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Cannot assign simulated inputs to objects.", __FILE__, __LINE__);
      throw(e);
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Setting object levels");
    try {
      setObjectLevels(objects);
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Cannot set object levels.", __FILE__, __LINE__);
      throw(e);
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Setting up processing tree for selected objects");
    // Allocate outer vector of processing tree (= vector of levels)
    unsigned int maxLevel= 0;
    unsigned int minLevel= objects.size();
    for (unsigned int i=0; i<objects.size(); i++) {
      if (objects[i]->get_objectLevel() > maxLevel) {
        maxLevel= objects[i]->get_objectLevel();
      }
      if (objects[i]->get_objectLevel() < minLevel) {
        minLevel= objects[i]->get_objectLevel();
      }
    }
    processingTree.resize(maxLevel-minLevel+1);
    // Allocate inner vectors of processing tree (= object indices of a particular level)
    for (unsigned int i=0; i<objects.size(); i++) {
      processingTree[objects[i]->get_objectLevel()-minLevel].push_back(i);
    }
    lg.add(silent, "Levels for (optional) parallel processing: " +
      as_string(processingTree.size()));

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Reading tables of individual parameter functions");
    try {
      string key;
      for (unsigned int k=0; k<objectGroups.size(); k++) {
	try {
	  key= objectGroups[k]->get_idObjectGroup() + "_funParamsIndividual";
	  objectGroups[k]->set_tableParamsFun(control[key], input_colsep, input_commentchar);
	} catch (except) {
	  stringstream errmsg;
	  errmsg << "Failed to read table of individual parameter functions for objects of object group '" <<
	    objectGroups[k]->get_idObjectGroup() << "'.";
	  except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
	  throw(e);
	}
      }
    } catch (except) {
      except e(__PRETTY_FUNCTION__,"Failed to read table(s) of individual parameter functions.",__FILE__,__LINE__);
      throw(e);
    }
    lg.add(silent, "Initializing individual parameter functions");
    try {
      for (unsigned int i=0; i<objects.size(); i++) {
	objects[i]->init_paramsFun(input_colsep, input_commentchar);
      }
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Failed to initialize individual parameter functions.", __FILE__, __LINE__);
      throw(e);
    }
    lg.add(silent, "Clearing tables of individual parameter functions");
    for (unsigned int k=0; k<objectGroups.size(); k++) {
      objectGroups[k]->clear_tableParamsFun();
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Reading tables of individual scalar parameters");
    try {
      string key;
      for (unsigned int k=0; k<objectGroups.size(); k++) {
	try {
	  key= objectGroups[k]->get_idObjectGroup() + "_numParamsIndividual";
	  objectGroups[k]->set_tableParamsNum(control[key], input_colsep, input_commentchar);
	} catch (except) {
	  stringstream errmsg;
	  errmsg << "Failed to read table of individual scalar parameters for objects of object group '" <<
	    objectGroups[k]->get_idObjectGroup() << "'.";
	  except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
	  throw(e);
	}
      }
    } catch (except) {
      except e(__PRETTY_FUNCTION__,"Failed to read table(s) of individual scalar parameters.",__FILE__,__LINE__);
      throw(e);
    }
    lg.add(silent, "Initializing individual scalar parameters");
    try {
      for (unsigned int i=0; i<objects.size(); i++) {
	objects[i]->init_paramsNum();
      }
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Failed to initialize individual scalar parameters.", __FILE__, __LINE__);
      throw(e);
    }
    lg.add(silent, "Clearing tables of individual scalar parameters");
    for (unsigned int k=0; k<objectGroups.size(); k++) {
      objectGroups[k]->clear_tableParamsNum();
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Setting shared parameter functions");
    for (unsigned int k=0; k<objectGroups.size(); k++) {
      try {
	string key= objectGroups[k]->get_idObjectGroup() + "_funParamsShared";
	objectGroups[k]->set_sharedParamsFun(control[key], input_colsep, input_commentchar);
      } catch (except) {
	stringstream errmsg;
	errmsg << "Failed to set shared parameter functions for object group '" <<
	  objectGroups[k]->get_idObjectGroup() << "'.";
	except e(__PRETTY_FUNCTION__,errmsg, __FILE__, __LINE__);
	throw(e);
      }
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Setting shared scalar parameters");
    for (unsigned int k=0; k<objectGroups.size(); k++) {
      try {
	string key= objectGroups[k]->get_idObjectGroup() + "_numParamsShared";
	objectGroups[k]->set_sharedParamsNum(control[key], input_colsep, input_commentchar);
      } catch (except) {
	stringstream errmsg;
	errmsg << "Failed to set shared scalar parameters for object group '" <<
	  objectGroups[k]->get_idObjectGroup() << "'.";
	except e(__PRETTY_FUNCTION__,errmsg, __FILE__, __LINE__);
	throw(e);
      }
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Initializing time series of external variables");
    try {
      unsigned int bufferSize= as_unsigned_integer(control["externalInput_bufferSize"]);
      externalInputs.init(control["table_externalInput_datafiles"],
	input_colsep, input_commentchar, bufferSize);
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Cannot initialize time series of external variables.", __FILE__, __LINE__);
      throw(e);
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Assigning external time series to objects");
    try {
      table tab;
	    tab.read(control["table_externalInput_locations"], true, input_colsep, input_commentchar);
      for (unsigned int i=0; i<objects.size(); i++) {
	objects[i]->assign_inputsExt(tab, externalInputs);
      }
      tab.clear();
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Cannot assign external time series to objects.", __FILE__, __LINE__);
      throw(e);
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Setting initial values of scalar state variables");
    try {
      table tab;
	    tab.read(control["table_initialValues_scal"], true, input_colsep, input_commentchar);
      for (unsigned int i=0; i<objects.size(); i++) {
	objects[i]->init_statesScal(tab);
      }
      tab.clear();
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Cannot set initial values of scalar state variables.", __FILE__, __LINE__);
      throw(e);
    }
    lg.add(silent, "Setting initial values of vector state variables");
    try {
      table tab;
	    tab.read(control["table_initialValues_vect"], true, input_colsep, input_commentchar);
      for (unsigned int i=0; i<objects.size(); i++) {
	objects[i]->init_statesVect(tab);
      }
      tab.clear();
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Cannot set initial values of vector state variables.", __FILE__, __LINE__);
      throw(e);
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Initializing simulation time");
    try {
      // User input
      simtime.simStart.set(control["simStart"]);
      simtime.simEnd.set(control["simEnd"]);
      simtime.delta_t= as_unsigned_integer(control["delta_t"]);
    } catch (except) {
      except e(__PRETTY_FUNCTION__, "Cannot initialize simulation time.", __FILE__, __LINE__);
      throw(e);
    }
    if (simtime.simStart >= simtime.simEnd) {
      except e(__PRETTY_FUNCTION__, "Badly defined start/end time of simulation.", __FILE__, __LINE__);
      throw(e);
    }
    if (simtime.delta_t <= 0) {
      except e(__PRETTY_FUNCTION__, "Time step must be a positive number of seconds.", __FILE__, __LINE__);
      throw(e);
    }

    ////////////////////////////////////////////////////////////////////////////
    lg.add(silent, "Simulation started");

    // Start time loop
    simtime.numberOfSteps= ceil((simtime.simEnd.get() - simtime.simStart.get()) / simtime.delta_t);
    comptime.ini= time(0);
    comptime.seconds_remain= 0;
    for (simtime.stepCounter= 1; simtime.stepCounter <= simtime.numberOfSteps; simtime.stepCounter++) {
      simtime.stepStart= simtime.simStart + simtime.delta_t * (simtime.stepCounter -1);

      // Print state
      comptime.progressStepIndex= progressIndex(simtime.stepCounter, simtime.numberOfSteps, globalConst::progressSteps);
      if (comptime.progressStepIndex < globalConst::progressSteps.size()) {   
	stringstream msg;
	msg << setw(2) << setfill('0') << globalConst::progressSteps[comptime.progressStepIndex] <<
	    "% done.  Computing time remaining: " << floor(comptime.seconds_remain/3600.) << "h " <<
	    setw(2) << setfill('0') << floor((comptime.seconds_remain%3600)/60.) << "m " <<
	    setw(2) << setfill('0') << (comptime.seconds_remain%60) << "s ";
	lg.add(silent, msg.str());
      }

      // Update external input variables
      try {
	externalInputs.update(simtime.stepStart, simtime.stepStart + simtime.delta_t);
      } catch (except) {
	stringstream errmsg;
	errmsg << "Updating of external input variables failed in time step " <<
	  simtime.stepCounter << " of " << simtime.numberOfSteps << " starting at " <<
	  simtime.stepStart.get("-",":"," ") << ".";
	except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
	throw(e);
      }

      // Set time stamp for output
      simtime.stepEnd_asString= (simtime.stepStart + simtime.delta_t).get("-",":"," ");

      ////////////////////////////////////////////////////////////////////////
      // Start loop over objects (spatial loop)
      ////////////////////////////////////////////////////////////////////////

      // Outer loop (loop over levels -- sequential processing)
      for (unsigned int ix_outer=0; ix_outer < processingTree.size(); ix_outer++) {

	unsigned int nExcept= 0;
	#pragma omp parallel for if(processingTree[ix_outer].size() >= singlethread_if_less_than)
	// Inner loop (loop over objects of one level -- may be processed in parallel)
	for (unsigned int ix_inner=0; ix_inner < processingTree[ix_outer].size(); ix_inner++) {
	  // Index of current object
	  unsigned int i=processingTree[ix_outer][ix_inner];
	  // Run current object
	  try {
	    objects[i]->simulate(simtime.delta_t);        
	  } catch (except) {
	    stringstream errmsg;
	    errmsg << "Simulation failed for object '" << objects[i]->get_idObject() <<
	      "' in time step " << simtime.stepCounter << " of " << simtime.numberOfSteps <<
	      " starting at " << simtime.stepStart.get("-",":"," ") << ".";
	    except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
	    nExcept++;
	  }         
	  // Check for floating point exceptions
	  if (trap_fpe) {
	    try {
	      objects[i]->checkFPE();        
	    } catch (except) {
	      stringstream errmsg;
	      errmsg << "Floating point exception occurred in object '" << objects[i]->get_idObject() <<
		"' in time step " << simtime.stepCounter << " of " << simtime.numberOfSteps <<
		" starting at " << simtime.stepStart.get("-",":"," ") << ".";
	      except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
	      nExcept++;
	    }
	  }
	} // End of loop over objects (inner)
	if (nExcept > 0) {
	  stringstream errmsg;
	  errmsg << nExcept << " exceptions registered in time step " << simtime.stepCounter <<
	    " of " << simtime.numberOfSteps << ".";
	  except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
	  throw(e);
	}
      } // End of loop over objects (outer)

      // Output results
      for (unsigned int i=0; i<objects.size(); i++) {
	try {
	  objects[i]->output_selected(simtime.stepCounter==1,
            simtime.stepCounter==simtime.numberOfSteps,
            outdir, outfmt,
	    output_colsep, simtime.stepEnd_asString, simtime.delta_t);
	  objects[i]->output_debug(simtime.stepCounter==1, outdir,
	    output_colsep, simtime.stepEnd_asString);
	} catch (except) {
	  stringstream errmsg;
	  errmsg << "Cannot print output for object '" << objects[i]->get_idObject() <<
	    "' at end of time step " << simtime.stepCounter << " of " << simtime.numberOfSteps <<
	    " starting at " << simtime.stepStart.get("-",":"," ") << ".";
	  except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
	  throw(e);
	}
      }

      // Save state
      try {
	if ((saveFinalState) & (simtime.stepCounter == simtime.numberOfSteps)) {
	  times_stateOutput.clear();
	  times_stateOutput.push_back(simtime.stepStart + simtime.delta_t);
	}
	saveState(outdir, (simtime.stepStart + simtime.delta_t), times_stateOutput,
	  output_colsep, output_commentchar, objects);
      } catch (except) {
	stringstream errmsg;
	errmsg << "Cannot save object state at " << simtime.stepEnd_asString << ".";
	except e(__PRETTY_FUNCTION__, errmsg, __FILE__, __LINE__);
	throw(e);
      }

      // Estimate remaining computing time (for a single run)
      comptime.now= time(0);
      comptime.seconds_remain= ceil(difftime(comptime.now, comptime.ini) /
	simtime.stepCounter * (simtime.numberOfSteps - simtime.stepCounter));

    } // End time loop
    lg.add(silent, "Simulation finished");

    ////////////////////////////////////////////////////////////////////////////
    // Close output files
    lg.add(silent, "Closing output files");
    for (unsigned int i=0; i<objects.size(); i++) {
      objects[i]->closeOutput_selected();
      objects[i]->closeOutput_debug();
    }

    ////////////////////////////////////////////////////////////////////////////
    // Final clean up
    lg.add(silent, "Final clean-up");
    objectGroups.clear();
    objects.clear();
    control.clear();
    externalInputs.clear();

    ////////////////////////////////////////////////////////////////////////////
    // Program termination
    comptime.appEnd= time(0);
    comptime.appSeconds= ceil(difftime(comptime.appEnd, comptime.appStart));
    stringstream msg;
    msg << "Finished successfully after " <<
      setw(2) << setfill('0') << floor(comptime.appSeconds/3600.) << "h " <<
      setw(2) << setfill('0') << floor((comptime.appSeconds%3600)/60.) << "m " <<
      setw(2) << setfill('0') << (comptime.appSeconds%60) << "s " <<
      "(" << comptime.appSeconds << "s)";
    lg.add(silent, msg.str());
    return(0);
  } catch (except) {
    lg.add(silent, "Stopped due to exception. See traceback for details.");
    stringstream errmsg;
    errmsg << "Stopped due to exception. See traceback in '" << file_err << "'.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    e.print(file_err,format_err);
    return(1);
  } catch (exception e) {
    lg.add(silent, "Stopped due to standard/system exception.");
    stringstream errmsg;
    errmsg << "Stopped due to standard/system exception (" << e.what() <<
      "). Traceback not available. Please report this error.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    e.print(file_err,format_err);
    return(1);
	} catch (...) {
    lg.add(silent, "Stopped due to unknown error.");
    stringstream errmsg;
    errmsg << "Stopped due to unknown error. Traceback not available. Please report this error.";
    except e(__PRETTY_FUNCTION__,errmsg,__FILE__,__LINE__);
    e.print(file_err,format_err);
    return(1);
	}
}

