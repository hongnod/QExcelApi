#include "GlobalsSubstream.h"
#include <iostream>

using namespace  ExceLite;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 QGlobalsSubstream::QGlobalsSubstream():_boundsheets(0)
 {
   _formats.insert(0x00,"General");
   _formats.insert(0x01,"0");
   _formats.insert(0x02,"0.00");
   _formats.insert(0x03,"#,##0");
   _formats.insert(0x04,"#,##0.00");
   _formats.insert(0x05,"($#,##0_);($#,##0)");
   _formats.insert(0x06,"($#,##0_);[Red]($#,##0)");
   _formats.insert(0x07,"($#,##0.00_);($#,##0.00)");
   _formats.insert(0x08,"($#,##0.00_);[Red]($#,##0.00)");
   _formats.insert(0x09,"0%");
   _formats.insert(0x0a,"0.00%");
   _formats.insert(0x0b,"0.00E+00");
   _formats.insert(0x0c,"# ?/?");
   _formats.insert(0x0d,"# ??/??");
   _formats.insert(0x0e,"m/d/yy");
   _formats.insert(0x0f,"d-mmmm-yy");
   _formats.insert(0x10,"d-mmmm");
   _formats.insert(0x11,"mmmm-yy");
   _formats.insert(0x12,"h:mm AM/PM");
   _formats.insert(0x13,"h:mm:ss:AM/PM");
   _formats.insert(0x14,"h:mm");
   _formats.insert(0x15,"h:mm:ss");
   _formats.insert(0x16,"m/d/yy h:mm");
   _formats.insert(0x25,"(#,##0_);(#,##0)");
   _formats.insert(0x26,"(#,##0_);[Red](#,##0)");
   _formats.insert(0x27,"(#,##0.00_);(#,##0.00)");
   _formats.insert(0x28,"(#,##0.00_);[Red](#,##0.00)");
   _formats.insert(0x29,"_(*#,##0_);_(*(#,##0);_(*\"-\"_);_(@_)");
   _formats.insert(0x2a,"_($*#,##0_);_($*(#,##0);_($*\"-\"_);_(@_)");
   _formats.insert(0x2b,"_(*#,##0.00_);_(*(#,##0.00);_(*\"-\"??_);_(@_)");
   _formats.insert(0x2c,"_($*#,##0.00_);_($*(#,##0.00);_($*\"-\"??_);_(@_)");
   _formats.insert(0x2d,"mm:ss");
   _formats.insert(0x2e,"[h]:mm:ss");
   _formats.insert(0x2f,"mm:ss.0");
   _formats.insert(0x30,"##0.0E+0");
   _formats.insert(0x31,"@");
 }

 qint64 QGlobalsSubstream::readGlobal(RecordIO& recIO)
 {

   QByteArray recdata;

   Sst sst;

   qint16 leadingorder=ORDER::RO_UAV;

   recIO.goNext();
   while(recIO.order()!=ORDER::RO_EOF)
   {
     switch(recIO.order())
     {
         case ORDER::RO_BOF:
         {
             recIO.ReadRawRecord(recdata);
             Bof bof;
             bof.ParseRecord(recdata,recIO.byteOrder());

             if(bof.dt!=STREAMTYPE::GLOBALS_SUBSTREAM)
               { /*error*/}
         } break;
         case ORDER::RO_SST:
         {
            recIO.ReadRawRecord(recdata);
            sst.ParseRecord(recdata,_listString,recIO.byteOrder());
         } break;
         case ORDER::RO_BOUNDSHEET:
         {
             Boundsheet boundsheet;
             recIO.ReadRawRecord(recdata);
             boundsheet.ParseRecord(recdata,recIO.byteOrder());
             _boundsheets.append(boundsheet);
         }break;
         case ORDER::RO_XF:
         {
             Xf xf;
             recIO.ReadRawRecord(recdata);
             xf.ParseRecord(recdata,recIO.byteOrder());
             _xf.append(xf);
         }break;
         case ORDER::RO_FONT:
         {
             Font font;
             recIO.ReadRawRecord(recdata);
             font.ParseRecord(recdata,recIO.byteOrder());
             _fonts.append(font);
         }break;
         case ORDER::RO_FORMAT:
         {
             Format format;
             recIO.ReadRawRecord(recdata);
             format.ParseRecord(recdata,recIO.byteOrder());
             _formats.insert(format.ifmt,format.formating);
         }break;
         default:
         {
           recIO.SkipRecord();
         } break;
        }
        leadingorder=recIO.order();
        recIO.goNext();

        while(recIO.order()==ORDER::RO_CONTINUE)
        {
          switch(leadingorder)
          {
            case ORDER::RO_SST:
            {
               recIO.ReadRawRecord(recdata);
               sst.AppendContinue(recdata,_listString,recIO.byteOrder());
            }break;
            default:
            {
              recIO.SkipRecord();
            }break;
          }
          recIO.goNext();
        }

     }
  return 0;
 }

 QString& QGlobalsSubstream::getString(size_t index)
 {
     return _listString[index];
 }

