#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QTableView>

/**
 * @brief MainWindow::parseSymbolTable
 */
void MainWindow::parseSymbolTable(){
	int startSymbolTable = 0;				// Start of local symbols
	int endSymbolTable = 0;
	int startLocalSymbolTable = 0;
	int startGlobalSymbolTable = 0;
	int endLocalSymbolTable = 0;
	int endGlobalSymbolTable = 0;

	startSymbolTable = 0;

	for(int i=0; i<fileData.count(); i++){
		if( fileData.at(i).contains(QRegularExpression("Image Symbol Table"))) startSymbolTable = i;
		if( startSymbolTable && fileData.at(i).contains(QRegularExpression("===.+"))) endSymbolTable = i-3;

		if( startSymbolTable && fileData.at(i).contains(QRegularExpression(".*Global Symbols")) ) startGlobalSymbolTable = i+4;
		if(endSymbolTable) break;
	}

	startLocalSymbolTable = startSymbolTable + 6;
	endLocalSymbolTable = startGlobalSymbolTable - 5 ;
	endGlobalSymbolTable = endSymbolTable;

	qDebug() << "symboltables from "<< startSymbolTable << " to " << endSymbolTable;
	qDebug() << "localsymbols from "<< startLocalSymbolTable << " to " << endLocalSymbolTable;
	qDebug() << "globalsymbol from" << startGlobalSymbolTable << " to " << endGlobalSymbolTable;

	if(startSymbolTable < 0) return;
	if(endSymbolTable < 0)return;
	if(startLocalSymbolTable < 0)return;
	if(endLocalSymbolTable < 0)return;
	if(startGlobalSymbolTable < 0)return;
	if(endGlobalSymbolTable < 0)return;

	QString tab = "\t";
	QString empty = "";
	QString thumb = "ThumbCode";

	for(int i=startLocalSymbolTable; i<endLocalSymbolTable; i++){
		fileData[i].replace(QRegularExpression(" +"),tab);
		fileData[i].replace(QRegularExpression("^\t"),empty);
		fileData[i].replace(QRegularExpression("Thumb\tCode"),thumb);
	}

	for(int i=startGlobalSymbolTable; i<endGlobalSymbolTable; i++){
		fileData[i].replace(QRegularExpression(" +"),QString("\t"));
		fileData[i].replace(QRegularExpression("^\t"),QString(""));
		fileData[i].replace(QRegularExpression("Thumb\tCode"),QString("ThumbCode"));
	}

	ui->symbolTable->clearContents();
	ui->symbolTable->setRowCount(
				(endLocalSymbolTable - startLocalSymbolTable) +
				(endGlobalSymbolTable - startGlobalSymbolTable));
	int count = endLocalSymbolTable - startLocalSymbolTable;
	int i = 0;

	for(; i<count; i++){
		QStringList a = fileData.at(startLocalSymbolTable + i).split('\t');
		ui->symbolTable->setItem(i,0,new QTableWidgetItem(a[0])); // name
		ui->symbolTable->setItem(i,1,new QTableWidgetItem(a[1])); // value
		ui->symbolTable->setItem(i,2,new QTableWidgetItem(a[2])); // type
		QTableWidgetItem *item = new QTableWidgetItem();
		item->setData(Qt::EditRole, a[3].toInt());
		ui->symbolTable->setItem(i,3,item); // size
		ui->symbolTable->setItem(i,4,new QTableWidgetItem(a[4])); // object
		if( a.count() > 5)
			ui->symbolTable->setItem(i,5,new QTableWidgetItem(a[5])); // section
		ui->symbolTable->setItem(i,6,new QTableWidgetItem("Local")); // scope
	}

	int i_off = i;
	count = i+(endGlobalSymbolTable - startGlobalSymbolTable);
	for(; i<count; i++){
		QStringList a = fileData.at(startGlobalSymbolTable + i - i_off).split('\t');
		ui->symbolTable->setItem(i,0,new QTableWidgetItem(a[0])); // name
		ui->symbolTable->setItem(i,1,new QTableWidgetItem(a[1])); // value
		ui->symbolTable->setItem(i,2,new QTableWidgetItem(a[2])); // type
		QTableWidgetItem *item = new QTableWidgetItem();
		item->setData(Qt::EditRole, a[3].toInt());
		ui->symbolTable->setItem(i,3,item); // size
		ui->symbolTable->setItem(i,4,new QTableWidgetItem(a[4])); // object
		if( a.count() > 5)
			ui->symbolTable->setItem(i,5,new QTableWidgetItem(a[5])); // section
		ui->symbolTable->setItem(i,6,new QTableWidgetItem("Global")); // scope
	}

}

