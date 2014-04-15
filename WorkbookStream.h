#ifndef BIFF8RC_H
#define BIFF8RC_H
/*////////////////////////////////////////////////////////////////////////////////////////////////
The Workbook Stream specifies global properties and data for a workbook, as well as the sheets (1) that constitute a workbook.
The name of this stream (1) MUST be "Workbook". A file MUST contain exactly one Workbook Stream, which consists of several
substreams. There MUST be exactly one substream (Globals Substream), and the substream MUST be the first substream
to appear in the Workbook Stream, which MUST be followed by one or more of the following substreams:
    Chart Sheet Substream
    Dialog Sheet Substream
    Macro Sheet Substream
    Worksheet Substream

//////////////////////////////////////////////////////////////////////////////////////////////// */


#include "RecordReader.h"
#include "GlobalsSubstream.h"
#include "WorksheetSubstream.h"



namespace ExceLite
{
   class worksheet
   {
    friend class BIFF8Reader;
    public:
       size_t getTotalRows();
       size_t getTotalCols();

     private:
      bool    set(QWorksheetSubstream* psheet);
      QWorksheetSubstream* _psheet;
       //BasicExcelCell* Cell(size_t row, size_t col);
   };
   class cell
   {
       //int Type() const;
       //int GetInteger() const;
       //double GetDouble() const;
       //const char* GetString() const;
   };
   class BIFF8Reader
   {
    public:
     BIFF8Reader();
     BIFF8Reader(const char* filename);
     ~BIFF8Reader();
     bool Load(const char* filename);

    public:
    size_t GetTotalSheets();
    bool GetWorksheet(size_t sheetIndex,worksheet & sheet);
//    BasicExcelWorksheet* GetWorksheet(const char* name);

    private:

     RecordIO                 _recIO;
     QGlobalsSubstream             _global;
     QVector<QWorksheetSubstream*> _sheets;
   };
}


#endif // BIFF8RC_H
