/*Copyright (c) 2011, Texas Instruments Inc.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:
� Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
� Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.*/
#include "CodeWordDictionaryGeneration.h"

#define CW_GEN_DEFAULT_DICTIONARY_NAME "defaultDictionary"
#define CW_GEN_DEFAULT_DICTIONARY_EXT ".yml"
#define CW_GEN_DEFAULT_DICTIONARY_SIZE 200
#define CW_GEN_DEFAULT_IMAGELIST_FILENAME "imageList.txt"
#define CW_GEN_DEFAULT_IMAGELIST_BASE_DIR ""
#define CW_GEN_DEFAULT_FEATURE_FILELIST_FILENAME "imageList.txt"
#define CW_GEN_DEFAULT_FEATURE_FILELIST_BASE_DIR ""
#define CW_GEN_DEFAULT_FEATURE_FILE_OUTPUT_EXT ""  //".yml"
#define CW_GEN_DEFAULT_OUTPUT_BASE_DIR ""
#define CW_GEN_DEFAULT_GRID_SIZE_X 5
#define CW_GEN_DEFAULT_GRID_SIZE_Y 5

enum CodeWordGenerationInputType
{
	CODE_WORD_DIC_GEN_INPUT_TYPE_IMAGELIST,
	CODE_WORD_DIC_GEN_INPUT_TYPE_FEATURELIST,

	NUMBER_OF_CODE_WORD_DIC_GEN_INPUT_TYPES

};


enum CodeWordGenerationStates
{
	CW_GEN_STATE_INIT,
	CW_GEN_STATE_COLLECT_FEATURES,
	CW_GEN_STATE_SAVE_FEATURES,
	CW_GEN_STATE_CLUSTER_FEATURES,
	CW_GEN_STATE_SAVE_DICTIONARY,
	CW_GEN_STATE_DONE,
	CW_GEN_NUMBER_OF_STATES
};



struct CodeWordGenerationConfig
{
	string configFilename;

	string dictionaryFilename;
	string dictionaryFileExtension;
	int dictionarySize;

	string imageListFilename;
	string imageListBaseDirectory;

	string featureFileListFilename;
	string featureFileListBaseDirectory;

	string featureFileOutputExtension;

	string outputDirectory;

	bool showInputImage;	
	bool showFeatures;
	bool outputDictionaryPlus;

	bool outputFeatureFilelist;

	int gridSizeX;
	int gridSizeY;

};


struct CodeWordGenerationData
{
	FeatureLocalizationAlgos featureLocalAlgo;
	FeatureDescriptorAlgos featureDescAlgo;

	Ptr<FeatureDetector> featureDetector;
	Ptr<DescriptorExtractor> descriptorExtractor;


	HOGDescriptor * localizationHogPtr;
	HOGDescriptor * descHogPtr;

	vector <Mat> descriptorSets;
	vector <vector<KeyPoint> > keypointSets;

	Size frameSize;

	BOWKMeansTrainer * bowTrainer;

	int numberOfClusters;

	CodeWordGenerationInputType inputType;
	CodeWordGenerationStates currentState;

	Size gridSize;


	vector<string> imageList;
	vector<string> featureFileListIn;
	vector<string> featureFileListOut;

	Mat dictionary;
	Mat dictionaryLabels;
};
struct CodeWordGenerationInfo
{
	CodeWordGenerationConfig config;
	CodeWordGenerationData data;

};


vector<string> getCommandParams(int argc, char * argv[])
{
	vector<string> commandParams;
	for(int i = 0; i<argc; i++)
	{
		string newArg = argv[i];
		commandParams.push_back(newArg);

	}

	return commandParams;
}

bool loadConfigFile(vector<string> &commandArgs, string filename)
{

	std::ifstream configFile;

	string tmpString;
	bool returnVal = false;


	configFile.open(filename.c_str());


	if(configFile.good())
	{

		while(!configFile.eof())
		{
			getline(configFile,tmpString);
			if(tmpString.length() >0)
			{
				if(tmpString[0] != '#')
				{
					commandArgs.push_back(tmpString);
				}
			}
			else
			{

				commandArgs.push_back(tmpString);
			}
		}
		returnVal = true;

	}
	else
	{

		cout << "WARNING: Unable to open visual sonification params config file" << endl;

	}
	return returnVal;

}


bool loadTextFileList(vector<string> &fileLines, string filename)
{

	std::ifstream configFile;

	string tmpString;
	bool returnVal = false;


	configFile.open(filename.c_str());


	if(configFile.good())
	{

		while(!configFile.eof())
		{
			getline(configFile,tmpString);
			if(!(tmpString.compare("") == 0))
			{
				fileLines.push_back(tmpString);
			}
		}
		returnVal = true;

	}
	else
	{

		cout << "WARNING: Unable to open text list file: "<< filename << endl;

	}
	return returnVal;

}




