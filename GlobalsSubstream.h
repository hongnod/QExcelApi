#ifndef WORKGLOBAL_H
#define WORKGLOBAL_H
#include <QtCore>
#include "RecordReader.h"
#include "RecordParser.h"



namespace ExceLite
{

////////////////////////////////////////////////////////////////////////////////////////
   class QGlobalsSubstream
   {
    public:
     QGlobalsSubstream();
     QString& getString(size_t index);
     size_t  appendString(QString str){_listString.append(str);return _listString.count()-1;}
     size_t getStringCount(){return _listString.count();}
    //private:
     qint64     readGlobal(RecordIO& recIO);
     size_t      getSheetCount(){return _boundsheets.size();}
     //bool        getSheet(qint16& index,qint32&pos,QString& name);
     QVector<Boundsheet> _boundsheets;
    private:
     QVector<Font>   _fonts;
     QVector<Xf>      _xf;
     QMap<qint16, QString> _formats;
     QStringList         _listString;
   };
}// end name space


#endif // WORKGLOBAL_H

