#ifndef QMARKDOWNEDITOR_MAINWINDOW_H
#define QMARKDOWNEDITOR_MAINWINDOW_H

#include "HtmlConverter.hpp"
#include "settings.h"
#include <QApplication>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QMap>
#include <QMenuBar>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWebEngineView>

struct FileTab {
    QString filePath;
    QTextEdit *editor;
    QWebEngineView *preview;
    int scrollY;// 添加此字段用于存储滚动位置
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile(QListWidgetItem *item);
    void onThemeChanged(const QString &theme);
    void onTextChanged();
    void createNewFile();
    void deleteFile();
    void saveFile();
    void saveFileAs();
    void insertImage();
    void openFileDialog();
    void openFolderDialog();
    void onTabChanged(int index);// 新增的槽函数

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void setupUi();
    void loadFileList();
    void loadFile(const QString &filePath);
    void loadFilesInDirectory(const QString &folderPath);
    void loadLastOpenedFile();
    void saveLastOpenedFile(const QString &filePath);
    void loadSettings();
    void saveSettings();
    void updatePalette(const QString &theme) noexcept;
    inline void loadMarkdown(const QString &markdown, FileTab *tab) noexcept;
    void applyThemeToAllTabs();
    inline void refreshPreviews()noexcept;
    QString readFile(const QString &filePath);
    void autoSaveFile();


private:
    QSplitter *verticalSplitter;
    QListWidget *fileList;
    QTabWidget *fileTabs;
    Settings settings;
    QString currentTheme;
    QLabel *wordCountLabel;
    QLabel *lastSavedLabel;

    QList<FileTab *> openTabs;
    QTimer *autoSaveTimer;
    QTimer *debounceTimer;// 新增：防抖定时器
};

#endif// QMARKDOWNEDITOR_MAINWINDOW_H