void parseParamsVector(vector<string> & commandArgs,CodeWordGenerationInfo & info)
{


	stringstream stringBuffer("");

	//Set the default

	info.data.inputType = CODE_WORD_DIC_GEN_INPUT_TYPE_IMAGELIST;
	
	info.data.featureDescAlgo = FEATURE_DESC_SIFT;
	info.data.featureLocalAlgo = FEATURE_LOCALIZATION_SIFT;


	info.config.configFilename = "";

	info.config.dictionaryFilename = CW_GEN_DEFAULT_DICTIONARY_NAME;
	info.config.dictionaryFileExtension = CW_GEN_DEFAULT_DICTIONARY_EXT;
	info.config.dictionarySize = CW_GEN_DEFAULT_DICTIONARY_SIZE;

	info.config.imageListFilename  = CW_GEN_DEFAULT_IMAGELIST_FILENAME;
	info.config.imageListBaseDirectory  = CW_GEN_DEFAULT_IMAGELIST_BASE_DIR;

	info.config.featureFileListFilename = CW_GEN_DEFAULT_FEATURE_FILELIST_FILENAME;
	info.config.featureFileListBaseDirectory = CW_GEN_DEFAULT_FEATURE_FILELIST_BASE_DIR;

	info.config.featureFileOutputExtension = CW_GEN_DEFAULT_FEATURE_FILE_OUTPUT_EXT;

	info.config.outputDirectory = CW_GEN_DEFAULT_OUTPUT_BASE_DIR;

	info.config.gridSizeX = CW_GEN_DEFAULT_GRID_SIZE_X;
	info.config.gridSizeY = CW_GEN_DEFAULT_GRID_SIZE_Y;


	info.config.showInputImage = true;
	info.config.showFeatures = true;
	info.config.outputFeatureFilelist = false;

	info.config.outputDictionaryPlus = false;



	//This allows the first param to be a base config file that command line params can overwrite
	if ((int)commandArgs.size() > 1)
	{

		if(commandArgs[0].compare("-configFile")==0 || commandArgs[0].compare("-ConfigFile")==0)
		{
			info.config.configFilename = commandArgs[1];
			cout << "Loading config file: " << info.config.configFilename << endl;
			vector<string> commandArgsFromFile;
			loadConfigFile(commandArgsFromFile, info.config.configFilename);
			for(int j=2; j< commandArgs.size(); j++)
			{
				commandArgsFromFile.push_back(commandArgs[j]);
			}

			commandArgs=commandArgsFromFile;
		}
	}
	if ((int)commandArgs.size() > 2)
	{
		if(commandArgs[1].compare("-configFile")==0 || commandArgs[1].compare("-ConfigFile")==0)
		{
			info.config.configFilename = commandArgs[2];
			cout << "Loading config file: " << info.config.configFilename << endl;
			vector<string> commandArgsFromFile;
			loadConfigFile(commandArgsFromFile, info.config.configFilename);
			for(int j=3; j< commandArgs.size(); j++)
			{
				commandArgsFromFile.push_back(commandArgs[j]);
			}

			commandArgs=commandArgsFromFile;
		}


	}


	//Process the command line arguments
	for(int i=0; i < (int)commandArgs.size(); i++)
	{
		if(commandArgs[i].compare("-configFile")==0 || commandArgs[i].compare("-ConfigFile")==0)
		{
			info.config.configFilename = commandArgs[i+1];
			cout << "Loading config file: " << info.config.configFilename << endl;
			loadConfigFile(commandArgs, info.config.configFilename);
		}

		if(commandArgs[i].compare("-inputType")==0 || commandArgs[i].compare("-InputType")==0)
		{
			if(commandArgs[i+1].compare("imageList")==0)
			{
				info.data.inputType = CODE_WORD_DIC_GEN_INPUT_TYPE_IMAGELIST;
				
			}
			if(commandArgs[i+1].compare("featureFileList")==0)
			{
				info.data.inputType = CODE_WORD_DIC_GEN_INPUT_TYPE_FEATURELIST;
				
			}


		}


		if(commandArgs[i].compare("-dicFileBase")==0 || commandArgs[i].compare("-DicFileBase")==0)
		{
			info.config.dictionaryFilename = commandArgs[i+1];

		}


		if(commandArgs[i].compare("-dicFileExt")==0 || commandArgs[i].compare("-DicFileExt")==0)
		{
			info.config.dictionaryFileExtension = commandArgs[i+1];


		}

		if(commandArgs[i].compare("-dicSize")==0 || commandArgs[i].compare("-DicSize")==0)
		{
			stringBuffer.str("");
			stringBuffer.clear();
			stringBuffer << commandArgs[i+1];
			if((stringBuffer >> info.config.dictionarySize).fail())
			{
				cout << "Dictionary size could not be parsed." << endl;
			}

		}

		if(commandArgs[i].compare("-gridSizeX")==0 || commandArgs[i].compare("-GridSizeX")==0)
		{
			stringBuffer.str("");
			stringBuffer.clear();
			stringBuffer << commandArgs[i+1];
			if((stringBuffer >> info.config.gridSizeX).fail())
			{
				cout << "Grid size x could not be parsed." << endl;
			}

		}

		if(commandArgs[i].compare("-gridSizeY")==0 || commandArgs[i].compare("-GridSizeY")==0)
		{
			stringBuffer.str("");
			stringBuffer.clear();
			stringBuffer << commandArgs[i+1];
			if((stringBuffer >> info.config.gridSizeY).fail())
			{
				cout << "Grid size y could not be parsed." << endl;
			}

		}


		if(commandArgs[i].compare("-imageList")==0 || commandArgs[i].compare("-ImageList")==0)
		{
			info.config.imageListFilename = commandArgs[i+1];
			cout << "Setting list name to : " << info.config.imageListFilename << endl;

		}


		if(commandArgs[i].compare("-imageListBaseDir")==0 || commandArgs[i].compare("-ImageListBaseDir")==0)
		{

			info.config.imageListBaseDirectory = commandArgs[i+1];

		}

		if(commandArgs[i].compare("-featureFileOutExt")==0 || commandArgs[i].compare("-FeatureFileOutExt")==0)
		{
			info.config.featureFileOutputExtension = commandArgs[i+1];


		}

		if(commandArgs[i].compare("-outputFeatureFileList")==0 || commandArgs[i].compare("-OutputFeatureFileList")==0)
		{

			if(commandArgs[i+1].compare("Y")==0 || commandArgs[i+1].compare("y")==0 ||commandArgs[i+1].compare("1")==0 || commandArgs[i+1].compare("true")==0 ||commandArgs[i+1].compare("TRUE")==0 ||commandArgs[i+1].compare("")==0)
			{
				cout << "Setting True" << endl;
				info.config.outputFeatureFilelist = true;
			}
			else 
			{
				info.config.outputFeatureFilelist = false;

			}
			

		}



		if(commandArgs[i].compare("-featureFileList")==0 || commandArgs[i].compare("-FeatureFileList")==0)
		{
			info.config.featureFileListFilename = commandArgs[i+1];


		}

		if(commandArgs[i].compare("-featureFileListBaseDir")==0 || commandArgs[i].compare("-FeatureFileListBaseDir")==0)
		{
			info.config.featureFileListBaseDirectory = commandArgs[i+1];


		}



		if(commandArgs[i].compare("-imageListBaseDir")==0 || commandArgs[i].compare("-ImageListBaseDir")==0)
		{

			info.config.imageListBaseDirectory = commandArgs[i+1];

		}

		if(commandArgs[i].compare("-outputDir")==0 || commandArgs[i].compare("-OutputDir")==0)
		{

			info.config.outputDirectory = commandArgs[i+1];

		}

		if(commandArgs[i].compare("-showInputImage")==0 || commandArgs[i].compare("-ShowInputImage")==0)
		{

			if(commandArgs[i+1].compare("Y")==0 || commandArgs[i+1].compare("y")==0 ||commandArgs[i+1].compare("1")==0 || commandArgs[i+1].compare("true")==0 ||commandArgs[i+1].compare("TRUE")==0 ||commandArgs[i+1].compare("")==0)
			{
				info.config.showInputImage = true;
			}
			else 
			{
				info.config.showInputImage = false;

			}
			

		}

		if(commandArgs[i].compare("-showFeatures")==0 || commandArgs[i].compare("-ShowFeatures")==0)
		{

			if(commandArgs[i+1].compare("Y")==0 || commandArgs[i+1].compare("y")==0 ||commandArgs[i+1].compare("1")==0 || commandArgs[i+1].compare("true")==0 ||commandArgs[i+1].compare("TRUE")==0 ||commandArgs[i+1].compare("")==0)
			{
				info.config.showFeatures = true;
			}
			else 
			{
				info.config.showFeatures = false;

			}
			

		}

		if(commandArgs[i].compare("-outputDictionaryPlus")==0 || commandArgs[i].compare("-OutputDictionaryPlus")==0)
		{

			if(commandArgs[i+1].compare("Y")==0 || commandArgs[i+1].compare("y")==0 ||commandArgs[i+1].compare("1")==0 || commandArgs[i+1].compare("true")==0 ||commandArgs[i+1].compare("TRUE")==0 ||commandArgs[i+1].compare("")==0)
			{
				info.config.outputDictionaryPlus = true;
			}
			else
			{
				info.config.outputDictionaryPlus = false;

			}


		}




		if(commandArgs[i].compare("-local")==0 || commandArgs[i].compare("-Local")==0)
		{
			if(commandArgs[i+1].compare("sift")==0)
			{
				info.data.featureLocalAlgo = FEATURE_LOCALIZATION_SIFT;

				//featureExtractionConfig.currentLocalizationAlgo = FEATURE_LOCALIZATION_SIFT;
			}
			if(commandArgs[i+1].compare("surf")==0)
			{
				info.data.featureLocalAlgo = FEATURE_LOCALIZATION_SURF;
				//featureExtractionConfig.currentLocalizationAlgo = FEATURE_LOCALIZATION_SURF;
				
			}
			if(commandArgs[i+1].compare("fast")==0)
			{
				info.data.featureLocalAlgo = FEATURE_LOCALIZATION_FAST;
				//featureExtractionConfig.currentLocalizationAlgo = FEATURE_LOCALIZATION_FAST;
			}
			if(commandArgs[i+1].compare("hog")==0)
			{
				info.data.featureLocalAlgo = FEATURE_LOCALIZATION_HOG;
				//featureExtractionConfig.currentLocalizationAlgo = FEATURE_LOCALIZATION_HoG;
			}
			if(commandArgs[i+1].compare("grid")==0)
			{
				info.data.featureLocalAlgo = FEATURE_LOCALIZATION_GRID;
				//featureExtractionConfig.currentLocalizationAlgo = FEATURE_LOCALIZATION_SLIDING_WINDOW;
			}


		}


		if(commandArgs[i].compare("-desc")==0 || commandArgs[i].compare("-Desc")==0)
		{
			if(commandArgs[i+1].compare("sift")==0)
			{

				info.data.featureDescAlgo = FEATURE_DESC_SIFT;

				//featureExtractionConfig.currentDescriptorAlgo = FEATURE_DESC_SIFT;

			}
			if(commandArgs[i+1].compare("surf")==0)
			{

				info.data.featureDescAlgo = FEATURE_DESC_SURF;				
				//featureExtractionConfig.currentDescriptorAlgo = FEATURE_DESC_SURF;
			}
			if(commandArgs[i+1].compare("brief")==0)
			{
				info.data.featureDescAlgo = FEATURE_DESC_BRIEF;
				
				//featureExtractionConfig.currentDescriptorAlgo = FEATURE_DESC_BRIEF;
			}
			if(commandArgs[i+1].compare("hog")==0)
			{
				info.data.featureDescAlgo = FEATURE_DESC_HOG;


				//featureExtractionConfig.currentDescriptorAlgo = FEATURE_DESC_HoG;
			}

		}





	}









}

