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

#include "itiotbvectorqglwidgetfullview.h"
#include "../../observables/itiviewerobservableregion.h"
#include "itiotbvectorimageviewer.h"
#include "../../utils/itiotbimagemanager.h"
#include "../../../../ports/otbvectorimageport.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "otbVectorRescaleIntensityImageFilter.h"


#include "../../utils/itiotbImageModelRendererFullView.h"
#include "../../utils/itiotbImageViewManipulatorFullView.h"

#include "../../models/itiotbVectorImageModel.h"
//
// Monteverdi includes (sorted by alphabetic order)
#include "../../models/itiotbAbstractImageModel.h"

using namespace otb;
using namespace itiviewer;
using namespace voreen;

ItiOtbVectorQGLWidgetFullView::ItiOtbVectorQGLWidgetFullView(ItiOtbVectorImageViewer *parent) :
    m_IsotropicZoom(1.0),
    m_SubsamplingRate(1),
    m_pImageViewManipulator( NULL ),
    m_pImageModelRenderer( NULL ),
    m_pItiOtbVectorImageViewer(parent),
    QGLWidget(parent)
{
    setAutoFillBackground(false);

    m_pImageViewManipulator = new ImageViewManipulatorFullView( this );
    m_pImageModelRenderer   = new ImageModelRendererFullView( this );

    m_pen = QPen(QBrush(Qt::red),2.0);
}


void ItiOtbVectorQGLWidgetFullView::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
}


void ItiOtbVectorQGLWidgetFullView::resizeGL(int w, int h)
{
    setupViewport(w,h);
}

void ItiOtbVectorQGLWidgetFullView::setupViewport(int w, int h){
//    if(!m_OpenGlBuffer)
//        return;


    ImageRegionType bufferedRegion = m_pImageViewManipulator->bufferRegion();

    m_IsotropicZoom = w < h ? static_cast<double>(w)/ static_cast<double>(bufferedRegion.GetSize()[0]) : static_cast<double>(h)/ static_cast<double>(bufferedRegion.GetSize()[1]);

    VectorSizeType size;
    size [0] = static_cast<unsigned int>(m_IsotropicZoom * static_cast<double>(bufferedRegion.GetSize()[0]));
    size [1] = static_cast<unsigned int>(m_IsotropicZoom * static_cast<double>(bufferedRegion.GetSize()[1]));

    ImageRegionType::IndexType index;
    index[0] = (w - static_cast<int>(size[0])) / 2;
    index[1] = (h - static_cast<int>(size[1])) / 2;

    ImageRegionType extent;

    extent.SetIndex(index);
    extent.SetSize(size);

    m_pImageViewManipulator->setExtent(extent);

    glViewport(0, 0, (GLint)w, (GLint)h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, (GLint)w, 0, (GLint)h, -1, 1);
}

//!
void ItiOtbVectorQGLWidgetFullView::paintEvent(QPaintEvent *event){
    //!
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    qglClearColor(Qt::black);

    //
    // TEST

    // Set the new rendering context to be known in the ModelRendere
    const AbstractImageModel* aiModel=  qobject_cast<AbstractImageModel*>(m_pItiOtbVectorImageViewer->model());

    // setup the rendering context
    if (aiModel)
    {
        const ImageRegionType extent(m_pImageViewManipulator->extent());

        // Get the region to draw from the ImageViewManipulator navigation
        // context
        const ImageRegionType region(
          m_pImageViewManipulator->bufferRegion());

        ImageModelRendererFullView::RenderingContext context(aiModel, region, extent, this->width(), this->height(), m_IsotropicZoom);

        // use the model renderer to paint the requested region of the image
        m_pImageModelRenderer->paintGL( context );
    }

    //
    // END OF TEST

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //! overpainting
    painter.setPen(m_pen);
    painter.drawRect(m_visibleRegion);

    painter.end();
}

//!
void ItiOtbVectorQGLWidgetFullView::draw(){

    // Set the new rendering context to be known in the ModelRendere
    const VectorImageModel* vModel=  qobject_cast<VectorImageModel*>(m_pItiOtbVectorImageViewer->model());

    if(!vModel)
        return;

    m_pImageViewManipulator->InitializeContext(width(),height());

    m_pImageViewManipulator->SetImageLargestRegion(vModel->GetLargestPossibleRegion());

    //!
    setMouseTracking(true);
}

