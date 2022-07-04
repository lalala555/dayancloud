#include "stdafx.h"
#include "PicturesViewer.h"
#include <QGLWidget>
#include <QGLFormat>

PicturesViewer::PicturesViewer(const QStringList& pictures, QWidget *parent)
    : Dialog(parent)
    , m_pictures(pictures)
    , m_sceneShow(new QGraphicsScene(this))
    , m_sceneList(new QGraphicsScene(this))
    , m_currentItem(0)
{
    ui.setupUi(this);

    ui.graphicsViewShow->setScene(m_sceneShow);
    ui.graphicsViewList->setScene(m_sceneList);

    // ui.graphicsViewShow->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    // ui.graphicsViewList->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));

    ui.graphicsViewShow->setStyleSheet("QGraphicsView{border:none;background-color:transparent;}");
    ui.graphicsViewList->setStyleSheet("QGraphicsView{border:none;background-color:transparent;}");

    ui.btnPreviousList->hide();
    ui.btnNextList->hide();
    ui.graphicsViewList->hide();

    ui.graphicsViewList->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui.graphicsViewList->setRenderHint(QPainter::Antialiasing);

    // connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem *item)), this, SLOT(onItemClicked(QListWidgetItem *item)));
}

PicturesViewer::~PicturesViewer()
{
    m_sceneShow->deleteLater();
    m_sceneList->deleteLater();
}

void PicturesViewer::initGraphicsView()
{
    // 添加图片到graphic list
    for (int i = 0; i < m_pictures.size(); i++) {
        QImage image(m_pictures[i]);
        QPixmap pixmap = QPixmap::fromImage(image);
        pixmap = pixmap.scaledToHeight(ui.listWidget->rect().height() - 15);
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pixmap);
        m_sceneList->addItem(item);
        item->setOffset(i * pixmap.width() + 100, pixmap.height());
        // item->setPos(i * pixmap.width() + 10, pixmap.height()); 

        QListWidgetItem* listItem = new QListWidgetItem(ui.listWidget);
        listItem->setSizeHint(pixmap.size());
        listItem->setData(Qt::UserRole, QVariant::fromValue<QString>(m_pictures[i]));
        listItem->setFlags(listItem->flags() & Qt::ItemIsEnabled);
        ClickLabel* label = new ClickLabel(ui.listWidget);
        label->setPixmap(pixmap);
        label->setPicPath(m_pictures[i]);
        label->setPosX(i * pixmap.width() + 100);
        ui.listWidget->addItem(listItem);
        ui.listWidget->setItemWidget(listItem, label);
        connect(label, SIGNAL(clicked(const QString&)), this, SLOT(onItemClicked(const QString&)));
    }

    // 选择一张图片显示大图
    if (!m_pictures.isEmpty()) {
        QList<QGraphicsItem*> items = m_sceneShow->items();
        for (int i = 0; i < items.size(); i++) {
            m_sceneShow->removeItem(items[i]);
        }

        QPixmap pixmap = QPixmap::fromImage(QImage(m_pictures[0]));
        pixmap = pixmap.scaledToHeight(ui.graphicsViewShow->rect().height());
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pixmap);
        m_sceneShow->addItem(item);
    }
}

void PicturesViewer::showPicture(const QString& item)
{
    if (item.isEmpty())
        return;

    QList<QGraphicsItem*> items = m_sceneShow->items();
    for (int i = 0; i < items.size(); i++) {
        m_sceneShow->removeItem(items[i]);
    }

    QPixmap pixmap = QPixmap::fromImage(QImage(item));
    pixmap = pixmap.scaledToHeight(ui.graphicsViewShow->rect().height());
    QGraphicsPixmapItem* pix = new QGraphicsPixmapItem(pixmap);
    m_sceneShow->addItem(pix);
}

void PicturesViewer::on_btnPreviousShow_clicked()
{
    m_currentItem -= 1;
    if (m_currentItem < 0) {
        m_currentItem = 0;
    }

    if (m_pictures.size() > 0) {
        showPicture(m_pictures[m_currentItem]);
    }  
}

void PicturesViewer::on_btnNextShow_clicked()
{
    m_currentItem += 1;
    if (m_currentItem >= m_pictures.size()) {
        m_currentItem = 0;
    }   

    if (m_pictures.size() > 0) {
        showPicture(m_pictures[m_currentItem]);
    }
}

void PicturesViewer::on_btnPreviousList_clicked()
{
    on_btnPreviousShow_clicked();
}

void PicturesViewer::on_btnNextList_clicked()
{
    on_btnNextShow_clicked();
}

void PicturesViewer::showEvent(QShowEvent *event)
{
    initGraphicsView();

    Dialog::showEvent(event);
}

void PicturesViewer::onItemClicked(const QString& path)
{
    showPicture(path);
}

void PicturesViewer::onDeleteClicked(const QString& path)
{
    // if (path.isEmpty())return;
    // m_pictures.removeOne(path);
    // initGraphicsView();
}