void displayParams(vector<string> commandArgs)
{

	cout << "Params (note later params overwrite earlier params):" << endl;
	for(int i = 0 ; i< (int)commandArgs.size(); i++)
	{
		if(commandArgs[i].compare("") != 0)
		{
			if(commandArgs[i].at(0) =='-')
			{
				cout << "Param :";

				cout << commandArgs[i] <<endl;

			}
			else if(commandArgs[i].at(0) !='#')
			{
				cout << "  Value :";
				cout << commandArgs[i] << "\n"<<endl;
			}


		}
	}




}
void handleParams(int argc, char * argv[],CodeWordGenerationInfo & info)
{
	vector<string> commandArgs = getCommandParams(argc, argv);
	parseParamsVector(commandArgs,info);
	displayParams(commandArgs);
}

Ptr<FeatureDetector> setupLocalization(CodeWordGenerationInfo & info)
{
    FeatureDetector* featureDetector = NULL;

	switch(info.data.featureLocalAlgo)
	{
		case FEATURE_LOCALIZATION_SIFT:
			{
				//SIFT::DetectorParams detectorParams;
				//SIFT::CommonParams commonParams;
				//featureDetector = new SiftFeatureDetector(detectorParams,commonParams);
				//SIFT::DescriptorParams descriptorParams;
				featureDetector = new SiftFeatureDetector( 0, 3,0.04,10,1.6);
			
				break;
			}
		case FEATURE_LOCALIZATION_SURF:
			{
				//SURF::CvSURFParams detectorParams;
				//detectorParams.hessianThreshold = 400.0;
				//featureDetector = new SurfFeatureDetector (detectorParams.hessianThreshold);
				featureDetector = new SurfFeatureDetector(400.0);
			
				break;
			}
		case FEATURE_LOCALIZATION_FAST:
			{

				int fastThreshold = 10;
				featureDetector = new FastFeatureDetector(fastThreshold,true);
				


			
				break;
			}
		case FEATURE_LOCALIZATION_HOG:
			{

				info.data.localizationHogPtr = new HOGDescriptor();
			
				break;
			}
		case FEATURE_LOCALIZATION_GRID:
			{
				info.data.gridSize.height = info.config.gridSizeY;
				info.data.gridSize.width = info.config.gridSizeX;			
				break;
			}

		default:

			break;
	}

	return featureDetector;

}

