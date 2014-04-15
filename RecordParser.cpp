#include "RecordParser.h"

using namespace  ExceLite;

    bool   Font::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs   >> dyHeight
               >> grbit
               >> icv
               >> bls
               >> sss
               >> uls
               >> bFamily
               >> bCharSet
               >> Reserved;
       biff8String curStr;
       fontName = curStr.ParseString(dqs,false);
       return true;
    }

    bool   Sst::ParseRecord(QByteArray& rawbytes,QStringList& list,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs >> cstTotal >> cstUnique;
       sharedRead(dqs,list);
       return true;
    }

    bool   Sst::sharedRead(QDataStream &dqs,QStringList& list)
    {
        QString rStr;
        while(!dqs.atEnd())
        {
           rStr=curStr.ParseString(dqs);
           if(rStr.count())  list.append(rStr);
        }

        return true;
    }

    bool   Sst::AppendContinue(QByteArray& continuebytes,QStringList& list,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(continuebytes);
       dqs.setByteOrder(byteorder);
       QString rStr=curStr.ParseString(dqs);
       if(rStr.count()) list.append(rStr);

       this->sharedRead(dqs,list);
       return true;
    }

    bool   Boundsheet::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs >> lbPlyPos >> grbit;
       biff8String curStr;

       name=curStr.ParseString(dqs,false);
       return true;
    }

    bool   Format::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs >> ifmt;
       biff8String curStr;
       formating=curStr.ParseString(dqs);
       return true;
    }


    bool   Xf::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs   >> ifnt
               >> ifmt
               >> fLocked
               >> alc
               >> cIndent
               >> dgLeft
               >> icvLeft
               >> icvTop
               >> icvFore;
       return true;
    }

    bool DIMENSIONS::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs >>rwMic >> rwMac >> colMic >> colMac >>reserv;
       return true;
    }
    bool LABELSST::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs >>rw >> col >> ixfe >> isst;
       return true;
    }

    bool RK::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs >>rw >> col >> ixfe >> rk;
       num=NumFromRk(rk);
       return true;
    }
    bool MULRK::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs >>rw >> colFirst;
       qint16 t;
       dqs>>t;
       while(!dqs.atEnd())
       {
           RKREC rkrec;
           dqs>>rkrec.rk;
           rkrec.ixfe=t;
           rkrec.num=NumFromRk(rkrec.rk);
           rgrkrec.append(rkrec);
           dqs>>t;
       }
       colLast=t;
       return true;
    }

    bool BLANK::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs >>rw >> col >> ixfe;
       return true;
    }

    bool FORMULA::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       quint64 tnum;
       dqs >>rw >> col >> ixfe>>tnum >> grbit >> chn >> cce;

       // Parsed expression not implemented
       //qint64 Num=qRound64(num);
       quint16 twohighbytes=(tnum>>48);

       bool isdouble =twohighbytes==0xFFFF;
       if(isdouble)
       {
          result_type=tnum&0xff;
          switch (result_type)
          {
              case 0:
              {
                  //it's a string,see result in following STRING record
              }break;
              case 1:
              {
                 booleanvalue=(tnum>>16)&0xff;
              }break;
              case 2:
              {
                 errorvalue=(tnum>>16)&0xff;
              }break;
          }
       }
       else
       {
            result_type=3;
            num=ieee754todec(tnum);
        }
     return true;
    }
    bool NUMBER::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       quint64 tnum;
       dqs >>rw >> col >> ixfe>>tnum;
      num=ieee754todec(tnum);
      return true;
    }
    bool MULBLANK::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs >>rw >> colFirst;
       qint16 t;
       while(!dqs.atEnd())
       {
           dqs >>t;
           rgixfe.append(t);
       }
       colLast=rgixfe.last();
       rgixfe.pop_back();
      return true;
    }
    bool MERGECELLS::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs >>cmcs;
       REF ref;
       while(!dqs.atEnd())
       {
           dqs >>ref.rwFirst>>ref.rwLast>>ref.colFirst>>ref.colLast;
           rgRef.append(ref);
       }
      return true;
    }
    bool ROW::ParseRecord(QByteArray& rawbytes,QDataStream::ByteOrder byteorder)
    {
       QDataStream dqs(rawbytes);
       dqs.setByteOrder(byteorder);
       dqs >> rw
            >> colMic
            >> colMac
            >> miyRw
            >> irwMac
            >> reserv
            >> grbit
            >> ixfe;
       return true;
    }
////////////////////////////////////////////////////////////////////////////
