#ifndef RECORD_PARSER_H
#define RECORD_PARSER_H
#include <QtCore>
#include "RecordReader.h"
/*
  datetime stored as double number,the integer part represent days offset to 1990-1-1,
  the fraction part represent seconds;
  total seconds=(fraction+0.0000001)*86400
                                $totalseconds = floor(SPREADSHEET_EXCEL_READER_MSINADAY * $fractionalDay);
                                $secs = $totalseconds % 60;
                                $totalseconds -= $secs;
                                $hours = floor($totalseconds / (60 * 60));
                                $mins = floor($totalseconds / 60) % 60;
  */


namespace ExceLite
{
    struct Font
    {
        qint16 dyHeight;    //o4s2 height of the font(1/20 of a point)
        qint16 grbit;       //o6s2 font attributes
        qint16 icv;         //o8s2 index to color palette
        qint16 bls;         //o10s2 Bold style
        qint16 sss;         //o12s2 00=None,01=Superscript,02=subscript
        qint8  uls;         //o14s1 Underline style 00=None 01=Single 02=Double 21=Single Accounting 22=Double Accounting
        qint8  bFamily;     //o15s1 font family
        qint8  bCharSet;    //o16s1 Character Set
        qint8  Reserved;    //o17s1 must be 0
        //qint8  cch;         //o18s1 Length of the font name
        QString fontName;   //o19sv
        bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
    };

    struct Sst
    {
      bool ParseRecord(QByteArray& rawbytes,QStringList& list,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
      bool AppendContinue(QByteArray& continuebytes,QStringList& list,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
      qint32  cstTotal;   //o4s4 Total number of strings in sst and extsst
      qint32  cstUnique;  //o8s4 Number of unique strings in sst
      private:
       bool sharedRead(QDataStream &odqs,QStringList& list);
       biff8String curStr;
    };

    struct Boundsheet
    {
        bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
        qint32 lbPlyPos;
        qint16 grbit;

        QString name;
    };

    struct Format
    {
        bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
        qint16 ifmt;
        QString formating;
    };

    struct Xf
    {
     public:
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
      qint16  ifnt;   //4
      qint16  ifmt;  //6
      qint16  fLocked; //8
      qint16  alc; //10
      qint16  cIndent; //12
      qint16  dgLeft;//14
      qint16  icvLeft; //16
      qint32  icvTop; //18
      qint16  icvFore;//22
    };


   struct DIMENSIONS//YES
  {
      quint32 rwMic;
      quint32 rwMac;
      quint16 colMic;
      quint16 colMac;
      quint16 reserv;
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct REF
  {
      quint16 rwFirst;
      quint16 rwLast;
      quint16 colFirst;
      quint16 colLast;
  };
  struct ARRAY//NO
  {
      quint16    rwFirst;
      quint16    rwLast;
      quint8     colFirst;
      quint8     colLast;
      quint8     grbit;
      quint32   chn;
      quint16   cce;
      QString rgce;//Parsed formula expression
    //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QStringList& list,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct BLANK//YES
  {
      quint16     rw;
      quint16     col;
      quint16     ixfe;
    //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct BOOLERR//NO
  {
      quint16     rw;
      quint16     col;
      quint16     ixfe;
      quint8      bBoolErr;
      quint8      fError;
      //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct FORMULA//part
  {
      quint16     rw;
      quint16     col;
      quint16     ixfe;
      double    num;
      quint16     grbit;
      quint32     chn;
      quint16     cce;
      //QString    rgce;//Parsed formula expression,not implemented current
      bool        booleanvalue;
      quint8       errorvalue;
      quint8       result_type;

      //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct LABEL//no
  {
      quint16     rw;
      quint16     col;
      quint16     ixfe;
      quint16     cch;
      quint8     grbit;
      QByteArray rgb;//Array of string characters
      //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct LABELSST//yes
  {
      quint16     rw;
      quint16     col;
      quint16     ixfe;
      quint32     isst;
      //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct NUMBER//yes
  {
      quint16     rw;
      quint16     col;
      quint16     ixfe;
      double     num;//Floating-point number value
      //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct MULBLANK//yes
  {
      quint16     rw;
      quint16     colFirst;
      QVector<qint16> rgixfe;//Array of indexes to XF records
      quint16     colLast;
      //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  typedef struct rkrec
  {
    quint16 ixfe; /* index to XF record */
    qint32 rk; /* RK number */
    double num;
  } RKREC;
  static double NumFromRk(qint32 rk)
  {
      double num;
      if(rk&0x02)
       {
           num=(double)(rk>>2);
       }
       else
       {
           *((qint32*)&num+1)=rk&0xfffffffc;
           *((qint32*)&num)=0;
       }
       if(rk&0x01)
       {
           num/=100;
       }
       return num;
   }
 struct MULRK//yes
 {
      quint16     rw;
      quint16     colFirst;
      QVector<RKREC> rgrkrec;
      quint16     colLast;
      //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct RK//分数，yes
  {
      quint16     rw;
      quint16     col;
      quint16     ixfe;
      qint32     rk;
      double    num;
      //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct SHRFMLA//no
  {
      quint16    rwFirst;
      quint16    rwLast;
      quint8     colFirst;
      quint8     colLast;
      qint16    Reserved;
      qint16    cce;
      QString  rgce;
      //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct STRING//yes
  {
      qint16   cch;
      qint8    grbit;
      QString rgch;
      //Access Interface
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };

  struct MERGECELLS//yes
  {
      qint16 cmcs;
      QVector<REF> rgRef;
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
  struct ROW
  {
      quint16 rw;
      quint16 colMic;
      quint16 colMac;
      qint16 miyRw;
      qint16 irwMac;
      qint16 reserv;
      qint16 grbit;
      qint16 ixfe;
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
  };
}
#endif // RECORD_PARSER_H