//!
void ItiOtbVectorQGLWidgetFullView::updateObserver(ItiViewerObservable *observable){
    ItiViewerObservableRegion *region = qobject_cast<ItiViewerObservableRegion*>(observable);
    if(!region)
        return;

    setupViewport(width(),height());

    ImageRegionType extent = m_pImageViewManipulator->extent();

    QRect rect = region->region();
    //! check if the x coordinate of the given rect is greater than zero
//    if(rect.x()>=0)
//        m_visibleRegion.setX(extent.GetIndex()[0]);
//    else //! if not , this means that the scrollable view has been resized to a value smaller than the original size of the image
        m_visibleRegion.setX(extent.GetIndex()[0] + (qAbs(rect.x()*m_IsotropicZoom)));

    //! same type of checking as the checking on the previous if statement
//    if(rect.y() >= 0)
//        m_visibleRegion.setY(extent.GetIndex()[1]);
//    else
        m_visibleRegion.setY(extent.GetIndex()[1]+(qAbs(rect.y()*m_IsotropicZoom) ));

    //! calculate the new width and height value;
    int nw = m_IsotropicZoom * rect.width();
    int nh = m_IsotropicZoom * rect.height();

    //! set the new width and height to the visible region
    m_visibleRegion.setWidth(nw);
    m_visibleRegion.setHeight(nh);

//    //!
//    //! The following code is dummy,
//    //! There was a problem of not updating the widget correctly
//    //! Forcing the widget to be updated by hiding and showing is a tweek to be updated correctly
//    //!
    hide();
    show();
}

//!
void ItiOtbVectorQGLWidgetFullView::mouseMoveEvent(QMouseEvent *event){

    OTBVectorImagePort *imgPort = (OTBVectorImagePort*)ITIOTBIMAGEMANAGER->port();
    if(imgPort && imgPort->isConnected()){
//        VectorImageType* imgType =  (VectorImageType*)imgPort->getData();
//        if(!imgType){
//            QGLWidget::mouseMoveEvent(event);
//            return;
//        }

//        //! get the position
//        QPoint point = event->pos();

//        VectorIndexType index;
//        index[0] = (point.x() - m_Extent.GetIndex()[0])/m_IsotropicZoom;
//        index[1] = (point.y() - m_Extent.GetIndex()[1])/m_IsotropicZoom;

//        QString text = ItiOtbVectorImageViewer::constructTextFromImageIndex(index,imgType);

//        emit currentIndexChanged(text);
    }

    QGLWidget::mouseMoveEvent(event);
}

//!
void ItiOtbVectorQGLWidgetFullView::mousePressEvent(QMouseEvent *event){

    //! setup translating functionality only on left button pressed mouse events
    if(event->button() == Qt::LeftButton){
        //! create a helper point value
        QPoint previousCenter = m_visibleRegion.center();

        //! get the position of the event
        QPoint point = event->pos();

        //! create a helper line object
        QLine line(previousCenter,point);

        ImageRegionType extent = m_pImageViewManipulator->extent();

        //! create helper values
        int dx = 0, dy = 0;

        //! check if the new rect right border exceeds the extends' width value
        if(point.x()+ qRound((double)m_visibleRegion.width()/2.0) > extent.GetSize()[0] + extent.GetIndex()[0]){
            dx = extent.GetIndex()[0] + extent.GetSize()[0] - qRound((double)m_visibleRegion.width()/2.0) - previousCenter.x();
        } //! check if the new rect left border exceeds the extend's index x value
        else if(point.x()- qRound((double)m_visibleRegion.width()/2.0) < extent.GetIndex()[0]){
            dx = extent.GetIndex()[0] + qRound((double)m_visibleRegion.width()/2.0) - previousCenter.x();
        }else //! else set the dx value equal to the line dx value
            dx = line.dx();

        //! check if the new rect right border exceeds the extends' height value
        if(point.y()+ qRound((double)m_visibleRegion.height()/2.0) > extent.GetSize()[1] + extent.GetIndex()[1]){
            dy = extent.GetSize()[1] + extent.GetIndex()[1] - qRound((double)m_visibleRegion.height()/2.0) - previousCenter.y();
        } //! check if the new rect left border exceeds the extend's index y value
        else if(point.y()- qRound((double)m_visibleRegion.height()/2.0) < extent.GetIndex()[1]){
            dy = extent.GetIndex()[1] + qRound((double)m_visibleRegion.height()/2.0) - previousCenter.y();
        }else //! else set the dy value equal to the line dy value
            dy = line.dy();

        //!translate the focus region
        m_visibleRegion.translate(dx,dy);

        //! emit signal in order to update the zoomable view
        emit visibleAreaTranslated(dx/m_IsotropicZoom,dy/m_IsotropicZoom);

        //! update widget
        update();
    }

    QGLWidget::mousePressEvent(event);
}


//void ItiOtbVectorQGLWidgetFullView::resizeEvent(QResizeEvent *event){
//    m_pImageViewManipulator->resizeEvent(event);
//}


//!
ItiOtbVectorQGLWidgetFullView::~ItiOtbVectorQGLWidgetFullView(){
//    ClearBuffer();
}
