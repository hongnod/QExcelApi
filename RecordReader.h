#ifndef RECORDS_H
#define RECORDS_H
#include <QtCore>

namespace ExceLite
{
   struct ORDER
   {
     typedef enum tagORDER
     {
        RO_UAV           =  0x0000,
        RO_ARRAY        =  0x0221,
        RO_BLANK        =  0x0201,
        RO_BOOLERR      =  0x0205,
        RO_FORMULA      =  0x0006,
        RO_LABEL        =  0x0018,
        RO_LABELSST     =  0x00FD,
        RO_NUMBER       =  0x0203,
        RO_MULBLANK     =  0x00BE,
        RO_MULRK        =  0x00BD,
        RO_RK           =  0x027E,
        RO_RSTRING      =  0x00D6,
        RO_SHRFMLA      =  0x04BC,
        RO_1904         =  0x0022,
        RO_BOF          =  0x0809,
        RO_BOUNDSHEET   =  0x0085,
        RO_COLINFO      =  0x007D,
        RO_CONTINUE     =  0x003C,
        RO_DBCELL       =  0x00D7,
        RO_DEFCOLWIDTH  =  0x0055,
        RO_DIMENSIONS   =  0x0200,
        RO_EOF          =  0x000A,
        RO_EXTSST       =  0x00FF,
        RO_FILEPASS     =  0x002F,
        RO_FONT         =  0x0031,
        RO_FORMAT       =  0x041E,
        RO_HLINK        =  0x01B8,
        RO_INDEX        =  0x020B,
        RO_MERGECELLS   =  0x00E5,
        RO_NAME         =  0x0218,
        RO_NOTE         =  0x001C,
        RO_PALETTE      =  0x0092,
        RO_ROW          =  0x0208,
        RO_SST          =  0x00FC,
        RO_STANDARDWIDTH=  0x0099,
        RO_STRING    = 0x0207,
        RO_TXO          =  0x01B6,
        RO_XF           =  0x00E0,
        RO_WINDOW2 = 0x023E,
        RO_SELECTION = 0x001D,
        RO_PHONETICPR = 0x00EF,//This record contains default settings for the “Asian Phonetic Settings” dialog and the addresses of all cells which show Asian phonetic text.
        RO_FEATHEADR = 0x0867,
     }RECORDORDER;
   };

   struct STREAMTYPE
   {
      typedef enum _tagStreamType
      {
         GLOBALS_SUBSTREAM=0x0005,
         VBM_SUBSTREAM=0x0006,
         WORKSHEET_OR_DIALOG_SUBSTREAM=0x0010,//If the fDialog field in that WsBool is 1 then the sheet (1) is dialog sheet otherwise the sheet (1) is a worksheet.
         CHART_SUBSTREAM=0x0020,
       }TYPEID;
   };



   struct CELLTYPE
   {
      typedef enum _tagCellDataType
      {
          BLANK=0x00,
          INT =0x01,
          FLOAT = 0x02,
          TEXT  = 0x03,
          ERROR = 0x04,
          BOOL = 0x05,
      } typeCELL;
   };

   struct CELLERROR
   {
      typedef enum _tagCellError
      {
          ERR_NULL=0x00,
          ERR_DIV  =0x07,
          ERR_VALUE = 0x0F,
          ERR_REF     = 0x17,
          ERR_NAME     = 0x1D,
          ERR_NUM     = 0x24,
          ERR_NA     = 0x2A,
      } CELLTYPE;
   };
   class RecordIO
   {
    public:
      RecordIO();
      ~RecordIO();

      qint16 order(){return _order;};
      qint16 size(){return _size;};
      QDataStream::ByteOrder byteOrder(){return _byteorder;};
      qint64 seekToBegin(qint64 beginpos);
      qint64 goNext();
      qint64 SkipRecord();
      bool   ReadRawRecord(QByteArray &data);
      bool LoadCompoundFile(QString xlsfilename);
     private:
      QTemporaryFile         _tmpDataFile;
      qint16                     _order;
      qint16                     _size;   //0<=_size<=8224
      QDataStream::ByteOrder   _byteorder;
   };

    struct Bof
   {
      qint16 vers;
      qint16 dt;
      qint16 rupBuild;
      qint16 rupYear;
      qint32 bfh;
      qint32 sfo;
      bool ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder=QDataStream::LittleEndian);
   };

   class biff8String
   {
     public:
       biff8String();
       void reset();
       QString ParseString(QDataStream& dqs,bool len2bytes=true);
       QString innerstr;
       bool isFinished;
     private:
       qint16  cch;        //o0s2 Count of characters in the string
       qint8   grbit;      //o2s1 Option flag
       qint16  crun;       //o3s2 if fRichSt=1
       qint32  cchExtRst;   //o3s4 if fRichSt=0,o5s4 fRichSt=1 Length of ExtRst data
       qint16  cch_read;
       qint16  restbytes;
       //QString innerstr;
       QVector<qint32> rgSTRUN;
       bool     fHighByte;
       bool     fExtSt;
       bool     fRichSt;
   };
  static double ieee754todec(quint64& in)
   {
      union
      {
          quint64 _intval;
          double    _double;
      }intdouble;
      intdouble._intval=in;
      return intdouble._double;
   }
}

#endif // RECORDS_H
