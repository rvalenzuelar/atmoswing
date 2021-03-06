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
 * Portions Copyright 2008-2013 Pascal Horton, University of Lausanne.
 * Portions Copyright 2013-2015 Pascal Horton, Terranum.
 */

#ifndef ASFILEGRIB2_H
#define ASFILEGRIB2_H

#include "asIncludes.h"
#include <asFile.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "grib2.h"
#ifdef __cplusplus
}
#endif

class asFileGrib2
        : public asFile
{
public:
    asFileGrib2(const wxString &fileName, const FileMode &fileMode);

    ~asFileGrib2() override;

    bool Open() override;

    bool Close() override;

    bool SetIndexPosition(vi gribCode, float level);

    bool GetVarArray(const int IndexStart[], const int IndexCount[], float *pValue);

    bool GetXaxis(a1d &uaxis) const;

    bool GetYaxis(a1d &vaxis) const;

    double GetTime() const;

protected:

private:
    FILE *m_filtPtr;
    int m_index;
    std::vector<g2int> m_messageOffsets;
    std::vector<g2int> m_messageSizes;
    vl m_fieldNum;
    vi m_parameterDisciplines;
    vi m_parameterCategories;
    vi m_parameterNums;
    vi m_levelTypes;
    vd m_refTimes;
    vd m_times;
    vd m_forecastTimes;
    vd m_levels;
    va1d m_xAxes;
    va1d m_yAxes;

    bool OpenDataset();

    bool ParseStructure();

    void handleGribError(g2int ierr) const;

    void BuildAxes(const gribfield *gfld);

    bool CheckGridDefinition(const gribfield *gfld) const;

    bool CheckProductDefinition(const gribfield *gfld) const;

    void GetLevel(const gribfield *gfld);
};

#endif // ASFILEGRIB2_H
