#include "RecordReader.h"
#include "CompoundFile.h"
using namespace  CompoundFile;
using namespace  ExceLite;

  RecordIO::RecordIO()
          : _byteorder(QDataStream::LittleEndian),
          _order(0),
          _size(0)
  {
     _tmpDataFile.open();
     _tmpDataFile.setAutoRemove(true);
  }
  RecordIO::~RecordIO()
  {
  }

  qint64 RecordIO::goNext()
  {
      QDataStream qs(_tmpDataFile.read(4));
      qs.setByteOrder(_byteorder);
      qs >> _order
          >> _size;
      return _tmpDataFile.pos();
  }

  qint64 RecordIO::SkipRecord()
  {
   _tmpDataFile.seek( _tmpDataFile.pos()+_size);
   return _tmpDataFile.pos();
  }

 qint64 RecordIO::seekToBegin(qint64 beginpos)
 {
     _tmpDataFile.seek(beginpos);
     return _tmpDataFile.pos();
 }

  bool   RecordIO::ReadRawRecord(QByteArray &data)
  {
      data=_tmpDataFile.read(this->_size);
     return true;
  }

  bool RecordIO::LoadCompoundFile(QString xlsfilename)
  {
    QCompoundFile  _xlsFile;
   _xlsFile.Open( xlsfilename);
    _byteorder=_xlsFile.byteOrder();

   _xlsFile.ReadStream(QString::fromAscii("Workbook"),QString::fromAscii("/"),_tmpDataFile);
   _xlsFile.Close();
   _tmpDataFile.seek(0);
   QTemporaryFile test;
   test.open();
   test.write(QByteArray("testing"));
   return true;
  }

   bool   Bof::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
 {
    QDataStream dqs(rawbytes);
    dqs.setByteOrder(byteorder);
    dqs>> vers
               >> dt
              >> rupBuild
              >> rupYear
              >> bfh
              >> sfo;
     return true;
 }

  biff8String::biff8String():cch(-1),grbit(0),fHighByte(false),fExtSt(false),
                                   fRichSt(false),crun(0),cchExtRst(0),cch_read(0),
                                   innerstr(""),rgSTRUN(0),restbytes(0),isFinished(true)
   {
      //init
   }

   void  biff8String::reset()
   {
          //reset to initial values
         cch=-1;
         grbit=0;
         fHighByte=false;
         fExtSt=false;
         fRichSt=false;
         crun=0;
         cchExtRst=0;
         cch_read=0;
         innerstr="";
         rgSTRUN.clear();
         restbytes=0;
         isFinished=true;
    }

   QString  biff8String::ParseString(QDataStream& dqs,bool len2bytes)
   {
     qint8    zippedchar;
     qint16  unzippedchar;
     if(!isFinished)
     {
            if(cch_read<cch)
            {
               qint8 reoption=0;
               dqs >> reoption;
               fHighByte=reoption&0x01;
            }
            while(cch_read<cch)
            {
               if(dqs.atEnd()) break;
               if( fHighByte)
               {
                  dqs >>unzippedchar; innerstr.append(QChar(unzippedchar));cch_read++;
                }
                else
                {
                   dqs >>zippedchar; innerstr.append(QChar(zippedchar));cch_read++;
                }
             }
             if(restbytes > 0)
             {
                int size2end=dqs.device()->size()-dqs.device()->pos();
                if(size2end>=restbytes)
                {
                   dqs.skipRawData(restbytes);
                   restbytes=0;
                }
                else
                {
                   dqs.skipRawData( size2end);
                   restbytes=restbytes-size2end;
                }
             }
             if (cch==cch_read&&restbytes==0)
             {
                isFinished=true;
               // tqs << innerstr<<"\n";
                return innerstr;
             }
             else
             {
                 isFinished=false;return "";
             }
         }

        this->reset();
        if(len2bytes) dqs >> cch;
         else { qint8 cch8;dqs >> cch8; cch=cch8;}

         dqs  >> grbit;
         //tqs << "begin of String:"<<cch <<","<< grbit <<"\n";
        fHighByte = grbit & 0x01;
        fExtSt      = grbit & 0x04;
        fRichSt     = grbit & 0x08;
        if(fRichSt ) dqs >> crun;
        if(fExtSt)   dqs >> cchExtRst;
        while(!dqs.atEnd()&&cch_read!=cch)
        {
            if( fHighByte)
             {
                dqs >>unzippedchar; innerstr.append(QChar(unzippedchar));cch_read++;
            }
            else
            {
                dqs >>zippedchar; innerstr.append(QChar(zippedchar));cch_read++;
            }
        }

       int size2end=dqs.device()->size()-dqs.device()->pos();
       int size2skip=cchExtRst+4*crun;
       if( size2end>=size2skip)
       {
         dqs.skipRawData(size2skip);
         restbytes=0;
       }
        else
        {
          dqs.skipRawData(size2end);
          restbytes=size2skip-size2end;
         }
        if(cch==cch_read&&restbytes==0)
        {
          isFinished=true;
          return innerstr;
        }
        else
        {
           isFinished=false;return "";
        }

   }

