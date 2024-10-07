#include "mainwindow.h"
#include "iostream"
#include <QCloseEvent>
#include <QDateTime>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QKeySequence>
#include <QMessageBox>
#include <QSettings>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), verticalSplitter(new QSplitter(Qt::Horizontal, this)),
      fileList(new QListWidget(this)), fileTabs(new QTabWidget(this)),
      settings(), autoSaveTimer(new QTimer(this)), debounceTimer(new QTimer(this)) {

    setupUi();
    settings.loadSettings();      // 加载设置
    currentTheme = settings.theme;// 使用加载的主题
    updatePalette(currentTheme);
    loadLastOpenedFile();
    applyThemeToAllTabs();// 确保主题应用到所有标签页

    // 初始化防抖定时器
    debounceTimer->setInterval(50);// 300毫秒
    debounceTimer->setSingleShot(true);
    connect(debounceTimer, &QTimer::timeout, this, &MainWindow::refreshPreviews);

    // 设置图标
    QIcon icon("../wyw.ico");
    setWindowIcon(icon);
}

MainWindow::~MainWindow() {
    // 清理所有打开的标签页
    for (auto tab: openTabs) {
        delete tab->editor;
        delete tab->preview;
        delete tab;
    }
}

void MainWindow::setupUi() {
    setWindowTitle("Bunny Note");
    resize(1600, 1200);// 设置窗口默认大小

    // 设置文件列表
    fileList->setFont(QFont("Consolas", 15));
    connect(fileList, &QListWidget::itemClicked, this, &MainWindow::openFile);

    // 设置标签页
    fileTabs->setTabsClosable(true);
    connect(fileTabs, &QTabWidget::tabCloseRequested, this, [&](int index) {
        if (index < 0 || index >= openTabs.size()) {
            return;// 防止越界
        }
        FileTab *tab = openTabs.at(index);
        if (!tab->filePath.isEmpty()) {
            // 自动保存
            QFile file(tab->filePath);
            if (file.open(QFile::WriteOnly | QFile::Text)) {
                QTextStream out(&file);
                out << tab->editor->toPlainText();
                file.close();
            }
        }
        // 移除并删除标签页
        delete tab->editor;
        delete tab->preview;
        delete openTabs[index];
        openTabs.removeAt(index);
        fileTabs->removeTab(index);
    });


    // 连接 currentChanged 信号到 onTabChanged 槽函数
    connect(fileTabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    // 创建垂直分割器并添加组件
    verticalSplitter->addWidget(fileList);
    verticalSplitter->addWidget(fileTabs);
    verticalSplitter->setStretchFactor(0, 1);// fileList 占比
    verticalSplitter->setStretchFactor(1, 4);// fileTabs 占比
                                             // 设置手柄的宽度（可根据需要调整）
    verticalSplitter->setHandleWidth(8);

    // 应用样式表
    QString splitterStyle = R"(
                                             QSplitter::handle {
                                                 background-color: #cccccc; /* 手柄的背景颜色 */
                                             }
                                             QSplitter::handle:hover {
                                                 background-color: #aaaaaa; /* 当鼠标悬停时的颜色 */
                                             }
                                             QSplitter::handle:pressed {
                                                 background-color: #888888; /* 当手柄被按下时的颜色 */
                                             }
                                             QSplitter::handle:horizontal {
                                                 width: 8px; /* 水平分隔器的手柄宽度 */
                                                 border-left: 1px solid #dddddd;
                                                 border-right: 1px solid #dddddd;
                                             }
                                             QSplitter::handle:vertical {
                                                 height: 8px; /* 垂直分隔器的手柄高度 */
                                                 border-top: 1px solid #dddddd;
                                                 border-bottom: 1px solid #dddddd;
                                             }
                                         )";
    verticalSplitter->setStyleSheet(splitterStyle);

    // 设置初始大小比例
    QList<int> sizes;
    sizes << width() / 5 << (width() * 4) / 5;
    verticalSplitter->setSizes(sizes);

    setCentralWidget(verticalSplitter);

    loadFileList();

    // 创建菜单栏
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // 文件菜单
    QMenu *fileMenu = menuBar->addMenu("文件");
    fileMenu->setStyleSheet("QMenu { background: #FDF6E3; color: #657B83; }");
    QAction *newFileAction = new QAction("新建文件 CTRL+N", this);
    QAction *saveFileAction = new QAction("保存文件 CTRL+S", this);
    QAction *deleteFileAction = new QAction("删除文件 CTRL+D", this);
    QAction *fontAction = new QAction("设置字体 CTRL+P", this);
    QAction *openFileAction = new QAction("打开文件 CTRL+O", this);
    QAction *openFolderAction = new QAction("打开文件夹 CTRL+L", this);
    QAction *insertImageAction = new QAction("插入图片 CTRL+I", this);
    fileMenu->addAction(insertImageAction);
    connect(insertImageAction, &QAction::triggered, this, &MainWindow::insertImage);

    fileMenu->addAction(openFileAction);
    fileMenu->addAction(openFolderAction);

    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFileDialog);
    connect(openFolderAction, &QAction::triggered, this, &MainWindow::openFolderDialog);

    fileMenu->addAction(newFileAction);
    fileMenu->addAction(saveFileAction);
    fileMenu->addAction(deleteFileAction);
    fileMenu->addAction(fontAction);

    connect(newFileAction, &QAction::triggered, this, &MainWindow::createNewFile);
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(deleteFileAction, &QAction::triggered, this, &MainWindow::deleteFile);

    connect(fontAction, &QAction::triggered, [this]() {
        bool ok;
        QFont defaultFont = QFont(settings.font, settings.fontSize);
        QFont font = QFontDialog::getFont(&ok, defaultFont, this);
        if (ok) {
            settings.font = font.family();
            settings.fontSize = font.pointSize();
            // 更新所有打开的编辑器
            for (auto tab: openTabs) {
                tab->editor->setFont(font);
                loadMarkdown(tab->editor->toPlainText(), tab);
            }
            saveSettings();
        }
    });

    // 主题菜单
    QMenu *themeMenu = menuBar->addMenu("Themes");
    QStringList themes = {"Light", "Dark", "Solarized Light", "Solarized Dark"};
    for (const QString &theme: themes) {
        QAction *action = new QAction(theme, this);
        themeMenu->addAction(action);
        connect(action, &QAction::triggered, this, [this, theme]() {
            onThemeChanged(theme);
        });
    }

    // 初始化自动保存定时器
    connect(autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSaveFile);
    autoSaveTimer->start(10000);// 每10秒自动保存

    // 设置快捷键
    new QShortcut(QKeySequence("Ctrl+N"), this, SLOT(createNewFile()));
    new QShortcut(QKeySequence("Ctrl+S"), this, SLOT(saveFile()));
    new QShortcut(QKeySequence("Ctrl+D"), this, SLOT(deleteFile()));
    new QShortcut(QKeySequence("Ctrl+P"), this, SLOT(fontAction()));
    new QShortcut(QKeySequence("Ctrl+O"), this, SLOT(openFileDialog()));
    new QShortcut(QKeySequence("Ctrl+L"), this, SLOT(openFolderDialog()));
    new QShortcut(QKeySequence("Ctrl+I"), this, SLOT(insertImage()));

    // 创建状态栏
    QStatusBar *statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    // 初始化状态栏标签
    wordCountLabel = new QLabel("字数: 0", this);
    statusBar->addWidget(wordCountLabel);
    lastSavedLabel = new QLabel("上次保存: 从未", this);
    statusBar->addWidget(lastSavedLabel);
}
inline void MainWindow::refreshPreviews() noexcept {
    int currentIndex = fileTabs->currentIndex();
    if (currentIndex != -1 && currentIndex < openTabs.size()) {
        FileTab *currentTab = openTabs[currentIndex];
        QString markdown = currentTab->editor->toPlainText();
        loadMarkdown(markdown, currentTab);
    }
}