Ptr<DescriptorExtractor> setupDescriptor(CodeWordGenerationInfo & info)
{
   Ptr<DescriptorExtractor> descriptorExtractor = NULL;

   switch(info.data.featureDescAlgo)
	{
		case FEATURE_DESC_SIFT:
			{
				//SIFT::DescriptorParams descriptorParams;
				//SIFT::CommonParams commonParams;
				//descriptorExtractor = new SiftDescriptorExtractor(descriptorParams,commonParams);
				descriptorExtractor = new SiftDescriptorExtractor( 0, 3,0.04,10,1.6);
			}
			break;

		case FEATURE_DESC_SURF:
			{
				//SURF::CvSURFParams detectorParams;
								
				//detectorParams.hessianThreshold = 400.0;
				//detectorParams.nOctaves = 4;
				//detectorParams.nOctaveLayers = 2;
				//descriptorExtractor = new SurfDescriptorExtractor( detectorParams.nOctaves,detectorParams.nOctaveLayers);

				descriptorExtractor = new SurfDescriptorExtractor(400.0);

			}
			break;
		case FEATURE_DESC_HOG:
			{


				info.data.descHogPtr = new HOGDescriptor();

			}
			break;
		case FEATURE_DESC_BRIEF:
			{
				int briefDescriptorSize = 32;
				descriptorExtractor = new BriefDescriptorExtractor(briefDescriptorSize);
			}
			break;

		default:

			break;
	}

	return descriptorExtractor;



}


