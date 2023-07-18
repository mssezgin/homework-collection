#ifndef HW3_EXPLORER_HPP
#define HW3_EXPLORER_HPP

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <vector>

#include "fat32.h"
#include "image.hpp"

// ls flags
#define LS_LIST 0x01

class Explorer
{
private:
    Image image;
    std::vector<std::string> workingDirAbsolutePath;
    FatFileEntry workingDirFileEntry;
    uint32_t cachedClusterNumber;
    FatFileEntry* cachedCluster;
    uint32_t fileIndexCounter;

    uint8_t calculateChecksum(const uint8_t *pFCBName)
    {
        uint8_t sum = 0;
        for (uint8_t i = 11; i; i--) {
            sum = ((sum & 1) << 7) + (sum >> 1) + *pFCBName++;
        }
        return sum;
    }

    void fillLFN(const std::string& filename, std::vector<FatFileEntry>& fileEntry, uint8_t index, uint8_t checksum)
    {

        if (index < 0x14 && filename.size() >= index * 13) {
            fillLFN(filename, fileEntry, index + 1, checksum);
        }

        fileEntry.push_back({});
        FatFileEntry& currLFN = fileEntry.back();

        currLFN.lfn.sequence_number = index;
        currLFN.lfn.attributes = MSDOS_LFN;
        currLFN.lfn.reserved = 0x00;
        currLFN.lfn.checksum = checksum;
        currLFN.lfn.firstCluster = 0x0000;

        bool nameHasBeenWritten = false;
        for (uint8_t i = 0; i < 5; i++) {
            if (nameHasBeenWritten) {
                currLFN.lfn.name1[i] = 0xFFFF;
            } else {
                if ((currLFN.lfn.name1[i] = filename[(index - 1) * 13 + i]) == '\0') {
                    nameHasBeenWritten = true;
                }
            }
        }
        for (uint8_t i = 0; i < 6; i++) {
            if (nameHasBeenWritten) {
                currLFN.lfn.name2[i] = 0xFFFF;
            } else {
                if ((currLFN.lfn.name2[i] = filename[(index - 1) * 13 + 5 + i]) == '\0') {
                    nameHasBeenWritten = true;
                }
            }
        }
        for (uint8_t i = 0; i < 2; i++) {
            if (nameHasBeenWritten) {
                currLFN.lfn.name3[i] = 0xFFFF;
            } else {
                if ((currLFN.lfn.name3[i] = filename[(index - 1) * 13 + 11 + i]) == '\0') {
                    nameHasBeenWritten = true;
                }
            }
        }
    }

    void createFatFileEntry(const std::string& filename, std::vector<FatFileEntry>& fileEntry)
    {
        auto now = std::chrono::system_clock::now();
        time_t timer = std::chrono::system_clock::to_time_t(now);
        auto nowMS = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        tm bt = *std::localtime(&timer);

        uint32_t seed = fileIndexCounter + 1;
        FatFileEntry fileEntry83;
        fileEntry83.msdos.filename[0] = '~';
        for (uint8_t i = 7; i > 0; i--) {
            fileEntry83.msdos.filename[i] = Image::hexDigits[seed & 0x0000000F];
            seed = seed >> 4;
        }
        for (uint8_t i = 0; i < 3; i++) {
            fileEntry83.msdos.extension[i] = ' ';
        }
        fileEntry83.msdos.attributes = 0x00;
        fileEntry83.msdos.reserved = 0x00;
        fileEntry83.msdos.creationTimeMs = ((nowMS.count() % 2000) / 10);
        fileEntry83.msdos.creationTime = (bt.tm_hour << 11) | (bt.tm_min << 5) | (bt.tm_sec);
        fileEntry83.msdos.creationDate = ((bt.tm_year - 80) << 9) | ((bt.tm_mon + 1) << 5) | (bt.tm_mday);
        fileEntry83.msdos.lastAccessTime = fileEntry83.msdos.creationTime;
        fileEntry83.msdos.eaIndex = 0x0000;
        fileEntry83.msdos.modifiedTime = fileEntry83.msdos.creationTime;
        fileEntry83.msdos.modifiedDate = fileEntry83.msdos.creationDate;
        fileEntry83.msdos.firstCluster = 0x0000;

        fillLFN(filename, fileEntry, 1, calculateChecksum(fileEntry83.msdos.filename));
        fileEntry.front().lfn.sequence_number |= 0x40;
        fileEntry.push_back(fileEntry83);
        fileIndexCounter++;
    }

