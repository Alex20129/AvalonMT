#ifndef ASICTABLEWIDGET_H
#define ASICTABLEWIDGET_H

#include <QDesktopServices>
#include "asicdevice.h"

class QTableWidgetIPItem : public QTableWidgetItem
{
private:
public:
    virtual void setData(int role, const QVariant &value)
    {
        if(role==Qt::DisplayRole)
        {
            QTableWidgetItem::setData(Qt::DisplayRole, value.toUInt());
            QTableWidgetItem::setData(Qt::EditRole, value.toUInt());
        }
        else
        {
            QTableWidgetItem::setData(role, value);
        }
    }
    virtual QVariant data(int role) const
    {
        if(role==Qt::DisplayRole)
        {
            return QVariant(QHostAddress(QTableWidgetItem::data(Qt::DisplayRole).toUInt()).toString());
        }
        else
        {
            return QTableWidgetItem::data(role);
        }
    }
    bool operator <(const QTableWidgetItem &other) const
    {
        return QTableWidgetItem::data(Qt::EditRole).toUInt()<other.data(Qt::EditRole).toUInt();
    }
};

class ASICTableWidget : public QTableWidget
{
    Q_OBJECT
signals:
public:
    explicit ASICTableWidget(QWidget *parent=nullptr);
    void SetUserName(QString userName);
    void SetPassword(QString passWord);
    void SetWebPort(quint16 port);
    void SetAPIPort(quint16 port);
    void SetGroupID(uint id);
    QString Description, Title;
    uint GroupID;
    QVector <ASICDevice *> *DeviceList;
    QStringList *ColumnTitles;
public slots:
    void AddDevice(ASICDevice *deviceToAdd);
    void RemoveDevice(ASICDevice *deviceToRemove);
    //void Refresh();
    void on_cellDoubleClicked(int row, int column);
private:
    QString pUserName, pPassWord;
    quint16 pWebPort;
    quint16 pAPIPort;
    uint pGroupID;
private slots:
};

#endif // ASICTABLEWIDGET_H
