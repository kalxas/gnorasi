/********************************************************************************
 *                                                                              *
 * GNORASI - The Knowlegde-Based Remote Sensing Engine                          *
 *                                                                              *
 * Language:  C++                                                               *
 *                                                                              *
 * Copyright (c) Ioannis Tsampoulatidis <itsam@iti.gr>. All rights reserved. 	*
 * Copyright (c) Informatics and Telematics Institute                           *
 *	  Centre for Research and Technology Hellas. All rights reserved.           *
 * Copyright (c) National Technical University of Athens. All rights reserved.	*
 *                                                                              *
 *                                                                              *
 * This file is part of the GNORASI software package. GNORASI is free           *
 * software: you can redistribute it and/or modify it under the terms           *
 * of the GNU General Public License version 2 as published by the              *
 * Free Software Foundation.                                                    *
 *                                                                              *
 * GNORASI is distributed in the hope that it will be useful,                   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
 * GNU General Public License for more details.                                 *
 *                                                                              *
 * You should have received a copy of the GNU General Public License            *
 * in the file "LICENSE.txt" along with this program.                           *
 * If not, see <http://www.gnu.org/licenses/>.                                  *
 *                                                                              *
 ********************************************************************************/

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>

#include "itiotbvectorimageviewer.h"

#include "itiotbvectorqglwidgetscrollable.h"
#include "../../utils/itiotbimagemanager.h"
#include "../../observables/itiviewerobservableregion.h"
#include "../../../../ports/otbvectorimageport.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "otbVectorRescaleIntensityImageFilter.h"


#include "../../utils/itiotbImageModelRendererScrollable.h"
#include "../../utils/itiotbImageViewManipulatorScrollable.h"
#include "../../utils/itiotblevelutility.h"
#include "../../utils/itiotblevel.h"
#include "../../utils/itiotbregion.h"

#include "../../models/itiotbVectorImageModel.h"
//
// Monteverdi includes (sorted by alphabetic order)
#include "../../models/itiotbAbstractImageModel.h"


using namespace otb;
using namespace itiviewer;
using namespace voreen;

ItiOtbVectorQGLWidgetScrollable::ItiOtbVectorQGLWidgetScrollable(ItiOtbVectorImageViewer *parent) :
    m_IsotropicZoom(1.0),
    m_SubsamplingRate(1),
    m_pImageViewManipulator( NULL ),
    m_pImageModelRenderer( NULL ),
    m_pItiOtbVectorImageViewer(parent),
    m_currentLevelId(1),
    QGLWidget(parent)
{
    setAutoFillBackground(false);

    m_pImageViewManipulator = new ImageViewManipulatorScrollable( this );
    m_pImageModelRenderer   = new ImageModelRendererScrollable( this );

    m_pen = QPen(QBrush(Qt::red),2.0);

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
}


/*!
 * \brief ItiOtbVectorQGLWidgetScrollable::initializeGL
 */
void ItiOtbVectorQGLWidgetScrollable::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
}


void ItiOtbVectorQGLWidgetScrollable::resizeGL(int w, int h)
{
    //! firstly setup the viweport with the new width and height
    setupViewport(w,h);

    ImageRegionType extent = m_pImageViewManipulator->extent();
    ImageRegionType bufferRegion = m_pImageViewManipulator->bufferRegion();

    //!
    //! setup the number of rows and columns to be visualized
    unsigned int nb_d_cs = m_pImageModelRenderer->nbDisplayColumns();
    unsigned int nb_d_rs = m_pImageModelRenderer->nbDisplayRows();
    unsigned int f_d_c = m_pImageModelRenderer->firstDisplayColumn();
    unsigned int f_d_r = m_pImageModelRenderer->firstDisplayRow();

    //!
    //! check if the extend's index x value is greater than zero , if yes then the number of columns equals to the number of the buffered region
    if( extent.GetIndex()[0] > 0 ){
        nb_d_cs = bufferRegion.GetSize()[0];
        f_d_c = 0;
    } else {

        nb_d_cs = w / m_IsotropicZoom;

        //!
        //! setup the fisrt display column
        //!
        if(f_d_c + nb_d_cs > bufferRegion.GetSize()[0])
            f_d_c = bufferRegion.GetSize()[0] - nb_d_cs;
    }

    //!
    //! check if the extend's index y value is greater than zero , if yes then the number of columns equals to the number of the buffered region
    if( extent.GetIndex()[1] > 0 ){
        nb_d_rs = bufferRegion.GetSize()[1];
        f_d_r = extent.GetSize()[1] - nb_d_rs;
    } else {
        nb_d_rs = h / m_IsotropicZoom;

        //!
        //! setup first display row
        //!
        if(f_d_r + nb_d_rs > bufferRegion.GetSize()[1])
            f_d_r = bufferRegion.GetSize()[0] - nb_d_rs;
    }

    m_pImageModelRenderer->setPaintingParameters(nb_d_cs,nb_d_rs,f_d_c,f_d_r);


    //! create a rect
    QRect rect(f_d_c,bufferRegion.GetSize()[0] - nb_d_rs - f_d_r,nb_d_cs,nb_d_rs);

    //! emit the signal
    emit visibleAreaChanged(rect);
}