void codeWordLoadFileLists(CodeWordGenerationInfo &info)
{
	bool inputLoadResult = false;

	switch(info.data.inputType)
	{
		case CODE_WORD_DIC_GEN_INPUT_TYPE_IMAGELIST:
		{
			//Load the image list
			inputLoadResult = loadTextFileList(info.data.imageList, info.config.imageListFilename);

			//Set the feature file List out to the image list
			//We will use the image names but different file extensions for the feature file names
			info.data.featureFileListOut = info.data.imageList;


			//We may need to feature file list.  So I will do that before I append the directory, although in theory I could
			//Just rely on my python script
			ofstream featureFileListOutputFile;

			//If we are to output the FeeatureFileFileList then open it
			if(info.config.outputFeatureFilelist)
			{
				featureFileListOutputFile.open((info.config.outputDirectory + info.config.featureFileListFilename).c_str());
				if(!featureFileListOutputFile.is_open())
				{
					cout << "ERROR: Unable to open featureFileList" << endl;
				}
			}

			//Now lets create oue feature files names
			for(int i = 0; i < (int)info.data.featureFileListOut.size(); i++)
			{

				//In the current file name find the extension at the last .
				int extLocation = -1;
				extLocation = info.data.featureFileListOut[i].find_last_of(".");

				//Replace the image extension with the feature file extension
				info.data.featureFileListOut[i].replace(extLocation,info.config.featureFileOutputExtension.length(),info.config.featureFileOutputExtension); 

				//Put the featureFile in the File list file
				if(info.config.outputFeatureFilelist)
				{
					if(featureFileListOutputFile.is_open())
					{
						featureFileListOutputFile << info.data.featureFileListOut[i] << endl;
					}
					else
					{
						cout << "ERROR Unable to save Feature File List: " << info.data.featureFileListOut[i] << endl;
					}
				}

				
				//If there is an output directory where we are going to put this file
				//The go ahead an prepend that to the name to make the save simple
				if(info.config.outputDirectory.compare("") != 0)
				{
					info.data.featureFileListOut[i] = info.config.outputDirectory + info.data.featureFileListOut[i];

				}
			}

			//Close the feature list file
			if(info.config.outputFeatureFilelist)
			{
				if(featureFileListOutputFile.is_open())
				{
					featureFileListOutputFile.close();
				}
			}


			//If we have a base directory then lets go ahead and preppend that to the image list
			if(info.config.imageListBaseDirectory.compare("") != 0)
			{
				for(int i = 0; i < (int)info.data.imageList.size(); i++)
				{
					info.data.imageList[i] = info.config.imageListBaseDirectory + info.data.imageList[i];
				}
			}
			break;
			
		}
		case CODE_WORD_DIC_GEN_INPUT_TYPE_FEATURELIST:
		{
			inputLoadResult = loadTextFileList(info.data.featureFileListIn, info.config.featureFileListFilename);

			//If we have a base directory then lets go ahead and preppend that to the feature File list
			if(info.config.featureFileListBaseDirectory.compare("") != 0)
			{
				for(int i = 0; i < (int)info.data.featureFileListIn.size(); i++)
				{
					info.data.featureFileListIn[i] = info.config.featureFileListBaseDirectory + info.data.featureFileListIn[i];
				}
			}


			break;
		}
		default:
		{
			cout << "No valid input type " << endl;
			break;
		}
	}

	if(!inputLoadResult)
	{

		cout << "ERROR: Unable to load inputs.  Exiting...." << endl;
		exit(0);
	}
}