    std::string parseEntryName(const FatFileEntry* entry, uint32_t j, uint8_t* lenLFN)
    {
        std::string entryName;
        char* name;
        if (entry[0].msdos.attributes == MSDOS_LFN) {
            // lfn
            *lenLFN = entry[0].lfn.sequence_number & 0x1F;
            if (j + *lenLFN >= image.nFileEntriesPerCluster) {
                uint32_t nextCluster = image.getFATEntry(cachedClusterNumber) & 0x0FFFFFFF;
                if (IS_FAT_EOC(nextCluster)) {
                    // error
                    return entryName;
                }
                image.readDataCluster(&cachedCluster[image.nFileEntriesPerCluster], nextCluster);
            }
            uint32_t i = 0;
            name = new char[13 * *lenLFN + 1];
            for (uint8_t k = *lenLFN; k-- > 0;) {
                for (uint32_t j = 0; j < 5; i++, j++) {
                    name[i] = entry[k].lfn.name1[j];
                }
                for (uint32_t j = 0; j < 6; i++, j++) {
                    name[i] = entry[k].lfn.name2[j];
                }
                for (uint32_t j = 0; j < 2; i++, j++) {
                    name[i] = entry[k].lfn.name3[j];
                }
            }

        } else {
            // msdos
            *lenLFN = 0;
            uint32_t i = 0;
            name = new char[13];
            for (uint32_t j = 0; j < 8 && entry->msdos.filename[j] != ' '; j++) {
                name[i++] = entry->msdos.filename[j];
            }
            if (entry->msdos.extension[0] != ' ') {
                name[i++] = '.';
                for (uint32_t j = 0; j < 3 && entry->msdos.extension[j] != ' '; j++) {
                    name[i++] = entry->msdos.extension[j];
                }
            }
            name[i++] = 0x00;
        }
        entryName = name;
        delete[] name;
        return entryName;
    }

