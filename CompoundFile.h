#ifndef QCompoundFile_H
#define QCompoundFile_H
#include <QtCore>



namespace CompoundFile
{
    const quint64 OLE2MAGICTAG = 0xE11AB1A1E011CFD0;
    const qint32 MAXREGSECT   = 0xFFFFFFFA;//Maximum regular sector number
    const qint32 DIFSECT      = 0xFFFFFFFC;//Specifies a DIFAT sector in the FAT
    const qint32 FATSECT      = 0xFFFFFFFD;//Specifies a FAT sector in the FAT
    const qint32 ENDOFCHAIN   = 0xFFFFFFFE;//End of linked chain of sectors
    const qint32 FREESECT     = 0xFFFFFFFF;//Specifies unallocated sector in the FAT, Mini FAT, or DIFAT
 typedef quint64 TIME_T;
 typedef qint32  SECTID;
 typedef enum tagSTGTY
 {
    STGTY_INVALID = 0,
    STGTY_STORAGE = 1,
    STGTY_STREAM = 2,
    STGTY_LOCKBYTES = 3,
    STGTY_PROPERTY = 4,
    STGTY_ROOT = 5,
  } STGTY;
 typedef enum tagDECOLOR
 {
    DE_RED = 0,
    DE_BLACK = 1,
  } DECOLOR;

 typedef  enum  tagRESULT
 {
    SUCCESS=1,
    FILE_NOT_FOUND               = -1,
    NOT_COMPOUND_FILE         = -2,
    STREAM_NAME_TOO_LONG   =-3,
    STREAM_NOT_FOUND           =-4,
    DIRECTORY_NOT_EMPTY       =-5,
    SECTOR_NOT_SUPPORT         =-6,
    MINISECTOR_NOT_SUPPORT   =-7,
    BYTEORDER_NOT_SUPPORT    =-8,
  } RESULTCODE;

 class QCompoundFile
 {
  public:
    QCompoundFile();
    QCompoundFile(QString file);
    ~QCompoundFile();
    bool               Open(QString file);
    void              Close();
    bool                ReadStream(QString streamName,QString storage, QByteArray& data);
    bool            ReadStream(QString streamName,QString storage,QFile& tFile);
    QDataStream::ByteOrder     byteOrder(){return ByteOrder;}
/*
#ifdef _DEBUGCOMPDOC
     void PRINT_HEADER();
     void PRINT_SECTOR();
     void PRINT_FATSECTID();
     void PRINT_FAT();
     void PRINT_MINIFAT();
     void PRINT_DIRENTRIES();
     void PRINT_DATA();
#endif
*/
 protected:
  class Header //A single sector with fields needed to read the other structures of the compound file. This sector must be at file offset 0

  {
   public:
     Header();
     RESULTCODE  FromStream(QDataStream& buf);

     quint64 	    HeaderSignature;
     QUuid	    HeaderCLSID;
     quint16 	    MinorVersion;
     quint16 	    MajorVersion;
     quint16 	    ByteOrder;
     quint16 	    SectorShift;
     quint16 	    MiniSectorShift;
     quint16 	    Reserved2Bytes1st;
     quint32 	    Reserved4Bytes2nd;
     quint32 	    NumberofDirectorySectors;//_csectDirectory
     quint32 	    NumberofFATSectors;
     qint32 	    FirstDirectorySectorLocation;
     quint32 	    TransactionSignatureNumber;
     quint32 	    MiniStreamCutoffSize;
     qint32 	    FirstMiniFATSectorLocation;
     quint32 	    NumberofMiniFATSectors;
     qint32 	    FirstDIFATSectorLocation;
     quint32 	    NumberofDIFATSectors;
     QVector<SECTID>   DIFATinHeader;			// Array of block indices constituting the Block Allocation Table (BAT) (0x004C, 0x0050, 0x0054 ... 0x01FC)

   private:
     void Initialize();
   };
  Header _header;
  class DirectoryEntry
  {
    public:
      DirectoryEntry();
      RESULTCODE  FromStream(QDataStream& buf);
      QString            DirectoryEntryName;
      qint16             DirectoryEntryNameLength;
      quint8             ObjectType;
      quint8             ColorFlag;
      qint32             LeftSiblingID;
      qint32             RightSiblingID;
      qint32             ChildID;
      QUuid              DirectoryEntryCLSID;
      quint32            StateBits;
      TIME_T             CreationTime;
      TIME_T             ModifiedTime;
      qint32            StartingSectorLocation;
      quint64            StreamSize;
      //quint32            _dptPropType;
      QString            Path;
      qint16             DID;
   };

  private:
   QString                  FileName;
   QFile                    FileHandle;
   quint16                  BytesPerSector;
   quint16                  BytesPerMiniSector;
   quint16                  SECTIDsPerSector;   //Sectorid per Sector can store
   quint16                  DirectoryEntriesPerSector;    //Directory entries per Sector can have
   QDataStream::ByteOrder   ByteOrder;    //Byte_Order the CopoundDoc used

   QVector<SECTID>         DIFATs;      //ids of Sectors that used by Fat
   QVector<SECTID>         FATs;          //Fat list
   QVector<SECTID>         MiniFATs;
   QVector<DirectoryEntry>  DirectoryEntries;
   RESULTCODE  ReadHeader();
   bool      ReadOneSector(qint32 sectID, QByteArray& bytes);
   int       ReadSectorChains(qint32 startSectID,QByteArray& bytes);
   int       ReadSectorChains(qint32 startSectID,QFile& file);
   bool      LoadFatSectorIds();
   bool      LoadFAT();
   bool      LoadMiniFAT();
   bool      LoadOneDirectoryEntry(qint32 did,QString path,QByteArray sector);
   bool      LoadDirectoryEntries();
 };
}
#endif // QCompoundFile_H
