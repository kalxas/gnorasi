#include "ontologyclassview.h"

#include "../models/ontologyclass.h"
//#include "../models/ontologyclassmodel.h"
#include "../utils/ontologyclassificationmanager.h"
#include "../utils/objectlevelmanager.h"

#include "classdescriptiondialog.h"

#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMessageBox>

#include <QStandardItemModel>

#include <QtCore/QDebug>
#include <QColorDialog>

using namespace voreen;

OntologyClassView::OntologyClassView(QWidget *parent)
    : QTreeView(parent)
{
    setAlternatingRowColors(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
//    setSortingEnabled(true);
}

void OntologyClassView::contextMenuEvent ( QContextMenuEvent * e )
{
    QStandardItemModel *omodel = qobject_cast<QStandardItemModel*>(model());
    if(!omodel){
        QAbstractItemView::contextMenuEvent(e);
        return;
    }

//    if(!omodel->isEditable()){
//        QAbstractItemView::contextMenuEvent(e);
//        return;
//    }

    if(!OBJECTLEVELMANAGER->count())
    {
        QTreeView::contextMenuEvent(e);
        return;
    }

    QMenu *menu = new QMenu(this);
    QModelIndex index = indexAt(e->pos());
    if (index.isValid()){
        menu->addAction(tr("New subclass"),this,SLOT(onAddChildClass()));
        menu->addAction(tr("New sibling class"),this,SLOT(onAddSiblingClass()));
        menu->addAction(tr("Delete class"),this,SLOT(onRemoveCurrentClass()));
        menu->addAction(tr("Change color"),this,SLOT(changeClassColor()));
    }
    else{
        menu->addAction("New subclass",this,SLOT(onAddChildClass()));
    }

    menu->exec(QCursor::pos());

    QTreeView::contextMenuEvent(e);
}

void OntologyClassView::mousePressEvent(QMouseEvent *event){
    QModelIndex index = indexAt(event->pos());
    if(!index.isValid()){
        selectionModel()->clear();
    }

    QTreeView::mousePressEvent(event);
}


void OntologyClassView::mouseDoubleClickEvent(QMouseEvent *event){

//    QTreeView::mouseDoubleClickEvent(event);

    QModelIndex index = indexAt(event->pos());
    if(index.isValid()){
        QStandardItemModel *omodel = qobject_cast<QStandardItemModel*>(model());
        if(!omodel)
            return;

        QStandardItem *pItem = omodel->itemFromIndex(index);
        if(pItem){
            ClassDescriptionDialog *pClassDescriptionDialog = new ClassDescriptionDialog(this);
            QString classId = pItem->data(Qt::DisplayRole).toString();
            pClassDescriptionDialog->setClassId(classId);
            pClassDescriptionDialog->setupData();

            QModelIndex parentindex = index.parent();
            if(parentindex.isValid()){
                pClassDescriptionDialog->setParentClassId(omodel->data(parentindex).toString());
            }

            if(pClassDescriptionDialog->exec() == QDialog::Rejected){
//                QTreeView::mouseDoubleClickEvent(event);
                return;
            }

            QString className = pClassDescriptionDialog->classId();

            pItem->setData(className,Qt::DisplayRole);
            pItem->setData(className);

        }
    }
}

void OntologyClassView::onAddChildClass(){
    if(!OBJECTLEVELMANAGER->count())
    {
        return;
    }

    QStandardItemModel *omodel = qobject_cast<QStandardItemModel*>(model());
    if(!omodel)
        return;


    ClassDescriptionDialog *pClassDescriptionDialog = new ClassDescriptionDialog(this);

    QModelIndex index = selectionModel()->currentIndex();
    if(index.isValid()){
        pClassDescriptionDialog->setParentClassId(omodel->data(index).toString());
    }

    if(pClassDescriptionDialog->exec() == QDialog::Rejected)
        return;

    QString classId = pClassDescriptionDialog->classId();

    if(index.isValid()){

        QStandardItem *pItem = new QStandardItem();
        pItem->setData(classId,Qt::DisplayRole);
        pItem->setData(classId);

        OntologyClass *pClass = ONTOLOGYCLASSIFICATIONMANAGER->ontologyClassById(classId);
        if(pClass){

            QColor color = pClass->color();
            pItem->setData(color,Qt::DecorationRole);
        }

        QList<QStandardItem*> parentItemList = omodel->findItems(pClassDescriptionDialog->parentClassId(),Qt::MatchExactly | Qt::MatchRecursive);
        if(!parentItemList.isEmpty())
        {
            QStandardItem *pParentItem = parentItemList.first();
            pParentItem->setChild(pParentItem->rowCount(),pItem);
        }else
            omodel->setItem(omodel->rowCount(index),pItem);

        expand(index);

    }else{
        int childMax = omodel->rowCount();

        QStandardItem *pItem = new QStandardItem();
        pItem->setData(classId,Qt::DisplayRole);
        pItem->setData(classId);

        OntologyClass *pClass = ONTOLOGYCLASSIFICATIONMANAGER->ontologyClassById(classId);
        if(pClass){

            QColor color = pClass->color();
            pItem->setData(color,Qt::DecorationRole);
        }

        omodel->setItem(omodel->rowCount(),pItem);

        expand(index);

        setCurrentIndex(omodel->index(childMax,0));
    }


}

void OntologyClassView::onRemoveCurrentClass(){

    QModelIndex index = selectionModel()->currentIndex();
    if(!index.isValid())
        return;

    int ret  = QMessageBox::question(this,tr("Class delete"),tr("You are going to delete this Class, all sub classes and rules related to this class will be also deleted. If you want to delete this Class click Yes else click No."),QMessageBox::Yes,QMessageBox::No);
    if(ret == QMessageBox::No)
        return;

    if(ret == QMessageBox::Yes){
        QStandardItemModel *omodel = qobject_cast<QStandardItemModel*>(model());
        if(!omodel)
            return;

        QString id = omodel->data(index,Qt::DisplayRole).toString();
        OntologyClass *pClass = ONTOLOGYCLASSIFICATIONMANAGER->ontologyClassById(id);
        if(!pClass)
            return;

        if(pClass->childCount()){
            QMessageBox::critical(this,tr("Delete Failed"),tr("Failed to delete the class. Delete all sub classes first and then try again."));

            return;
        }

        OntologyClass *pParentClass = ONTOLOGYCLASSIFICATIONMANAGER->ontologyClassById(pClass->parentId());
        if(pParentClass)
            pParentClass->removeChild(pClass);

        pClass->clearFuzzyRuleHash();

        ONTOLOGYCLASSIFICATIONMANAGER->removeOntologyClass(pClass);

        omodel->removeRows(index.row(),1,index.parent());
    }
}

void OntologyClassView::onAddSiblingClass(){
    if(!OBJECTLEVELMANAGER->count())
        return;

    QModelIndex index = selectionModel()->currentIndex();
    if(!index.isValid())
        return;

    QStandardItemModel *omodel = qobject_cast<QStandardItemModel*>(model());
    if(!omodel)
        return;

    ClassDescriptionDialog *pClassDescriptionDialog = new ClassDescriptionDialog(this);

    pClassDescriptionDialog->setParentClassId(omodel->data(index.parent()).toString());

    if(pClassDescriptionDialog->exec() == QDialog::Rejected)
        return;

    QString classId = pClassDescriptionDialog->classId();

//    if(omodel->insertRows(omodel->rowCount(index.parent()),1,index.parent())){

        QStandardItem *pItem = new QStandardItem();
        pItem->setData(classId,Qt::DisplayRole);
        pItem->setData(classId);

        QList<QStandardItem*> parentItemList = omodel->findItems(pClassDescriptionDialog->parentClassId(),Qt::MatchExactly | Qt::MatchRecursive);
        if(!parentItemList.isEmpty())
        {
            QStandardItem *pParentItem = parentItemList.first();
            pParentItem->setChild(pParentItem->rowCount(),pItem);
        }else
            omodel->setItem(omodel->rowCount(index.parent()),pItem);

        setCurrentIndex(omodel->index(omodel->rowCount(index.parent()),0,index.parent()));
//    }
}


void OntologyClassView::changeClassColor(){

    if(!OBJECTLEVELMANAGER->count())
        return;

    QModelIndex index = selectionModel()->currentIndex();
    if(!index.isValid())
        return;

    QString classid = model()->data(index,Qt::DisplayRole).toString();

    QColor color;
    OntologyClass *pClass = ONTOLOGYCLASSIFICATIONMANAGER->ontologyClassById(classid);
    if(pClass)
        color = QColor(pClass->color());

    color = QColorDialog::getColor(color,this,tr("Select Color"));
    if(pClass)
        pClass->setColor(color.name());

    model()->setData(index,color,Qt::DecorationRole);
}

QString OntologyClassView::getUniqueNameFromIndex(const QModelIndex &index){
    QStandardItemModel *omodel = qobject_cast<QStandardItemModel*>(model());
    if(!omodel)
        return QString();

    QString name = QString::fromUtf8("New Item_%1").arg(QString::number(index.row()));

    QModelIndex parentIndex = omodel->parent(index);

    while(parentIndex.isValid()){

        int indexNo = name.indexOf('_') + 1;

        name.insert(indexNo,QString::number(parentIndex.row()));

        parentIndex = omodel->parent(parentIndex);
    }

    return name;
}

void OntologyClassView::onNameValidationError(){
    QStandardItemModel *omodel = qobject_cast<QStandardItemModel*>(model());
    if(!omodel)
        return;

    QMessageBox::warning(this,tr("Class name error"),
                         tr("The class name must be unique, please type a unique name and try again."));
}