/**
 * @brief MainWindow::parseRemovedSections
 */
void MainWindow::parseRemovedSections(){
	int startRemoved = 0;
	int endRemoved = 0;
	int removedTotal = 0;
	for(int i=0; i<fileData.count(); i++){
		if( fileData.at(i).contains(QRegularExpression("Removing Unused input sections from the image"))) startRemoved = i+2;

		if( startRemoved && fileData.at(i).contains(QRegularExpression(".*removed from the image")) ) removedTotal = i;
		if(removedTotal) break;
	}
	if(startRemoved < 0) return;
	if(removedTotal < 0) return;
	endRemoved = removedTotal - 1;
	qDebug() << "removetable from "<< startRemoved << " to " << endRemoved;
	qDebug() << "removetotals "<< removedTotal;
	// Removing main.o(.emb_text), (8 bytes).
	// 52 unused section(s) (total 7697 bytes) removed from the image.
	ui->removedTotalLabel->setText(fileData[removedTotal]);
	for(int i=startRemoved; i<endRemoved; i++){
		fileData[i].replace(QRegularExpression(".*Removing "),QString(""));
		fileData[i].replace(QString(".o("),QString(".o\t"));
		fileData[i].replace(QString("), ("),QString("\t"));
		fileData[i].replace(QString("bytes)."),QString(""));
	}
	//	qDebug() << "########################## removed";
	//	for(int i=startRemoved; i<endRemoved; i++){
	//		qDebug() << fileData.at(i);
	//	}
	int count = endRemoved - startRemoved;
	ui->removedTable->clearContents();
	ui->removedTable->setRowCount(count);
	for(int i=0; i<count; i++){
		QStringList a = fileData.at(i+startRemoved).split('\t');
		ui->removedTable->setItem(i,0,new QTableWidgetItem(a[0])); // name
		ui->removedTable->setItem(i,1,new QTableWidgetItem(a[1])); // type
		QTableWidgetItem *item = new QTableWidgetItem();
		item->setData(Qt::EditRole, a[2].toInt());
		ui->removedTable->setItem(i,2,item); // size
	}
}

/**
 * @brief MainWindow::parseCrossSections
 */
void MainWindow::parseCrossSections(){
	int startCross = 0;
	int endCross = 0;
	for(int i=0; i<fileData.count(); i++){
		if( fileData.at(i).contains(QRegularExpression("Section Cross References"))) startCross = i+2;
		if( startCross && fileData.at(i).contains(QRegularExpression("===.+"))) endCross = i-2;
		if(endCross) break;
	}
	qDebug() << "crosstable from "<< startCross << " to " << endCross;
	//main.o(.text) refers to chevents.o(.text) for chEvtSignalFlags
	// name - type - name - type - symbol
	for(int i=startCross; i<endCross; i++){
		fileData[i].replace(QRegularExpression("^ +"),QString(""));
		fileData[i].replace(QString(".o("),QString("\t"));
		fileData[i].replace(QRegularExpression(". refers*.*to "),QString("\t"));
		fileData[i].replace(QString(") for "),QString("\t"));
	}
	//	qDebug() << "########################## crosslinks";
	//	for(int i=startCross; i<endCross; i++){
	//		qDebug() << fileData.at(i);
	//	}
	int count = endCross - startCross;
	ui->crossTable->clearContents();
	ui->crossTable->setRowCount(count);
	for(int i=0; i<count; i++){
		QStringList a = fileData.at(i+startCross).split('\t');
		if(a.count() > 0)
			ui->crossTable->setItem(i,0,new QTableWidgetItem(a[0])); // name
		if(a.count() > 1)
			ui->crossTable->setItem(i,1,new QTableWidgetItem(a[1])); // type
		if(a.count() > 2)
			ui->crossTable->setItem(i,2,new QTableWidgetItem(a[2])); // name
		if(a.count() > 3)
			ui->crossTable->setItem(i,3,new QTableWidgetItem(a[3])); // type
		if(a.count() > 4)
			ui->crossTable->setItem(i,4,new QTableWidgetItem(a[4])); // symbol
	}
}

