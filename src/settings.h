#ifndef QMARKDOWNEDITOR_SETTINGS_HPP
#define QMARKDOWNEDITOR_SETTINGS_HPP

#include <QJsonObject>
#include <QFile>
#include <QJsonDocument>
#include <QDir>

class Settings {
public:
    void loadSettings();
    void saveSettings();
    void saveLastOpenedFile(const QString &filePath);
    QString getLastOpenedFile() const;

    QString theme; // 主题
    QString font;  // 字体
    int fontSize;  // 字体大小

private:
    const QString settingsFilePath = QDir::homePath() + "/markdown_editor_settings.json"; // 设置文件路径
    QString lastOpenedFile; // 最近打开的文件路径
};

#endif // QMARKDOWNEDITOR_SETTINGS_HPP