//!
void ItiOtbVectorQGLWidgetScrollable::setupViewport(int w, int h){
    ImageRegionType extent;
    ImageRegionType bufferRegion = m_pImageViewManipulator->bufferRegion();
    ImageRegionType::SizeType size;

    size [0] = static_cast<unsigned int>(m_IsotropicZoom * static_cast<double>(bufferRegion.GetSize()[0]));
    size [1] = static_cast<unsigned int>(m_IsotropicZoom * static_cast<double>(bufferRegion.GetSize()[1]));

    ImageRegionType::IndexType index;
    index[0] = (w - static_cast<int>(size[0])) / 2;
    index[1] = (h - static_cast<int>(size[1])) / 2;

    extent.SetIndex(index);
    extent.SetSize(size);

    m_pImageViewManipulator->setExtent(extent);

    // initialize with the given size
    if(w && h)
        m_pImageViewManipulator->InitializeContext(w,h);
    else
        m_pImageViewManipulator->InitializeContext(100,100);

    glViewport(0, 0, (GLint)w, (GLint)h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, (GLint)w, 0, (GLint)h, -1, 1);
}

//!
void ItiOtbVectorQGLWidgetScrollable::initializeColumnRowParameters(){

    setupViewport(width(),height());

    ImageRegionType extent = m_pImageViewManipulator->extent();
    ImageRegionType bufferedRegion = m_pImageViewManipulator->bufferRegion();

    unsigned int nb_d_cs    = 0;
    unsigned int nb_d_rs    = 0;
    unsigned int f_d_c      = 0;
    unsigned int f_d_r      = 0;

    if( extent.GetIndex()[0] > 0 )
        nb_d_cs = bufferedRegion.GetSize()[0];
    else
        nb_d_cs = width() / m_IsotropicZoom;

    if( extent.GetIndex()[1] > 0 )
        nb_d_rs = bufferedRegion.GetSize()[1];
    else
        nb_d_rs = height() / m_IsotropicZoom;

    //!
    //! the values along the y-axes follow some kind of a transormation matrix [1,-1]
    //!
    f_d_r = extent.GetSize()[1] - nb_d_rs;

    //!
    m_pImageModelRenderer->setPaintingParameters(nb_d_cs,nb_d_rs,f_d_c,f_d_r);
}