/**
 * @brief MainWindow::parseImageComponents
 */
void MainWindow::parseImageComponents(){
	int startUserObject = 0;
	int endUserObject = 0;
	int startLibObjects = 0;
	int endLibObjects = 0;
	int startLibs = 0;
	int endLibs = 0;

	for(int i=0; i<fileData.count(); i++){
		if( fileData.at(i).contains(QString("Image component sizes"))) startUserObject = i+5;
		if( startUserObject && fileData.at(i).contains(QRegularExpression("---.+"))) endUserObject = i-1;
		if(endUserObject) break;
	}
	for(int i=endUserObject; i<fileData.count(); i++){
		if( fileData.at(i).contains(QString("Library Member Name"))) startLibObjects = i+2;
		if( startLibObjects && fileData.at(i).contains(QRegularExpression("---.+"))) endLibObjects = i-1;
		if(endLibObjects) break;
	}
	for(int i=endLibObjects; i<fileData.count(); i++){
		if( fileData.at(i).contains(QString("Library Name"))) startLibs = i+2;
		if( startLibs && fileData.at(i).contains(QRegularExpression("---.+"))) endLibs = i-1;
		if(endLibs) break;
	}

	qDebug() << "User image component sizes from "<< startUserObject << " to " << endUserObject;
	qDebug() << "Lib image component sizes from "<< startLibObjects << " to " << endLibObjects;
	qDebug() << "Lib component sizes from "<< startLibs << " to " << endLibs;

	if(startUserObject <= 0 ) return;
	if(endUserObject <= 0 ) return;
	if(startLibObjects <= 0 ) return;
	if(endLibObjects <= 0 ) return;
	if(startLibs <= 0 ) return;
	if(endLibs <= 0 ) return;

	//       Code (inc. data)   RO Data    RW Data    ZI Data      Debug   Object Name
	for(int i=startUserObject; i<endUserObject; i++){
		fileData[i].replace(QRegularExpression("^ +"),QString(""));
		fileData[i].replace(QRegularExpression(" +"),QString("\t"));
	}
	for(int i=startLibObjects; i<endLibObjects; i++){
		fileData[i].replace(QRegularExpression("^ +"),QString(""));
		fileData[i].replace(QRegularExpression(" +"),QString("\t"));
	}
	for(int i=startLibs; i<endLibs; i++){
		fileData[i].replace(QRegularExpression("^ +"),QString(""));
		fileData[i].replace(QRegularExpression(" +"),QString("\t"));
	}

	QString totals;
	totals.append(fileData.at(startUserObject -5 + 3)); totals.append("\n");
	totals.append(fileData.at(endUserObject + 2)); totals.append("\n");
	totals.append(fileData.at(endUserObject + 3)); totals.append("\n");
	totals.append(fileData.at(endUserObject + 4)); totals.append("\n");
	totals.append(fileData.at(endLibObjects + 2)); totals.append("\n");
	totals.append(fileData.at(endLibObjects + 3)); totals.append("\n");
	totals.append(fileData.at(endLibs + 2)); totals.append("\n");
	ui->userImageTotalLabel->setText(totals);
	ui->userImageTotalLabel->setFont(QFont(QString("courier")));

	int count = endUserObject - startUserObject;
	int totalRows = (endUserObject - startUserObject) + (endLibObjects - startLibObjects) + (endLibs - startLibs);
	int rowCount = 0;
	ui->componentTable->clearContents();
	ui->componentTable->setRowCount(totalRows);
	for(int i=0; i<count; i++){
		QStringList a = fileData.at(i+startUserObject).split('\t');
		for(int j=0; j<6; j++){
			QTableWidgetItem *item = new QTableWidgetItem();
			item->setData(Qt::EditRole, a[j].toInt());
			ui->componentTable->setItem(rowCount,j,item);
		}
		ui->componentTable->setItem(rowCount,6,new QTableWidgetItem(a[6])); // symbol
		ui->componentTable->setItem(rowCount++,7,new QTableWidgetItem(QString("user object")));
	}

	count = endLibObjects - startLibObjects;
	for(int i=0; i<count; i++){
		QStringList a = fileData.at(i+startLibObjects).split('\t');
		for(int j=0; j<6; j++){
			QTableWidgetItem *item = new QTableWidgetItem();
			item->setData(Qt::EditRole, a[j].toInt());
			ui->componentTable->setItem(rowCount,j,item);
		}
		ui->componentTable->setItem(rowCount,6,new QTableWidgetItem(a[6])); // symbol
		ui->componentTable->setItem(rowCount++,7,new QTableWidgetItem(QString("lib object")));
	}

	count = endLibs - startLibs;
	for(int i=0; i<count; i++){
		QStringList a = fileData.at(i+startLibs).split('\t');
		for(int j=0; j<6; j++){
			QTableWidgetItem *item = new QTableWidgetItem();
			item->setData(Qt::EditRole, a[j].toInt());
			ui->componentTable->setItem(rowCount,j,item);
		}
		ui->componentTable->setItem(rowCount,6,new QTableWidgetItem(a[6])); // symbol
		ui->componentTable->setItem(rowCount++,7,new QTableWidgetItem(QString("lib")));
	}
}

