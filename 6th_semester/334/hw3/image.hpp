#ifndef HW3_IMAGE_HPP
#define HW3_IMAGE_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "fat32.h"

// FatFileEntry83 State
#define MSDOS_EMPTY_ENTRY       0x00
#define MSDOS_DELETED1_ENTRY    0x05
#define MSDOS_DOT_ENTRY         0x2E
#define MSDOS_DELETED2_ENTRY    0xE5

// FatFileEntry Attributes
#define MSDOS_READ_ONLY         0x01
#define MSDOS_HIDDEN            0x02
#define MSDOS_SYSTEM            0x04
#define MSDOS_VOLUME_ID         0x08
#define MSDOS_DIRECTORY         0x10
#define MSDOS_ARCHIVE           0x20
#define MSDOS_LFN               (MSDOS_READ_ONLY | MSDOS_HIDDEN | MSDOS_SYSTEM | MSDOS_VOLUME_ID)

// FAT Special Values
#define FAT_EMPTY               0x00000000
#define FAT_BAD                 0x0FFFFFF7
#define FAT_EOC                 0x0FFFFFFF
#define IS_FAT_EOC(f)           ((f) >= 0x0FFFFFF0)


class Image
{
public:
    char* imgFileName;
    std::fstream imgFile;
    BPB_struct BPB;
    uint32_t nFATEntriesPerCluster;
    uint32_t nFileEntriesPerCluster;
    uint32_t rootSector;
    uint32_t nDataCluster;
    FatFileEntry rootFileEntry;
    uint32_t* FAT;
    uint32_t iLastEmptyFatEntry;
    static const char* months[];
    static const char hexDigits[];

    Image(char* _imgFileName) : imgFileName(_imgFileName)
    {
        imgFile.open(imgFileName, std::ios::in | std::ios::out | std::ios::binary);
        imgFile.seekg(0, imgFile.beg);
        imgFile.read((char*) &BPB, sizeof(BPB));

        nFATEntriesPerCluster = BPB.BytesPerSector * BPB.SectorsPerCluster / 4;
        nFileEntriesPerCluster = BPB.BytesPerSector * BPB.SectorsPerCluster / sizeof(FatFileEntry);
        rootSector = BPB.ReservedSectorCount + BPB.NumFATs * BPB.extended.FATSize;
        nDataCluster = (BPB.TotalSectors32 - rootSector) / BPB.SectorsPerCluster;

        for (uint32_t i = 0; i < 8; i++) {
            rootFileEntry.msdos.filename[i] = ' ';
        }
        for (uint32_t i = 0; i < 3; i++) {
            rootFileEntry.msdos.extension[i] = ' ';
        }
        rootFileEntry.msdos.attributes = MSDOS_DIRECTORY;
        rootFileEntry.msdos.eaIndex = (BPB.extended.RootCluster & 0xFFFF0000) >> 16;
        rootFileEntry.msdos.firstCluster = BPB.extended.RootCluster & 0x0000FFFF;

        FAT = new uint32_t[nDataCluster * 4];
        readFAT(1);
        iLastEmptyFatEntry = BPB.extended.RootCluster;
    }

    ~Image()
    {
        imgFile.close();
        delete[] FAT;
        FAT = nullptr;
    }

    void readFAT(uint8_t i = 1)
    {
        imgFile.seekg((BPB.ReservedSectorCount + (i - 1) * BPB.extended.FATSize) * BPB.BytesPerSector, imgFile.beg);
        imgFile.read((char*) FAT, nDataCluster * 4);
    }

    void writeFAT(uint8_t i = 1)
    {
        imgFile.seekg((BPB.ReservedSectorCount + (i - 1) * BPB.extended.FATSize) * BPB.BytesPerSector, imgFile.beg);
        imgFile.write((char*) FAT, nDataCluster * 4);
    }

    void writeAllFATs()
    {
        for (uint8_t i = 1; i <= BPB.NumFATs; i++) {
            writeFAT(i);
        }
    }

    uint32_t getFATEntry(uint32_t index)
    {
        uint32_t value = FAT_BAD;
        if (index >= BPB.extended.RootCluster && index < BPB.extended.RootCluster + nDataCluster) {
            imgFile.seekg(BPB.ReservedSectorCount * BPB.BytesPerSector + index * 4, imgFile.beg);
            imgFile.read((char*) &value, 4);
        }
        return value;
    }