void MainWindow::applyThemeToAllTabs() {
    for (auto tab: openTabs) {
        // 应用编辑器的样式
        if (currentTheme == "Light") {
            tab->editor->setStyleSheet("background-color: white; color: black;");
        } else if (currentTheme == "Dark") {
            tab->editor->setStyleSheet("background-color: black; color: white;");
        } else if (currentTheme == "Solarized Light") {
            tab->editor->setStyleSheet("background-color: #FDF6E3; color: #657B83;");
        } else if (currentTheme == "Solarized Dark") {
            tab->editor->setStyleSheet("background-color: #073642; color: #839496;");
        }

        // 重新加载 Markdown 以应用主题变化
        loadMarkdown(tab->editor->toPlainText(), tab);
    }
}

void MainWindow::loadFileList() {
    fileList->clear();
    QDir dir(QDir::currentPath());
    QStringList files = dir.entryList(QStringList() << "*.md", QDir::Files);
    fileList->addItems(files);
}

void MainWindow::openFile(QListWidgetItem *item) {
    QString fileName = item->text();
    QString filePath = QDir::currentPath() + "/" + fileName;

    // 检查文件是否已在标签页中打开
    for (int i = 0; i < openTabs.size(); ++i) {
        if (openTabs[i]->filePath == filePath) {
            fileTabs->setCurrentIndex(i);
            return;
        }
    }

    // 创建新的标签页
    FileTab *newTab = new FileTab;
    newTab->filePath = filePath;
    newTab->editor = new QTextEdit(this);
    newTab->preview = new QWebEngineView(this);
    newTab->scrollY = 0;// 初始化滚动位置

    // 禁用滚动动画
    newTab->preview->settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, false);
    newTab->preview->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    // 启用 GPU 加速
    newTab->preview->settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    newTab->preview->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, true);

    // 尝试禁用平滑滚动
    newTab->preview->settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, false);


    // 应用设置
    QFont font(settings.font, settings.fontSize);
    newTab->editor->setFont(font);

    // 设置 Tab 停靠距离为 4 个字符宽度
    QFontMetrics metrics(font);
    int charWidth = metrics.horizontalAdvance(' ');// 获取空格字符的宽度
    newTab->editor->setTabStopDistance(4 * charWidth);

    // 设置主题样式
    if (currentTheme == "Light") {
        newTab->editor->setStyleSheet("background-color: white; color: black;");
    } else if (currentTheme == "Dark") {
        newTab->editor->setStyleSheet("background-color: black; color: white;");
    } else if (currentTheme == "Solarized Light") {
        newTab->editor->setStyleSheet("background-color: #FDF6E3; color: #657B83;");
    } else if (currentTheme == "Solarized Dark") {
        newTab->editor->setStyleSheet("background-color: #073642; color: #839496;");
    }

    // 加载文件内容
    QFile file(filePath);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        newTab->editor->setPlainText(QString::fromUtf8(file.readAll()));
        file.close();
    }

    // 设置预览
    loadMarkdown(newTab->editor->toPlainText(), newTab);

    // 连接文本变化信号
    connect(newTab->editor, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);

    // 创建布局
    QWidget *tabWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tabWidget);
    QSplitter *splitter = new QSplitter(Qt::Vertical, tabWidget);

    QString splitterStyle = R"(
    QSplitter::handle {
        background-color: #cccccc; /* 手柄的背景颜色 */
    }
    QSplitter::handle:hover {
        background-color: #aaaaaa; /* 鼠标悬停时的颜色 */
    }
    QSplitter::handle:pressed {
        background-color: #888888; /* 手柄被按下时的颜色 */
    }
    QSplitter::handle:horizontal {
        width: 8px; /* 水平分隔器的手柄宽度 */
        border-left: 1px solid #dddddd;
        border-right: 1px solid #dddddd;
    }
    QSplitter::handle:vertical {
        height: 8px; /* 垂直分隔器的手柄高度 */
        border-top: 1px solid #dddddd;
        border-bottom: 1px solid #dddddd;
    }
)";
    splitter->setStyleSheet(splitterStyle);
    splitter->addWidget(newTab->editor);
    splitter->addWidget(newTab->preview);
    splitter->setStretchFactor(0, 2);// 编辑区占比
    splitter->setStretchFactor(1, 3);// 预览区占比

    // 设置预览区的初始大小比例 (40% 编辑区, 60% 预览区)
    QList<int> splitterSizes;
    splitterSizes << height() * 2 / 5 << height() * 3 / 5;
    splitter->setSizes(splitterSizes);

    layout->addWidget(splitter);
    tabWidget->setLayout(layout);

    // 添加到标签页
    QString displayName = QFileInfo(filePath).fileName();

    QString tabWidgetStyle = R"(