void codeWordInitFeatureExtraction(CodeWordGenerationInfo & info)
{
	info.data.featureDetector = setupLocalization(info);
	info.data.descriptorExtractor = setupDescriptor(info);



}

void codeWordGenInit(CodeWordGenerationInfo & info)
{
	
	codeWordInitFeatureExtraction(info);
	codeWordLoadFileLists(info);
	info.data.numberOfClusters = info.config.dictionarySize;




	return;
}


void localizeFeaturePoints(CodeWordGenerationInfo & info, Mat & inputImage,  vector<KeyPoint> & keyPoints )
{
	keyPoints.clear();

	switch(info.data.featureLocalAlgo)
	{
		case FEATURE_LOCALIZATION_SIFT:
		case FEATURE_LOCALIZATION_SURF:
		case FEATURE_LOCALIZATION_FAST:
		{
			info.data.featureDetector->detect(inputImage,keyPoints);
			break;
		}
		case FEATURE_LOCALIZATION_HOG:
		{
			vector<Rect> bboxes;
			info.data.localizationHogPtr->detectMultiScale(inputImage, bboxes, 0, Size(8,8), Size(32,32), 1.05, 2);
			for(unsigned int i = 0; i< bboxes.size(); i++)
			{
				KeyPoint currentPoint;
				currentPoint.pt.x = (float)bboxes[i].x;
				currentPoint.pt.y = (float)bboxes[i].y;
				currentPoint.size = (float)bboxes[i].height/(float)info.data.localizationHogPtr->winSize.height;
				keyPoints.push_back(currentPoint);
			}
			break;

		}
		case FEATURE_LOCALIZATION_GRID:
			{
				for(int y = 0; y < inputImage.rows; y+= info.data.gridSize.height)
				{
					
					for(int x = 0; x < inputImage.cols; x+= info.data.gridSize.width)
					{
						KeyPoint newLocation((float)x,(float)y,1);
						keyPoints.push_back(newLocation);
					}

				
				}


				break;
			}

		default:

			break;



	}

		
	

}


void buildFeatureDescriptors(CodeWordGenerationInfo & info, Mat & inputImage, vector<KeyPoint> & keyPoints, Mat & descriptors)
{

   switch(info.data.featureDescAlgo)
	{
		case FEATURE_DESC_SIFT:
		{
			info.data.descriptorExtractor->compute( inputImage, keyPoints, descriptors );
			descriptors = descriptors * (1/512.0);
			break;
		}

		case FEATURE_DESC_SURF:
		{
			info.data.descriptorExtractor->compute( inputImage, keyPoints, descriptors );			
			break;
		}
		case FEATURE_DESC_HOG:
		{
			vector<float> descriptorsVec;

			if(keyPoints.size() != 0 && info.data.featureLocalAlgo != FEATURE_LOCALIZATION_HOG)
			{
				vector<Point> points;
				for(unsigned int i = 0 ; i< keyPoints.size(); i++)
				{
					Point currentPoint;
					currentPoint.x =(int) keyPoints[i].pt.x;
					currentPoint.y = (int)keyPoints[i].pt.y;
					points.push_back(currentPoint);
				}

				info.data.descHogPtr->compute( inputImage,descriptorsVec,Size(),Size(),points);
			}
			else
			{


				info.data.descHogPtr->compute( inputImage,descriptorsVec,Size(8,8),Size(32,32));
			}
			unsigned int numberOfFeatures =descriptorsVec.size()/info.data.descHogPtr->getDescriptorSize();

			descriptors.create(numberOfFeatures,info.data.descHogPtr->getDescriptorSize(),CV_32FC1);
			for(unsigned int row = 0; row <numberOfFeatures; row++)
			{
				for(unsigned int col = 0; col< info.data.descHogPtr->getDescriptorSize(); col++)
				{
					descriptors.at<float>(row,col) = descriptorsVec[col + row*info.data.descHogPtr->getDescriptorSize()];

				}
			}
			
			
			break;
		}
		case FEATURE_DESC_BRIEF:
		{
			info.data.descriptorExtractor->compute( inputImage, keyPoints, descriptors );			
			
			break;
		}
		default:
		{

			cout << "WARNING: No descriptors made." << endl;
			break;
		}
	}





}



