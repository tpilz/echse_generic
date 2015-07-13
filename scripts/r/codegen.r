
################################################################################
# NOTE: You probably DON'T want to run this script through R! Please run the
#       OS-specific calling script to be found in the parent folder instead.
################################################################################

# Check env. vars
enginesDir=Sys.getenv("ECHSE_ENGINES")
if (enginesDir == "")
  stop("Environment variable 'ECHSE_ENGINES' not set.")

# Get name of engine for which code is to be generated
engine= commandArgs(trailingOnly=TRUE)
if (length(engine) != 1)
  stop("Expecting name of model engine as input.")

# Assemble standard directory/file names
FILE_ENGINEDEF= paste(enginesDir,"/def/",engine,".txt",sep="")
DIR_CLASSDECL= paste(enginesDir,"/classes/declaration",sep="")
DIR_GENERATED= paste(enginesDir,"/generated/",engine,sep="")

# Read names of classes to be used in this engine
if (!file.exists(FILE_ENGINEDEF))
  stop(paste("Cannot determine classes to be used in engine '",engine,
  "'. Expected to find class names in file '",FILE_ENGINEDEF,"'.",sep=""))
classes= read.table(file=FILE_ENGINEDEF, header=FALSE)
if ((nrow(classes) == 0) || (ncol(classes) != 1))
  stop(paste("Cannot determine classes to be used in engine '",engine,
  "'. from file '",FILE_ENGINEDEF,"'. Expected a one-column table with a",
  " single class name in each row.",sep=""))
classes= as.character(classes[,1])

# Prepare vector of class declaration files (element names == class names)
files= paste(DIR_CLASSDECL,"/",classes,".txt",sep="")
names(files)= classes
if (!all(file.exists(files)))
  stop(paste("Missing declaration file(s) corresponding to class(es) listed in '",
  FILE_ENGINEDEF,"'. Expecting the declaration file(s) to reside in folder '",
  DIR_CLASSDECL,"'.",sep=""))

# Check output directory for existence
if (!file_test("-d",DIR_GENERATED)) {
  if (!dir.create(path=DIR_GENERATED, showWarnings=FALSE, recursive=FALSE))
    stop(paste("Directory for generated code corresponding to engine '",engine,
      "' does not exist at expected path '",DIR_GENERATED,"'. Automatic",
      " creation of this directory failed as well.",sep=""))
}

# Check required package
if (!library(codegen, logical.return=TRUE, quietly=TRUE))
  stop(paste("Failed to load package 'codegen'. You need to install this",
    " package in order to generate code for ECHSE-based model engines.",sep=""))
# Code generation
generate(files=files, outdir=DIR_GENERATED, overwrite=TRUE)

