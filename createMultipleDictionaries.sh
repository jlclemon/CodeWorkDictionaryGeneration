#!/bin/bash


export LD_LIBRARY_PATH=$OpenCV_DIR/lib:$LD_LIBRARY_PATH


SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ] ; do SOURCE="$(readlink "$SOURCE")"; done
CURRENT_DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

EXECUTABLE_PATH="$CURRENT_DIR/bin"

EXECUTABLE="CodeWorkDictionaryGeneration"


DICTIONARY_GEN_EXEC="$EXECUTABLE_PATH/$EXECUTABLE"

CONIFG_FILE_BASE_PATH="$CURRENT_DIR"

CONFIG_FILE="configFile.txt"


CONIFG_FILE_PARAM="-configFile $CONIFG_FILE_BASE_PATH/$CONFIG_FILE"


OUTPUT_DIR_BASE="./Dictionary"





DictionarySizeList=('10' '15' '20' '25' '30' '40' '50' '75' '100')
let DictionarySizeCount=${#DictionarySizeList[@]}


FeatureDescList=('sift' 'surf')
FeatureLocalList=('sift' 'surf')
let FeatureCount=${#FeatureDescList[@]}





for (( currentDictionary=0;currentDictionary<$DictionarySizeCount;currentDictionary++)); do

	for (( currentFeatures=0;currentFeatures<FeatureCount;currentFeatures++)); do
		

		CURRENT_FEATURE_PARAM="-local ${FeatureLocalList[${currentFeatures}]} -desc ${FeatureDescList[${currentFeatures}]}"


		CURRENT_DICTIONARY_SIZE_PARAM="-dicSize ${DictionarySizeList[${currentDictionary}]}" 



		CURRENT_OUTPUT_DIR="$OUTPUT_DIR_BASE/${FeatureLocalList[${currentFeatures}]}"
		
		CURRENT_OUTPUT_DIR_PARAM="-outputDir $CURRENT_OUTPUT_DIR/"

		CURRENT_COMMAND="$DICTIONARY_GEN_EXEC $CONIFG_FILE_PARAM $CURRENT_FEATURE_PARAM $CURRENT_DICTIONARY_SIZE_PARAM $CURRENT_OUTPUT_DIR_PARAM"


		


		if [ ! -d "$CURRENT_OUTPUT_DIR" ]; then 
		    echo "Making Directory for Dictionaries"
		    mkdir -p "$CURRENT_OUTPUT_DIR"
		fi

		
		
		echo "Command: $CURRENT_COMMAND"
		$CURRENT_COMMAND


	done
done 















