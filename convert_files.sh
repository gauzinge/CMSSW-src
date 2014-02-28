#!/bin/bash 
# georg.auzinger@cern.ch
# script to run .dat to digi (.root) conversion from any convenient location
# usage: copy to analysis directory and give files as input

cmsswdir='/afs/cern.ch/user/g/gauzinge/scratch/CMSSW_6_2_0/src'
homedir='/afs/cern.ch/user/g/gauzinge/'
echo 'converter script for CMSSW .dat files to EDM .root files!'
echo

if [[ $# -ne 0 ]];
	then
	echo 'goint to CMSSW src directory and setting the environment!'
	echo $cmsswdir
	cd $cmsswdir
	eval `scramv1 runtime -sh`
	# cmssw
	# cmsenv
	echo
	counter=0;
	echo 'Converting files: '
	for i in $*
	do
		echo $i
	done
	echo
	for i in $*
	do
		echo 'Processing' $i':'
		echo 'cmsRun FileConverterdattoroot.py' $homedir$i
		cmsRun FileConverterdattoroot.py $homedir$i
		((counter++));
	done
	cd
	echo 'Converted $counter .dat files to EDM .root!'
else
	echo 'No data file provided!'
	echo 'Syntax ' $0 ' .dat filename (including Wildchars!)';
fi
	