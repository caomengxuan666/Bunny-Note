#include "settings.h"

void Settings::loadSettings() {
    QFile file(settingsFilePath);
    if (file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject json = doc.object();

        theme = json.value("theme").toString("Solarized Light"); // 默认主题
        font = json.value("font").toString("Arial"); // 默认字体
        fontSize = json.value("fontSize").toInt(12); // 默认字体大小
        lastOpenedFile = json.value("lastOpenedFile").toString(); // 加载最近打开文件路径

        file.close();
    }
}

void Settings::saveSettings() {
    QJsonObject json;
    json["theme"] = theme;
    json["font"] = font;
    json["fontSize"] = fontSize;
    json["lastOpenedFile"] = lastOpenedFile; // 保存最近打开文件路径

    QJsonDocument doc(json);
    QFile file(settingsFilePath);
    if (file.open(QFile::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}
void Settings::saveLastOpenedFile(const QString &filePath) {
    lastOpenedFile = filePath; // 更新成员变量
    saveSettings(); // 立即保存设置
}

QString Settings::getLastOpenedFile() const {
    return lastOpenedFile; // 返回最近打开的文件路径
}
