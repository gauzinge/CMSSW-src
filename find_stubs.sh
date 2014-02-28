#!/bin/bash 
# georg.auzinger@cern.ch
# script to run CBC stub producer on digis (after runningn converter) from any convenient location
# usage: copy to analysis directory and give files as input

cmsswdir='/afs/cern.ch/user/g/gauzinge/scratch/CMSSW_6_2_0/src'
homedir='/afs/cern.ch/user/g/gauzinge/'
echo 'Script for batched running of Stub Finder!'
echo

if [[ $# -ne 0 ]];
	then
	echo 'goint to CMSSW src directory and setting the environment!'
	echo $cmsswdir
	cd $cmsswdir
	eval `scramv1 runtime -sh`
	echo
	counter=0;
	echo 'Looking for Stubs in files: '
	for i in $*
	do
		echo $i
	done
	
	for i in $*
	do
		echo 'Processing '$i':'
		echo 'CBC_StubFinder.py' $homedir$i
		cmsRun CBC_StubFinder.py $homedir$i
		((counter++));
	done
	
	cd
	echo 'Ran stub finding on $counter files!'
else
	echo 'No data file provided!'
	echo 'Syntax ' $0 ' filename (including Wildchars!)';
fi