/**
 * @brief MainWindow::parseTotals
 */
void MainWindow::parseTotals(){
	int startTotals = 0;
	int endTotals = 0;

	for(int i=0; i<fileData.count(); i++){
		if( fileData.at(i).contains(QString("Grand Totals"))) startTotals = i-2;
	}
	endTotals = startTotals + 10;
	(void)endTotals;

	if(startTotals > 0){
		QString totals;
		totals.append(fileData.at(startTotals + 0)); totals.append("\n");
		totals.append(fileData.at(startTotals + 1)); totals.append("\n");
		totals.append(fileData.at(startTotals + 2)); totals.append("\n");
		totals.append(fileData.at(startTotals + 3)); totals.append("\n");
		totals.append(fileData.at(startTotals + 4)); totals.append("\n");
		totals.append(fileData.at(startTotals + 5)); totals.append("\n");
		totals.append(fileData.at(startTotals + 6)); totals.append("\n");
		totals.append(fileData.at(startTotals + 7)); totals.append("\n");
		totals.append(fileData.at(startTotals + 8)); totals.append("\n");
		totals.append(fileData.at(startTotals + 9)); totals.append("\n");
		totals.append(fileData.at(startTotals + 10)); totals.append("\n");
		totals.append(fileData.at(startTotals + 11)); totals.append("\n");
		ui->totalsLabel->setText(totals);
	}else{
		ui->totalsLabel->setText(QString("Totals information is missing from file."));
	}

	ui->totalsLabel->setFont(QFont(QString("courier")));
}

/**
 * @brief MainWindow::parseMemMap
 */
