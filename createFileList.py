#!/usr/bin/python
import fnmatch
import os

def createImageList(filename, pathToFilesDir,fileNamePath, fileExt, append):
    if append == 1:
        openString = "a"
    else:
        openString = "w"
    try:
        dataFile = open(pathToFilesDir+"/"+filename,openString)
    except IOError as e:
        print("({})".format(e))
        print("File not found moving on")
        return 0
    fileList = []
    fileFileExt = '.' + fileExt

    if not (fileNamePath ==""):
        fileNamePathWithSep = fileNamePath + "/"
    else:
        fileNamePathWithSep = fileNamePath


    for fileName in os.listdir(pathToFilesDir):
        #print fileName
        #print fileFileExt
        if fnmatch.fnmatch(fileName,"*"+fileFileExt):
            fileList.append(fileName)
            inputLine = fileNamePathWithSep+fileName
            dataFile.write(inputLine)
            dataFile.write("\n")
            print fileName

 

    dataFile.close()
    return len(fileList)


def createImageListMultipleImageTypes(filename, pathToFilesDir,fileNamePath, fileExts, append):
    total = 0
    total += createImageList(filename, pathToFilesDir,fileNamePath, fileExts[0], append)
    for i in range(1,len(fileExts)):
        total += createImageList(filename, pathToFilesDir,fileNamePath, fileExts[i], 1)
    print("Total Number of files", total)
    return

filename = "imageListTest.txt"
fileNamePath=""#"C:/Users/jlclemon/Documents/VisionData/SurfLocalSurfDesc/ProbBookAloneWithImages"
#pathToFilesDir = "C:/Users/jlclemon/Documents/VisionData/SP/test/carperson"#C:/Users/jlclemon/Documents/VisionData/SP/train/carperson"#"C:/Users/jlclemon/Documents/VisionData/SurfLocalSurfDesc/ProbBookAloneWithImages"
pathToFilesDir = "C:/Users/jlclemon/Documents/VisionData/SP/train/allobjects"
fileExts = ["png"]#, "jpg", "gif"]
append = 0
createImageListMultipleImageTypes(filename, pathToFilesDir,fileNamePath, fileExts, append)
