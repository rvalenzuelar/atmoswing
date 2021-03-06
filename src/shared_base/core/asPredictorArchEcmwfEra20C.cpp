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

#include "asPredictorArchEcmwfEra20C.h"

#include <asTimeArray.h>
#include <asAreaCompGrid.h>


asPredictorArchEcmwfEra20C::asPredictorArchEcmwfEra20C(const wxString &dataId)
        : asPredictorArch(dataId)
{
    // Set the basic properties.
    m_datasetId = "ECMWF_ERA_20C_3h";
    m_provider = "ECMWF";
    m_datasetName = "ERA 20th Century";
    m_fileType = asFile::Netcdf;
    m_strideAllowed = true;
    m_fStr.dimLatName = "latitude";
    m_fStr.dimLonName = "longitude";
    m_fStr.dimTimeName = "time";
    m_fStr.dimLevelName = "level";
    m_subFolder = wxEmptyString;
}

bool asPredictorArchEcmwfEra20C::Init()
{
    CheckLevelTypeIsDefined();

    // List of variables: http://rda.ucar.edu/datasets/ds627.0/docs/era_interim_grib_table.html

    // Identify data ID and set the corresponding properties.
    if (m_product.IsSameAs("pressure_levels", false) || m_product.IsSameAs("pressure", false) ||
        m_product.IsSameAs("press", false) || m_product.IsSameAs("pl", false)) {
        m_fStr.hasLevelDim = true;
        m_subFolder = "pressure_level";
        if (m_dataId.IsSameAs("z", false) || m_dataId.IsSameAs("hgt", false)) {
            m_parameter = Geopotential;
            m_parameterName = "Geopotential";
            m_fileVarName = "z";
            m_unit = m2_s2;
        } else if (m_dataId.IsSameAs("t", false)) {
            m_parameter = AirTemperature;
            m_parameterName = "Temperature";
            m_fileVarName = "t";
            m_unit = degK;
        } else if (m_dataId.IsSameAs("r", false) || m_dataId.IsSameAs("rh", false)) {
            m_parameter = RelativeHumidity;
            m_parameterName = "Relative humidity";
            m_fileVarName = "r";
            m_unit = percent;
        } else if (m_dataId.IsSameAs("omega", false) || m_dataId.IsSameAs("w", false)) {
            m_parameter = VerticalVelocity;
            m_parameterName = "Vertical velocity";
            m_fileVarName = "w";
            m_unit = Pa_s;
        } else {
            m_parameter = ParameterUndefined;
            m_parameterName = "Undefined";
            m_fileVarName = m_dataId;
            m_unit = UnitUndefined;
        }
        m_fileNamePattern = m_fileVarName + ".nc";

    } else if (m_product.IsSameAs("surface", false) || m_product.IsSameAs("surf", false) ||
               m_product.IsSameAs("sfc", false)) {
        m_fStr.hasLevelDim = false;
        m_subFolder = "surface";
        if (m_dataId.IsSameAs("tcw", false)) {
            m_parameter = PrecipitableWater;
            m_parameterName = "Total column water";
            m_fileVarName = "tcw";
            m_unit = kg_m2;
        } else if (m_dataId.IsSameAs("tp", false)) {
            m_parameter = Precipitation;
            m_parameterName = "Total precipitation";
            m_fileVarName = "tp";
            m_unit = m;
        } else if (m_dataId.IsSameAs("mslp", false) || m_dataId.IsSameAs("msl", false)) {
            m_parameter = Pressure;
            m_parameterName = "Sea level pressure";
            m_fileVarName = "msl";
            m_unit = Pa;
        } else {
            m_parameter = ParameterUndefined;
            m_parameterName = "Undefined";
            m_fileVarName = m_dataId;
            m_unit = UnitUndefined;
        }
        m_fileNamePattern = m_fileVarName + ".nc";

    } else {
        asThrowException(_("level type not implemented for this reanalysis dataset."));
    }

    // Check data ID
    if (m_fileNamePattern.IsEmpty() || m_fileVarName.IsEmpty()) {
        wxLogError(_("The provided data ID (%s) does not match any possible option in the dataset %s."), m_dataId,
                   m_datasetName);
        return false;
    }

    // Check directory is set
    if (GetDirectoryPath().IsEmpty()) {
        wxLogError(_("The path to the directory has not been set for the data %s from the dataset %s."), m_dataId,
                   m_datasetName);
        return false;
    }

    // Set to initialized
    m_initialized = true;

    return true;
}

void asPredictorArchEcmwfEra20C::ListFiles(asTimeArray &timeArray)
{
    m_files.push_back(GetFullDirectoryPath() + m_fileNamePattern);
}

double asPredictorArchEcmwfEra20C::ConvertToMjd(double timeValue, double refValue) const
{
    timeValue = (timeValue / 24.0); // hours to days
    timeValue += asTime::GetMJD(1900, 1, 1); // to MJD: add a negative time span

    return timeValue;
}

