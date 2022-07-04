#include "stdafx.h"
#include "LoadConfigDataThread.h"

LoadConfigDataThread::LoadConfigDataThread(const ConfigAnalyzeParams& params, QObject *parent)
    : QThread(parent)
    , m_params(params)
{
    if (!m_params.fieldOrder.isEmpty()) {
        m_fieldNames = m_params.fieldOrder.toLower().split("|");
    }
}

LoadConfigDataThread::~LoadConfigDataThread()
{
}

int LoadConfigDataThread::filePreCheck()
{
    QString filePath = m_params.filePath;
    QFile file(filePath);
    if (!file.exists()) {
        return CODE_FILE_NOT_EXIST_ERROR;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        file.close();
        return CODE_FILE_LOAD_FAILED_ERROR;
    }

    QString line;
    int lineCount = m_params.ignoreLines.toInt() + 1;
    QString ch = m_params.spliteChar;
    qDeleteAll(m_dataItems);
    m_dataItems.clear();
    int exit_code = CODE_SUCCESS;
    int read_line_count = 10;


    // 读取内容并解析，如果每行的内容都解析都失败，说明忽略行设置错误
    int successCount = 0;
    while (!file.atEnd() && lineCount > 0) {
        line = file.readLine();
        QStringList list = line.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        if (list.isEmpty()) continue;
        line = list.at(0);
        if (line.isEmpty()) {
            file.close();
            emit loadDataFinished(CODE_IGNORE_LINE_SET_ERROR);
            return CODE_IGNORE_LINE_SET_ERROR;
        }

        QStringList items;
        for (int i = 0; i < EN_SPLITE_TOTAL; i++) {
            items = line.split(RBHelper::getSplitChar(i));
            if (items.size() >= 3) {
                successCount++;
            }
        }
        lineCount--;
    }
    if (successCount == 0) {
        emit loadDataFinished(CODE_IGNORE_LINE_SET_ERROR);
        return CODE_IGNORE_LINE_SET_ERROR;
    }

    lineCount = m_params.ignoreLines.toInt();
    while (!file.atEnd() && read_line_count > 0) {
        line = file.readLine();
        if (lineCount-- > 0) continue;
        QStringList list = line.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        if (list.isEmpty()) continue;
        line = list.at(0);
        if (line.isEmpty()) {
            file.close();
            return CODE_IGNORE_LINE_SET_ERROR;
        }

        QStringList items = line.split(ch);
        if (items.isEmpty() || items.size() < 3) {
            file.close();
            return CODE_SPLITE_CHAR_SET_ERROR;
        }

        QString name;
        int index = getFieldOrderIndex("name");
        if (index >= 0 && index < items.size()) {
            name = items[index];
        } else {
            name = items[0];
        }

        // 检查文件是否匹配
        QFileInfo info(name);
        if (m_params.fileType == enTypePosFile) {
            if (info.suffix().isEmpty())
                return CODE_ERROR_NOT_POS_FILE;
        } else if (m_params.fileType == enTypeCtrlPointFile) {
            if (!info.suffix().isEmpty())
                return CODE_ERROR_NOT_CTRL_FILE;
        }

        --read_line_count;
    }

    return exit_code;
}

void LoadConfigDataThread::run()
{
    QString filePath = m_params.filePath;
    QFile file(filePath);
    if (!file.exists()) {
        emit loadDataFinished(CODE_FILE_NOT_EXIST_ERROR);
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        file.close();
        emit loadDataFinished(CODE_FILE_LOAD_FAILED_ERROR);
        return;
    }

    QString line;
    int lineCount = m_params.ignoreLines.toInt() + 1;
    QString ch = m_params.spliteChar;
    qDeleteAll(m_dataItems);
    m_dataItems.clear();
    int exit_code = CODE_SUCCESS;

#if 0
    // 读取内容并解析，如果每行的内容都解析都失败，说明忽略行设置错误
    int successCount = 0;
    while (!file.atEnd() && lineCount > 0) {
        line = file.readLine();
        QStringList list = line.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        if (list.isEmpty()) continue;
        line = list.at(0);
        if (line.isEmpty()) {
            file.close();
            emit loadDataFinished(CODE_IGNORE_LINE_SET_ERROR);
            return;
        }

        QStringList items;
        for (int i = 0; i < EN_SPLITE_TOTAL; i++) {
            items = line.split(RBHelper::getSplitChar(i));
            if (items.size() >= 3) {
                successCount++;
            }
        }
        lineCount--;
    }
    if (successCount == 0) {
        emit loadDataFinished(CODE_IGNORE_LINE_SET_ERROR);
        return;
    }
#endif

    lineCount = m_params.ignoreLines.toInt();
    while (!file.atEnd()) {
        line = file.readLine();
        if (lineCount-- > 0) continue;
        QStringList list = line.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        if (list.isEmpty()) continue;
        line = list.at(0);
        if (line.isEmpty()) {
            file.close();
            emit loadDataFinished(CODE_IGNORE_LINE_SET_ERROR);
            return;
        }

        QStringList items = line.split(ch);
        if (items.isEmpty() || items.size() < 3) {
            file.close();
            emit loadDataFinished(CODE_SPLITE_CHAR_SET_ERROR);
            return;
        }

        ConfigDataItem *pos = new ConfigDataItem;

        // index
        pos->index = m_dataItems.size() + 1;

        // name
        int index = getFieldOrderIndex("name");
        if (index >= 0 && index < items.size()) {
            pos->imageName = items[index];
        } else {
            pos->imageName = items[0];
        }

        // Longitude
        index = getFieldOrderIndex("longitude");
        if (index == -1) {
            index = getFieldOrderIndex("x");
        }
        if (index >= 0 && index < items.size()) {
            pos->picLongitude = items[index];
        } else {
            pos->picLongitude = items[1];
        }
        
        // Latitude
        index = getFieldOrderIndex("latitude");
        if (index == -1) {
            index = getFieldOrderIndex("y");
        }
        if (index >= 0 && index < items.size()) {
            pos->picLatitude = items[index];
        } else {
            pos->picLatitude = items[2];
        }
        
        // Altitude
        index = getFieldOrderIndex("altitude");
        if (index >= 0 && index < items.size()) {
            pos->picAltitude = items[index];
        } else {
            pos->picAltitude = items[3];
        }      

        m_dataItems.append(pos);
    }
    file.close();

    exit_code = m_dataItems.isEmpty() ? CODE_ILLEGAL_FILE : exit_code;

    emit loadDataFinished(exit_code);
}

int LoadConfigDataThread::getFieldOrderIndex(const QString& fieldName)
{
    if (m_fieldNames.isEmpty() || fieldName.isEmpty()) {
        return -1;
    }

    return m_fieldNames.indexOf(fieldName.toLower());
}

QList<ConfigDataItem*> LoadConfigDataThread::getConfigDataItems()
{
    return m_dataItems;
}

void LoadConfigDataThread::onLoadConfigDataFinished(int code)
{
    emit loadConfigDataFinished(code);
}

qint64 LoadConfigDataThread::getTaskId()
{
    return m_params.taskId;
}

bool LoadConfigDataThread::checkRowNumEnable(const QString& line)
{
    return false;
}