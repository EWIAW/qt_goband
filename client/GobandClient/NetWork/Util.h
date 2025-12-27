#ifndef UTIL_H
#define UTIL_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QDebug>

//工具类文件

//封装json工具类
class Json_Util
{
public:
    //将json数据转换成QByteArray
    static QByteArray jsonToByteArray(const QJsonDocument &doc)
    {
        if (doc.isNull())
        {
            qWarning() << "[" << __FILE__ << ":" << __LINE__ << "]" << Q_FUNC_INFO << "[jsonToByteArray] QJsonDocument is null!";
            return QByteArray();
        }

        QByteArray data = doc.toJson(QJsonDocument::Compact);

        if (data.isEmpty())
        {
            qWarning() << "[jsonDocumentToByteArray] Failed to convert QJsonDocument to QByteArray!";
        }

        return data;
    }

    //将QByteArray转换成json
    static QJsonDocument byteArrayToJson(const QByteArray &data)
    {
        if (data.isEmpty())
        {
            qWarning() << "[byteArrayToJsonDocument] Input QByteArray is empty!";
            return QJsonDocument();
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError)
        {
            qWarning() << "[byteArrayToJsonDocument] JSON parse error at offset"
                       << parseError.offset << ":" << parseError.errorString();
            return QJsonDocument();
        }

        return doc;
    }
};


//class Util
//{
//public:
//    Util();
//};

#endif // UTIL_H
