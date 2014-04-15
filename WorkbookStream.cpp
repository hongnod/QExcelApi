#include "WorkbookStream.h"
#include <iostream>
using namespace ExceLite;

bool worksheet::set(QWorksheetSubstream* psheet)
{
    _psheet=psheet;
    return true;
}
size_t worksheet::getTotalRows()
{
    return _psheet->totalRows();
}

size_t worksheet::getTotalCols()
{
    return _psheet->totalCols();
}

 BIFF8Reader::BIFF8Reader()
 {

 }
 BIFF8Reader::BIFF8Reader(const char* filename)
 {
   this->Load(filename);
 }
 BIFF8Reader::~BIFF8Reader()
 {
    for(int i=0;i<_sheets.count();i++)
     if(_sheets[i]) delete _sheets[i];
 }

 bool BIFF8Reader::Load(const char* filename)
 {
    _recIO.LoadCompoundFile(QString::fromLocal8Bit(filename));
    _recIO.seekToBegin(0);
    _global.readGlobal(_recIO);
    _sheets.resize(_global.getSheetCount());
    _sheets.fill(NULL);
    return true;
 }

 size_t BIFF8Reader::GetTotalSheets()
 {
     return _global.getSheetCount();
 }

 bool BIFF8Reader::GetWorksheet(size_t sheetIndex,worksheet & sheet)
 {
     if(_sheets[sheetIndex])
         {
             sheet.set(_sheets[sheetIndex]);
             return true;
         }

     QWorksheetSubstream* tsheet=new QWorksheetSubstream;
     _recIO.seekToBegin(_global._boundsheets[sheetIndex].lbPlyPos);
     //std::cout<<_global._boundsheets[sheetIndex].name.toLocal8Bit() <<"\n";
     _sheets[sheetIndex]=tsheet;
     tsheet->setGlobal(&_global);
     tsheet->readSheet(_recIO);
     sheet.set(_sheets[sheetIndex]);
     return true;
 }