QTabWidget::pane { /* 标签页框架样式 */
    border-top: 2px solid #C2C7CB;
    border-bottom-left-radius: 4px;
    border-bottom-right-radius: 4px;
    margin: 0px;
    padding: 0px;
}

QTabBar::tab {
    background: #F0F0F0;
    border: 1px solid #C2C7CB;
    padding: 10px;
    min-width: 100px;
    color: #333;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
    margin-right: 5px; /* 增加间距 */
    transition: background-color 0.2s ease-in-out; /* 平滑过渡 */
}

QTabBar::tab:selected {
    background: #FFFFFF;
    border-bottom-color: #FFFFFF; /* 使选中的标签看起来与面板连接 */
    color: #000; /* 选中状态文字颜色 */
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); /* 添加阴影效果 */
}

QTabBar::tab:hover {
    background: #E0E0E0;
}

QTabBar::tab:!selected {
    margin-top: 2px; /* 未选中的标签在顶部留出间隙 */
}

QTabBar::tab:last {
    margin-right: 0; /* 最后一个标签不增加间距 */
}

/* 美化关闭按钮 */
QTabBar::close-button {
    subcontrol-position: right center;
    subcontrol-origin: padding;
    background: #D3D3D3; /* 设置默认背景色 */
    border: 1px solid #C2C7CB; /* 添加边框 */
    border-radius: 8px; /* 圆角 */
    width: 16px;
    height: 16px;
    margin: 0px;
    padding: 0px;
}