//!
void ItiOtbVectorQGLWidgetScrollable::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);

    //!
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Set the new rendering context to be known in the ModelRendere
    const AbstractImageModel* aiModel=  qobject_cast<AbstractImageModel*>(m_pItiOtbVectorImageViewer->model());

    // setup the rendering context
    if (aiModel)
    {
        setupViewport(width(), height());

        // Get the region to draw from the ImageViewManipulator navigation
        // context
        const ImageRegionType region(
          m_pImageViewManipulator->bufferRegion());

        ImageRegionType extent(m_pImageViewManipulator->extent());

        ImageModelRendererScrollable::RenderingContext context(aiModel, region, extent, this->width(), this->height(), m_IsotropicZoom);

        // use the model renderer to paint the requested region of the image
        m_pImageModelRenderer->paintGL( context );
    }

    //! overpainting
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //!
    Level *pLevel = ITIOTBIMAGEMANAGER->levelById(m_currentLevelId);
    if(pLevel){

        ImageRegionType extent = m_pImageViewManipulator->extent();

        QList<Region*> regions = pLevel->regions();
        QList<Region*>::const_iterator i;
        for(i = regions.constBegin(); i != regions.constEnd(); i++){
            Region *pRegion = *i;
            pRegion->drawRegion(&painter, extent, m_IsotropicZoom);
        }
    }

    //! draw focus region
    m_pen.setWidth(2.0);
    m_pen.setColor(Qt::red);
    painter.setPen(m_pen);
    painter.setBrush(QBrush(Qt::transparent));
    painter.setOpacity(1.0);
    painter.drawRect(m_focusRegion);

    painter.end();
}

//!
void ItiOtbVectorQGLWidgetScrollable::draw(){

    // Set the new rendering context to be known in the ModelRendere
    const VectorImageModel* vModel=  qobject_cast<VectorImageModel*>(m_pItiOtbVectorImageViewer->model());

    if(!vModel)
        return;

    m_pImageViewManipulator->InitializeContext(this->width(),this->height());

    m_pImageViewManipulator->SetImageLargestRegion(vModel->GetLargestPossibleRegion());

    //! mouse tracking is disabled on startup, set it on
    setMouseTracking(true);

    //! initialize the column and row related parameters
    initializeColumnRowParameters();
}

//! The observer gets notified on a zoomable view's change ( either zooming or resizing events)
void ItiOtbVectorQGLWidgetScrollable::updateObserver(ItiViewerObservable *observable){    

    ItiViewerObservableRegion *region = qobject_cast<ItiViewerObservableRegion*>(observable);
    if(!region)
        return;

    ImageRegionType extent(m_pImageViewManipulator->extent());

    //! get the current rect from the region
    QRect rregion = region->region();

    //! create helper values from the rect's parameters
    int x       = rregion.x();
    int y       = rregion.y();
    int width   = rregion.width();
    int height  = rregion.height();

    m_focusRegion.setX(extent.GetIndex()[0] + x);
    //!
    //! Take notice that the y-axes is inverted [1,-1]. That's why the focus resion's y value equals to
    //! the following calculated value
    //!
    m_focusRegion.setY(extent.GetIndex()[1] + extent.GetSize()[1] - height - y);
    m_focusRegion.setWidth(width);
    m_focusRegion.setHeight(height);

    //! finally update the view
    update();
}

//!
void ItiOtbVectorQGLWidgetScrollable::wheelEvent(QWheelEvent *event){
    double deltaval = (double)event->delta();

    //! check the delta value if is more than zero, if yes zoomin, else zoomout
    if(deltaval>0)
        emit zoomIn();
    else
        emit zoomOut();

    event->accept();
}

//!
void ItiOtbVectorQGLWidgetScrollable::mousePressEvent(QMouseEvent *event){

    //! setup translating functionality only on left button pressed mouse events
    if(event->button() == Qt::LeftButton){
        //! create a helper point value
        QPoint previousCenter = m_focusRegion.center();

        //! get the position of the event
        QPoint point = event->pos();

        //! create a helper line object
        QLine line(previousCenter,point);

        ImageRegionType extent = m_pImageViewManipulator->extent();

        //! create helper values
        int dx = 0, dy = 0;

        //! check if the new rect right border exceeds the extends' width value
        if(point.x()+ qRound((double)m_focusRegion.width()/2.0) > extent.GetSize()[0] + extent.GetIndex()[0]){
            dx = extent.GetIndex()[0] + extent.GetSize()[0] - qRound((double)m_focusRegion.width()/2.0) - previousCenter.x();
        } //! check if the new rect left border exceeds the extend's index x value
        else if(point.x()- qRound((double)m_focusRegion.width()/2.0) < extent.GetIndex()[0]){
            dx = extent.GetIndex()[0] + qRound((double)m_focusRegion.width()/2.0) - previousCenter.x();
        }else //! else set the dx value equal to the line dx value
            dx = line.dx();

        //! check if the new rect right border exceeds the extends' height value
        if(point.y()+ qRound((double)m_focusRegion.height()/2.0) > extent.GetSize()[1] + extent.GetIndex()[1]){
            dy = extent.GetSize()[1] + extent.GetIndex()[1] - qRound((double)m_focusRegion.height()/2.0) - previousCenter.y();
        } //! check if the new rect left border exceeds the extend's index y value
        else if(point.y()- qRound((double)m_focusRegion.height()/2.0) < extent.GetIndex()[1]){
            dy = extent.GetIndex()[1] + qRound((double)m_focusRegion.height()/2.0) - previousCenter.y();
        }else //! else set the dy value equal to the line dy value
            dy = line.dy();

        //!translate the focus region
        m_focusRegion.translate(dx,dy);

        //! emit signal in order to update the zoomable view
        emit focusRegionTranslated(dx,dy);

        //! update widget
        update();
    }

    QGLWidget::mousePressEvent(event);
}

