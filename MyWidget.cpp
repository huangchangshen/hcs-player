#include "MyWidget.h"

MyWidget::MyWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//QPushButton * btn = new QPushButton("begin");
	//btn->setParent(this);
	//btn->setFixedSize(100, 100);

	
	//QObject::connect(btn, &QPushButton::clicked, [=]() {
	//	Play(m_url.c_str());
	//	});


	m_model = new QFileSystemModel;
	m_model->setRootPath("./video");
	// 创建视图
	m_listView = new QListView;
	m_listView->setModel(m_model);
	m_listView->setRootIndex(m_model->index("./video")); // 设置根目录

	m_listView->setFixedSize(350, 1000);


	//分割窗口
	m_splitter = new QSplitter(this);
	m_splitter->setOrientation(Qt::Horizontal);

	// 将部件添加到QSplitter中
	m_splitter->addWidget(m_listView);
	m_splitter->addWidget(m_video_output);

	ui.horizontalLayout->addWidget(m_splitter);

	//绑定 双击播放视频
	connect(m_listView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(ItemDoubleClicked(const QModelIndex&)));


	 //创建主窗口
	/*ui.horizontalLayout ->addWidget(m_treeView);
	this->setLayout(ui.horizontalLayout);
	this->setWindowTitle("文件列表");
	this->show();*/
	
	

}

MyWidget::~MyWidget()
{
}

void MyWidget::SetUrl(const char* url)
{
	m_url = url;
}

void MyWidget::ItemClicked(const QModelIndex& index)
{

}

void MyWidget::ItemDoubleClicked(const QModelIndex& index)
{
	if (SDL_WasInit(0) & SDL_INIT_VIDEO)
	{
		SDL_Event Event;
		Event.type = SDL_QUIT;
		if (SDL_PushEvent(&Event) != 1)
		{
			fprintf(stderr, "Failed to push quit event: %s\n", SDL_GetError());
		}
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	
	std::cout << "播放" << m_model->fileName(index).toStdString().c_str() << std::endl;
	Play(m_model->fileName(index).toStdString().c_str(), this, m_splitter);
}

void MyWidget::closeEvent(QCloseEvent* event)
{
	// 要使得 player.cpp从 MainLoop跳出
	SDL_Event Event;
	Event.type = SDL_QUIT;
	if (SDL_PushEvent(&Event) != 1)
	{
		fprintf(stderr, "Failed to push quit event: %s\n", SDL_GetError());
	}

}