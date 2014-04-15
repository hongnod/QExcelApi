#include "CompoundFile.h"
#include <iostream>

using namespace  CompoundFile;

    QCompoundFile::QCompoundFile():
            FileName(""),
            FATs(0),
            DirectoryEntries(0),
            MiniFATs(0),
            DIFATs(0)
    {
    }

    QCompoundFile::QCompoundFile(QString file):
            FileName(file),
            FATs(0),
            DirectoryEntries(0),
            MiniFATs(0),
            DIFATs(0)
    {
        this->Open(file);
    }

    QCompoundFile::~QCompoundFile()
    {
        this->Close();
    }
    bool QCompoundFile::Open(QString file)
    {
       FileName=file;
       FileHandle.setFileName(FileName);
       FileHandle.open(QIODevice::ReadOnly);
       this->ReadHeader();
       this->LoadFatSectorIds();
       this->LoadFAT();
       this->LoadMiniFAT();
       this->LoadDirectoryEntries();
       return true;
    }

    void QCompoundFile::Close( )
    {
        if(FileHandle.isOpen()) FileHandle.close();
    }
    RESULTCODE QCompoundFile::ReadHeader()
    {
        QDataStream _stream(&FileHandle);
        _stream.device()->seek(0);
        RESULTCODE code=_header.FromStream(_stream);
        if(code!=SUCCESS) return code;
        BytesPerSector     = (1 << _header.SectorShift);                //
        BytesPerMiniSector = (1<< _header.MiniSectorShift);	 //
        if(BytesPerSector%4 ||  BytesPerSector%128) return SECTOR_NOT_SUPPORT;
        SECTIDsPerSector     =  BytesPerSector/4;
        DirectoryEntriesPerSector      =  BytesPerSector/128;
        if(!( _header.ByteOrder == 0xFFFE || _header.ByteOrder == 0xFEFF)) return BYTEORDER_NOT_SUPPORT;
        ByteOrder = _header.ByteOrder== 0xFFFE?QDataStream::LittleEndian:QDataStream::BigEndian;

        return SUCCESS;
    }
