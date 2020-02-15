#include "BNBLEditor.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

const QString btnTextPrefix = QObject::tr("Rectangle") + " ";

BNBLEditor::BNBLEditor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setFixedSize(this->size());

    btn_dragTimer.setSingleShot(false);
    btn_dragTimer.setInterval(1);
    connect(&btn_dragTimer, &QTimer::timeout, this, &BNBLEditor::rectBtn_dragLoop);

    this->lastDir = QDir::homePath() + "/desktop";

    ui.statusBar->showMessage(tr("Open a file or create a new one."));
}

//GUI

void BNBLEditor::rectCount_changed(int count)
{
    if(count > 256 && !(last_rectCountValue > 256))
    {
        QMessageBox::StandardButton reply = QMessageBox::warning(this, tr("Warning!"), tr("Going beyond 256 rectangles is unstable!\n(DS hardware might struggle and editor might run out of memory.)\nDo you still want to continue?"), QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::No)
        {
            ui.rectCount_sb->setValue(256);
            return;
        }
    }

    int rectCount = bnbl.rectangles.count();
    int diff = count - rectCount;
    if (diff > 0)
    {
        for (int i = 0; i < diff; i++)
        {
            BNBL::Rectangle rect = { 0, 0, 96, 32 };
            append_rectBtn(i + rectCount, rect);
            bnbl.rectangles.append(rect);
        }
    }
    else
    {
        for (int i = 0; i > diff; i--)
        {
            delete pushBtns.last();
            pushBtns.removeLast();
            bnbl.rectangles.removeLast();
        }
    }

    int max = count - 1;
    if (max < 0)
    {
        max = 0;
        ui.selRect_sb->setEnabled(false);
        ui.xPos_sb->setEnabled(false);
        ui.yPos_sb->setEnabled(false);
        ui.width_sb->setEnabled(false);
        ui.height_sb->setEnabled(false);
    }
    else
    {
        ui.selRect_sb->setEnabled(true);
        ui.xPos_sb->setEnabled(true);
        ui.yPos_sb->setEnabled(true);
        ui.width_sb->setEnabled(true);
        ui.height_sb->setEnabled(true);
    }

    if(last_rectCountValue == 0 && count == 1)
        ui.selRect_sb->valueChanged(0);

    ui.selRect_sb->setMaximum(max);

    last_rectCountValue = count;
}

void BNBLEditor::selRect_changed(int selRect)
{
    for(int i = 0; i < pushBtns.count(); i++)
    {
        QPushButton* pb = pushBtns[i];
        if(i == selRect)
            setRectBtnSelected(pb, true);
        else
            setRectBtnSelected(pb, false);
    }

    BNBL::Rectangle rect = bnbl.rectangles[selRect];
    ui.xPos_sb->setValue(rect.xpos);
    ui.yPos_sb->setValue(rect.ypos);
    ui.width_sb->setValue(rect.width);
    ui.height_sb->setValue(rect.height);
}

void BNBLEditor::rectProperties_changed()
{
    int selRect = ui.selRect_sb->value();
    QPushButton* pb = pushBtns[selRect];

    BNBL::Rectangle* rect = &bnbl.rectangles[selRect];
    rect->xpos = ui.xPos_sb->value();
    rect->ypos = ui.yPos_sb->value();
    rect->width = ui.width_sb->value();
    rect->height = ui.height_sb->value();

    pb->setGeometry(rect->xpos, rect->ypos, rect->width, rect->height);
}

