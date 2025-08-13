#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <functional>
using namespace std;

// ZIP file structures
#pragma pack(push, 1)
struct ZipLocalFileHeader {
    uint32_t signature;          // 0x04034b50
    uint16_t version;
    uint16_t flags;
    uint16_t compression;
    uint16_t modTime;
    uint16_t modDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t filenameLength;
    uint16_t extraFieldLength;
};

struct ZipCentralDirectoryHeader {
    uint32_t signature;          // 0x02014b50
    uint16_t versionMadeBy;
    uint16_t versionNeeded;
    uint16_t flags;
    uint16_t compression;
    uint16_t modTime;
    uint16_t modDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t filenameLength;
    uint16_t extraFieldLength;
    uint16_t commentLength;
    uint16_t diskNumber;
    uint16_t internalAttributes;
    uint32_t externalAttributes;
    uint32_t relativeOffset;
};

struct ZipEndOfCentralDirectory {
    uint32_t signature;          // 0x06054b50
    uint16_t diskNumber;
    uint16_t centralDirDisk;
    uint16_t numEntriesThisDisk;
    uint16_t numEntries;
    uint32_t centralDirSize;
    uint32_t centralDirOffset;
    uint16_t commentLength;
};
#pragma pack(pop)


// Structure to represent a package dependency
struct Package {
    string name;
    string version;
    string source;
    string installDate;
    vector<string> dependencies;
};
