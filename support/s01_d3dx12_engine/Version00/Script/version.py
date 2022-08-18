# <1:git root (to get ignore)> <2:relative dir from git root to gather hash> <3:output dir>
#C:\Python310\python.exe C:\development\game04\support\s01_d3dx12_engine\Version00\Script\version.py C:\development\game04 support\s01_d3dx12_engine C:\development\game04\support\s01_d3dx12_engine\Version00\Output

import sys
import os
import re
import hashlib

#os.environ["PYTHONHASHSEED"] = "0"

# https://stackoverflow.com/questions/22058048/hashing-a-file-in-python
# BUF_SIZE is totally arbitrary, change for your app!
BUF_SIZE = 65536  # lets read stuff in 64kb chunks!
def FastHash(hashValue, filePath):
    #print("FastHash {0}".format(filePath))
    value = 0
    with open(filePath, 'rb') as f:
        while True:
            data = f.read(BUF_SIZE)
            if not data:
                break
            #value ^= hash(data)
            hashValue.update(data)
    #return value

def DealItem(setFilesIgnore, line): 
    if line in setFilesIgnore:
        return

    line = line.replace(".", "\.")
    line = line.replace("*", ".*")
    prog = re.compile(line)
    setFilesIgnore[line] = prog

#foo = set(), bar = {1,2,3}
class GitIgnore:
    def __init__(self, setFilesIgnore, setFolderIgnore):
        self.setFilesIgnore = setFilesIgnore # dictionatry of regulare expression string and compiled pattern
        self.setFolderIgnore = setFolderIgnore
    def IsFileValid(self, fileName):
        #return fileName not in self.setFilesIgnore
        for value in self.setFilesIgnore.values():
            if None != value.fullmatch(fileName):
                return False
        return True

    def IsFolderValid(self, folderName):
        return folderName not in self.setFolderIgnore
    def MakeChild(self, folder):
        return GitIgnore.Factory(folder, self);
    @classmethod
    def Factory(cls, folder, parentGitIgnoreOrNone=None):
        setFilesIgnore = {}
        setFolderIgnore = set()
        if parentGitIgnoreOrNone is not None:
            setFilesIgnore = parentGitIgnoreOrNone.setFilesIgnore.copy()
            setFolderIgnore = parentGitIgnoreOrNone.setFolderIgnore.copy()
        gitIgnorePath = os.path.join(folder, ".gitignore")
        if True == os.path.exists(gitIgnorePath):
            with open(gitIgnorePath) as file:
                for line in file:
                    line = line.strip()
                    if not line:
                        continue
                    if line[0] == '#':
                        continue
                    if line[-1] == '/':
                        folder = line[:-1]
                        if folder not in setFolderIgnore:
                            setFolderIgnore.add(folder)
                    else:
                        DealItem(setFilesIgnore, line) 
        return GitIgnore(setFilesIgnore, setFolderIgnore)


def TraversData(outputFileList, folderPath, gitIgnoreOrNull, relativePathSplit):
    ignoreLocal = GitIgnore.Factory(folderPath, gitIgnoreOrNull)
    #print("TraversData {0} {1}".format(folderPath, relativePathSplit))
    for (dirpath, folderList, fileList) in os.walk(folderPath):

        if 0 < len(relativePathSplit):
            front = relativePathSplit.pop(0)
            if front not in folderList:
                #print("{0} not found under {1}".format(front, folderPath))
                return
            newDir = os.path.join(folderPath, front)
            TraversData(outputFileList, newDir, ignoreLocal, relativePathSplit)
        else:
            for file in fileList:
                newFile = os.path.join(folderPath, file)
                if False == ignoreLocal.IsFileValid(file):
                    #print("ignore {0}".format(newFile))
                    continue
                outputFileList.append(newFile)
            for folder in folderList:
                newDir = os.path.join(folderPath, folder)
                if False == ignoreLocal.IsFolderValid(folder):
                    #print("ignore {0}".format(newDir))
                    continue
                TraversData(outputFileList, newDir, ignoreLocal, relativePathSplit)
        break

def WriteStringToFile(filePath, stringData):
    file = open(filePath, "w")
    file.write(stringData)
    file.close()

def ReadStringFromFile(filePath, defaultData):
    #print("ReadStringFromFile {0} {1}".format(filePath, defaultData))
    result = defaultData
    if True == os.path.exists(filePath):
        file = open(filePath, "r")
        result = file.read()
        file.close()
    #print(bytes(result, 'utf-8'))
    return result

def DealHash(hashValue, outputDir):

    hashFilePath = os.path.join(outputDir, "BuildHash.txt")
    oldHash = ReadStringFromFile(hashFilePath, "")

    if oldHash == hashValue:
        print("Hash value match {0}".format(hashValue))
        return


    majorFilePath = os.path.join(outputDir, "BuildVersionMajor.txt")
    major = int(ReadStringFromFile(majorFilePath, "0").strip())

    minorFilePath = os.path.join(outputDir, "BuildVersionMinor.txt")
    minor = int(ReadStringFromFile(minorFilePath, "0").strip())

    patchFilePath = os.path.join(outputDir, "BuildVersionPatch.txt")
    patch = int(ReadStringFromFile(patchFilePath, "0").strip())

    storeFilePath = os.path.join(outputDir, "BuildVersionStore.txt")
    store = int(ReadStringFromFile(storeFilePath, "0").strip())

    patch += 1
    version = "{0}.{1}.{2}.{3}".format(major, minor, patch, store)
    versionComma = "{0},{1},{2},{3}".format(major, minor, patch, store)

    WriteStringToFile(majorFilePath, str(major))
    WriteStringToFile(minorFilePath, str(minor))
    WriteStringToFile(patchFilePath, str(patch))
    WriteStringToFile(storeFilePath, str(store))

    versionFilePath = os.path.join(outputDir, "BuildVersion.txt")
    WriteStringToFile(versionFilePath, version)

    versionCommaFilePath = os.path.join(outputDir, "BuildVersionComma.txt")
    WriteStringToFile(versionCommaFilePath, versionComma)

    WriteStringToFile(hashFilePath, hashValue)

    print("Hash {0} {1} version {2}".format(hashValue, oldHash, version))


def main(*args):
    if 4 != len(args):
        print("usage: version.py <1:git root (to get ignore)> <2:relative dir from git root to gather hash> <3:output dir>")
        return
    gitRoot = args[1]
    relativePath = args[2]
    outputDir = args[3]

    #ignoreRoot = GitIgnore.Factory(gitRoot)
    relativePathSplit = relativePath.split("\\")
    fileList = []
    TraversData(fileList, gitRoot, None, relativePathSplit)

    hashValue = hashlib.md5()
    for file in fileList:
        #print(file)
        #hashValue ^= FastHash(file)
        FastHash(hashValue, file)
        #break

    #print(hashValue.hexdigest())
    DealHash(hashValue.hexdigest(), outputDir)

if __name__ == "__main__":
    main(*sys.argv)