void codeWordGenGetFeaturesFromSingleImage(CodeWordGenerationInfo & info,string imageFile, Mat & features, vector<KeyPoint> &keypoints )
{
	Mat currentImage = imread(imageFile);


	if(info.config.showInputImage)
	{
		namedWindow("Input Image");

		imshow("Input Image", currentImage);
		waitKey(33);

	}

	localizeFeaturePoints(info, currentImage,  keypoints );
	buildFeatureDescriptors(info, currentImage, keypoints, features);
	if(info.config.showFeatures)
	{
			Mat tmpImage;
			Mat displayImage;
			if(currentImage.channels() ==3)
			{
				cvtColor(currentImage,tmpImage,CV_BGR2GRAY);
			}
			else
			{
				currentImage.copyTo(tmpImage);
			}

			drawKeypoints(tmpImage,keypoints,displayImage,Scalar(255.0,0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);			
			namedWindow("Current Feature Points Window");
			imshow("Current Feature Points Window",displayImage);
			waitKey(33);

	}



}
void codeWordGenSaveFeatureFiles(CodeWordGenerationInfo &info)
{

	cout << "Saving Features Files...."; 
	for(int i =0; i<(int)info.data.featureFileListOut.size(); i++)
	{

	
		FileStorage fs;
	
		fs.open(info.data.featureFileListOut[i],FileStorage::WRITE);
		
		if(fs.isOpened())
		{

			cout << info.data.featureFileListOut[i] << "...";
			fs << "Descriptors" << info.data.descriptorSets[i];
			write(fs,"Keypoints",info.data.keypointSets[i]);
			fs.release();
		}
		else
		{

			cout << "ERROR: Unable to save file:" << info.data.featureFileListOut[i] <<endl;
		}
	}

	cout << "Done" << endl; 


}


void codeWordGenGetFeaturesFromImages( CodeWordGenerationInfo & info,vector<string> imageFileList, vector<Mat> & featureSets, vector <vector<KeyPoint> >&keypointSets)
{
	for(int i = 0; i< (int)imageFileList.size(); i++)
	{
		Mat currentFeatures;
		vector<KeyPoint> currentKeypoints;
		cout << "Exracting Keypoints from: " <<  imageFileList[i] << "....";
		codeWordGenGetFeaturesFromSingleImage(info,imageFileList[i],currentFeatures,currentKeypoints);
		cout << "Found " << currentFeatures.rows << " features." << endl;
		featureSets.push_back(currentFeatures);
		keypointSets.push_back(currentKeypoints);
	}



}

void codeWordGenGetFeaturesFromFiles( CodeWordGenerationInfo & info,vector<string> featureFileList, vector<Mat> & featureSets, vector <vector<KeyPoint> >&keypointSets)
{
	for(int i = 0; i< (int)featureFileList.size(); i++)
	{
		Mat currentFeatures;
		vector<KeyPoint> currentKeypoints;
		cout << "Loading feature data from: " <<  featureFileList[i] << "....";
		codeWordGenLoadFeatureFiles(featureFileList[i], currentFeatures, currentKeypoints);		
		cout << "Loaded " << currentFeatures.rows << " features." << endl;
		featureSets.push_back(currentFeatures);
		keypointSets.push_back(currentKeypoints);
	}



}


void codeWordGenGetFeatures(CodeWordGenerationInfo & info)
{
	vector<Mat> featureSets;	
	int numberOfFeatureSets;

	switch(info.data.inputType)
	{
		case CODE_WORD_DIC_GEN_INPUT_TYPE_IMAGELIST:
		{			
			numberOfFeatureSets = info.data.imageList.size();
						
			codeWordGenGetFeaturesFromImages( info,info.data.imageList, info.data.descriptorSets, info.data.keypointSets);



			break;
		}
		case CODE_WORD_DIC_GEN_INPUT_TYPE_FEATURELIST:
		{

			numberOfFeatureSets = info.data.featureFileListIn.size();
			codeWordGenGetFeaturesFromFiles( info,info.data.featureFileListIn, info.data.descriptorSets, info.data.keypointSets);
			break;
		}
		default:
		{
			cout << "WARNING: Unknown Input Type" << endl;
			break;
		}
	}	







}

void codeWordGenClusterFeatures(CodeWordGenerationInfo & info)
{
	Mat allFeatures;

	cout << "Performaing dictionary generation" << "...";
	allFeatures = info.data.descriptorSets[0].clone();

	for(int i=1; i < (int)info.data.descriptorSets.size(); i++)
	{
		allFeatures.push_back(info.data.descriptorSets[i]);

	}

	kmeans(allFeatures, info.data.numberOfClusters, info.data.dictionaryLabels, TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0),
               20, KMEANS_PP_CENTERS, info.data.dictionary);


	cout << "Done" << endl;
}

