/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can read the License at http://opensource.org/licenses/CDDL-1.0
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * 
 * When distributing Covered Code, include this CDDL Header Notice in 
 * each file and include the License file (licence.txt). If applicable, 
 * add the following below this CDDL Header, with the fields enclosed
 * by brackets [] replaced by your own identifying information:
 * "Portions Copyright [year] [name of copyright owner]"
 * 
 * The Original Software is AtmoSwing.
 * The Original Software was developed at the University of Lausanne.
 * All Rights Reserved.
 * 
 */

/*
 * Portions Copyright 2016 Pascal Horton, University of Bern.
 */

#ifndef ASPREDICTORARCHNASAMERRA2_H
#define ASPREDICTORARCHNASAMERRA2_H

#include <asIncludes.h>
#include <asPredictorArch.h>

class asGeoArea;

class asPredictorArchNasaMerra2
        : public asPredictorArch
{
public:
    asPredictorArchNasaMerra2(const wxString &dataId);

    virtual ~asPredictorArchNasaMerra2();

    bool Init();

protected:
    virtual void ListFiles(asTimeArray &timeArray);

    virtual bool ExtractFromFile(const wxString &fileName, asGeoAreaCompositeGrid *&dataArea, asTimeArray &timeArray,
                                 vvva2f &compositeData);

    virtual double ConvertToMjd(double timeValue, double refValue = NaNd) const;

private:

};

#endif // ASPREDICTORARCHNASAMERRA2_H