void BNBLEditor::setRectBtnSelected(QPushButton* pb, bool selected)
{
    if(selected)
        pb->setStyleSheet("background-color: rgba(0, 192, 255, 128);"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;");
    else
        pb->setStyleSheet("background-color: rgba(225, 225, 225, 128);"
                          "border-style: outset;"
                          "border-width: 1px;"
                          "border-color: black;");
}

void BNBLEditor::append_rectBtn(int i, BNBL::Rectangle& rect)
{
    QPushButton* pb = new QPushButton(ui.playground);
    pb->setGeometry(rect.xpos, rect.ypos, rect.width, rect.height);
    pb->setText(btnTextPrefix + QString::number(i));
    connect(pb, &QPushButton::pressed, this, &BNBLEditor::rectBtn_pressed);
    connect(pb, &QPushButton::released, this, &BNBLEditor::rectBtn_released);
    setRectBtnSelected(pb, i == 0);
    pb->show();
    pushBtns.append(pb);
}

void BNBLEditor::rectBtn_dragLoop()
{
    int btnId = ui.selRect_sb->value();
    QPushButton* pb = pushBtns[btnId];

    btn_dragMouse = QPoint(ui.playground->mapFromGlobal(QCursor::pos()).x(), ui.playground->mapFromGlobal(QCursor::pos()).y());

    QPoint posDiffs = btn_dragMouseStart - btn_dragMouse;
    QPoint newPos = btn_dragPosAtStart - posDiffs;

    if(newPos.x() < 0)
        newPos.setX(0);
    if(newPos.x() + pb->width() > 256)
        newPos.setX(256 - pb->width());
    if(newPos.y() < 0)
        newPos.setY(0);
    if(newPos.y() + pb->height() > 192)
        newPos.setY(192 - pb->height());

    //pb->move(newPos);
    ui.xPos_sb->setValue(newPos.x());
    ui.yPos_sb->setValue(newPos.y());
}

void BNBLEditor::rectBtn_pressed()
{
    QPushButton* pb = (QPushButton*)sender();
    int btnId = pb->text().remove(btnTextPrefix).toInt();

    btn_dragMouseStart = QPoint(ui.playground->mapFromGlobal(QCursor::pos()).x(),
                                ui.playground->mapFromGlobal(QCursor::pos()).y());
    btn_dragPosAtStart = pb->pos();

    ui.selRect_sb->setValue(btnId);
    btn_dragTimer.start();
}

void BNBLEditor::rectBtn_released()
{
    btn_dragTimer.stop();
}

//ACTIONS

bool BNBLEditor::openFileWarning()
{
    if (!fileName.isNull())
    {
        QMessageBox::StandardButton reply = QMessageBox::warning(this, tr("Warning!"), tr("You have a file opened, do you really to close it and continue?"), QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
            return false;
    }
    return true;
}

void BNBLEditor::setFileName(const QString& path)
{
    this->fileName = path;
    ui.menuFilename->setTitle(path);
}

void BNBLEditor::newFile()
{
    if(!openFileWarning())
        return;

    closeFile();
    setFileName("memory/bnbl.bnbl");

    ui.rectCount_sb->setEnabled(true);
    ui.actionSave->setEnabled(false);
    ui.actionSave_as->setEnabled(true);
    ui.actionClose->setEnabled(true);
}

void BNBLEditor::openFile()
{
    if(!openFileWarning())
        return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open BNBL"), this->lastDir, tr("BNBL Files") + " (*.bnbl)");
    if (fileName.isNull())
        return;
    this->lastDir = fileName;

    closeFile();
    setFileName(fileName);

    bnbl.loadFromFile(this->fileName);

    int rectCount = bnbl.rectangles.count();
    for (int i = 0; i < rectCount; i++)
        append_rectBtn(i, bnbl.rectangles[i]);

    ui.rectCount_sb->setEnabled(true);
    ui.rectCount_sb->setValue(rectCount);
    if(rectCount > 0)
        ui.selRect_sb->valueChanged(0);

    ui.actionSave->setEnabled(true);
    ui.actionSave_as->setEnabled(true);
    ui.actionClose->setEnabled(true);
}

void BNBLEditor::closeFile()
{
    setFileName(nullptr);

    ui.rectCount_sb->blockSignals(true);
    ui.rectCount_sb->setValue(0);
    ui.rectCount_sb->setEnabled(false);
    ui.rectCount_sb->blockSignals(false);

    ui.selRect_sb->blockSignals(true);
    ui.selRect_sb->setValue(0);
    ui.selRect_sb->setEnabled(false);
    ui.selRect_sb->blockSignals(false);

    ui.xPos_sb->blockSignals(true);
    ui.xPos_sb->setValue(0);
    ui.xPos_sb->setEnabled(false);
    ui.xPos_sb->blockSignals(false);
    ui.yPos_sb->blockSignals(true);
    ui.yPos_sb->setValue(0);
    ui.yPos_sb->setEnabled(false);
    ui.yPos_sb->blockSignals(false);
    ui.width_sb->blockSignals(true);
    ui.width_sb->setValue(0);
    ui.width_sb->setEnabled(false);
    ui.width_sb->blockSignals(false);
    ui.height_sb->blockSignals(true);
    ui.height_sb->setValue(0);
    ui.height_sb->setEnabled(false);
    ui.height_sb->blockSignals(false);

    bnbl.rectangles.clear();
    for(QPushButton* pb : pushBtns)
        delete pb;
    pushBtns.clear();

    ui.actionSave->setEnabled(false);
    ui.actionSave_as->setEnabled(false);
    ui.actionClose->setEnabled(false);
}

void BNBLEditor::saveFile()
{
    bool saved = bnbl.saveToFile(this->fileName);
    if (!saved)
        QMessageBox::critical(this, tr("Error!"), tr("Could not open file for writing."));
}

void BNBLEditor::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save BNBL"), this->lastDir, tr("BNBL Files") + " (*.bnbl)");
    if (fileName.isNull())
        return;
    this->lastDir = fileName;
    setFileName(fileName);

    saveFile();
}

void BNBLEditor::openOrCloseBg()
{
    if(ui.actionOpenBg->isEnabled())
    {
        QString fileName = QFileDialog::getOpenFileName(this, "", this->lastDir, "PNG Files (*.png)");
        if(fileName == "")
            return;
        this->lastDir = fileName;
        ui.playground->setStyleSheet("QWidget#playground { background-image:url(" + fileName + "); }");

        ui.actionOpenBg->setEnabled(false);
        ui.actionCloseBg->setEnabled(true);
    }
    else
    {
        ui.playground->setStyleSheet("QWidget#playground { background-color: rgb(218, 218, 218); }");

        ui.actionOpenBg->setEnabled(true);
        ui.actionCloseBg->setEnabled(false);
    }
}
