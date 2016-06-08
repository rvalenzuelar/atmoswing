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

#include "asDataPredictorArchiveNcepReanalysis1Subset.h"

#include <asTimeArray.h>
#include <asGeoAreaCompositeGrid.h>
#include <asFileNetcdf.h>


asDataPredictorArchiveNcepReanalysis1Subset::asDataPredictorArchiveNcepReanalysis1Subset(const wxString &dataId)
        : asDataPredictorArchive(dataId)
{
    // Set the basic properties.
    m_initialized = false;
    m_datasetId = "NCEP_Reanalysis_v1_terranum";
    m_originalProvider = "NCEP/NCAR";
    m_transformedBy = "Pascal Horton";
    m_datasetName = "Reanalysis 1 subset";
    m_originalProviderStart = asTime::GetMJD(1948, 1, 1);
    m_originalProviderEnd = NaNDouble;
    m_timeZoneHours = 0;
    m_timeStepHours = 6;
    m_firstTimeStepHours = 0;
    m_nanValues.push_back(32767);
    m_nanValues.push_back(936 * std::pow(10.f, 34.f));
    m_xAxisShift = 0;
    m_yAxisShift = 0;
    m_xAxisStep = 2.5;
    m_yAxisStep = 2.5;
    m_subFolder = wxEmptyString;
    m_fileAxisLatName = "lat";
    m_fileAxisLonName = "lon";
    m_fileAxisTimeName = "time";
    m_fileAxisLevelName = "level";

    // Identify data ID and set the corresponding properties.
    if (m_dataId.IsSameAs("hgt", false)) {
        m_parameter = GeopotentialHeight;
        m_parameterName = "Geopotential Height";
        m_fileNamePattern = "hgt.nc";
        m_fileVariableName = "hgt";
        m_unit = m;
    } else if (m_dataId.IsSameAs("air", false)) {
        m_parameter = AirTemperature;
        m_parameterName = "Air Temperature";
        m_fileNamePattern = "air.nc";
        m_fileVariableName = "air";
        m_unit = degK;
    } else if (m_dataId.IsSameAs("omega", false)) {
        m_parameter = Omega;
        m_parameterName = "Omega (Vertical Velocity)";
        m_fileNamePattern = "omega.nc";
        m_fileVariableName = "omega";
        m_unit = Pa_s;
    } else if (m_dataId.IsSameAs("rhum", false)) {
        m_parameter = RelativeHumidity;
        m_parameterName = "Relative Humidity";
        m_fileNamePattern = "rhum.nc";
        m_fileVariableName = "rhum";
        m_unit = percent;
    } else if (m_dataId.IsSameAs("shum", false)) {
        m_parameter = SpecificHumidity;
        m_parameterName = "Specific Humidity";
        m_fileNamePattern = "shum.nc";
        m_fileVariableName = "shum";
        m_unit = kg_kg;
    } else if (m_dataId.IsSameAs("uwnd", false)) {
        m_parameter = Uwind;
        m_parameterName = "U-Wind";
        m_fileNamePattern = "uwnd.nc";
        m_fileVariableName = "uwnd";
        m_unit = m_s;
    } else if (m_dataId.IsSameAs("vwnd", false)) {
        m_parameter = Vwind;
        m_parameterName = "V-Wind";
        m_fileNamePattern = "vwnd.nc";
        m_fileVariableName = "vwnd";
        m_unit = m_s;
    } else if (m_dataId.IsSameAs("prwtr", false)) {
        m_parameter = PrecipitableWater;
        m_parameterName = "Precipitable water";
        m_fileNamePattern = "pr_wtr.nc";
        m_fileVariableName = "pr_wtr";
        m_unit = mm;
    } else {
        asThrowException(wxString::Format(_("No '%s' parameter identified for the provided level type (%s)."),
                                          m_dataId, LevelEnumToString(m_levelType)));
    }
}

asDataPredictorArchiveNcepReanalysis1Subset::~asDataPredictorArchiveNcepReanalysis1Subset()
{

}

bool asDataPredictorArchiveNcepReanalysis1Subset::Init()
{
    // Check data ID
    if (m_fileNamePattern.IsEmpty() || m_fileVariableName.IsEmpty()) {
        asLogError(
                wxString::Format(_("The provided data ID (%s) does not match any possible option in the dataset %s."),
                                 m_dataId, m_datasetName));
        return false;
    }

    // Check directory is set
    if (GetDirectoryPath().IsEmpty()) {
        asLogError(
                wxString::Format(_("The path to the directory has not been set for the data %s from the dataset %s."),
                                 m_dataId, m_datasetName));
        return false;
    }

    // Set to initialized
    m_initialized = true;

    return true;
}

