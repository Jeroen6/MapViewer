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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void init(QString filename);

private slots:
    void on_actionOpen_triggered();

		void on_actionRefresh_triggered();

private:
    Ui::MainWindow *ui;
    QFile file;
    QString lastOpenFile;
    QString lastOpenDir;
    QFileInfo lastOpenFileInfo;
    QStringList fileData;

    void openFileDialog();
		bool loadFile();
		void parseData();
		void parseSymbolTable();
		void parseRemovedSections();
		void parseCrossSections();
		void parseImageComponents();
		void parseTotals();
		void parseMemMap();
};

#endif // MAINWINDOW_H