QTabBar::close-button:hover {
    background: red; /* 鼠标悬停时背景色变为红色 */
    border: 1px solid red; /* 边框变为红色 */
}

QTabBar::close-button:pressed {
    background: darkred; /* 按下时背景色变为深红 */
}
)";

    fileTabs->setStyleSheet(tabWidgetStyle);
    fileTabs->addTab(tabWidget, displayName);
    openTabs.append(newTab);// 确保同步
    fileTabs->setCurrentWidget(tabWidget);

    // 更新最近打开的文件
    saveLastOpenedFile(filePath);
}


void MainWindow::createNewFile() {
    bool ok;
    QString fileName = QInputDialog::getText(this, "新建文件", "输入文件名（不含后缀）:", QLineEdit::Normal, "", &ok);

    if (ok && !fileName.isEmpty()) {
        QString fullFileName = fileName + ".md";
        QString filePath = QDir::currentPath() + "/" + fullFileName;

        // 创建空文件
        QFile file(filePath);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "创建文件失败", "无法创建文件。");
            return;
        }
        file.close();

        // 更新文件列表
        loadFileList();

        // 打开新文件
        QList<QListWidgetItem *> items = fileList->findItems(fullFileName, Qt::MatchExactly);
        if (!items.isEmpty()) {
            openFile(items.first());
        }
    }
}

void MainWindow::deleteFile() {
    QListWidgetItem *item = fileList->currentItem();
    if (item) {
        QString fileName = item->text();
        QString filePath = QDir::currentPath() + "/" + fileName;
        if (QFile::remove(filePath)) {
            // 关闭已打开的标签页
            for (int i = 0; i < openTabs.size(); ++i) {
                if (openTabs[i]->filePath == filePath) {
                    fileTabs->removeTab(i);
                    delete openTabs[i]->editor;
                    delete openTabs[i]->preview;
                    delete openTabs[i];
                    openTabs.removeAt(i);
                    break;
                }
            }
            // 从列表中移除
            delete fileList->takeItem(fileList->row(item));
            QMessageBox::information(this, "删除文件", "文件已成功删除。");
        } else {
            QMessageBox::warning(this, "删除文件", "删除文件失败。");
        }
    }
}

