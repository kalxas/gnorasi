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
 * Copyright (c) Argyros Argyridis <arargyridis@gmail.com>. All rights reserved. *
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


#include "fuzzyfunctionfactory.h"

#include "fuzzyfunctionleftshoulder.h"
#include "fuzzyfunctionrightshoulder.h"
#include "fuzzyfunctiontriangular.h"
#include "fuzzyfunctiontrapezoidal.h"

#define FUZZYFUNCTIONLEFTSHOULDER "Left Shoulder"
#define FUZZYFUNCTIONRIGHTSHOULDER "Right Shoulder"
#define FUZZYFUNCTIONTRIANGLULAR "Triangular"
#define FUZZYFUNCTIONTRAPEZOIDAL "Trapezoidal"

#include <iostream>

using namespace std;
FuzzyFunctionFactory::FuzzyFunctionFactory(QObject *parent) :
    QObject(parent)
{
}


FuzzyFunction* FuzzyFunctionFactory::createFuzzyFunction(const QString &name){

    if(!name.compare(QString::fromLatin1(FUZZYFUNCTIONLEFTSHOULDER)))
        return new FuzzyFunctionLeftShoulder();
    else if(!name.compare(QString::fromLatin1(FUZZYFUNCTIONRIGHTSHOULDER)))
        return new FuzzyFunctionRightShoulder();
     else if(!name.compare(QString::fromLatin1(FUZZYFUNCTIONTRIANGLULAR)))
            return new FuzzyFunctionTriangular();
      else if(!name.compare(QString::fromLatin1(FUZZYFUNCTIONTRAPEZOIDAL)))
        return new FuzzyFunctionTrapezoidal();
    else
        return new FuzzyFunctionLeftShoulder(); // this is the default fuzzy function
}
