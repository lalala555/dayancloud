#pragma once

#include "Dialog.h"
#include "ui_PicturesViewer.h"

class ClickLabel : public QLabel 
{
    Q_OBJECT
public:
    ClickLabel(QWidget *parent = Q_NULLPTR) :QLabel(parent), m_showBtn(false){
        m_delBtn = new QPushButton(this);
        m_delBtn->setStyleSheet("QPushButton{border-image:url(:/images/delete.png);border:"
            "none;background-color:transparent;max-height:15px;max-width:15px;padding:0px;}"
            "QPushButton:hover{border-image:url(:/images/delete.png);} ");
        m_delBtn->hide();
        connect(m_delBtn, SIGNAL(clicked()), this, SLOT(onDeleteClicked()));
    }
    void setPicPath(const QString& path) { m_picPath = path; }
    QString getPicPath() { return m_picPath; }
    void setPosX(qreal x) { m_posX = x; }

protected:
    virtual void mousePressEvent(QMouseEvent * e)
    {
        emit clicked(m_picPath);
        QLabel::mousePressEvent(e);
    }

    virtual void enterEvent(QEvent *e)
    {
        // m_delBtn->show();
        // m_showBtn = true;
        // QPoint pos = m_delBtn->pos();
        // pos.setX(this->width() - m_delBtn->width());
        // m_delBtn->move(pos);
        QLabel::enterEvent(e);
    }

    virtual void leaveEvent(QEvent *e)
    {
        // m_delBtn->hide();
        // m_showBtn = false;
        QLabel::leaveEvent(e);
    }

    virtual void paintEvent(QPaintEvent *e)
    {
        Q_UNUSED(e);
        QLabel::paintEvent(e);
    }

signals:
    void clicked(const QString&);
    void deleteClicked(const QString&);

private slots:
    void onDeleteClicked() 
    {
        emit clicked(m_picPath);
    }

private:
    QString m_picPath;
    bool m_showBtn;
    QPushButton* m_delBtn;
    qreal m_posX;
};

class PicturesViewer : public Dialog
{
    Q_OBJECT

public:
    PicturesViewer(const QStringList& pictures, QWidget *parent = Q_NULLPTR);
    ~PicturesViewer();

public:
    void initGraphicsView();
    void showPicture(const QString& item);

public slots:
    void on_btnPreviousShow_clicked();
    void on_btnNextShow_clicked();
    void on_btnPreviousList_clicked();
    void on_btnNextList_clicked();
    void onItemClicked(const QString& path);
    void onDeleteClicked(const QString& path);

protected:
    virtual void showEvent(QShowEvent *event);

private:
    Ui::PicturesViewer ui;
    QStringList m_pictures;
    QGraphicsScene* m_sceneShow;
    QGraphicsScene* m_sceneList;
    int m_currentItem;

};