void MainWindow::saveFile() {
    int currentIndex = fileTabs->currentIndex();
    if (currentIndex != -1 && currentIndex < openTabs.size()) {
        FileTab *currentTab = openTabs[currentIndex];
        if (currentTab->filePath.isEmpty()) {
            saveFileAs();
            return;
        }

        QFile file(currentTab->filePath);
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            out << currentTab->editor->toPlainText();
            file.close();
            lastSavedLabel->setText(QString("上次保存: %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
        } else {
            QMessageBox::warning(this, "保存失败", "无法保存文件。");
        }
    }
}

void MainWindow::saveFileAs() {
    int currentIndex = fileTabs->currentIndex();
    if (currentIndex != -1 && currentIndex < openTabs.size()) {
        FileTab *currentTab = openTabs[currentIndex];
        QString fileName = QFileDialog::getSaveFileName(this, "另存为", "", "Markdown Files (*.md);;All Files (*)");
        if (!fileName.isEmpty()) {
            currentTab->filePath = fileName;
            QFile file(fileName);
            if (file.open(QFile::WriteOnly | QFile::Text)) {
                QTextStream out(&file);
                out << currentTab->editor->toPlainText();
                file.close();
                // 更新标签标题
                QString displayName = QFileInfo(fileName).fileName();
                fileTabs->setTabText(currentIndex, displayName);
                // 更新文件列表
                loadFileList();
                lastSavedLabel->setText(QString("上次保存: %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
            } else {
                QMessageBox::warning(this, "保存失败", "无法保存文件。");
            }
        }
    }
}


void MainWindow::openFileDialog() {
    QString filePath = QFileDialog::getOpenFileName(this, "打开文件", "", "Markdown Files (*.md);;All Files (*)");
    if (!filePath.isEmpty()) {
        // 提取文件名
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();
        // 设置当前目录
        QDir::setCurrent(fileInfo.dir().absolutePath());
        // 检查文件是否已在列表中
        if (!fileList->findItems(fileName, Qt::MatchExactly).count()) {
            fileList->addItem(fileName);
        }
        // 查找并打开文件
        QList<QListWidgetItem *> items = fileList->findItems(fileName, Qt::MatchExactly);
        if (!items.isEmpty()) {
            openFile(items.first());
        }
    }
}

void MainWindow::openFolderDialog() {
    QString folderPath = QFileDialog::getExistingDirectory(this, "打开文件夹");
    if (!folderPath.isEmpty()) {
        QDir::setCurrent(folderPath);
        loadFilesInDirectory(folderPath);
    }
}

void MainWindow::loadFile(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists() && fileInfo.isFile()) {
        // 打开文件并添加到文件列表
        QString fileName = fileInfo.fileName();
        if (!fileList->findItems(fileName, Qt::MatchExactly).count()) {
            fileList->addItem(fileName);
        }
        QList<QListWidgetItem *> items = fileList->findItems(fileName, Qt::MatchExactly);
        if (!items.isEmpty()) {
            openFile(items.first());
        }
    }
}

void MainWindow::loadFilesInDirectory(const QString &folderPath) {
    QDir dir(folderPath);
    QStringList files = dir.entryList(QStringList() << "*.md", QDir::Files);
    fileList->clear();
    fileList->addItems(files);
}

void MainWindow::autoSaveFile() {
    for (int i = 0; i < openTabs.size(); ++i) {
        FileTab *tab = openTabs[i];
        if (!tab->filePath.isEmpty()) {
            QFile file(tab->filePath);
            if (file.open(QFile::WriteOnly | QFile::Text)) {
                QTextStream out(&file);
                out << tab->editor->toPlainText();
                file.close();
                // 仅更新当前标签的保存时间
                if (fileTabs->currentIndex() == i) {
                    lastSavedLabel->setText(QString("上次保存: %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
                }
            }
        }
    }
}

void MainWindow::onTextChanged() {
    // 更新当前标签的预览和字数
    int currentIndex = fileTabs->currentIndex();
    if (currentIndex != -1 && currentIndex < openTabs.size()) {
        FileTab *currentTab = openTabs[currentIndex];
        QString markdown = currentTab->editor->toPlainText();
        loadMarkdown(markdown, currentTab);// 传递正确的 FileTab*

        // 更新字数
        int charCount = markdown.length();
        wordCountLabel->setText(QString("字数: %1").arg(charCount));
    }
}

void MainWindow::insertImage() {
    int currentIndex = fileTabs->currentIndex();
    if (currentIndex == -1 || currentIndex >= openTabs.size()) {
        QMessageBox::warning(this, "插入图片", "没有打开的文件。");
        return;
    }

    FileTab *currentTab = openTabs[currentIndex];
    QString imagePath = QFileDialog::getOpenFileName(this, "选择图片", "", "Image Files (*.png *.jpg *.jpeg *.bmp *.gif);;All Files (*)");
    if (!imagePath.isEmpty() && !currentTab->filePath.isEmpty()) {
        QFileInfo currentFileInfo(currentTab->filePath);
        QString imagesDir = currentFileInfo.dir().absoluteFilePath("images");
        QDir().mkpath(imagesDir);

        QString imageName = QFileInfo(imagePath).fileName();
        QString newImagePath = imagesDir + "/" + imageName;

        if (!QFile::exists(newImagePath)) {
            if (!QFile::copy(imagePath, newImagePath)) {
                QMessageBox::critical(this, "错误", "无法复制图片");
                return;
            }
        }

        QString relativePath = QDir::current().relativeFilePath(newImagePath);// 使用相对路径

        QTextCursor cursor = currentTab->editor->textCursor();
        cursor.insertText(QString("![图片](%1)").arg(relativePath));// 插入Markdown
        currentTab->editor->setTextCursor(cursor);

        QString markdown = currentTab->editor->toPlainText();
        loadMarkdown(markdown, currentTab);// 传递正确的 FileTab*
        onTextChanged();
    } else {
        QMessageBox::warning(this, "警告", "请先保存文件后再插入图片。");
    }
}


inline void MainWindow::loadMarkdown(const QString &markdown, FileTab *tab) noexcept {
    if (!tab || !tab->preview)
        return;

    // 将 Markdown 转换为 HTML
    QString html = HtmlConverter::convertToHtml(markdown);

    // 获取样式和主题
    QPalette globalPalette = QApplication::palette();
    QString bgColor = globalPalette.color(QPalette::Window).name();
    QString textColor = globalPalette.color(QPalette::WindowText).name();
    QString fontFamily = tab->editor->font().family();
    int fontSize = tab->editor->font().pointSize();

    // 引入 Highlight.js 的 CSS 和 JS
    static QString highlightCss = R"(
        <link rel="stylesheet"
              href="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.7.0/styles/github.min.css">
    )";

    static QString highlightJs = R"(
        <script src="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.7.0/highlight.min.js"></script>
        <script>hljs.highlightAll();</script>
    )";

    // 构建完整的 HTML
    QString combinedHtml = QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            %1
            %2
            <style>
                body {
                    background-color: %3;
                    color: %4;
                    font-family: '%5';
                    font-size: %6pt;
                    padding: 20px;
                    overflow-y: scroll;
                }
                pre, code {
                    tab-size: 4;
                    -moz-tab-size: 4;
                    -o-tab-size: 4;
                }
                pre {
                    background-color: #f0f0f0;
                    color: #333333;
                    padding: 10px;
                    border-radius: 5px;
                    overflow: auto;
                }
                code {
                    background-color: #f0f0f0;
                    color: #333333;
                    padding: 2px 4px;
                    border-radius: 3px;
                }
            </style>
        </head>
        <body>
            %7
        </body>
        </html>
    )")
                                   .arg(highlightCss)
                                   .arg(highlightJs)
                                   .arg(bgColor)
                                   .arg(textColor)
                                   .arg(fontFamily)
                                   .arg(fontSize)
                                   .arg(html);

    // 设置 HTML 内容
    tab->preview->setHtml(combinedHtml);
}


void MainWindow::onThemeChanged(const QString &theme) {
    currentTheme = theme;
    updatePalette(theme);
    applyThemeToAllTabs();// 确保所有标签页应用新主题
}

inline void MainWindow::updatePalette(const QString &theme) noexcept {
    QPalette palette;

    if (theme == "Light") {
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::WindowText, Qt::black);
        // 更新编辑器和文件列表的样式
        QString style = "background-color: white; color: black;";
        for (auto tab: openTabs) {
            tab->editor->setStyleSheet(style);
        }
        fileList->setStyleSheet(style);
        menuBar()->setStyleSheet("QMenuBar { background: white; color: black; } QMenu { background: white; color: black; }");
    } else if (theme == "Dark") {
        palette.setColor(QPalette::Window, Qt::black);
        palette.setColor(QPalette::WindowText, Qt::white);
        QString style = "background-color: black; color: white;";
        for (auto tab: openTabs) {
            tab->editor->setStyleSheet(style);
        }
        fileList->setStyleSheet(style);
        menuBar()->setStyleSheet("QMenuBar { background: black; color: white; } QMenu { background: black; color: white; }");
    } else if (theme == "Solarized Light") {
        palette.setColor(QPalette::Window, QColor("#FDF6E3"));
        palette.setColor(QPalette::WindowText, QColor("#657B83"));
        QString style = "background-color: #FDF6E3; color: #657B83;";
        for (auto tab: openTabs) {
            tab->editor->setStyleSheet(style);
        }
        fileList->setStyleSheet(style);
        menuBar()->setStyleSheet("QMenuBar { background: #FDF6E3; color: #657B83; } QMenu { background: #FDF6E3; color: #657B83; }");
    } else if (theme == "Solarized Dark") {
        palette.setColor(QPalette::Window, QColor("#073642"));
        palette.setColor(QPalette::WindowText, QColor("#839496"));
        QString style = "background-color: #073642; color: #839496;";
        for (auto tab: openTabs) {
            tab->editor->setStyleSheet(style);
        }
        fileList->setStyleSheet(style);
        menuBar()->setStyleSheet("QMenuBar { background: #073642; color: #839496; } QMenu { background: #073642; color: #839496; }");
    }

    QApplication::setPalette(palette);
}