bool asDataPredictorArchiveNcepReanalysis1Subset::ExtractFromFiles(asGeoAreaCompositeGrid *&dataArea,
                                                                     asTimeArray &timeArray,
                                                                     VVArray2DFloat &compositeData)
{
    // Build the file path
    wxString fileFullPath = GetFullDirectoryPath() + m_fileNamePattern;

    // Open the NetCDF 
    ThreadsManager().CritSectionNetCDF().Enter();
    asFileNetcdf ncFile(fileFullPath, asFileNetcdf::ReadOnly);
    if (!ncFile.Open()) {
        ThreadsManager().CritSectionNetCDF().Leave();
        return false;
    }

    // Number of dimensions
    int nDims = ncFile.GetNDims();
    wxASSERT(nDims >= 3);
    wxASSERT(nDims <= 4);

    // Get some attributes
    float dataAddOffset = ncFile.GetAttFloat("add_offset", m_fileVariableName);
    if (asTools::IsNaN(dataAddOffset))
        dataAddOffset = 0;
    float dataScaleFactor = ncFile.GetAttFloat("scale_factor", m_fileVariableName);
    if (asTools::IsNaN(dataScaleFactor))
        dataScaleFactor = 1;
    bool scalingNeeded = true;
    if (dataAddOffset == 0 && dataScaleFactor == 1)
        scalingNeeded = false;

    // Get full axes from the netcdf file
    Array1DFloat axisDataLon(ncFile.GetVarLength(m_fileAxisLonName));
    ncFile.GetVar(m_fileAxisLonName, &axisDataLon[0]);
    Array1DFloat axisDataLat(ncFile.GetVarLength(m_fileAxisLatName));
    ncFile.GetVar(m_fileAxisLatName, &axisDataLat[0]);
    Array1DFloat axisDataLevel;
    if (nDims == 4) {
        axisDataLevel.resize(ncFile.GetVarLength(m_fileAxisLevelName));
        ncFile.GetVar(m_fileAxisLevelName, &axisDataLevel[0]);
    }

    // Adjust axes if necessary
    dataArea = AdjustAxes(dataArea, axisDataLon, axisDataLat, compositeData);
    if (dataArea) {
        wxASSERT(dataArea->GetNbComposites() > 0);
    }

    // Time array takes ages to load !! Avoid if possible. Get the first value of the time array.
    size_t axisDataTimeLength = ncFile.GetVarLength(m_fileAxisTimeName);
    double valFirstTime = ncFile.GetVarOneDouble(m_fileAxisTimeName, 0);
    valFirstTime = (valFirstTime / 24.0); // hours to days
    valFirstTime += asTime::GetMJD(1, 1, 1); // to MJD: add a negative time span
    double valLastTime = ncFile.GetVarOneDouble(m_fileAxisTimeName, axisDataTimeLength - 1);
    valLastTime = (valLastTime / 24.0); // hours to days
    valLastTime += asTime::GetMJD(1, 1, 1); // to MJD: add a negative time span

    // Check requested time array
    if (timeArray.GetFirst() < valFirstTime) {
        asLogError(wxString::Format(_("The requested data starts before (%s) the actual dataset (%s)"),
                                    asTime::GetStringTime(timeArray.GetFirst()), asTime::GetStringTime(valFirstTime)));
        ncFile.Close();
        ThreadsManager().CritSectionNetCDF().Leave();
        return false;
    }
    if (timeArray.GetLast() > valLastTime) {
        asLogError(wxString::Format(_("The requested data ends after (%s) the actual dataset (%s)"),
                                    asTime::GetStringTime(timeArray.GetLast()), asTime::GetStringTime(valLastTime)));
        ncFile.Close();
        ThreadsManager().CritSectionNetCDF().Leave();
        return false;
    }

    // Get start and end of the serie
    double timeStart = timeArray.GetFirst();
    double timeEnd = timeArray.GetLast();

    // Get the time length
    double timeArrayIndexStart = timeArray.GetIndexFirstAfter(timeStart);
    double timeArrayIndexEnd = timeArray.GetIndexFirstBefore(timeEnd);
    int indexLengthTime = timeArrayIndexEnd - timeArrayIndexStart + 1;
    int indexLengthTimeArray = indexLengthTime;

    // Correct the time start and end
    size_t indexStartTime = 0;
    int cutStart = timeArrayIndexStart;
    int cutEnd = 0;
    while (valFirstTime < timeArray[timeArrayIndexStart]) {
        valFirstTime += m_timeStepHours / 24.0;
        indexStartTime++;
    }
    if (indexStartTime + indexLengthTime > axisDataTimeLength) {
        indexLengthTime--;
        cutEnd++;
    }

    // Containers for extraction
    VectorInt vectIndexLengthLat;
    VectorInt vectIndexLengthLon;
    VVectorShort vectData;

    for (int i_area = 0; i_area < (int) compositeData.size(); i_area++) {
        int indexStartLon, indexStartLat, indexLengthLon, indexLengthLat;
        if (dataArea) {
            wxASSERT(dataArea->GetNbComposites() > 0);

            // Get the spatial extent
            float lonMin = dataArea->GetXaxisCompositeStart(i_area);
            float latMinStart = dataArea->GetYaxisCompositeStart(i_area);
            float latMinEnd = dataArea->GetYaxisCompositeEnd(i_area);

            // The dimensions lengths
            indexLengthLon = dataArea->GetXaxisCompositePtsnb(i_area);
            indexLengthLat = dataArea->GetYaxisCompositePtsnb(i_area);

            // Get the spatial indices of the desired data
            indexStartLon = asTools::SortedArraySearch(&axisDataLon[0], &axisDataLon[axisDataLon.size() - 1], lonMin,
                                                       0.01f, asHIDE_WARNINGS);
            if (indexStartLon == asOUT_OF_RANGE) {
                // If not found, try with negative angles
                indexStartLon = asTools::SortedArraySearch(&axisDataLon[0], &axisDataLon[axisDataLon.size() - 1],
                                                           lonMin - 360, 0.01f, asHIDE_WARNINGS);
            }
            if (indexStartLon == asOUT_OF_RANGE) {
                // If not found, try with angles above 360 degrees
                indexStartLon = asTools::SortedArraySearch(&axisDataLon[0], &axisDataLon[axisDataLon.size() - 1],
                                                           lonMin + 360, 0.01f);
            }
            if (indexStartLon < 0) {
                asLogError(wxString::Format("Cannot find lonMin (%f) in the array axisDataLon ([0]=%f -> [%d]=%f) ",
                                            lonMin, axisDataLon[0], (int) axisDataLon.size(),
                                            axisDataLon[axisDataLon.size() - 1]));
                ncFile.Close();
                ThreadsManager().CritSectionNetCDF().Leave();
                return false;
            }
            wxASSERT_MSG(indexStartLon >= 0,
                         wxString::Format("axisDataLon[0] = %f, &axisDataLon[%d] = %f & lonMin = %f", axisDataLon[0],
                                          (int) axisDataLon.size(), axisDataLon[axisDataLon.size() - 1], lonMin));

            int indexStartLat1 = asTools::SortedArraySearch(&axisDataLat[0], &axisDataLat[axisDataLat.size() - 1],
                                                            latMinStart, 0.01f);
            int indexStartLat2 = asTools::SortedArraySearch(&axisDataLat[0], &axisDataLat[axisDataLat.size() - 1],
                                                            latMinEnd, 0.01f);
            wxASSERT_MSG(indexStartLat1 >= 0,
                         wxString::Format("Looking for %g in %g to %g", latMinStart, axisDataLat[0],
                                          axisDataLat[axisDataLat.size() - 1]));
            wxASSERT_MSG(indexStartLat2 >= 0, wxString::Format("Looking for %g in %g to %g", latMinEnd, axisDataLat[0],
                                                               axisDataLat[axisDataLat.size() - 1]));
            indexStartLat = wxMin(indexStartLat1, indexStartLat2);
        } else {
            indexStartLon = 0;
            indexStartLat = 0;
            indexLengthLon = m_lonPtsnb;
            indexLengthLat = m_latPtsnb;
        }
        int indexLevel = 0;
        if (nDims == 4) {
            indexLevel = asTools::SortedArraySearch(&axisDataLevel[0], &axisDataLevel[axisDataLevel.size() - 1],
                                                    m_level, 0.01f);
            if (indexLevel < 0) {
                asLogWarning(wxString::Format(_("The desired level (%g) does not exist for %s"), m_level,
                                              m_fileVariableName));
                ncFile.Close();
                ThreadsManager().CritSectionNetCDF().Leave();
                return false;
            }
        } else {
            if (m_level > 0) {
                asLogWarning(wxString::Format(_("The desired level (%g) does not exist for %s"), m_level,
                                              m_fileVariableName));
                ncFile.Close();
                ThreadsManager().CritSectionNetCDF().Leave();
                return false;
            }
        }

        // Create the arrays to receive the data
        VectorShort data;

        // Resize the arrays to store the new data
        int totLength = indexLengthTimeArray * indexLengthLat * indexLengthLon;
        wxASSERT(totLength > 0);
        data.resize(totLength);

        // Fill empty begining with NaNs
        int indexBegining = 0;
        if (cutStart > 0) {
            int latlonlength = indexLengthLat * indexLengthLon;
            for (int i_empty = 0; i_empty < cutStart; i_empty++) {
                for (int i_emptylatlon = 0; i_emptylatlon < latlonlength; i_emptylatlon++) {
                    data[indexBegining] = NaNShort;
                    indexBegining++;
                }
            }
        }

        // Fill empty end with NaNs
        int indexEnd = indexLengthTime * indexLengthLat * indexLengthLon - 1;
        if (cutEnd > 0) {
            int latlonlength = indexLengthLat * indexLengthLon;
            for (int i_empty = 0; i_empty < cutEnd; i_empty++) {
                for (int i_emptylatlon = 0; i_emptylatlon < latlonlength; i_emptylatlon++) {
                    indexEnd++;
                    data[indexEnd] = NaNShort;
                }
            }
        }

        // Get the indices for data
        if (nDims == 4) {
            // Set the indices for data
            size_t indexStartData[4] = {0, 0, 0, 0};
            indexStartData[0] = indexStartTime;
            indexStartData[1] = indexLevel;
            indexStartData[2] = indexStartLat;
            indexStartData[3] = indexStartLon;
            size_t indexCountData[4] = {0, 0, 0, 0};
            indexCountData[0] = indexLengthTime;
            indexCountData[1] = 1;
            indexCountData[2] = indexLengthLat;
            indexCountData[3] = indexLengthLon;
            ptrdiff_t indexStrideData[4] = {0, 0, 0, 0};
            indexStrideData[0] = m_timeIndexStep;
            indexStrideData[1] = 1;
            indexStrideData[2] = m_latIndexStep;
            indexStrideData[3] = m_lonIndexStep;

            // In the netCDF Common Data Language, variables are printed with the outermost dimension first and the innermost dimension last.
            ncFile.GetVarSample(m_fileVariableName, indexStartData, indexCountData, indexStrideData,
                                &data[indexBegining]);
        } else {
            // Set the indices for data
            size_t indexStartData[3] = {0, 0, 0};
            indexStartData[0] = indexStartTime;
            indexStartData[1] = indexStartLat;
            indexStartData[2] = indexStartLon;
            size_t indexCountData[3] = {0, 0, 0};
            indexCountData[0] = indexLengthTime;
            indexCountData[1] = indexLengthLat;
            indexCountData[2] = indexLengthLon;
            ptrdiff_t indexStrideData[3] = {0, 0, 0};
            indexStrideData[0] = m_timeIndexStep;
            indexStrideData[1] = m_latIndexStep;
            indexStrideData[2] = m_lonIndexStep;

            // In the netCDF Common Data Language, variables are printed with the outermost dimension first and the innermost dimension last.
            ncFile.GetVarSample(m_fileVariableName, indexStartData, indexCountData, indexStrideData,
                                &data[indexBegining]);
        }

        // Keep data for later treatment
        vectIndexLengthLat.push_back(indexLengthLat);
        vectIndexLengthLon.push_back(indexLengthLon);
        vectData.push_back(data);
    }

    // Close the nc file
    ncFile.Close();
    ThreadsManager().CritSectionNetCDF().Leave();

    // Allocate space into compositeData if not already done
    if (compositeData[0].capacity() == 0) {
        int totSize = 0;
        for (int i_area = 0; i_area < (int) compositeData.size(); i_area++) {
            int indexLengthLat = vectIndexLengthLat[i_area];
            int indexLengthLon = vectIndexLengthLon[i_area];
            totSize += m_time.size() * indexLengthLat * (indexLengthLon);
        }
        compositeData.reserve(totSize);
    }

    // Transfer data
    for (int i_area = 0; i_area < (int) compositeData.size(); i_area++) {
        // Extract data
        int indexLengthLat = vectIndexLengthLat[i_area];
        int indexLengthLon = vectIndexLengthLon[i_area];
        VectorShort data = vectData[i_area];

        // Loop to extract the data from the array
        int ind = 0;
        for (int i_time = 0; i_time < indexLengthTimeArray; i_time++) {
            Array2DFloat latlonData(indexLengthLat, indexLengthLon);
            for (int i_lat = 0; i_lat < indexLengthLat; i_lat++) {
                for (int i_lon = 0; i_lon < indexLengthLon; i_lon++) {
                    ind = i_lon + i_lat * indexLengthLon + i_time * indexLengthLon * indexLengthLat;

                    if (scalingNeeded) {
                        latlonData(i_lat, i_lon) = (float) data[ind] * dataScaleFactor + dataAddOffset;
                    } else {
                        latlonData(i_lat, i_lon) = (float) data[ind];
                    }

                    // Check if not NaN
                    bool notNan = true;
                    for (size_t i_nan = 0; i_nan < m_nanValues.size(); i_nan++) {
                        if ((float) data[ind] == m_nanValues[i_nan] || latlonData(i_lat, i_lon) == m_nanValues[i_nan]) {
                            notNan = false;
                        }
                    }
                    if (!notNan) {
                        latlonData(i_lat, i_lon) = NaNFloat;
                    }
                }
            }
            compositeData[i_area].push_back(latlonData);
        }
        data.clear();
    }

    return true;
}