    bool locatePath(std::vector<std::string> &path, FatFileEntry* fileEntry, std::vector<std::string>* absolutePath) // , uint32_t* offset = nullptr)
    {
        FatFileEntry* itemEntry = nullptr;
        std::vector<std::string> itemAbsolutePath;
        uint32_t currClusterNumber;
        size_t iStart;

        if (path[0].compare("") == 0) {
            currClusterNumber = image.BPB.extended.RootCluster;
            iStart = 1;
            itemEntry = &image.rootFileEntry;
            itemAbsolutePath.push_back("");
        } else {
            currClusterNumber = (workingDirFileEntry.msdos.eaIndex << 16) | workingDirFileEntry.msdos.firstCluster;
            iStart = 0;
            itemEntry = &workingDirFileEntry;
            itemAbsolutePath = workingDirAbsolutePath;
        }

        for (size_t i = iStart; i < path.size(); i++) {
            const std::string& itemName = path[i];
            itemEntry = nullptr;

            while (!IS_FAT_EOC(currClusterNumber)) {
                image.readDataCluster(cachedCluster, currClusterNumber);
                cachedClusterNumber = currClusterNumber;

                for (uint32_t j = 0; j < image.nFileEntriesPerCluster; j++) {
                    // empty entry
                    if (cachedCluster[j].msdos.filename[0] == 0x00) {
                        continue;
                    }
                    // deleted entry
                    if (cachedCluster[j].msdos.filename[0] == 0x05 || cachedCluster[j].msdos.filename[0] == 0xE5) {
                        continue;
                    }
                    // dot entry
                    if (cachedCluster[j].msdos.filename[0] == '.') {
                        if (itemName.compare(".") == 0 && cachedCluster[j].msdos.filename[1] == ' ') {
                            itemEntry = &cachedCluster[j];
                            // itemOffset = j;
                            break;
                        } else if (itemName.compare("..") == 0 && cachedCluster[j].msdos.filename[1] == '.') {
                            itemEntry = &cachedCluster[j];
                            // itemOffset = j;
                            itemAbsolutePath.pop_back();
                            break;
                        } else {
                            continue;
                        }
                    }
                    // normal entry
                    uint8_t lenLFN;
                    std::string itemFileEntryName = parseEntryName(&cachedCluster[j], j, &lenLFN);
                    j += lenLFN;
                    if (itemFileEntryName.compare(itemName) != 0) {
                        continue;
                    } else {
                        itemEntry = &cachedCluster[j];
                        itemAbsolutePath.push_back(itemFileEntryName);
                        break;
                    }
                }

                if (itemEntry == nullptr) {
                    currClusterNumber = image.getFATEntry(currClusterNumber) & 0x0FFFFFFF;
                } else {
                    break;
                }
            }

            if (itemEntry == nullptr) {
                return false;
            } else if (i < path.size() - 1) {
                if ((itemEntry->msdos.attributes & MSDOS_DIRECTORY) == 0x00) {
                    return false;
                } else {
                    currClusterNumber = (itemEntry->msdos.eaIndex << 16) | itemEntry->msdos.firstCluster;
                    if (currClusterNumber == 0) {
                        itemEntry = &image.rootFileEntry;
                        currClusterNumber = image.BPB.extended.RootCluster;
                    }
                }
            } else {
                currClusterNumber = (itemEntry->msdos.eaIndex << 16) | itemEntry->msdos.firstCluster;
            }
        }

        // itemEntry should never be nullptr here
        if (itemEntry == nullptr) {
            return false;
        } else {
            *fileEntry = *itemEntry;
            *absolutePath = itemAbsolutePath;
            return true;
        }
    }

public:
    Explorer(char* imgFileName) : image(imgFileName)
    {
        workingDirAbsolutePath.push_back("");
        workingDirFileEntry = image.rootFileEntry;
        cachedClusterNumber = 0;
        cachedCluster = new FatFileEntry[image.nFileEntriesPerCluster * 2];
        fileIndexCounter = 0;
    }

    ~Explorer()
    {
        delete[] cachedCluster;
        cachedCluster = nullptr;
    }

    std::vector<std::string>* getWD()
    {
        return &workingDirAbsolutePath;
    }

    void cd(std::vector<std::string>& path)
    {
        FatFileEntry folderEntry;
        std::vector<std::string> absolutePath;
        if (locatePath(path, &folderEntry, &absolutePath) == true) {
            if (folderEntry.msdos.attributes & MSDOS_DIRECTORY) {
                workingDirFileEntry = folderEntry;
                workingDirAbsolutePath = absolutePath;
            }
        }
    }

