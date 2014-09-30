/* MapViewer, a tool to improve readability of Keil ARM MDK's linker map files.
 * Copyright (C) 2014  Jeroen Lodder
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * http://www.gnu.org/licenses/gpl-2.0.txt
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QTableView>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

/**
 * @brief MainWindow::init
 * @details initiliaze the MainWindow after the MainWindow is created.
 *          This way all GUI elements have been loaded safely.
 * @param filename
 */
void MainWindow::init(QString filename)
{
	if(filename == ""){
		openFileDialog();
	}else{
		lastOpenFile = filename;
	}

	if(loadFile()){
		parseData();
	}
	// exit(0);
}

/**
 * @brief MainWindow::openFileDialog
 * @details This function pops-up an file-Open dialog
 */
void MainWindow::openFileDialog(void){

	QFileDialog dialog(this, "Select linker map file", lastOpenDir, "Map file (*.map)");
	dialog.setFileMode(QFileDialog::ExistingFile);
	if(dialog.exec()){
		lastOpenFile = dialog.selectedFiles().first();
		lastOpenDir = lastOpenFile;
		lastOpenDir.chop(lastOpenDir.count() - lastOpenFile.lastIndexOf('/'));
	}
}

/**
 * @brief MainWindow::loadFile
 * @details This function loads the file to a QStringList
 */
bool MainWindow::loadFile(){
	lastOpenFileInfo.setFile(lastOpenFile);
	if(!lastOpenFileInfo.exists()){
		QMessageBox::critical(this,tr("Error"),
													tr("Could not retrieve file details.\n"),
													QMessageBox::Ok);
		return false;
	}else{
		file.setFileName(lastOpenFile);
		file.open(QIODevice::ReadOnly);
		fileData.clear();
		while(!file.atEnd()){
			QString l = file.readLine();
			l.remove('\n');
			l.remove('\r');
			fileData.append(l);
		}
		file.close();
	}
	return true;
}

/**
 * @brief MainWindow::parseData
 */
void MainWindow::parseData(){

	//ARM Linker, 5.03 [Build 76] [MDK-ARM Standard]
	if( fileData.at(0).contains(QString("ARM Linker, 5.03")) ){

	}else{
		QMessageBox::information(this,tr("Note"),
													tr("The ARM Linker version of this map file is untested\n"),
													QMessageBox::Ok);
	}



	parseRemovedSections();
	parseSymbolTable();
	parseCrossSections();
	parseImageComponents();
	parseTotals();
	parseMemMap();
}

/**
 * @brief MainWindow::on_actionOpen_triggered
 */
void MainWindow::on_actionOpen_triggered()
{
	openFileDialog();
	loadFile();
	parseData();
}

/**
 * @brief MainWindow::on_actionRefresh_triggered
 */
void MainWindow::on_actionRefresh_triggered()
{
	if(loadFile()){
		parseData();
	}
}
