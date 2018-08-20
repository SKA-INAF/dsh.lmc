#!/bin/bash

NARGS="$#"
echo "INFO: NARGS= $NARGS"

if [ "$NARGS" -lt 3 ]; then
	echo "ERROR: Invalid number of arguments...see script usage!"
  echo ""
	echo "**************************"
  echo "***     USAGE          ***"
	echo "**************************"
 	echo "$0 [ARGS]"
	echo ""
	echo "=========================="
	echo "==    ARGUMENT LIST     =="
	echo "=========================="
	echo "*** MANDATORY ARGS ***"
	echo "--db=[DB_NAME] - Name of Tango MySQL HDB DB" 
	echo "--dbuser=[DB_USER] - User name to access Tango MySQL HDB DB"
	echo "--dt=[DT] - Time in minutes wrt now before which archived data are removed from the archive"
	echo ""
	echo "*** OPTIONAL ARGS ***"
	echo "--sqlscript=[SQL_SCRIPT] - Path to sql script to be executed [default=clear_hdbarchive.sql]"
	echo "--sqldump=[SQLDUMPFILE] - Sql dump file [default=hdbdump.sql]"
	echo "--outdir=[OUTDIR] - Output dir where to create the archive dump file [DEFAULT=PWD]"
	echo "=========================="
	exit 1
fi



#######################################
##         PARSE ARGS
#######################################
DB_NAME=""
DB_USER=""
DT=""
SQLSCRIPT="clear_hdbarchive.sql"
SQLDUMPFILE="hdbdump.sql"
OUTDIR="$PWD"

for item in "$@"
do
	case $item in 
		--db=*)
    	DB_NAME=`echo $item | sed 's/[-a-zA-Z0-9]*=//'`
			if [ "$DB_NAME" == "" ]; then
				echo "ERROR: Empty DB_NAME argument given!"
				exit 1
			fi
    ;;
		--dbuser=*)
    	DB_USER=`echo $item | sed 's/[-a-zA-Z0-9]*=//'`
			if [ "$DB_USER" == "" ]; then
				echo "ERROR: Empty DB_USER argument given!"
				exit 1
			fi
    ;;
#		--dbpass=*)
#    	DB_PASS=`echo $item | sed 's/[-a-zA-Z0-9]*=//'`
#			if [ "$DB_PASS" == "" ]; then
#				echo "ERROR: Empty DB_PASS argument given!"
#				exit 1
#			fi
#    ;;
		--dt=*)
    	DT=`echo $item | sed 's/[-a-zA-Z0-9]*=//'`
			if [ "$DT" == "" ]; then
				echo "ERROR: Empty DT argument given!"
				exit 1
			fi
    ;;
		--outdir=*)
    	OUTDIR=`echo $item | sed 's/[-a-zA-Z0-9]*=//'`
			if [ "$OUTDIR" == "" ]; then
				echo "ERROR: Empty OUTDIR argument given!"
				exit 1
			fi
    ;;
		--sqldump=*)
    	SQLDUMPFILE=`echo $item | sed 's/[-a-zA-Z0-9]*=//'`
			if [ "$SQLDUMPFILE" == "" ]; then
				echo "ERROR: Empty SQLDUMPFILE argument given!"
				exit 1
			fi
    ;;
		--sqlscript=*)
    	SQLSCRIPT=`echo $item | sed 's/[-a-zA-Z0-9]*=//'`
			if [ "$SQLSCRIPT" == "" ]; then
				echo "ERROR: Empty SQLSCRIPT argument given!"
				exit 1
			fi
    ;;

    *)
    # Unknown option
    echo "ERROR: Unknown option ($item)...exit!"
    exit 1
    ;;
	esac
done



echo ""
echo "*****  PARSED ARGUMENTS ****"
echo "DB_NAME: $DB_NAME"
echo "DB_USER: $DB_USER"
echo "DT: $DT"
echo "SQLSCRIPT: $SQLSCRIPT"
echo "SQLDUMPFILE: $SQLDUMPFILE"
echo "OUTDIR= $OUTDIR"
echo "****************************"
echo ""


#######################################
##         CREATE ARCHIVE DUMP
#######################################
CMD='mysqldump -u '"$DB_USER"' -p '"$DB_NAME"' > '"$OUTDIR/$SQLDUMPFILE"
echo "INFO: Executing command $CMD ..."
eval $CMD
echo ""

#######################################
##         RUN SQL SCRIPT
#######################################
echo "INFO: Running sql script to clear old data from HDB archive $DB_NAME ..."
##CMD='/usr/bin/mysql -u '"$DB_USER"' -D '"$DB_NAME"' -p -A -e "set @db_name='"${DB_NAME}"'; set @time_interval='"${DT}"'; source '"${SQLSCRIPT}"';"'
CMD='/usr/bin/mysql -u '"$DB_USER"' -D '"$DB_NAME"' -p -A -e "set @time_interval='"${DT}"'; source '"${SQLSCRIPT}"';"'

echo "INFO: Executing cmd $CMD ..."
eval $CMD
echo ""


echo "INFO: End script run"
