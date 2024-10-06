//
// Created by CaoMengxuan on 2024/10/5.
//

#ifndef QMARKDOWNEDITOR_HTMLCONVERTER_HPP
#define QMARKDOWNEDITOR_HTMLCONVERTER_HPP

#include <cmark.h>
#include <QString>

class HtmlConverter {
public:
    inline static QString convertToHtml(QString markdown){
        QByteArray markdown_bytes = markdown.toUtf8();
        const char *markdown_c_str = markdown_bytes.constData();

        cmark_node *doc = cmark_parse_document(markdown_c_str, markdown_bytes.size(), CMARK_OPT_DEFAULT);
        char *html = cmark_render_html(doc, CMARK_OPT_DEFAULT);
        QString result = QString::fromUtf8(html);
        cmark_node_free(doc);
        free(html);
        return result;
    }

};

#endif//QMARKDOWNEDITOR_HTMLCONVERTER_HPP
