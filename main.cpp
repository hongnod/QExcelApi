#include <QtCore>
#include <iostream>
#include "CompoundFile.h"
#include "WorkbookStream.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

     //CompoundDoc::CompdDoc xls("c:/doc/test1.xls");

    //cout << xls._sizeSector << " and " <<xls._sizeMiniSector;
#ifdef _DEBUGCOMPDOC
    xls.PRINT_HEADER();
    xls.PRINT_SECTOR();
    xls.PRINT_FATSECTID();
    xls.PRINT_FAT();
    xls.PRINT_MINIFAT();
    xls.PRINT_DIRENTRIES();
    xls.PRINT_DATA();
#endif
   /* std::cout<<"Finished!";
    QString t=xls.ReadStream(QString::fromAscii("Workbook"),QString::fromAscii("/"));
    QFile comoundfile(t);
    comoundfile.open(QIODevice::ReadWrite);
    QDataStream qs(&comoundfile);
    qs.setByteOrder(xls.byteOrder());
    quint16 i;
    qs >> i;
    if(i!=0x0809)
     std::cout<< "it's not a biff8 file\n";
    else
      std::cout<< "it looks like a biff8 file\n";
    */
    ExceLite::BIFF8Reader biff8("d:/doc/test9.xls");
    ExceLite::worksheet  sheet;
    std::cout << biff8.GetTotalSheets() << "\n";
    biff8.GetWorksheet(0,sheet);
    std::cout<<"Finished!";
    return a.exec();
}