    void updateFATEntry(uint32_t index, uint32_t value)
    {
        if (index >= BPB.extended.RootCluster && index < BPB.extended.RootCluster + nDataCluster) {
            for (uint8_t i = 0; i < BPB.NumFATs; i++) {
                imgFile.seekg((BPB.ReservedSectorCount + i * BPB.extended.FATSize) * BPB.BytesPerSector + index * 4, imgFile.beg);
                imgFile.write((char*) &value, 4);
            }
        }
    }

    void readDataCluster(void* buffer, uint32_t i)
    {
        if (i >= BPB.extended.RootCluster && i < BPB.extended.RootCluster + nDataCluster) {
            imgFile.seekg((rootSector + (i - BPB.extended.RootCluster) * BPB.SectorsPerCluster) * BPB.BytesPerSector, imgFile.beg);
            imgFile.read((char*) buffer, BPB.SectorsPerCluster * BPB.BytesPerSector);
        }
    }

    void writeDataCluster(void* buffer, uint32_t i)
    {
        if (i >= BPB.extended.RootCluster && i < BPB.extended.RootCluster + nDataCluster) {
            imgFile.seekg((rootSector + (i - BPB.extended.RootCluster) * BPB.SectorsPerCluster) * BPB.BytesPerSector, imgFile.beg);
            imgFile.write((char*) buffer, BPB.SectorsPerCluster * BPB.BytesPerSector);
        }
    }

    void readEmptyCluster(void* buffer)
    {
        for (uint32_t i = 0; i < nFileEntriesPerCluster; i++) {
            ((FatFileEntry*) buffer)[i].msdos.filename[0] = MSDOS_EMPTY_ENTRY;
        }
    }

    uint32_t allocateNewCluster()
    {
        while ((getFATEntry(iLastEmptyFatEntry) & 0x0FFFFFFF) != FAT_EMPTY) {
            iLastEmptyFatEntry++;
            if (iLastEmptyFatEntry >= BPB.extended.RootCluster + nDataCluster) {
                iLastEmptyFatEntry = BPB.extended.RootCluster;
            }
        }
        updateFATEntry(iLastEmptyFatEntry & 0x0FFFFFFF, FAT_EOC);

        uint32_t nFreeClusters;
        imgFile.seekg(BPB.extended.FSInfo * BPB.BytesPerSector + 488, imgFile.beg);
        imgFile.read((char*) &nFreeClusters, 4);
        nFreeClusters--;
        imgFile.seekg(BPB.extended.FSInfo * BPB.BytesPerSector + 488, imgFile.beg);
        imgFile.write((char*) &nFreeClusters, 4);

        return iLastEmptyFatEntry;
    }

    void allocateNewClusterAtTheEnd(uint32_t i)
    {
        uint32_t nextClusternumber = getFATEntry(i) & 0x0FFFFFFF;
        while (!IS_FAT_EOC(nextClusternumber)) {
            i = nextClusternumber;
            nextClusternumber = getFATEntry(i) & 0x0FFFFFFF;
        }
        while ((getFATEntry(iLastEmptyFatEntry) & 0x0FFFFFFF) != FAT_EMPTY) {
            iLastEmptyFatEntry++;
            if (iLastEmptyFatEntry >= BPB.extended.RootCluster + nDataCluster) {
                iLastEmptyFatEntry = BPB.extended.RootCluster;
            }
        }
        updateFATEntry(i, iLastEmptyFatEntry & 0x0FFFFFFF);
        updateFATEntry(iLastEmptyFatEntry & 0x0FFFFFFF, FAT_EOC);

        uint32_t nFreeClusters;
        imgFile.seekg(BPB.extended.FSInfo * BPB.BytesPerSector + 488, imgFile.beg);
        imgFile.read((char*) &nFreeClusters, 4);
        nFreeClusters--;
        imgFile.seekg(BPB.extended.FSInfo * BPB.BytesPerSector + 488, imgFile.beg);
        imgFile.write((char*) &nFreeClusters, 4);
    }
};


const char* Image::months[] = {
    "January", "February", "March",
    "April",   "May",      "June",
    "July",    "August",   "September",
    "October", "November", "December"
};


const char Image::hexDigits[] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'
};


#endif // HW3_IMAGE_HPP
