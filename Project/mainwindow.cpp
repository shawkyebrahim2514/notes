#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "editnote.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // make this file if doesn't exist
    QDir notesDir;
    notesDir.mkpath("notes");

    ui->setupUi(this);
    QFile stylefile("stylesheet.qss");
    stylefile.open(QFile::ReadOnly);
    QString style = stylefile.readAll();
    stylefile.close();
    ui->listWidget->setStyleSheet(style);

    QDir dir("notes");
    foreach(QFileInfo val, dir.entryInfoList()){
        QFile file(val.filePath());
        if(file.open(QFile::ReadOnly | QFile::Text)){
            allNotes[val.fileName().toStdString().substr(0,val.fileName().size()-4)] = file.readAll().toStdString();
        }
        file.close();
    }
    for(auto item : allNotes){
        ui->listWidget->addItem(QString::fromStdString(item.first));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);
}


void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    // look here !!!
    for(int i = 0;i < ui->tabWidget->count(); i++){
        if(ui->tabWidget->tabText(i) == item->text()){
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }

    editNote *screen = new editNote(item->text(),QString::fromStdString(allNotes[item->text().toStdString()]));
    ui->tabWidget->addTab(screen,item->text());
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}


void MainWindow::on_addNew_clicked()
{
    QString tabName = QInputDialog::getText(this,"New tab","Enter new tab name");
    if(tabName.isEmpty()){
        QMessageBox::warning(this,"Error","Don't enter empty title");
        return;
    }
    for(auto& letter : tabName){
        if(!letter.isLetterOrNumber() && letter != ' '){
            QMessageBox::warning(this,"Error","Enter valid tab name\n( only alphabets or digits )");
            return;
        }
    }
    editNote *screen = new editNote(tabName,"");
    ui->tabWidget->addTab(screen,tabName);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
    ui->listWidget->addItem(tabName);
    ui->listWidget->item(ui->listWidget->count() - 1)->setSelected(1);
    allNotes[tabName.toStdString()] = "";
    QFile file("notes/" + tabName + ".txt");
    file.open(QFile::WriteOnly);
    file.close();
}


void MainWindow::on_saveNote_clicked()
{
    editNote *currentNote = (editNote*)ui->tabWidget->widget(ui->tabWidget->currentIndex());
    QString tabName = currentNote->noteTitle;
    /*if(tabName != ui->tabWidget->tabText(ui->tabWidget->currentIndex())){
        allNotes.erase(ui->tabWidget->tabText(ui->tabWidget->currentIndex()).toStdString());
        qDeleteAll(ui->listWidget->selectedItems());
        ui->listWidget->addItem(tabName);

        QFile file("notes/" + ui->tabWidget->tabText(ui->tabWidget->currentIndex()) + ".txt");
        file.open(QFile::WriteOnly);
        file.rename(tabName + ".txt");

        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), tabName);
    }*/
    allNotes[tabName.toStdString()] = currentNote->allParagraph.toStdString();

    QFile file("notes/" + tabName + ".txt");
    file.open(QFile::WriteOnly);
    QTextStream out(&file);
    out << currentNote->allParagraph;
    file.flush();
    file.close();
}


void MainWindow::on_deleteNoteBtn_clicked()
{
    editNote *currentNote = (editNote*)ui->tabWidget->widget(ui->tabWidget->currentIndex());
    QString tabName = currentNote->noteTitle;
    QFile file("notes/" + tabName + ".txt");
    file.remove();
    file.close();
    qDeleteAll(ui->listWidget->selectedItems());
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
}


void MainWindow::on_changeNoteTitleBtn_clicked()
{
    // change current tab title
    QString curTabName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    QString newTabName = QInputDialog::getText(this,"change tab title","Enter new title");
    for(auto& letter : newTabName){
        if(!letter.isLetterOrNumber() && letter != ' '){
            QMessageBox::warning(this,"Error","Enter valid tab name\n( only alphabets or digits )");
            return;
        }
    }
    allNotes.erase(curTabName.toStdString()); //  -------------delete this set at all
    qDeleteAll(ui->listWidget->selectedItems());
    ui->listWidget->addItem(newTabName);
    QFile::rename("notes/" + curTabName + ".txt", "notes/" + newTabName + ".txt");
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), newTabName);
    allNotes[newTabName.toStdString()] = curTabName.toStdString();

    on_tabWidget_currentChanged(0);
}


void MainWindow::on_tabWidget_currentChanged(int index)
{
    QString tarList = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    for(int i = 0; i < ui->listWidget->count(); i++){
        if(ui->listWidget->item(i)->text() == tarList){
            ui->listWidget->setCurrentItem(ui->listWidget->item(i));
            return;
        }
    }
}