/*
#ifdef _DEBUGCOMPDOC
     void QCompoundFile::PRINT_HEADER()
     {
         QFile _f("C:/doc/header.txt");
         _f.open(QIODevice::WriteOnly);
         QTextStream qs(&_f);
         //qs.setByteOrder(ByteOrder);
       qs << _header.HeaderSignature  <<'\n'
            << _header.HeaderCLSID  <<'\n'
            << _header.MinorVersion <<'\n'
            << _header.MajorVersion <<'\n'
            << _header.ByteOrder <<'\n'
            << _header.SectorShift <<'\n'
            << _header.MiniSectorShift <<'\n'
            << _header.Reserved2Bytes1st <<'\n'
            << _header.Reserved4Bytes2nd <<'\n'
            << _header.NumberofDirectorySectors <<'\n'
            << _header.NumberofFATSectors <<'\n'
            << _header.FirstDirectorySectorLocation <<'\n'
            << _header.TransactionSignatureNumber <<'\n'
            << _header.MiniStreamCutoffSize <<'\n'
            << _header.FirstMiniFATSectorLocation <<'\n'
            << _header.NumberofMiniFATSectors <<'\n'
            << _header.FirstDIFATSectorLocation <<'\n'
            << _header.NumberofDIFATSectors <<'\n';
         for(int i=0;i<109;i++)
            qs << _header.DIFATinHeader[i] <<'\n';
     }
     void  QCompoundFile::PRINT_SECTOR()
     {
         QFile _f("C:/doc/sector0.txt");
         _f.open(QIODevice::WriteOnly);
         QDataStream qs(&_f);

         QByteArray _bytes;
         //ReadOneSector(0,_bytes);
         ReadSectorChains(_header.FirstDirectorySectorLocation,_bytes);
         qs.writeRawData(_bytes.data(),_bytes.count());
     }

     void QCompoundFile::PRINT_FATSECTID()
     {
          QFile _f("C:/doc/difs.txt");
          _f.open(QIODevice::WriteOnly);
          QTextStream qs(&_f);
         // qs.setByteOrder(ByteOrder);
          for(int i=0;i<DIFATs.count();i++)
              qs << DIFATs[i]<<'\n';
              qs << "number of difs:" << DIFATs.count();
     }
     void QCompoundFile::PRINT_FAT()
     {
          QFile _f("C:/doc/fat.txt");
          _f.open(QIODevice::WriteOnly);
          QTextStream qs(&_f);
          //qs.setByteOrder(ByteOrder);
          qint32 p;
          //p=_header.FirstDirectorySectorLocation;

          for(int i=0;i<FATs.count(); i++)
          {

              p=FATs[i];qs <<i<<":"<< p << '\n';
          }
     }
     void QCompoundFile::PRINT_MINIFAT()
     {
          QFile _f("C:/doc/minifat.txt");
          _f.open(QIODevice::WriteOnly);
          QDataStream qs(&_f);
          qs.setByteOrder(ByteOrder);
          for(int i=0;i<MiniFATs.count(); i++)
              qs << FATs[i];
     }

     void QCompoundFile::PRINT_DIRENTRIES()
     {
          QFile _f("C:/doc/de.txt");
          _f.open(QIODevice::WriteOnly);
          QTextStream qs(&_f);
          //qs.setByteOrder(ByteOrder);
          for(int i=0;i<DirectoryEntries.count(); i++)
          {
              qs << DirectoryEntries[i].DirectoryEntryName
                 << '\n'
                 << DirectoryEntries[i].DirectoryEntryNameLength
                 << '\n'
                 << DirectoryEntries[i].ObjectType
                 << '\n'
                 <<  DirectoryEntries[i].StartingSectorLocation
                 << '\n'
                 << DirectoryEntries[i].StreamSize
                 << '\n'
                 << DirectoryEntries[i].Path
                 << "\n===========================\n";
          }

     }
     void QCompoundFile::PRINT_DATA()
     {
         QFile _f("C:/doc/data.txt");
         _f.open(QIODevice::WriteOnly);
         QTextStream qs(&_f);
         QByteArray data;
         ReadStream(QString::fromAscii("Workbook"),QString::fromAscii("/"),data);
         qs << data;
     }
#endif
*/
    QCompoundFile::Header::Header():
        HeaderSignature(OLE2MAGICTAG),
        HeaderCLSID(0),
        MinorVersion(0x003E),
        MajorVersion(0x0003),
        ByteOrder(0xFFFE),
        SectorShift(9),
        MiniSectorShift(6),
        Reserved2Bytes1st(0),
        Reserved4Bytes2nd(0),
        NumberofDirectorySectors(0),
        NumberofFATSectors(0),
        FirstDirectorySectorLocation(ENDOFCHAIN),
        TransactionSignatureNumber(0),
        MiniStreamCutoffSize(0x1000),
        FirstMiniFATSectorLocation(ENDOFCHAIN),
        NumberofMiniFATSectors(0),
        FirstDIFATSectorLocation(ENDOFCHAIN),
        NumberofDIFATSectors(0)
    {
        DIFATinHeader.fill(FREESECT,109);
    }

   RESULTCODE QCompoundFile::Header::FromStream(QDataStream& buf)
    {
       buf.setByteOrder(QDataStream::LittleEndian);
       buf >> HeaderSignature
            >> HeaderCLSID
            >> MinorVersion
            >> MajorVersion
            >> ByteOrder
            >> SectorShift
            >> MiniSectorShift
            >> Reserved2Bytes1st
            >> Reserved4Bytes2nd
            >> NumberofDirectorySectors
            >> NumberofFATSectors
            >> FirstDirectorySectorLocation
            >> TransactionSignatureNumber
            >> MiniStreamCutoffSize
            >> FirstMiniFATSectorLocation
            >> NumberofMiniFATSectors
            >> FirstDIFATSectorLocation
            >> NumberofDIFATSectors;
         for(int i=0;i<109;i++)
            buf >> DIFATinHeader[i];
         if(HeaderSignature!=OLE2MAGICTAG) return NOT_COMPOUND_FILE;
         return SUCCESS;
    }

    QCompoundFile::DirectoryEntry::DirectoryEntry():
      DirectoryEntryName(""),
      DirectoryEntryNameLength(0),
      ObjectType(STGTY_STORAGE),
      ColorFlag(DE_BLACK),
      LeftSiblingID(-1),
      RightSiblingID(-1),
      ChildID(-1),
      DirectoryEntryCLSID(0),
      StateBits(0),
      CreationTime(0),
      ModifiedTime(0),
      StartingSectorLocation(ENDOFCHAIN),
      StreamSize(0),
      Path("")
    {

    }

    RESULTCODE  QCompoundFile::DirectoryEntry::FromStream(QDataStream& buf)
    {
        quint16 qc;
        for(int i=0;i<32;i++)
        {
            buf >> qc;
            DirectoryEntryName.append(qc);
        }
        buf >> DirectoryEntryNameLength
           >> ObjectType
           >> ColorFlag
           >> LeftSiblingID
           >> RightSiblingID
           >> ChildID
           >> DirectoryEntryCLSID
           >> StateBits
           >> CreationTime
           >> ModifiedTime
           >> StartingSectorLocation
           >> StreamSize;
           //>> _dptPropType;
        DirectoryEntryName=DirectoryEntryName.left((DirectoryEntryNameLength>>1)-1);
        return SUCCESS;
    }
 //Read a single Sector
    bool QCompoundFile::ReadOneSector(qint32 sectID, QByteArray& bytes)
    {
        FileHandle.seek((1 + sectID) * BytesPerSector);
        bytes=FileHandle.read(BytesPerSector);
        return true;
    }

 // read a chains of Sectors
   int QCompoundFile::ReadSectorChains(qint32 startSectID,QByteArray& bytes)
   {
       qint32 next=startSectID;
       QByteArray tmparray;

       int count=0;
       while(next >=0)
       {
          ReadOneSector(next,tmparray);
           bytes+=tmparray;
           count++;
           next = FATs[next];
       }
       return count*BytesPerSector;
   }

   int QCompoundFile::ReadSectorChains(qint32 startSectID,QFile& file)
   {
       QDataStream qs(&file);
       qs.setByteOrder(ByteOrder);
       qint32 next=startSectID;
       QByteArray tmparray;
       int count=0;
       while(next >=0)
       {
          ReadOneSector(next,tmparray);
          qs.writeRawData(tmparray.data(),tmparray.count());
          count++;
          next = FATs[next];
       }
       return count*BytesPerSector;
   }

 // load Fat Sector Ids both in header and extend Difs
    bool QCompoundFile::LoadFatSectorIds()
    {
       for(int i=0;i<109;i++)
       {
           if(FREESECT == _header.DIFATinHeader[i]) break;
           DIFATs.push_back(_header.DIFATinHeader[i]);
       }

       qint32 next = _header.FirstDIFATSectorLocation; //can't use quint32,if,then compare not working
       qint32 id;
       while(next >= 0) //in the specification it says,dif chain teminated by ENDOFCHAIN,but some software make FREESECT
       {
         QByteArray difArray;
         ReadOneSector(next,difArray);
         QDataStream qs(difArray);
         qs.setByteOrder(ByteOrder);
         for(int i=0;i<SECTIDsPerSector-1;i++)
         {
             qs >> id;
             if(id < 0) break;
             DIFATs.push_back(id);
         }
         qs>>next;   // need to test with very big file
       }
      return true;
    }

    bool QCompoundFile::LoadFAT()
    {
       QByteArray fatArray;
       for(int i=0;i<DIFATs.count();i++)
       {
          fatArray.clear();
           ReadOneSector(DIFATs[i],fatArray);
          QDataStream qs(fatArray);
          qs.setByteOrder(ByteOrder);
          qint32 id;
          for(int sec=0;sec<SECTIDsPerSector;sec++)
          {
              qs >> id;
              FATs.push_back(id);
          }
       }
       return true;
    }

    bool QCompoundFile::LoadMiniFAT()
    {
        QByteArray minifatArray;
        ReadSectorChains(_header.FirstMiniFATSectorLocation,minifatArray);
        QDataStream qs(minifatArray);
        qs.setByteOrder(ByteOrder);
        quint32 id;
        while(!qs.atEnd())
        {
            qs >> id;
            MiniFATs.push_back(id);
        }
        return true;
    }

    bool QCompoundFile::LoadDirectoryEntries()
    {
        QByteArray   dirSectors;
        ReadSectorChains(_header.FirstDirectorySectorLocation,dirSectors);
        LoadOneDirectoryEntry(0,"",dirSectors);
        return true;
    }

    bool QCompoundFile::LoadOneDirectoryEntry(qint32 did,QString path,QByteArray sectors)
    {
        QByteArray tArray;
        tArray = sectors.mid(did*128,128);

        QDataStream qs(tArray);
        qs.setByteOrder(ByteOrder);
        DirectoryEntry de;
        de.FromStream(qs);

        de.DID=did;
        de.Path=path;

        if(de.ObjectType!=STGTY_INVALID)
        {
            DirectoryEntries.append(de);
        }
        if(de.LeftSiblingID>0)  LoadOneDirectoryEntry(de.LeftSiblingID,path,sectors);
        if(de.ChildID>0)
        {
            if(did == 0)
                LoadOneDirectoryEntry(de.ChildID,"/",sectors);
            else
            {
                QString tpath=path;
                tpath.append(de.DirectoryEntryName);

                tpath.append("/");
                LoadOneDirectoryEntry(de.ChildID,tpath,sectors);
            }
        }
        if(de.RightSiblingID>0) LoadOneDirectoryEntry(de.RightSiblingID,path,sectors);


        return true;
    }

    bool QCompoundFile::ReadStream(QString streamName,QString storage, QByteArray& data)
    {
        DirectoryEntry* de = NULL;

        for(int i=0;i<DirectoryEntries.count();i++)
        {
            if( DirectoryEntries[i].DirectoryEntryName.compare(streamName) == 0 && DirectoryEntries[i].Path == storage )
            {de = &DirectoryEntries[i]; break;}
        }

        if(de!=NULL)
        {
            if(de->StreamSize>BytesPerMiniSector)
            {
               if(de->StreamSize!= ReadSectorChains(de->StartingSectorLocation,data))
                {

                std::cout << "Incorrect data size!.....";

                   return false;
                }

            }
             else
             {
                 //to be implemented
             }
        }
        else
        {
            //to be implemented
        }

        return true;
    }

   bool QCompoundFile::ReadStream(QString streamName,QString storage,QFile& tFile)
    {
        DirectoryEntry* de = NULL;

        for(int i=0;i<DirectoryEntries.count();i++)
        {

            if( DirectoryEntries[i].DirectoryEntryName.compare(streamName) == 0 && DirectoryEntries[i].Path == storage )
                de = &DirectoryEntries[i];
        }

        if(de!=NULL)
        {
            if(de->StreamSize>BytesPerMiniSector)
            {
                ReadSectorChains(de->StartingSectorLocation,tFile);
            }
             else
             {
                 //to be implemented
             }
        }
        else
        {

        }
        return true;
    }


