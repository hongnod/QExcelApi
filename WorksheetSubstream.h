#ifndef WORKSHEET_H
#define WORKSHEET_H

#include "GlobalsSubstream.h"
#include "RecordParser.h"



namespace ExceLite
{
  class QWorksheetSubstream
  {
      public:
        QWorksheetSubstream();
        void setGlobal(QGlobalsSubstream *global){_global=global;}
        quint16 totalRows(){return _lastRow-_firstRow;}
        quint16 totalCols(){return _lastCol-_firstCol;}
        bool readSheet(RecordIO& recIO);
      private:
        struct celldata
        {
            CELLTYPE::typeCELL type;
            qint16 ixfe;
            QVariant data;
        };
        quint16 _firstRow;
        quint16 _lastRow;
        quint16 _firstCol;
        quint16 _lastCol;
        QGlobalsSubstream*   _global;
        QVector<QVector<celldata> > _cells;
        MERGECELLS _mergecells;
  };
}//end namespace
#endif // WORKSHEET_H