QString MainWindow::readFile(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QFile::ReadOnly)) {
        QString content = QString::fromUtf8(file.readAll());
        file.close();
        return content;
    }
    return QString();
}

void MainWindow::loadLastOpenedFile() {
    QSettings settings("MyApp", "MarkdownEditor");
    QString lastFile = settings.value("lastOpenedFile", "").toString();
    if (!lastFile.isEmpty()) {
        QFileInfo fileInfo(lastFile);
        if (fileInfo.exists()) {
            QDir::setCurrent(fileInfo.dir().absolutePath());
            loadFile(lastFile);
        }
    }
}

void MainWindow::saveLastOpenedFile(const QString &filePath) {
    QSettings settings("MyApp", "MarkdownEditor");
    settings.setValue("lastOpenedFile", filePath);
}

void MainWindow::loadSettings() {
    settings.loadSettings();
}

void MainWindow::saveSettings() {
    settings.saveSettings();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // 保存所有打开的文件
    for (int i = 0; i < openTabs.size(); ++i) {
        FileTab *tab = openTabs[i];
        if (!tab->filePath.isEmpty()) {
            QFile file(tab->filePath);
            if (file.open(QFile::WriteOnly | QFile::Text)) {
                QTextStream out(&file);
                out << tab->editor->toPlainText();
                file.close();
            }
        }
    }
    settings.theme = currentTheme;
    if (!openTabs.isEmpty()) {
        settings.saveLastOpenedFile(openTabs.first()->filePath);
    }
    settings.saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::onTabChanged(int index) {
    if (index < 0 || index >= openTabs.size()) {
        fileList->clearSelection();
        return;
    }
    FileTab *currentTab = openTabs.at(index);
    QString fileName = QFileInfo(currentTab->filePath).fileName();

    // 在 fileList 中找到对应的项并选中
    QList<QListWidgetItem *> items = fileList->findItems(fileName, Qt::MatchExactly);
    if (!items.isEmpty()) {
        QListWidgetItem *item = items.first();
        fileList->setCurrentItem(item);
        fileList->scrollToItem(item);// 确保选中的项可见
    }
}
