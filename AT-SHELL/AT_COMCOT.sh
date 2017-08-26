#!/bin/bash
#STEP0:   scp -p=== [path of AT_COMCOT.sh] [User name]@140.115.===.===:~
#STEP1:   ssh -p=== [User name]@140.115.===.===
#STEP2:   cd to the comcot directory
#STEP3:   ~/AT_COMCOT.sh [node number] [working directory]
#========================   <MUST>  =====  <ELECTIVE> ====
line="=================================================="
COMPUTE_NODE='compute-0-'
WORKING_DIR=''
OUTFILE='JOB_DIARY.dat'


if [[ $2 == '' ]]; then
  WORKING_DIR=$(pwd)'/'
elif [[ -d $2 ]]; then
  WORKING_DIR=$2
  # transfer to absolute pathname
  cd $WORKING_DIR
  WORKING_DIR=$(pwd)'/'
  cd -
else
  echo "invalid directory" >&2
  exit 1
fi
if [ ! -f "$WORKING_DIR/comcot" ]; then # check if comcot exist
    echo "comcot execution file not exist"
    exit 1
fi
OUTFILE=$WORKING_DIR$OUTFILE

if [[ $1 -lt 1 ]]||[[ $1 -gt 42 ]]||[[ ${1//[0-9]/''} != '' ]]||[[ $1 == '' ]]; then
  echo "invalid computing node" >&2
  exit 1
else
  COMPUTE_NODE=$COMPUTE_NODE$1
fi


echo "WORKING DIRECTORY '$WORKING_DIR'"
echo "COMPUTING LOG WILL BE SAVED AT '$OUTFILE'"
echo "==================DATE & TIME==================">$OUTFILE
date >> $OUTFILE
echo $line
echo "STARTING 'AT' SHELL"
at now >> $OUTFILE 2>&1 << EOAT
  echo $line >>$OUTFILE
  echo "switching to $COMPUTE_NODE" >> $OUTFILE
  echo $line >>$OUTFILE
  ssh -T $COMPUTE_NODE >> $OUTFILE 2>&1 << EOSSH
    cd $WORKING_DIR
    echo "=================CURRENT HOST================" >>$OUTFILE
    hostname >>$OUTFILE
    echo $line >>$OUTFILE
    ./comcot >> $OUTFILE 2>&1
EOSSH
EOAT
echo $line
atq
echo "USE 'atq' TO CHECK CURRENT JOBs"
echo "SSH TO $COMPUTE_NODE IF YOU WANT TO KILL THE PROCESS"
echo "==================CURRENT LOG===================="
cat $OUTFILE