    void ls(std::vector<std::string>& path, uint8_t flags)
    {
        FatFileEntry folderEntry;
        std::vector<std::string> absolutePath;
        if (path.size() == 0) {
            folderEntry = workingDirFileEntry;
        } else if (locatePath(path, &folderEntry, &absolutePath) == false) {
            return;
        }

        if (folderEntry.msdos.attributes & MSDOS_DIRECTORY) {
            uint32_t itemCount = 0;
            uint32_t currClusterNumber;
            currClusterNumber = (folderEntry.msdos.eaIndex << 16) | folderEntry.msdos.firstCluster;
            if (currClusterNumber == 0) {
                currClusterNumber = image.BPB.extended.RootCluster;
            }

            uint32_t j = 0;
            while (!IS_FAT_EOC(currClusterNumber)) {
                image.readDataCluster(cachedCluster, currClusterNumber);
                cachedClusterNumber = currClusterNumber;

                if (j >= image.nFileEntriesPerCluster) {
                    j -= image.nFileEntriesPerCluster;
                }
                for (; j < image.nFileEntriesPerCluster; j++) {
                    // empty entry
                    if (cachedCluster[j].msdos.filename[0] == 0x00) {
                        continue;
                    }
                    // deleted entry
                    if (cachedCluster[j].msdos.filename[0] == 0x05 || cachedCluster[j].msdos.filename[0] == 0xE5) {
                        continue;
                    }
                    // dot entry
                    if (cachedCluster[j].msdos.filename[0] == '.') {
                        continue;
                    }
                    // normal entry
                    uint8_t lenLFN;
                    std::string itemFileEntryName = parseEntryName(&cachedCluster[j], j, &lenLFN);
                    j += lenLFN;
                    if (cachedCluster[j].msdos.attributes & MSDOS_HIDDEN) {
                        continue;
                    }
                    itemCount++;
                    if (flags & LS_LIST) {
                        if (cachedCluster[j].msdos.attributes & MSDOS_DIRECTORY) {
                            std::cout << "drwx------";
                        } else {
                            std::cout << "-rwx------";
                        }
                        std::cout << " 1 root root ";
                        std::cout << cachedCluster[j].msdos.fileSize << " ";
                        uint16_t x;
                        // print months
                        x = (cachedCluster[j].msdos.modifiedDate & (uint16_t) 0x01E0) >> 5;
                        if (x >= 1 && x <= 12) {
                            std::cout << Image::months[x - 1] << " ";
                        }
                        // print days
                        x = (cachedCluster[j].msdos.modifiedDate & (uint16_t) 0x001F);
                        if (x < (uint16_t) 10) {
                            std::cout << 0;
                        }
                        std::cout << x << " ";
                        // print hours
                        x = (cachedCluster[j].msdos.modifiedTime & (uint16_t) 0xF800) >> 11;
                        if (x < (uint16_t) 10) {
                            std::cout << 0;
                        }
                        std::cout << x;
                        std::cout << ":";
                        // print minutes
                        x = (cachedCluster[j].msdos.modifiedTime & (uint16_t) 0x07E0) >> 5;
                        if (x < (uint16_t) 10) {
                            std::cout << 0;
                        }
                        std::cout << x << " ";
                        // print name
                        std::cout << itemFileEntryName << std::endl;
                    } else {
                        // print name
                        std::cout << itemFileEntryName << " ";
                    }
                }
                currClusterNumber = image.getFATEntry(currClusterNumber) & 0x0FFFFFFF;
            }

            if (!(flags & LS_LIST) && itemCount > 0) {
                std::cout << std::endl;
            }
        }
    }