void MainWindow::parseMemMap(){
	int startMemMap = 0;
	int endMemMap = 0;
	QList< QList<int>* > loadRegions;

	for(int i=0; i<fileData.count(); i++){
		if( fileData.at(i).contains(QRegularExpression("Memory Map of the image"))) startMemMap = i+2;
		if( startMemMap && fileData.at(i).contains(QRegularExpression("===.+"))) endMemMap = i-2;
		if(endMemMap) break;
	}

	if(startMemMap > 0){

		// entry point
		ui->memMapEntry->setText(fileData.at(startMemMap));
		// point to first load region
		startMemMap += 2;

		// Find load regions
		for(int i=startMemMap; i<endMemMap; i++){
			if( fileData.at(i).contains(QRegularExpression("Load Region"))){
				QList<int>* region = new QList<int>;
				region->append(i);
				loadRegions.append(region);
			}
		}

		// Find exec regions
		for(int lr=0; lr<loadRegions.count(); lr++){
			int start = loadRegions.at(lr)->at(0);
			int stop = endMemMap;
			if( lr < loadRegions.count()-1 )
				stop = loadRegions.at(lr+1)->at(0);

			for(int i = start; i<stop; i++){
				if( fileData.at(i).contains(QRegularExpression("Execution Region"))){
					loadRegions.at(lr)->append(i);
				}
			}
		}

		// Print region headers
		for(int lr=0; lr<loadRegions.count(); lr++){
			for(int er=0; er<loadRegions.at(lr)->count(); er++){
				if(er == 0){
					qDebug() << fileData.at( loadRegions.at(lr)->at(er) );
				}else{
					qDebug() << "\t" << fileData.at( loadRegions.at(lr)->at(er) );
				}
			}
		}

		ui->treeMemMap->clear();

		// Fill treeview
		for(int lr=0; lr<loadRegions.count(); lr++){
			// load region
			QString reg = fileData.at( loadRegions.at(lr)->at(0) );
			reg.replace(QRegularExpression(".*Region"),QString(""));
			reg.replace(QString(" (Base: "),QString("\t"));
			reg.replace(QString(", Size: "),QString("\t"));
			reg.replace(QString(", Max: "),QString("\t"));
			reg.replace(QString(", "),QString("\t"));
			reg.replace(QString(")"),QString(""));
			QStringList region = reg.split(QString("\t"));

			QTreeWidgetItem *lritem = new QTreeWidgetItem(region);
			ui->treeMemMap->addTopLevelItem(lritem);

			for(int er=1; er < loadRegions.at(lr)->count(); er++){
				// exec region
				QString reg = fileData.at( loadRegions.at(lr)->at(er) );
				reg.replace(QRegularExpression(".*Region"),QString(""));
				reg.replace(QString(" (Base: "),QString("\t"));
				reg.replace(QString(", Size: "),QString("\t"));
				reg.replace(QString(", Max: "),QString("\t"));
				reg.replace(QString(", "),QString("\t"));
				reg.replace(QString(")"),QString(""));
				QStringList region = reg.split(QString("\t"));

				QTreeWidgetItem *eritem = new QTreeWidgetItem(region);
				lritem->addChild(eritem);

				int start = loadRegions.at(lr)->at(er) + 4;

				for(int i=start; ; i++){
					if( fileData.at(i).contains(QRegularExpression("^$"))) break;
					if( fileData.at(i).contains(QRegularExpression("PAD$"))){
						fileData[i].replace(QRegularExpression(" +"),QString("\t"));
						fileData[i].replace(QRegularExpression("^\t"),QString(""));
						QStringList entry = fileData[i].split(QString("\t"));

						QStringList view;
						view.append(entry.at(2));	// name (pad)
						view.append(entry.at(0));	// base
						view.append(entry.at(1));	// size
						view.append(QString("")); // end
						view.append(entry.at(2));	// type (pad)

						QTreeWidgetItem *item = new QTreeWidgetItem(view);
						eritem->addChild(item);
					}else{
						fileData[i].replace(QRegularExpression(" +"),QString("\t"));
						fileData[i].replace(QRegularExpression("^\t"),QString(""));
						QStringList entry = fileData[i].split(QString("\t"));

						QStringList view;
						view.append(entry.at(6));	// name
						view.append(entry.at(0));	// base
						view.append(entry.at(1));	// size
						view.append(QString("")); // end

						QString type;
						type.append(entry.at(3) + QString(" ") + entry.at(2));
						view.append(type);	// type

						qDebug() << entry;
						qDebug() << view;

						QTreeWidgetItem *item = new QTreeWidgetItem(view);
						eritem->addChild(item);
					}
				}
			}
		}
	}else{
		ui->memMapEntry->setText(QString("Memory map missing from file"));
	}
}
