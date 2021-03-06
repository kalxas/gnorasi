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

#ifndef ITIOTBIMAGEVIEWERPANELHISTOGRAMTAB_H
#define ITIOTBIMAGEVIEWERPANELHISTOGRAMTAB_H

#include <QWidget>

class QPushButton;

class HistogramView;
class HistogramGenerator;
class MyHistogramList;

namespace itiviewer{

class ItiOtbImageViewerPanel;

/*!
 * \brief The ItiOtbImageViewerPanelHistogramTab class
 */
class ItiOtbImageViewerPanelHistogramTab : public QWidget
{
    Q_OBJECT
public:
    //! ctor
    explicit ItiOtbImageViewerPanelHistogramTab(ItiOtbImageViewerPanel *panel);

    /*!
     * \brief setupHistogram
     */
    void setupHistogram();

    // set the current channels
    void setCurrentChannels(int red, int green, int blue);


public slots:

private slots:
//    void onClicked();
    void onHistogramFinished(MyHistogramList* );

private:
    void initialize();
    
    /*!
     * \brief m_pItiOtbImageViewerPanel
     */
    ItiOtbImageViewerPanel* m_pItiOtbImageViewerPanel;

    /*!
     * \brief m_pHistogramGenerator
     */
    HistogramGenerator *m_pHistogramGenerator;

    /*!
     * \brief m_pHistogramViewRed
     */
    HistogramView *m_pHistogramViewRed;

    /*!
     * \brief m_pHistogramViewBlue
     */
    HistogramView *m_pHistogramViewBlue;

    /*!
     * \brief m_pHistogramViewGreen
     */
    HistogramView *m_pHistogramViewGreen;

    /*!
     * \brief m_pHistogramViewGreyscale
     */
    HistogramView *m_pHistogramViewGreyscale;
};

}

#endif // ITIOTBIMAGEVIEWERPANELHISTOGRAMTAB_H
