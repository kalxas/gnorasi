/***********************************************************************************
 *                                                                                 *
 * Voreen - The Volume Rendering Engine                                            *
 *                                                                                 *
 * Copyright (C) 2005-2012 University of Muenster, Germany.                        *
 * Visualization and Computer Graphics Group <http://viscg.uni-muenster.de>        *
 * For a list of authors please refer to the file "CREDITS.txt".                   *
 *                                                                                 *
 * This file is part of the Voreen software package. Voreen is free software:      *
 * you can redistribute it and/or modify it under the terms of the GNU General     *
 * Public License version 2 as published by the Free Software Foundation.          *
 *                                                                                 *
 * Voreen is distributed in the hope that it will be useful, but WITHOUT ANY       *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR   *
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.      *
 *                                                                                 *
 * You should have received a copy of the GNU General Public License in the file   *
 * "LICENSE.txt" along with this file. If not, see <http://www.gnu.org/licenses/>. *
 *                                                                                 *
 * For non-commercial academic use see the license exception specified in the file *
 * "LICENSE-academic.txt". To get information about commercial licensing please    *
 * contact the authors.                                                            *
 *                                                                                 *
 ***********************************************************************************/

#ifndef VRN_VOLUMESLICEHELPER_H
#define VRN_VOLUMESLICEHELPER_H

#include "tgt/texture.h"
#include "tgt/matrix.h"

#include "voreen/core/datastructures/volume/volumeram.h"
#include "voreen/core/datastructures/volume/volume.h"

namespace voreen {

enum SliceAlignment {
    YZ_PLANE = 0,
    XZ_PLANE = 1,
    XY_PLANE = 2
};

FaceGeometry getSliceGeometry(const VolumeBase* vh, SliceAlignment alignment, float sliceIndex, bool applyTransformation = true);

//TODO Should be replaced by new image class as soon as possible
class Slice {
public:
    Slice(tgt::vec3 origin, tgt::vec3 xVec, tgt::vec3 yVec, tgt::Texture* tex, RealWorldMapping rwm);
    ~Slice();

    tgt::mat4 getTextureToWorldMatrix() const;
    tgt::mat4 getWorldToTextureMatrix() const;

    RealWorldMapping getRealWorldMapping() const { return rwm_; }

    const tgt::Texture* getTexture() const { return tex_; }
private:
    tgt::vec3 origin_;
    tgt::vec3 xVec_;
    tgt::vec3 yVec_;

    RealWorldMapping rwm_;

    tgt::Texture* tex_;
};

Slice* getVolumeSlice(const VolumeBase* vh, tgt::plane pl, float samplingRate);

} // namespace voreen

#endif