    void mkdir(std::vector<std::string>& path, std::string& foldername)
    {
        FatFileEntry parentFolderEntry;
        std::vector<std::string> absolutePath;
        if (locatePath(path, &parentFolderEntry, &absolutePath) == true) {
            if (parentFolderEntry.msdos.attributes & MSDOS_DIRECTORY) {
                // create file entries
                std::vector<FatFileEntry> newFolderEntry;
                createFatFileEntry(foldername, newFolderEntry);
                newFolderEntry.back().msdos.attributes = MSDOS_DIRECTORY;
                uint32_t allocatedClusterNumber = image.allocateNewCluster();
                newFolderEntry.back().msdos.eaIndex = (allocatedClusterNumber & 0xFFFF0000) >> 16;
                newFolderEntry.back().msdos.firstCluster = allocatedClusterNumber & 0x0000FFFF;
                newFolderEntry.back().msdos.fileSize = 0x00;

                // create dot entries
                cachedClusterNumber = allocatedClusterNumber;
                image.readDataCluster(cachedCluster, cachedClusterNumber);
                // create single dot entry
                cachedCluster[0] = newFolderEntry.back();
                cachedCluster[0].msdos.filename[0] = MSDOS_DOT_ENTRY;
                for (uint8_t i = 1; i < 8; i++) {
                    cachedCluster[0].msdos.filename[i] = ' ';
                }
                for (uint8_t i = 0; i < 2; i++) {
                    cachedCluster[0].msdos.extension[i] = ' ';
                }
                // create double dot entry
                if ((uint32_t) ((parentFolderEntry.msdos.eaIndex << 16) | parentFolderEntry.msdos.firstCluster) == image.BPB.extended.RootCluster) {
                    cachedCluster[1] = cachedCluster[0];
                    cachedCluster[1].msdos.filename[0] = MSDOS_DOT_ENTRY;
                    cachedCluster[1].msdos.filename[1] = MSDOS_DOT_ENTRY;
                    cachedCluster[1].msdos.eaIndex = 0x0000;
                    cachedCluster[1].msdos.firstCluster = 0x0000;
                } else {
                    cachedCluster[1] = parentFolderEntry;
                    cachedCluster[1].msdos.filename[0] = MSDOS_DOT_ENTRY;
                    cachedCluster[1].msdos.filename[1] = MSDOS_DOT_ENTRY;
                    for (uint8_t i = 2; i < 8; i++) {
                        cachedCluster[1].msdos.filename[i] = ' ';
                    }
                    for (uint8_t i = 0; i < 2; i++) {
                        cachedCluster[1].msdos.extension[i] = ' ';
                    }
                }
                // update cluster
                image.writeDataCluster(cachedCluster, cachedClusterNumber);

                // allocate new space in the parent directory
                cachedClusterNumber = (parentFolderEntry.msdos.eaIndex << 16) | parentFolderEntry.msdos.firstCluster;
                while (!IS_FAT_EOC(cachedClusterNumber)) {

                    // read the cluster
                    image.readDataCluster(cachedCluster, cachedClusterNumber);

                    // read the next cluster, allocate a new one (but not update FAT) if EOC
                    uint32_t nextClusterNumber = image.getFATEntry(cachedClusterNumber) & 0x0FFFFFFF;
                    if (IS_FAT_EOC(nextClusterNumber)) {
                        image.readEmptyCluster(cachedCluster + image.nFileEntriesPerCluster);
                    } else {
                        image.readDataCluster(cachedCluster + image.nFileEntriesPerCluster, nextClusterNumber);
                    }

                    // search for available space for the entries of new file
                    size_t i;
                    bool available;
                    for (i = 0; i <= 2 * image.nFileEntriesPerCluster - newFolderEntry.size(); i++) {
                        available = true;
                        for (size_t j = 0; j < newFolderEntry.size(); j++) {
                            if ((
                                cachedCluster[i + j].msdos.filename[0] == MSDOS_EMPTY_ENTRY    ||
                                cachedCluster[i + j].msdos.filename[0] == MSDOS_DELETED1_ENTRY ||
                                cachedCluster[i + j].msdos.filename[0] == MSDOS_DELETED2_ENTRY
                            ) == false) {
                                available = false;
                                i += j;
                                break;
                            }
                        }
                        if (available) {
                            break;
                        }
                    }

                    if (available) {
                        // fill available entries
                        for (size_t j = 0; j < newFolderEntry.size(); j++) {
                            cachedCluster[i + j] = newFolderEntry[j];
                        }
                        // update the first cluster
                        image.writeDataCluster(cachedCluster, cachedClusterNumber);
                        // update the second cluster if needed
                        if (i + newFolderEntry.size()) {
                            // allocate new cluster if needed
                            if (IS_FAT_EOC(nextClusterNumber)) {
                                image.allocateNewClusterAtTheEnd(cachedClusterNumber);
                                nextClusterNumber = image.getFATEntry(cachedClusterNumber) & 0x0FFFFFFF;
                            }
                            image.writeDataCluster(cachedCluster + image.nFileEntriesPerCluster, nextClusterNumber);
                        }
                        break;
                    } else {
                        cachedClusterNumber = nextClusterNumber;
                    }
                }

                // TODO: update modification datetime of parent folder
            }
        }
    }