void codeWordGenSaveDictionary(CodeWordGenerationInfo & info)
{
	string dictionaryFullFilename = "";
	stringstream stringBuffer;
	stringBuffer.str("");
	stringBuffer.clear();
	stringBuffer << info.data.numberOfClusters;



	dictionaryFullFilename= info.config.outputDirectory + info.config.dictionaryFilename + "_" + stringBuffer.str() + info.config.dictionaryFileExtension;
	FileStorage fs;

	cout << "Writing dictionary to: " << dictionaryFullFilename << "..."; 
	fs.open(dictionaryFullFilename,FileStorage::WRITE);
	if(!fs.isOpened())
	{
		cout << "ERROR: Unable to write dictionary file" << endl;
		return;

	}
	fs <<"Dictionary"<< info.data.dictionary;
	fs << "DictionarySize" << info.data.numberOfClusters;

	fs.release();
	cout << "Done"<< endl;
}
void codeWordGenSaveDictionaryPlus(CodeWordGenerationInfo & info)
{
	string dictionaryFullFilename = "";
	stringstream stringBuffer;
	stringBuffer.str("");
	stringBuffer.clear();
	stringBuffer << info.data.numberOfClusters;



	dictionaryFullFilename= info.config.outputDirectory + info.config.dictionaryFilename + "_" + stringBuffer.str()+"_Plus" + info.config.dictionaryFileExtension;
	FileStorage fs;

	cout << "Writing dictionary to: " << dictionaryFullFilename << "..."; 
	fs.open(dictionaryFullFilename,FileStorage::WRITE);
	if(!fs.isOpened())
	{
		cout << "ERROR: Unable to write dictionary file" << endl;
		return;

	}

	Mat allFeatures;


	allFeatures = info.data.descriptorSets[0].clone();

	for(int i=1; i < (int)info.data.descriptorSets.size(); i++)
	{
		allFeatures.push_back(info.data.descriptorSets[i]);

	}
	vector<KeyPoint> allKeypoints;
	for(int i=1; i < (int)info.data.keypointSets.size(); i++)
	{
		for(int j = 0; j < (int)info.data.keypointSets[i].size(); j++)
		{
			allKeypoints.push_back(info.data.keypointSets[i][j]);
		}
	}



	fs <<"Dictionary"<< info.data.dictionary;
	fs << "DictionarySize" << info.data.numberOfClusters;
	fs << "Descriptors" << allFeatures;
	fs << "Labels" <<info.data.dictionaryLabels;
	write(fs,"Keypoints",allKeypoints);	

	fs.release();
	cout << "Done"<< endl;
}






int main(int argc, char * argv[])
{

	CodeWordGenerationInfo info;
	handleParams(argc, argv,info);

	info.data.currentState = CW_GEN_STATE_INIT;

	bool done = false;
	while(!done)
	{
		switch(info.data.currentState)
		{
			case CW_GEN_STATE_INIT:
			{
				cout << endl;
				codeWordGenInit(info);
				info.data.currentState = CW_GEN_STATE_COLLECT_FEATURES;
				break;
			}
			case CW_GEN_STATE_COLLECT_FEATURES:
			{
				cout << endl;
				codeWordGenGetFeatures(info);
				info.data.currentState = CW_GEN_STATE_SAVE_FEATURES;
				break;
			}
			case CW_GEN_STATE_SAVE_FEATURES:
			{
				cout << endl;
				if(info.config.featureFileOutputExtension.compare("") != 0)
				{
					codeWordGenSaveFeatureFiles(info);
				}
				info.data.currentState = CW_GEN_STATE_CLUSTER_FEATURES;
				break;
			}

			case CW_GEN_STATE_CLUSTER_FEATURES:
			{
				cout << endl;
				codeWordGenClusterFeatures(info);
				info.data.currentState = CW_GEN_STATE_SAVE_DICTIONARY;
				break;
			}
			case CW_GEN_STATE_SAVE_DICTIONARY:
			{
				cout << endl;
				codeWordGenSaveDictionary(info);
				if(info.config.outputDictionaryPlus)
				{
					codeWordGenSaveDictionaryPlus(info);
				}
				info.data.currentState = CW_GEN_STATE_DONE;
				break;
			}
			case CW_GEN_STATE_DONE:
			{
				cout << endl;
				cout << "Code Word Generation is Complete" << endl;
				done = true;
				break;
			}
			default:
			{
				done = true;
				break;
			}
		}
	}
//	string input;
//	cout << "\nWaitng for ack string to terminate program string:";
//	cin >> input;
	cout << "Bye bye..." << endl;


}
