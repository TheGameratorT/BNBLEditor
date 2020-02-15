#pragma once
#include "BNBL.h"

#include <QVector>
#include <QPushButton>
#include <QTimer>
#include <QPoint>

#include <QtWidgets/QMainWindow>
#include "ui_BNBLEditor.h"

class BNBLEditor : public QMainWindow
{
	Q_OBJECT

public:
	BNBLEditor(QWidget *parent = Q_NULLPTR);

private:
	Ui::BNBLEditorClass ui;

	BNBL bnbl;

	QString fileName;

    QString lastDir;

	QVector<QPushButton*> pushBtns;

    int last_rectCountValue = 0;

    QTimer btn_dragTimer;
    QPoint btn_dragMouseStart;
    QPoint btn_dragMouse;
    QPoint btn_dragPosAtStart;

    bool openFileWarning();

    void setFileName(const QString& path);

    void setRectBtnSelected(QPushButton* pb, bool selected);

    void append_rectBtn(int i, BNBL::Rectangle& rect);

private slots:

	//GUI
	void rectCount_changed(int count);

	void selRect_changed(int selRect);

	void rectProperties_changed();

	void rectBtn_dragLoop();

	void rectBtn_pressed();

	void rectBtn_released();

	//ACTIONS
	void newFile();

	void openFile();

	void closeFile();

	void saveFile();

	void saveFileAs();

	void openOrCloseBg();
};
