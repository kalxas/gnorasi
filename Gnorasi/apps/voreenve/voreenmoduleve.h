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

#ifndef VRN_VOREENMODULEVE_H
#define VRN_VOREENMODULEVE_H

#include "tgt/exception.h"

#include "voreen/qt/voreenmoduleqt.h"

#include <string>
#include <vector>

namespace voreen {

class VoreenVEPlugin;

/**
 * Responsible for registering VoreenVE resources:
 * - VoreenVEPlugin
 */
class VoreenModuleVE : public VoreenModuleQt {

    friend class VoreenVEApplication;

public:
    /**
     * Instantiate and register the module's resources
     * in the derived class's constructor.
     */
    VoreenModuleVE(const std::string& modulePath);

    virtual ~VoreenModuleVE();

    /**
     * Returns the module's VoreenVE plugins.
     *
     * @see addVoreenVEPlugin
     */
    const std::vector<VoreenVEPlugin*>& getVoreenVEPlugins() const;

protected:
    /**
     * Registers the passed VoreenVE plugin. To be called in the
     * derived class's constructor.
     */
    void addVoreenVEPlugin(VoreenVEPlugin* plugin);

private:
    std::vector<VoreenVEPlugin*> vePlugins_;

    static const std::string loggerCat_;
};

} // namespace

#endif // VRN_VOREENMODULEVE_H
