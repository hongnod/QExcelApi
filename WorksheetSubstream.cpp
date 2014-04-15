#include "WorksheetSubstream.h"
#include "RecordParser.h"


using namespace ExceLite;

   QWorksheetSubstream::QWorksheetSubstream(){}

   bool QWorksheetSubstream::readSheet(RecordIO& recIO)
   {
      QFile _f("C:/doc/sheet.txt");
     _f.open(QIODevice::WriteOnly);
     QTextStream tqs(&_f);
     QByteArray recdata;

     quint16 cur_row=0,cur_col=0;
     bool hasNext=false;
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
               tqs << "BOF Record:";
               if(bof.dt==STREAMTYPE::WORKSHEET_OR_DIALOG_SUBSTREAM) //还要看WsBool的fDialog位是否为0
               tqs <<  "This is work sheet sub-stream\n";
             } break;
           case ORDER::RO_DIMENSIONS:
           {
               recIO.ReadRawRecord(recdata);
               DIMENSIONS dim;
               dim.ParseRecord(recdata,recIO.byteOrder());
               _firstRow=dim.rwMic;_lastRow=dim.rwMac;
               _firstCol=dim.colMic;_lastCol=dim.colMac;
               _cells.resize(this->totalRows());
               for(int i=0;i<_cells.count();i++) _cells[i].resize(this->totalCols());
               tqs << "Dimensions:row"<<_firstRow<<"-"<<_lastRow<<",col"<<_firstCol<<"-"<<_lastCol<<"\n";
           } break;
           case ORDER::RO_LABELSST:
           {
               recIO.ReadRawRecord(recdata);
               LABELSST text;
               text.ParseRecord(recdata,recIO.byteOrder());
               cur_row=text.rw-_firstRow;
               cur_col=text.col-_firstCol;
               _cells[cur_row][cur_col].type=CELLTYPE::TEXT;
               _cells[cur_row][cur_col].ixfe=text.ixfe;
               _cells[cur_row][cur_col].data=text.isst;
               tqs << "text cell,row:"<<text.rw
                    << ",col:" <<text.col
                    <<"content:"<<_global->getString(text.isst)
                    <<",text index:"<<text.isst<<"\n";
           } break;

           case ORDER::RO_RK:
           {
               recIO.ReadRawRecord(recdata);
               RK  rk;
               rk.ParseRecord(recdata,recIO.byteOrder());
               cur_row=rk.rw-_firstRow;
               cur_col=rk.col-_firstCol;
               _cells[cur_row][cur_col].type=CELLTYPE::FLOAT;
               _cells[cur_row][cur_col].ixfe=rk.ixfe;
               _cells[cur_row][cur_col].data=rk.num;

               tqs << "rk cell,row:"<<rk.rw
                    << ",col:" <<rk.col
                    <<"content:"<<rk.num<<"\n";
           } break;
           case ORDER::RO_BLANK:
           {
               recIO.ReadRawRecord(recdata);
               BLANK  blank;
               blank.ParseRecord(recdata,recIO.byteOrder());
               cur_row=blank.rw-_firstRow;
               cur_col=blank.col-_firstCol;
               _cells[cur_row][cur_col].type=CELLTYPE::BLANK;
               _cells[cur_row][cur_col].ixfe=blank.ixfe;
               _cells[cur_row][cur_col].data="";

               tqs << "blank cell,row:"<<blank.rw
                    << ",col:" <<blank.col<<"\n";
           } break;
           case ORDER::RO_FORMULA:
           {
               recIO.ReadRawRecord(recdata);
               FORMULA  formula;
               formula.ParseRecord(recdata,recIO.byteOrder());
               cur_row=formula.rw-_firstRow;
               cur_col=formula.col-_firstCol;
               _cells[cur_row][cur_col].ixfe=formula.ixfe;
               switch(formula.result_type)
               {
                 case 0://string
                   {
                     recIO.goNext();
                     hasNext=true;

                            tqs << "formula cell,row:"<<formula.rw
                    << ",col:" <<formula.col<<",type:"<<formula.result_type<<",value:"<<formula.num<<"\n";
                     continue;
                   }break;
                  case 1:
                  {
                       _cells[cur_row][cur_col].type=CELLTYPE::BOOL;
                       _cells[cur_row][cur_col].data=formula.booleanvalue;
                  }break;
                  case 2:
                  {
                       _cells[cur_row][cur_col].type=CELLTYPE::ERROR;
                       _cells[cur_row][cur_col].data=formula.errorvalue;
                  }break;
                 case 3:
                  {
                       _cells[cur_row][cur_col].type=CELLTYPE::FLOAT;
                       _cells[cur_row][cur_col].data=formula.num;
                  }break;
               }
               tqs << "formula cell,row:"<<formula.rw
                    << ",col:" <<formula.col<<",type:"<<formula.result_type<<",value:"<<formula.num<<"\n";
           } break;

           case ORDER::RO_STRING:
           {
               recIO.ReadRawRecord(recdata);
              QDataStream dqs(recdata);
              dqs.setByteOrder(recIO.byteOrder());

              biff8String curStr;
              QString str=curStr.ParseString(dqs);
              qint32 index=_global->appendString(str);
               if(hasNext)
               {
                   _cells[cur_row][cur_col].data=index;
                   _cells[cur_row][cur_col].type=CELLTYPE::TEXT;

                   tqs << "result string  of formula,row:"<<cur_row+_firstRow
                    << ",col:" <<cur_col+_firstCol<<",value:"<<_global->getString(index)<<" count:"<<_global->getStringCount()<<"\n";
               }
               else
               {
                   //Error
               }
               hasNext=false;
            } break;
           case ORDER::RO_NUMBER:
           {
               recIO.ReadRawRecord(recdata);
               NUMBER  number;
               number.ParseRecord(recdata,recIO.byteOrder());
               cur_row=number.rw-_firstRow;
               cur_col=number.col-_firstCol;
               _cells[cur_row][cur_col].type=CELLTYPE::FLOAT;
               _cells[cur_row][cur_col].ixfe=number.ixfe;
               _cells[cur_row][cur_col].data=number.num;

               tqs << "number cell,row:"<<number.rw
                    << ",col:" <<number.col<<",value:"<<number.num<<"\n";
           } break;
           case ORDER::RO_MULBLANK:
           {
               recIO.ReadRawRecord(recdata);
               MULBLANK  mulblank;
               mulblank.ParseRecord(recdata,recIO.byteOrder());
               cur_row=mulblank.rw-_firstRow;
               for(int i=mulblank.colFirst;i<=mulblank.colLast;i++)
               {
                   cur_col=i-_firstCol;
                  _cells[cur_row][cur_col].type=CELLTYPE::BLANK;
                  _cells[cur_row][cur_col].ixfe=mulblank.rgixfe[i-mulblank.colFirst];
                  _cells[cur_row][cur_col].data="";
               }
               tqs << "mulblank cell,row:"<<mulblank.rw
                    << ";col,from:" <<mulblank.colFirst<<" to:"<<mulblank.colLast<<"\n";
           } break;
           case ORDER::RO_MERGECELLS:
           {
               recIO.ReadRawRecord(recdata);
               _mergecells.ParseRecord(recdata,recIO.byteOrder());
               //_cells[blank.rw-_firstRow][blank.col-_firstCol].celltype=CELLDATATYPE::BLANK_CELL;

               tqs << "mergecells count:"<<_mergecells.cmcs<<" :";
               for(int i=0;i<_mergecells.cmcs;i++)
               {
                   tqs<<_mergecells.rgRef[i].rwFirst<<"-"<<_mergecells.rgRef[i].rwLast<<","<<_mergecells.rgRef[i].colFirst<<"-"<<_mergecells.rgRef[i].colLast<<";";
               }
               tqs<<"\n";
           } break;
           case ORDER::RO_MULRK:
           {
               recIO.ReadRawRecord(recdata);
               MULRK mulrk;
               mulrk.ParseRecord(recdata,recIO.byteOrder());
               cur_row=mulrk.rw-_firstRow;
               tqs<<"mulrk row:"<<mulrk.rw<<",col from:"<<mulrk.colFirst<<" to "<<mulrk.colLast<<",details:";
               for(qint16 col=mulrk.colFirst;col<=mulrk.colLast;col++)
               {
                 cur_col=col-_firstCol;
                 _cells[cur_row][cur_col].type=CELLTYPE::FLOAT;
                 _cells[cur_row][cur_col].ixfe= mulrk.rgrkrec[col-mulrk.colFirst].ixfe;
                 _cells[cur_row][cur_col].data=mulrk.rgrkrec[col-mulrk.colFirst].num;
                 tqs<<"col:"<<mulrk.rgrkrec[col-mulrk.colFirst].num<<";";
               }
               tqs<<"\n";
           } break;
           case ORDER::RO_SHRFMLA://do nothing
           {
              //recIO.ReadRawRecord(recdata);
              recIO.SkipRecord();
              tqs<<"SHRFMLA"<<"\n";
           }break;
           case ORDER::RO_DBCELL://do nothing
           {
              //recIO.ReadRawRecord(recdata);
              recIO.SkipRecord();
              tqs<<"DBCELL"<<"\n";
           }break;
           case ORDER::RO_ROW:
           {
              recIO.ReadRawRecord(recdata);
              ROW row;
              row.ParseRecord(recdata,recIO.byteOrder());

              tqs<<"row number:"<<row.rw<<",from col:"<<row.colMic<<",to col:"<<row.colMac<<";\n";
           }break;
           case ORDER::RO_COLINFO:
           {
              recIO.SkipRecord();
              tqs<<"COLINFO"<<"\n";
           }break;            
           default:
           {
              tqs << "UnParsed Record:";
              tqs <<  recIO.order() << ","<< recIO.size() << "\n";
              recIO.SkipRecord();
            }break;
          }
         recIO.goNext();
       }
       return true;
   }
