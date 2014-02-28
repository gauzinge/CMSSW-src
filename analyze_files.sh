#!/bin/bash 
# georg.auzinger@cern.ch
# script to run CBC analysis on cluster digi (after runningn stub finder) from any convenient location
# usage: copy to analysis directory and give files as input

cmsswdir='/afs/cern.ch/user/g/gauzinge/scratch/CMSSW_6_2_0/src'
homedir='/afs/cern.ch/user/g/gauzinge/'
echo 'Script for batched analysis of several runs!'
echo

if [[ $# -ne 0 ]];
	then
	echo 'goint to CMSSW src directory and setting the environment!'
	echo $cmsswdir
	cd $cmsswdir
	eval `scramv1 runtime -sh`
	echo
	counter=0;
	echo 'Analyzing files: '
	for i in $*
	do
		echo $i
	done
	
	for i in $*
	do
		echo 'Processing '$i':'
		echo 'cmsRun CBC_Analysis.py' $homedir$i
		cmsRun CBC_Analysis.py $homedir$i
		((counter++));
	done
	cd
	echo 'Analyzed $counter files!'
else
	echo 'No data file provided!'
	echo 'Syntax ' $0 ' filename (including Wildchars!)';
fi