//!
void ItiOtbVectorQGLWidgetScrollable::mouseMoveEvent(QMouseEvent *event){
    //! get the current image port from the manager
    Port *port = ITIOTBIMAGEMANAGER->port();

    //! check if the image port is valid and connected with other ports
    if(port && port->isConnected()){
        //! get the image from the port
//        VectorImageType* imgType =  (VectorImageType*)ITIOTBIMAGEMANAGER->image();
//        if(!imgType){
//            QGLWidget::mouseMoveEvent(event);
//            return;
//        }

//        //! get the position
//        QPoint point = event->pos();

//        VectorIndexType index;
//        index[0] = (point.x() - m_Extent.GetIndex()[0])/m_IsotropicZoom;
//        index[1] = (point.y() - m_Extent.GetIndex()[1])/m_IsotropicZoom;

//        //! construct a text value related to pixel info data
//        QString text = ItiOtbVectorImageViewer::constructTextFromImageIndex(index,imgType);

//        //! emit the signal
//        emit currentIndexChanged(text);
    }

    //! call the parent's class method
    QGLWidget::mouseMoveEvent(event);
}

/*!
 * \brief ItiOtbVectorQGLWidgetScrollable::translate
 * \param rect
 */
void ItiOtbVectorQGLWidgetScrollable::translate(int dx, int dy){

    unsigned int nb_d_cs = m_pImageModelRenderer->nbDisplayColumns();
    unsigned int nb_d_rs = m_pImageModelRenderer->nbDisplayRows();
    unsigned int f_d_c = m_pImageModelRenderer->firstDisplayColumn();
    unsigned int f_d_r = m_pImageModelRenderer->firstDisplayRow();

    ImageRegionType bufferedRegion  = m_pImageViewManipulator->bufferRegion();

    //! create a helper value equal to the f_d_c plus the dx parameter
    int helperX = f_d_c + dx;

    //! check that the helperX value is less than zero
    if(helperX < 0)
        f_d_c = 0;
    //! check if it is beween the buffered region extends
    else if(helperX > bufferedRegion.GetSize()[0] - nb_d_cs)
        f_d_c = bufferedRegion.GetSize()[0] - nb_d_cs;
    else
        f_d_c = helperX;

    //!
    int helperY = f_d_r - dy;

    //! check that it is less than zero
    if(helperY < 0)
        f_d_r = 0;
    //! check that it is between the buffered region's y-axes size
    else if(helperY > bufferedRegion.GetSize()[1] - nb_d_rs)
        f_d_r = bufferedRegion.GetSize()[1] - nb_d_rs;
    else //! else set the f_d_r equal to the helperY value
        f_d_r = helperY;

    //!
    m_pImageModelRenderer->setPaintingParameters(nb_d_cs,nb_d_rs,f_d_c,f_d_r);

    //! update painting
//    updateGL();
    update();
}

void ItiOtbVectorQGLWidgetScrollable::showEvent(QShowEvent *event){
    Q_UNUSED(event);

    resizeGL(width(),height());
}


//!
ItiOtbVectorQGLWidgetScrollable::~ItiOtbVectorQGLWidgetScrollable(){
//    ClearBuffer();
}
