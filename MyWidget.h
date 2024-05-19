#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qdirmodel.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlistview.h>
#include <QtWidgets/qfilesystemmodel.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qtextedit.h>
#include <QtGui/qpainter.h>
#include <Qtcore/qtimer.h>

#include "GeneratedFiles/ui_MyWidget.h"

#include "player.h"

class VideoOutput;

class MyWidget : public QWidget
{
	Q_OBJECT

public:
	MyWidget(QWidget* parent = nullptr);
	~MyWidget();

	void SetUrl(const char* url);

public slots:
	void ItemClicked(const QModelIndex& index);
	void ItemDoubleClicked(const QModelIndex& index);
	void closeEvent(QCloseEvent* event) override;

private:

	Ui::MyWidgetClass ui;
	std::string m_url;
	QFileSystemModel* m_model;
	QListView* m_listView;
	QSplitter* m_splitter;
};