    void touch(std::vector<std::string>& path, std::string& filename)
    {
        FatFileEntry parentFolderEntry;
        std::vector<std::string> absolutePath;
        if (locatePath(path, &parentFolderEntry, &absolutePath) == true) {
            if (parentFolderEntry.msdos.attributes & MSDOS_DIRECTORY) {
                // create file entries
                std::vector<FatFileEntry> newFileEntry;
                createFatFileEntry(filename, newFileEntry);
                newFileEntry.back().msdos.attributes = MSDOS_ARCHIVE;
                newFileEntry.back().msdos.eaIndex = 0x00;
                newFileEntry.back().msdos.firstCluster = 0x00;
                newFileEntry.back().msdos.fileSize = 0x00;

                // allocate new space in the parent directory
                cachedClusterNumber = (parentFolderEntry.msdos.eaIndex << 16) | parentFolderEntry.msdos.firstCluster;
                while (!IS_FAT_EOC(cachedClusterNumber)) {

                    // read the cluster
                    image.readDataCluster(cachedCluster, cachedClusterNumber);

                    // read the next cluster, allocate a new one (but not update FAT) if EOC
                    uint32_t nextClusterNumber = image.getFATEntry(cachedClusterNumber) & 0x0FFFFFFF;
                    if (IS_FAT_EOC(nextClusterNumber)) {
                        image.readEmptyCluster(cachedCluster + image.nFileEntriesPerCluster);
                    } else {
                        image.readDataCluster(cachedCluster + image.nFileEntriesPerCluster, nextClusterNumber);
                    }

                    // search for available space for the entries of new file
                    size_t i;
                    bool available;
                    for (i = 0; i <= 2 * image.nFileEntriesPerCluster - newFileEntry.size(); i++) {
                        available = true;
                        for (size_t j = 0; j < newFileEntry.size(); j++) {
                            if ((
                                cachedCluster[i + j].msdos.filename[0] == MSDOS_EMPTY_ENTRY    ||
                                cachedCluster[i + j].msdos.filename[0] == MSDOS_DELETED1_ENTRY ||
                                cachedCluster[i + j].msdos.filename[0] == MSDOS_DELETED2_ENTRY
                            ) == false) {
                                available = false;
                                i += j;
                                break;
                            }
                        }
                        if (available) {
                            break;
                        }
                    }

                    if (available) {
                        // fill available entries
                        for (size_t j = 0; j < newFileEntry.size(); j++) {
                            cachedCluster[i + j] = newFileEntry[j];
                        }
                        // update the first cluster
                        image.writeDataCluster(cachedCluster, cachedClusterNumber);
                        // update the second cluster if needed
                        if (i + newFileEntry.size()) {
                            // allocate new cluster if needed
                            if (IS_FAT_EOC(nextClusterNumber)) {
                                image.allocateNewClusterAtTheEnd(cachedClusterNumber);
                                nextClusterNumber = image.getFATEntry(cachedClusterNumber) & 0x0FFFFFFF;
                            }
                            image.writeDataCluster(cachedCluster + image.nFileEntriesPerCluster, nextClusterNumber);
                        }
                        break;
                    } else {
                        cachedClusterNumber = nextClusterNumber;
                    }
                }

                // TODO: update modification datetime of parent folder
            }
        }
    }

    void mv(std::vector<std::string>& path1, std::string& filename, std::vector<std::string>& path2)
    {
    }

    void cat(std::vector<std::string>& path)
    {
        FatFileEntry fileEntry;
        std::vector<std::string> absolutePath;
        if (locatePath(path, &fileEntry, &absolutePath) == true) {
            if ((fileEntry.msdos.attributes & MSDOS_ARCHIVE) && fileEntry.msdos.fileSize > 0) {
                char* cachedFileCluster = (char*) cachedCluster;
                uint32_t cachedFileClusterNumber = (fileEntry.msdos.eaIndex << 16) | fileEntry.msdos.firstCluster;
                while (!IS_FAT_EOC(cachedFileClusterNumber)) {
                    image.readDataCluster(cachedFileCluster, cachedFileClusterNumber);
                    cachedFileCluster[image.BPB.BytesPerSector * image.BPB.SectorsPerCluster] = '\0';
                    std::cout << cachedFileCluster;
                    cachedFileClusterNumber = image.getFATEntry(cachedFileClusterNumber) & 0x0FFFFFFF;
                }
            }
        }
    }
};


#endif // HW3_EXPLORER_HPP
