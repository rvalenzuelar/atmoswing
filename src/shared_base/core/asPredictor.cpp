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

#include "asPredictor.h"

#include <asTimeArray.h>
#include <asAreaCompRegGrid.h>
#include <asAreaCompGenGrid.h>
#include <wx/dir.h>


asPredictor::asPredictor(const wxString &dataId)
        : m_initialized(false),
          m_axesChecked(false),
          m_dataId(dataId),
          m_parameter(ParameterUndefined),
          m_unit(UnitUndefined),
          m_strideAllowed(false),
          m_level(0),
          m_latPtsnb(0),
          m_lonPtsnb(0),
          m_isLatLon(true),
          m_isPreprocessed(false),
          m_isEnsemble(false),
          m_canBeClipped(true),
          m_parseTimeReference(false)
{

    m_fStr.hasLevelDim = true;
    m_fStr.singleLevel = false;
    m_fStr.timeStep = 0;
    m_fStr.timeStart = 0;
    m_fStr.timeEnd = 0;
    m_fStr.timeLength = 0;
    m_fInd.memberStart = 0;
    m_fInd.memberCount = 1;
    m_fInd.cutEnd = 0;
    m_fInd.cutStart = 0;
    m_fInd.latStep = 0;
    m_fInd.lonStep = 0;
    m_fInd.level = 0;
    m_fInd.timeArrayCount = 0;
    m_fInd.timeCount = 0;
    m_fInd.timeStart = 0;
    m_fInd.timeStep = 0;

    int arr[] = {asNOT_FOUND, asNOT_FOUND, asNOT_FOUND, asNOT_FOUND};
    AssignGribCode(arr);

    if (dataId.Contains('/')) {
        wxString levelType = dataId.BeforeFirst('/');
        m_product = levelType;
        m_dataId = dataId.AfterFirst('/');
    } else {
        wxLogVerbose(_("The data ID (%s) does not contain the level type"), dataId);
    }

}

bool asPredictor::SetData(vva2f &val)
{
    wxASSERT(m_time.size() > 0);
    wxASSERT((int) m_time.size() == (int) val.size());

    m_latPtsnb = (int) val[0][0].rows();
    m_lonPtsnb = (int) val[0][0].cols();
    m_data.clear();
    m_data.reserve(m_time.size() * val[0].size() * m_latPtsnb * m_lonPtsnb);
    m_data = val;

    return true;
}

bool asPredictor::CheckFilesPresence()
{
    if (m_files.empty()) {
        wxLogError(_("Empty files list."));
        return false;
    }

    int nbDirsToRemove = 0;

    for (int i = 0; i < m_files.size(); i++) {
        if (i > 0 && nbDirsToRemove > 0) {
            wxFileName fileName(m_files[i]);
            for (int j = 0; j < nbDirsToRemove; ++j) {
                fileName.RemoveLastDir();
            }
            m_files[i] = fileName.GetFullPath();
        }

        if (!wxFile::Exists(m_files[i])) {
            // Search recursively in the parent directory
            wxFileName fileName(m_files[i]);
            while (true) {
                // Check for wildcards
                if (wxIsWild(fileName.GetPath())) {
                    wxLogError(_("No wildcard is yet authorized in the path (%s)"), fileName.GetPath());
                    return false;
                } else if (wxIsWild(fileName.GetFullName())) {
                    wxArrayString files;
                    size_t nb = wxDir::GetAllFiles(fileName.GetPath(), &files, fileName.GetFullName());
                    if (nb == 1) {
                        m_files[i] = files[0];
                        break;
                    } else if (nb > 1) {
                        wxLogError(_("Multiple files were found matching the name %s:"), fileName.GetFullName());
                        for (int j = 0; j < nb; ++j) {
                            wxLogError(files[j]);
                        }
                        return false;
                    }
                }

                if (i == 0) {
                    if (fileName.GetDirCount() < 2) {
                        wxLogError(_("File not found: %s"), m_files[i]);
                        return false;
                    }

                    fileName.RemoveLastDir();
                    nbDirsToRemove++;
                    if (fileName.Exists()) {
                        m_files[i] = fileName.GetFullPath();
                        break;
                    }
                }
            }
        }
    }

    return true;
}

bool asPredictor::Load(asAreaCompGrid *desiredArea, asTimeArray &timeArray, float level)
{
    m_level = level;

    if (!m_initialized) {
        if (!Init()) {
            wxLogError(_("Error at initialization of the predictor dataset %s."), m_datasetName);
            return false;
        }
    }

    try {
        // List files and check availability
        ListFiles(timeArray);
        if (!CheckFilesPresence()) {
            return false;
        }

        // Get file axes
        if (!EnquireFileStructure()) {
            return false;
        }

        // Check the time array
        if (!CheckTimeArray(timeArray)) {
            wxLogError(_("The time array is not valid to load data."));
            return false;
        }

        // Create a new area matching the dataset
        asAreaCompGrid *dataArea = CreateMatchingArea(desiredArea);

        // Store time array
        m_time = timeArray.GetTimeArray();
        m_fInd.timeStep = wxMax(timeArray.GetTimeStepHours() / m_fStr.timeStep, 1);

        // Number of composites
        int compositesNb = 1;
        if (dataArea) {
            compositesNb = dataArea->GetNbComposites();
            wxASSERT(compositesNb > 0);
        }

        // Extract composite data from files
        vvva2f compositeData((unsigned long) compositesNb);
        if (!ExtractFromFiles(dataArea, timeArray, compositeData)) {
            wxLogWarning(_("Extracting data from files failed."));
            wxDELETE(dataArea);
            return false;
        }

        // Transform data
        if (!TransformData(compositeData)) {
            wxFAIL;
            return false;
        }

        // Merge the composites into m_data
        if (!MergeComposites(compositeData, dataArea)) {
            wxLogError(_("Merging the composites failed."));
            wxDELETE(dataArea);
            return false;
        }

        // Interpolate the loaded data on the desired grid
        if (desiredArea && desiredArea->IsRegular() && !InterpolateOnGrid(dataArea, desiredArea)) {
            wxLogError(_("Interpolation failed."));
            wxDELETE(dataArea);
            return false;
        }

        // Check the data container length
        if ((unsigned) m_time.size() > m_data.size()) {
            wxLogError(_("The date and the data array lengths do not match (time = %d and data = %d)."),
                       (int) m_time.size(), (int) m_data.size());
            wxLogError(_("Time array starts on %s and ends on %s."), asTime::GetStringTime(m_time[0], ISOdatetime),
                       asTime::GetStringTime(m_time[m_time.size() - 1], ISOdatetime));
            wxDELETE(dataArea);
            return false;
        }

        wxDELETE(dataArea);
    } catch (std::bad_alloc &ba) {
        wxString msg(ba.what(), wxConvUTF8);
        wxLogError(_("Bad allocation caught when loading data: %s"), msg);
        return false;
    } catch (asException &e) {
        wxString fullMessage = e.GetFullMessage();
        if (!fullMessage.IsEmpty()) {
            wxLogError(fullMessage);
        }
        wxLogError(_("Failed to load data."));
        return false;
    }

    return true;
}

bool asPredictor::Load(asAreaCompGrid &desiredArea, asTimeArray &timeArray, float level)
{
    return Load(&desiredArea, timeArray, level);
}

bool asPredictor::Load(asAreaCompGrid &desiredArea, double date, float level)
{
    asTimeArray timeArray(date, asTimeArray::SingleDay);
    timeArray.Init();

    return Load(&desiredArea, timeArray, level);
}

bool asPredictor::Load(asAreaCompGrid *desiredArea, double date, float level)
{
    asTimeArray timeArray(date, asTimeArray::SingleDay);
    timeArray.Init();

    return Load(desiredArea, timeArray, level);
}

bool asPredictor::EnquireFileStructure()
{
    wxASSERT(m_files.size() > 0);

    switch (m_fileType) {
        case (asFile::Netcdf) : {
            if (!EnquireNetcdfFileStructure()) {
                return false;
            }
            break;
        }
        case (asFile::Grib2) : {
            if (!EnquireGribFileStructure()) {
                return false;
            }
            break;
        }
        default: {
            wxLogError(_("Predictor file type not correctly defined."));
        }
    }

    return true;
}

bool asPredictor::ExtractFromFiles(asAreaCompGrid *&dataArea, asTimeArray &timeArray, vvva2f &compositeData)
{
    switch (m_fileType) {
        case (asFile::Netcdf) : {
            for (const auto &fileName : m_files) {
                if (!ExtractFromNetcdfFile(fileName, dataArea, timeArray, compositeData)) {
                    return false;
                }
            }
            break;
        }
        case (asFile::Grib2) : {
            for (const auto &fileName : m_files) {
                if (!ExtractFromGribFile(fileName, dataArea, timeArray, compositeData)) {
                    return false;
                }
            }
            break;
        }
        default: {
            wxLogError(_("Predictor file type not correctly defined."));
        }
    }

    return true;
}

bool asPredictor::EnquireNetcdfFileStructure()
{
    // Open the NetCDF file
    ThreadsManager().CritSectionNetCDF().Enter();
    asFileNetcdf ncFile(m_files[0], asFileNetcdf::ReadOnly);
    if (!ncFile.Open()) {
        ThreadsManager().CritSectionNetCDF().Leave();
        wxFAIL;
        return false;
    }

    // Parse file structure
    if (!ParseFileStructure(ncFile)) {
        ncFile.Close();
        ThreadsManager().CritSectionNetCDF().Leave();
        wxFAIL;
        return false;
    }

    // Close the nc file
    ncFile.Close();
    ThreadsManager().CritSectionNetCDF().Leave();

    return true;
}

bool asPredictor::ExtractFromNetcdfFile(const wxString &fileName, asAreaCompGrid *&dataArea, asTimeArray &timeArray,
                                        vvva2f &compositeData)
{
    // Open the NetCDF file
    ThreadsManager().CritSectionNetCDF().Enter();
    asFileNetcdf ncFile(fileName, asFileNetcdf::ReadOnly);
    if (!ncFile.Open()) {
        ThreadsManager().CritSectionNetCDF().Leave();
        wxFAIL;
        return false;
    }

    // Parse file structure
    if (!ParseFileStructure(ncFile)) {
        ncFile.Close();
        ThreadsManager().CritSectionNetCDF().Leave();
        wxFAIL;
        return false;
    }

    // Get indexes
    if (!GetAxesIndexes(dataArea, timeArray, compositeData)) {
        ncFile.Close();
        ThreadsManager().CritSectionNetCDF().Leave();
        return false;
    }

    // Load data
    if (!GetDataFromFile(ncFile, compositeData)) {
        ncFile.Close();
        ThreadsManager().CritSectionNetCDF().Leave();
        wxFAIL;
        return false;
    }

    // Close the nc file
    ncFile.Close();
    ThreadsManager().CritSectionNetCDF().Leave();

    return true;
}

bool asPredictor::EnquireGribFileStructure()
{
    wxASSERT(m_files.size() > 1);

    // Open 2 Grib files
    ThreadsManager().CritSectionGrib().Enter();
    asFileGrib2 gbFile0(m_files[0], asFileGrib2::ReadOnly);
    if (!gbFile0.Open()) {
        ThreadsManager().CritSectionGrib().Leave();
        wxFAIL;
        return false;
    }
    asFileGrib2 gbFile1(m_files[1], asFileGrib2::ReadOnly);
    if (!gbFile1.Open()) {
        ThreadsManager().CritSectionGrib().Leave();
        wxFAIL;
        return false;
    }

    // Set index position
    if (!gbFile0.SetIndexPosition(m_gribCode, m_level)) {
        gbFile0.Close();
        gbFile1.Close();
        ThreadsManager().CritSectionGrib().Leave();
        wxFAIL;
        return false;
    }
    if (!gbFile1.SetIndexPosition(m_gribCode, m_level)) {
        gbFile0.Close();
        gbFile1.Close();
        ThreadsManager().CritSectionGrib().Leave();
        wxFAIL;
        return false;
    }

    // Parse file structure
    if (!ParseFileStructure(&gbFile0, &gbFile1)) {
        gbFile0.Close();
        gbFile1.Close();
        ThreadsManager().CritSectionGrib().Leave();
        wxFAIL;
        return false;
    }

    // Close the nc file
    gbFile0.Close();
    gbFile1.Close();
    ThreadsManager().CritSectionGrib().Leave();

    return true;
}

bool asPredictor::ExtractFromGribFile(const wxString &fileName, asAreaCompGrid *&dataArea, asTimeArray &timeArray,
                                      vvva2f &compositeData)
{
    // Open the Grib file
    ThreadsManager().CritSectionGrib().Enter();
    asFileGrib2 gbFile(fileName, asFileGrib2::ReadOnly);
    if (!gbFile.Open()) {
        ThreadsManager().CritSectionGrib().Leave();
        wxFAIL;
        return false;
    }

    // Set index position
    if (!gbFile.SetIndexPosition(m_gribCode, m_level)) {
        gbFile.Close();
        ThreadsManager().CritSectionGrib().Leave();
        wxFAIL;
        return false;
    }

    // Parse file structure
    if (!ParseFileStructure(&gbFile)) {
        gbFile.Close();
        ThreadsManager().CritSectionGrib().Leave();
        wxFAIL;
        return false;
    }

    // Get indexes
    if (!GetAxesIndexes(dataArea, timeArray, compositeData)) {
        gbFile.Close();
        ThreadsManager().CritSectionGrib().Leave();
        wxFAIL;
        return false;
    }

    // Load data
    if (!GetDataFromFile(gbFile, compositeData)) {
        gbFile.Close();
        ThreadsManager().CritSectionGrib().Leave();
        wxFAIL;
        return false;
    }

    // Close the nc file
    gbFile.Close();
    ThreadsManager().CritSectionGrib().Leave();

    return true;
}

bool asPredictor::ParseFileStructure(asFileNetcdf &ncFile)
{
    if (!ExtractSpatialAxes(ncFile)) return false;
    if (!ExtractLevelAxis(ncFile)) return false;
    if (!ExtractTimeAxis(ncFile)) return false;

    return CheckFileStructure();
}

bool asPredictor::ExtractTimeAxis(asFileNetcdf &ncFile)
{
    // Time dimension takes ages to load !! Avoid and get the first value.
    m_fStr.timeLength = ncFile.GetVarLength(m_fStr.dimTimeName);

    double timeFirstVal, timeLastVal;
    nc_type ncTypeTime = ncFile.GetVarType(m_fStr.dimTimeName);
    switch (ncTypeTime) {
        case NC_DOUBLE:
            timeFirstVal = ncFile.GetVarOneDouble(m_fStr.dimTimeName, 0);
            timeLastVal = ncFile.GetVarOneDouble(m_fStr.dimTimeName, m_fStr.timeLength - 1);
            break;
        case NC_FLOAT:
            timeFirstVal = (double) ncFile.GetVarOneFloat(m_fStr.dimTimeName, 0);
            timeLastVal = (double) ncFile.GetVarOneFloat(m_fStr.dimTimeName, m_fStr.timeLength - 1);
            break;
        case NC_INT:
            timeFirstVal = (double) ncFile.GetVarOneInt(m_fStr.dimTimeName, 0);
            timeLastVal = (double) ncFile.GetVarOneInt(m_fStr.dimTimeName, m_fStr.timeLength - 1);
            break;
        default:
            wxLogError(_("Variable type not supported yet for the time dimension."));
            return false;
    }

    double refValue = NaNd;
    if (m_parseTimeReference) {
        wxString refValueStr = ncFile.GetAttString("units", m_fStr.dimTimeName);
        int start = refValueStr.Find("since");
        if (start != wxNOT_FOUND) {
            refValueStr = refValueStr.Remove(0, (size_t) start + 6);
            int end = refValueStr.Find(" ");
            if (end != wxNOT_FOUND) {
                refValueStr = refValueStr.Remove((size_t) end, refValueStr.Length() - end);
            }
            refValue = asTime::GetTimeFromString(refValueStr);
        } else {
            wxLogError(_("Time reference could not be extracted."));
            return false;
        }
    }

    m_fStr.timeStart = ConvertToMjd(timeFirstVal, refValue);
    m_fStr.timeEnd = ConvertToMjd(timeLastVal, refValue);
    m_fStr.timeStep = asRound(24 * (m_fStr.timeEnd - m_fStr.timeStart) / (m_fStr.timeLength - 1));
    m_fStr.firstHour = fmod(24 * m_fStr.timeStart, m_fStr.timeStep);

    return true;
}

bool asPredictor::ExtractLevelAxis(asFileNetcdf &ncFile)
{
    if (m_fStr.hasLevelDim) {
        m_fStr.levels = a1d(ncFile.GetVarLength(m_fStr.dimLevelName));

        nc_type ncTypeLevel = ncFile.GetVarType(m_fStr.dimLevelName);
        switch (ncTypeLevel) {
            case NC_FLOAT: {
                a1f axisLevelFloat(ncFile.GetVarLength(m_fStr.dimLevelName));
                ncFile.GetVar(m_fStr.dimLevelName, &axisLevelFloat[0]);
                for (int i = 0; i < axisLevelFloat.size(); ++i) {
                    m_fStr.levels[i] = (double) axisLevelFloat[i];
                }
            }
                break;
            case NC_INT: {
                a1i axisLevelInt(ncFile.GetVarLength(m_fStr.dimLevelName));
                ncFile.GetVar(m_fStr.dimLevelName, &axisLevelInt[0]);
                for (int i = 0; i < axisLevelInt.size(); ++i) {
                    m_fStr.levels[i] = (double) axisLevelInt[i];
                }
            }
                break;
            case NC_DOUBLE: {
                ncFile.GetVar(m_fStr.dimLevelName, &m_fStr.levels[0]);
            }
                break;
            default:
                wxLogError(_("Variable type not supported yet for the level dimension."));
                return false;
        }

        // Check unit
        wxString unit = ncFile.GetAttString("units", m_fStr.dimLevelName);
        if (unit.IsSameAs("millibars", false) || unit.IsSameAs("millibar", false) || unit.IsSameAs("hPa", false) ||
                unit.IsSameAs("mbar", false) || unit.IsSameAs("m", false) || unit.IsEmpty()) {
            // Nothing to do.
        } else if (unit.IsSameAs("Pa", false)) {
            for (int i = 0; i < m_fStr.levels.size(); ++i) {
                m_fStr.levels[i] /= 100;
            }
        } else {
            wxLogError(_("Unknown unit for the level dimension: %s."), unit);
            return false;
        }

    }

    return true;
}

bool asPredictor::ExtractSpatialAxes(asFileNetcdf &ncFile)
{
    if (!ncFile.HasVariable(m_fStr.dimLonName)) {
        if (ncFile.HasVariable("x")) {
            m_fStr.dimLonName = "x";
        } else {
            wxLogError(_("X axis not found."));
            return false;
        }
    }

    if (!ncFile.HasVariable(m_fStr.dimLatName)) {
        if (ncFile.HasVariable("y")) {
            m_fStr.dimLatName = "y";
        } else {
            wxLogError(_("Y axis not found."));
            return false;
        }
    }

    m_fStr.lons = a1d(ncFile.GetVarLength(m_fStr.dimLonName));
    m_fStr.lats = a1d(ncFile.GetVarLength(m_fStr.dimLatName));

    wxASSERT(ncFile.GetVarType(m_fStr.dimLonName) == ncFile.GetVarType(m_fStr.dimLatName));
    nc_type ncTypeAxes = ncFile.GetVarType(m_fStr.dimLonName);
    switch (ncTypeAxes) {
        case NC_FLOAT: {
            a1f axisLonFloat(ncFile.GetVarLength(m_fStr.dimLonName));
            a1f axisLatFloat(ncFile.GetVarLength(m_fStr.dimLatName));
            ncFile.GetVar(m_fStr.dimLonName, &axisLonFloat[0]);
            ncFile.GetVar(m_fStr.dimLatName, &axisLatFloat[0]);
            for (int i = 0; i < axisLonFloat.size(); ++i) {
                m_fStr.lons[i] = (double) axisLonFloat[i];
            }
            for (int i = 0; i < axisLatFloat.size(); ++i) {
                m_fStr.lats[i] = (double) axisLatFloat[i];
            }
            break;
        }
        case NC_DOUBLE: {
            ncFile.GetVar(m_fStr.dimLonName, &m_fStr.lons[0]);
            ncFile.GetVar(m_fStr.dimLatName, &m_fStr.lats[0]);
            break;
        }
        default:
            wxLogError(_("Variable type not supported yet for the level dimension."));
            return false;
    }

    return true;
}

bool asPredictor::ParseFileStructure(asFileGrib2 *gbFile0, asFileGrib2 *gbFile1)
{
    // Get full axes from the file
    gbFile0->GetXaxis(m_fStr.lons);
    gbFile0->GetYaxis(m_fStr.lats);

    if (m_fStr.hasLevelDim && !m_fStr.singleLevel) {
        wxLogError(_("The level dimension is not yet implemented for Grib files."));
        return false;
    }

    // Yet handle a unique time value per file.
    m_fStr.timeLength = 1;
    m_fStr.timeStart = gbFile0->GetTime();
    m_fStr.timeEnd = gbFile0->GetTime();

    if (gbFile1 != nullptr) {
        double secondFileTime = gbFile1->GetTime();
        m_fStr.timeStep = asRound(24 * (secondFileTime - m_fStr.timeStart));
        m_fStr.firstHour = fmod(24 * m_fStr.timeStart, m_fStr.timeStep);
    }

    return CheckFileStructure();
}

bool asPredictor::CheckFileStructure()
{
    // Check for breaks in the longitude axis.
    if (m_fStr.lons.size() > 1) {
        if (m_fStr.lons[m_fStr.lons.size() - 1] < m_fStr.lons[0]) {
            int iBreak = 0;
            for (int i = 1; i < m_fStr.lons.size(); ++i) {
                if (m_fStr.lons[i] < m_fStr.lons[i - 1]) {
                    if (iBreak != 0) {
                        wxLogError(_("Longitude axis seems not consistent (multiple breaks)."));
                        return false;
                    }
                    iBreak = i;
                }
            }
            for (int i = iBreak; i < m_fStr.lons.size(); ++i) {
                m_fStr.lons[i] += 360;
            }
        }
    }

    return true;
}

asAreaCompGrid *asPredictor::CreateMatchingArea(asAreaCompGrid *desiredArea)
{
    wxASSERT(m_fStr.lons.size() > 0);
    wxASSERT(m_fStr.lats.size() > 0);

    if (desiredArea) {

        bool strideAllowed = m_fileType == asFile::Netcdf;

        if (!desiredArea->InitializeAxes(m_fStr.lons, m_fStr.lats, true)) {
            asThrowException(_("Failed at initializing the axes."));
        }

        if (desiredArea->IsRegular()) {
            auto desiredAreaReg = dynamic_cast<asAreaCompRegGrid *> (desiredArea);

            if (!strideAllowed) {
                m_fInd.lonStep = 1;
                m_fInd.latStep = 1;
            } else {
                m_fInd.lonStep = desiredAreaReg->GetXstepStride();
                m_fInd.latStep = desiredAreaReg->GetYstepStride();
            }

            auto dataArea = new asAreaCompRegGrid(*desiredAreaReg);
            if (!dataArea->InitializeAxes(m_fStr.lons, m_fStr.lats, strideAllowed)) {
                asThrowException(_("Failed at initializing the axes."));
            }

            dataArea->CorrectCornersWithAxes();

            if (!strideAllowed) {
                dataArea->SetSameStepAsData();
            }

            m_lonPtsnb = dataArea->GetXptsNb();
            m_latPtsnb = dataArea->GetYptsNb();
            m_axisLon = desiredArea->GetXaxis();
            m_axisLat = desiredArea->GetYaxis();

            // Order latitude axis (as data will also be ordered)
            asSortArray(&m_axisLat[0], &m_axisLat[m_axisLat.size()-1], Desc);

            return dataArea;

        } else {
            auto desiredAreaGen = dynamic_cast<asAreaCompGenGrid *> (desiredArea);
            m_fInd.lonStep = 1;
            m_fInd.latStep = 1;
            auto dataArea = new asAreaCompGenGrid(*desiredAreaGen);
            if (!dataArea->InitializeAxes(m_fStr.lons, m_fStr.lats, strideAllowed)) {
                asThrowException(_("Failed at initializing the axes."));
            }

            m_lonPtsnb = dataArea->GetXptsNb();
            m_latPtsnb = dataArea->GetYptsNb();
            m_axisLon = desiredArea->GetXaxis();
            m_axisLat = desiredArea->GetYaxis();

            // Order latitude axis (as data will also be ordered)
            asSortArray(&m_axisLat[0], &m_axisLat[m_axisLat.size()-1], Desc);

            return dataArea;
        }

    }

    return nullptr;
}

void asPredictor::AssignGribCode(const int arr[])
{
    m_gribCode.clear();
    for (int i = 0; i < 4; ++i) {
        m_gribCode.push_back(arr[i]);
    }
}

size_t *asPredictor::GetIndexesStartNcdf(int iArea) const
{
    if (!m_isEnsemble) {
        if (m_fStr.hasLevelDim) {
            static size_t array[4] = {0, 0, 0, 0};
            array[0] = (size_t) m_fInd.timeStart;
            array[1] = (size_t) m_fInd.level;
            array[2] = (size_t) m_fInd.areas[iArea].latStart;
            array[3] = (size_t) m_fInd.areas[iArea].lonStart;

            return array;
        } else {
            static size_t array[3] = {0, 0, 0};
            array[0] = (size_t) m_fInd.timeStart;
            array[1] = (size_t) m_fInd.areas[iArea].latStart;
            array[2] = (size_t) m_fInd.areas[iArea].lonStart;

            return array;
        }
    } else {
        if (m_fStr.hasLevelDim) {
            static size_t array[5] = {0, 0, 0, 0, 0};
            array[0] = (size_t) m_fInd.timeStart;
            array[1] = (size_t) m_fInd.memberStart;
            array[2] = (size_t) m_fInd.level;
            array[3] = (size_t) m_fInd.areas[iArea].latStart;
            array[4] = (size_t) m_fInd.areas[iArea].lonStart;

            return array;
        } else {
            static size_t array[4] = {0, 0, 0, 0};
            array[0] = (size_t) m_fInd.timeStart;
            array[1] = (size_t) m_fInd.memberStart;
            array[2] = (size_t) m_fInd.areas[iArea].latStart;
            array[3] = (size_t) m_fInd.areas[iArea].lonStart;

            return array;
        }
    }

    return nullptr;
}

size_t *asPredictor::GetIndexesCountNcdf(int iArea) const
{
    if (!m_isEnsemble) {
        if (m_fStr.hasLevelDim) {
            static size_t array[4] = {0, 0, 0, 0};
            array[0] = (size_t) m_fInd.timeCount;
            array[1] = 1;
            array[2] = (size_t) m_fInd.areas[iArea].latCount;
            array[3] = (size_t) m_fInd.areas[iArea].lonCount;

            return array;
        } else {
            static size_t array[3] = {0, 0, 0};
            array[0] = (size_t) m_fInd.timeCount;
            array[1] = (size_t) m_fInd.areas[iArea].latCount;
            array[2] = (size_t) m_fInd.areas[iArea].lonCount;

            return array;
        }
    } else {
        if (m_fStr.hasLevelDim) {
            static size_t array[5] = {0, 0, 0, 0, 0};
            array[0] = (size_t) m_fInd.timeCount;
            array[1] = (size_t) m_fInd.memberCount;
            array[2] = 1;
            array[3] = (size_t) m_fInd.areas[iArea].latCount;
            array[4] = (size_t) m_fInd.areas[iArea].lonCount;

            return array;
        } else {
            static size_t array[4] = {0, 0, 0, 0};
            array[0] = (size_t) m_fInd.timeCount;
            array[1] = (size_t) m_fInd.memberCount;
            array[2] = (size_t) m_fInd.areas[iArea].latCount;
            array[3] = (size_t) m_fInd.areas[iArea].lonCount;

            return array;
        }
    }

    return nullptr;
}

ptrdiff_t *asPredictor::GetIndexesStrideNcdf() const
{
    if (!m_isEnsemble) {
        if (m_fStr.hasLevelDim) {
            static ptrdiff_t array[4] = {0, 0, 0, 0};
            array[0] = (ptrdiff_t) m_fInd.timeStep;
            array[1] = 1;
            array[2] = (ptrdiff_t) m_fInd.latStep;
            array[3] = (ptrdiff_t) m_fInd.lonStep;

            return array;
        } else {
            static ptrdiff_t array[3] = {0, 0, 0};
            array[0] = (ptrdiff_t) m_fInd.timeStep;
            array[1] = (ptrdiff_t) m_fInd.latStep;
            array[2] = (ptrdiff_t) m_fInd.lonStep;

            return array;
        }
    } else {
        if (m_fStr.hasLevelDim) {
            static ptrdiff_t array[5] = {0, 0, 0, 0, 0};
            array[0] = (ptrdiff_t) m_fInd.timeStep;
            array[1] = 1;
            array[2] = 1;
            array[3] = (ptrdiff_t) m_fInd.latStep;
            array[4] = (ptrdiff_t) m_fInd.lonStep;

            return array;
        } else {
            static ptrdiff_t array[4] = {0, 0, 0, 0};
            array[0] = (ptrdiff_t) m_fInd.timeStep;
            array[1] = 1;
            array[2] = (ptrdiff_t) m_fInd.latStep;
            array[3] = (ptrdiff_t) m_fInd.lonStep;

            return array;
        }
    }

    return nullptr;
}

int *asPredictor::GetIndexesStartGrib(int iArea) const
{
    static int array[2] = {0, 0};
    array[0] = m_fInd.areas[iArea].lonStart;
    array[1] = m_fInd.areas[iArea].latStart;

    return array;
}

int *asPredictor::GetIndexesCountGrib(int iArea) const
{
    static int array[2] = {0, 0};
    array[0] = m_fInd.areas[iArea].lonCount;
    array[1] = m_fInd.areas[iArea].latCount;

    return array;
}

bool asPredictor::GetDataFromFile(asFileNetcdf &ncFile, vvva2f &compositeData)
{

    // Check if scaling is needed
    bool scalingNeeded = true;
    float dataAddOffset = 0, dataScaleFactor = 1;
    if (ncFile.HasAttribute("add_offset", m_fileVarName)) {
        dataAddOffset = ncFile.GetAttFloat("add_offset", m_fileVarName);
    }
    if (ncFile.HasAttribute("scale_factor", m_fileVarName)) {
        dataScaleFactor = ncFile.GetAttFloat("scale_factor", m_fileVarName);
    }
    if (dataAddOffset == 0 && dataScaleFactor == 1)
        scalingNeeded = false;

    vvf vectData;

    for (int iArea = 0; iArea < compositeData.size(); iArea++) {

        // Create the arrays to receive the data
        vf dataF;

        // Resize the arrays to store the new data
        unsigned int totLength = (unsigned int) m_fInd.memberCount * m_fInd.timeArrayCount *
                                 m_fInd.areas[iArea].latCount * m_fInd.areas[iArea].lonCount;
        wxASSERT(totLength > 0);
        dataF.resize(totLength);

        // Fill empty beginning with NaNs
        int indexBegining = 0;
        if (m_fInd.cutStart > 0) {
            int latlonlength = m_fInd.memberCount * m_fInd.areas[iArea].latCount * m_fInd.areas[iArea].lonCount;
            for (int iEmpty = 0; iEmpty < m_fInd.cutStart; iEmpty++) {
                for (int iEmptylatlon = 0; iEmptylatlon < latlonlength; iEmptylatlon++) {
                    dataF[indexBegining] = NaNf;
                    indexBegining++;
                }
            }
        }

        // Fill empty end with NaNs
        int indexEnd = m_fInd.memberCount * m_fInd.timeCount * m_fInd.areas[iArea].latCount *
                       m_fInd.areas[iArea].lonCount - 1;
        if (m_fInd.cutEnd > 0) {
            int latlonlength = m_fInd.memberCount * m_fInd.areas[iArea].latCount *
                               m_fInd.areas[iArea].lonCount;
            for (int iEmpty = 0; iEmpty < m_fInd.cutEnd; iEmpty++) {
                for (int iEmptylatlon = 0; iEmptylatlon < latlonlength; iEmptylatlon++) {
                    indexEnd++;
                    dataF[indexEnd] = NaNf;
                }
            }
        }

        // In the netCDF Common Data Language, variables are printed with the outermost dimension first and the innermost dimension last.
        ncFile.GetVarSample(m_fileVarName, GetIndexesStartNcdf(iArea), GetIndexesCountNcdf(iArea),
                            GetIndexesStrideNcdf(), &dataF[indexBegining]);

        // Keep data for later treatment
        vectData.push_back(dataF);
    }

    // Allocate space into compositeData if not already done
    if (compositeData[0].capacity() == 0) {
        unsigned int totSize = 0;
        for (int iArea = 0; iArea < compositeData.size(); iArea++) {
            totSize += m_fInd.memberCount * m_time.size() * m_fInd.areas[iArea].latCount *
                       (m_fInd.areas[iArea].lonCount + 1); // +1 in case of a border
        }
        compositeData.reserve(totSize);
    }

    // Transfer data
    for (int iArea = 0; iArea < compositeData.size(); iArea++) {
        // Extract data
        vf data = vectData[iArea];

        // Loop to extract the data from the array
        int ind = 0;
        for (int iTime = 0; iTime < m_fInd.timeArrayCount; iTime++) {
            va2f memlatlonData;
            for (int iMem = 0; iMem < m_fInd.memberCount; iMem++) {
                a2f latlonData(m_fInd.areas[iArea].latCount, m_fInd.areas[iArea].lonCount);

                for (int iLat = 0; iLat < m_fInd.areas[iArea].latCount; iLat++) {
                    for (int iLon = 0; iLon < m_fInd.areas[iArea].lonCount; iLon++) {
                        ind = iLon + iLat * m_fInd.areas[iArea].lonCount +
                              iMem * m_fInd.areas[iArea].lonCount * m_fInd.areas[iArea].latCount +
                              iTime * m_fInd.memberCount * m_fInd.areas[iArea].lonCount * m_fInd.areas[iArea].latCount;
                        if (m_fStr.lats.size() > 0 && m_fStr.lats[1] > m_fStr.lats[0]) {
                            int latRevIndex = m_fInd.areas[iArea].latCount - 1 - iLat;
                            ind = iLon + latRevIndex * m_fInd.areas[iArea].lonCount +
                                  iMem * m_fInd.areas[iArea].lonCount * m_fInd.areas[iArea].latCount +
                                  iTime * m_fInd.memberCount * m_fInd.areas[iArea].lonCount * m_fInd.areas[iArea].latCount;
                        }

                        latlonData(iLat, iLon) = data[ind];

                        // Check if not NaN
                        bool notNan = true;
                        for (double nanValue : m_nanValues) {
                            if (data[ind] == nanValue || latlonData(iLat, iLon) == nanValue) {
                                notNan = false;
                            }
                        }
                        if (!notNan) {
                            latlonData(iLat, iLon) = NaNf;
                        }
                    }
                }

                if (scalingNeeded) {
                    latlonData = latlonData * dataScaleFactor + dataAddOffset;
                }
                memlatlonData.push_back(latlonData);
            }
            compositeData[iArea].push_back(memlatlonData);
        }
        data.clear();
    }

    return true;
}

bool asPredictor::GetDataFromFile(asFileGrib2 &gbFile, vvva2f &compositeData)
{
    // Grib files do not handle stride
    if (m_fInd.lonStep != 1 || m_fInd.latStep != 1) {
        wxLogError(_("Grib files do not handle stride."));
        return false;
    }

    vvf vectData;

    for (int iArea = 0; iArea < compositeData.size(); iArea++) {

        // Create the arrays to receive the data
        vf dataF;

        // Resize the arrays to store the new data
        unsigned int totLength = (unsigned int) m_fInd.memberCount * m_fInd.timeArrayCount *
                                 m_fInd.areas[iArea].latCount * m_fInd.areas[iArea].lonCount;
        wxASSERT(totLength > 0);
        dataF.resize(totLength);

        // Extract data
        gbFile.GetVarArray(GetIndexesStartGrib(iArea), GetIndexesCountGrib(iArea), &dataF[0]);

        // Keep data for later treatment
        vectData.push_back(dataF);
    }

    // Allocate space into compositeData if not already done
    if (compositeData[0].capacity() == 0) {
        unsigned int totSize = 0;
        for (int iArea = 0; iArea < compositeData.size(); iArea++) {
            totSize += m_fInd.memberCount * m_time.size() * m_fInd.areas[iArea].latCount *
                       (m_fInd.areas[iArea].lonCount + 1); // +1 in case of a border
        }
        compositeData.reserve(totSize);
    }

    // Transfer data
    for (int iArea = 0; iArea < compositeData.size(); iArea++) {
        // Extract data
        vf data = vectData[iArea];
        va2f memlatlonData;

        for (int iMem = 0; iMem < m_fInd.memberCount; iMem++) {

            // Loop to extract the data from the array
            int ind = 0;
            a2f latlonData(m_fInd.areas[iArea].latCount, m_fInd.areas[iArea].lonCount);

            for (int iLat = 0; iLat < m_fInd.areas[iArea].latCount; iLat++) {
                for (int iLon = 0; iLon < m_fInd.areas[iArea].lonCount; iLon++) {
                    int latRevIndex = m_fInd.areas[iArea].latCount - 1 - iLat; // Index reversed in Grib files
                    ind = iLon + latRevIndex * m_fInd.areas[iArea].lonCount;
                    latlonData(iLat, iLon) = data[ind];

                    // Check if not NaN
                    bool notNan = true;
                    for (double nanValue : m_nanValues) {
                        if (data[ind] == nanValue || latlonData(iLat, iLon) == nanValue) {
                            notNan = false;
                        }
                    }
                    if (!notNan) {
                        latlonData(iLat, iLon) = NaNf;
                    }
                }
            }
            memlatlonData.push_back(latlonData);
        }
        compositeData[iArea].push_back(memlatlonData);

        data.clear();
    }

    return true;
}

bool asPredictor::TransformData(vvva2f &compositeData)
{
    // See http://www.ecmwf.int/en/faq/geopotential-defined-units-m2/s2-both-pressure-levels-and-surface-orography-how-can-height
    if (m_parameter == Geopotential) {
        for (auto &area : compositeData) {
            for (int iTime = 0; iTime < area.size(); iTime++) {
                for (int iMem = 0; iMem < area[0].size(); iMem++) {
                    area[iTime][iMem] = area[iTime][iMem] / 9.80665;
                }
            }
        }
        m_parameter = GeopotentialHeight;
        m_parameterName = "Geopotential height";
        m_unit = m;
    }

    return true;
}

bool asPredictor::Inline()
{
    // Already inlined
    if (m_lonPtsnb == 1 || m_latPtsnb == 1) {
        return true;
    }

    wxASSERT(!m_data.empty());

    auto timeSize = (unsigned int) m_data.size();
    auto membersNb = (unsigned int) m_data[0].size();
    auto cols = (unsigned int) m_data[0][0].cols();
    auto rows = (unsigned int) m_data[0][0].rows();

    a2f inlineData = a2f::Zero(1, cols * rows);

    vva2f newData;
    newData.reserve((unsigned int) (membersNb * m_time.size() * m_lonPtsnb * m_latPtsnb));
    newData.resize(timeSize);

    for (unsigned int iTime = 0; iTime < timeSize; iTime++) {
        for (unsigned int iMem = 0; iMem < membersNb; iMem++) {
            for (unsigned int iRow = 0; iRow < rows; iRow++) {
                inlineData.block(0, iRow * cols, 1, cols) = m_data[iTime][iMem].row(iRow);
            }
            newData[iTime].push_back(inlineData);
        }
    }

    m_data = newData;

    m_latPtsnb = (int) m_data[0][0].rows();
    m_lonPtsnb = (int) m_data[0][0].cols();
    a1d emptyAxis(1);
    emptyAxis[0] = NaNd;
    m_axisLat = emptyAxis;
    m_axisLon = emptyAxis;

    return true;
}

bool asPredictor::MergeComposites(vvva2f &compositeData, asAreaCompGrid *area)
{
    if (area && area->GetNbComposites() > 1) {

        // Get a container with the final size
        unsigned long sizeTime = compositeData[0].size();
        unsigned long membersNb = compositeData[0][0].size();
        m_data = vva2f(sizeTime, va2f(membersNb, a2f(m_latPtsnb, m_lonPtsnb)));

        auto comp0cols = (int)compositeData[0][0][0].cols();
        auto comp1cols = (int)compositeData[1][0][0].cols();

        // Merge the composite data together
        for (unsigned int iTime = 0; iTime < sizeTime; iTime++) {
            for (unsigned int iMem = 0; iMem < membersNb; iMem++) {
                // Append the composite areas
                m_data[iTime][iMem].leftCols(comp0cols) = compositeData[0][iTime][iMem];
                m_data[iTime][iMem].rightCols(comp1cols) = compositeData[1][iTime][iMem];
            }
        }
    } else {
        m_data = compositeData[0];
    }

    return true;
}

bool asPredictor::InterpolateOnGrid(asAreaCompGrid *dataArea, asAreaCompGrid *desiredArea)
{
    wxASSERT(dataArea);
    wxASSERT(dataArea->GetNbComposites() > 0);
    wxASSERT(desiredArea);
    wxASSERT(desiredArea->GetNbComposites() > 0);
    bool changeXstart = false, changeXsteps = false, changeYstart = false, changeYsteps = false;

    // Check beginning on longitudes
    if (dataArea->GetComposite(0).GetXmin() != desiredArea->GetComposite(0).GetXmin()) {
        if (dataArea->GetComposite(0).GetXmin() + 360 != desiredArea->GetComposite(0).GetXmin() &&
            dataArea->GetComposite(0).GetXmin() - 360 != desiredArea->GetComposite(0).GetXmin()) {
            changeXstart = true;
        }
    }
    if (dataArea->GetNbComposites() > 1 && dataArea->GetComposite(1).GetXmin() != desiredArea->GetComposite(1).GetXmin()) {
        if (dataArea->GetComposite(1).GetXmin() + 360 != desiredArea->GetComposite(1).GetXmin() &&
            dataArea->GetComposite(1).GetXmin() - 360 != desiredArea->GetComposite(1).GetXmin()) {
            changeXstart = true;
        }
    }

    // Check beginning on latitudes
    if (dataArea->GetComposite(0).GetYmin() != desiredArea->GetComposite(0).GetYmin()) {
        changeYstart = true;
    }
    if (dataArea->GetNbComposites() > 1 && dataArea->GetComposite(1).GetYmin() != desiredArea->GetComposite(1).GetYmin()) {
        changeYstart = true;
    }

    // Check the cells size on longitudes
    if (dataArea->IsRegular() && !dataArea->GridsOverlay(desiredArea)) {
        changeXsteps = true;
        changeYsteps = true;
    }

    // Proceed to the interpolation
    if (changeXstart || changeYstart || changeXsteps || changeYsteps) {

        if (!dataArea->IsRegular()) {
            wxLogError(_("Interpolation not allowed on irregular grids."));
            return false;
        }

        // Containers for results
        int finalLengthLon = desiredArea->GetXptsNb();
        int finalLengthLat = desiredArea->GetYptsNb();
        vva2f latlonTimeData(m_data.size(), va2f(m_data[0].size(), a2f(finalLengthLat, finalLengthLon)));

        // Creation of the axes
        a1f axisDataLon;
        if (dataArea->GetXptsNb() > 1) {
            auto xMin = (float) dataArea->GetXmin();
            auto xMax = (float) dataArea->GetXmax();
            if (dataArea->IsLatLon() && xMax < xMin) {
                xMax += 360;
            }
            axisDataLon = a1f::LinSpaced(Eigen::Sequential, dataArea->GetXptsNb(), xMin, xMax);
        } else {
            axisDataLon.resize(1);
            axisDataLon << dataArea->GetXmin();
        }

        a1f axisDataLat;
        if (dataArea->GetYptsNb() > 1) {
            axisDataLat = a1f::LinSpaced(Eigen::Sequential, dataArea->GetYptsNb(), dataArea->GetYmax(),
                                         dataArea->GetYmin()); // From top to bottom
        } else {
            axisDataLat.resize(1);
            axisDataLat << dataArea->GetYmax();
        }

        a1f axisFinalLon;
        if (desiredArea->GetXptsNb() > 1) {
            auto xMin = (float) desiredArea->GetXmin();
            auto xMax = (float) desiredArea->GetXmax();
            if (desiredArea->IsLatLon() && xMax < xMin) {
                xMax += 360;
            }
            axisFinalLon = a1f::LinSpaced(Eigen::Sequential, desiredArea->GetXptsNb(), xMin, xMax);
        } else {
            axisFinalLon.resize(1);
            axisFinalLon << desiredArea->GetXmin();
        }

        a1f axisFinalLat;
        if (desiredArea->GetYptsNb() > 1) {
            axisFinalLat = a1f::LinSpaced(Eigen::Sequential, desiredArea->GetYptsNb(), desiredArea->GetYmax(),
                                          desiredArea->GetYmin()); // From top to bottom
        } else {
            axisFinalLat.resize(1);
            axisFinalLat << desiredArea->GetYmax();
        }

        // Indices
        int indexXfloor, indexXceil;
        int indexYfloor, indexYceil;
        int axisDataLonEnd = axisDataLon.size() - 1;
        int axisDataLatEnd = axisDataLat.size() - 1;

        // Pointer to last used element
        int indexLastLon = 0, indexLastLat = 0;

        // Variables
        double dX, dY;
        float valLLcorner, valULcorner, valLRcorner, valURcorner;

        // The interpolation loop
        for (unsigned int iTime = 0; iTime < m_data.size(); iTime++) {
            for (int iMem = 0; iMem < m_data[0].size(); iMem++) {
                // Loop to extract the data from the array
                for (int iLat = 0; iLat < finalLengthLat; iLat++) {
                    // Try the 2 next latitudes (from the top)
                    if (axisDataLat.size() > indexLastLat + 1 && axisDataLat[indexLastLat + 1] == axisFinalLat[iLat]) {
                        indexYfloor = indexLastLat + 1;
                        indexYceil = indexLastLat + 1;
                    } else if (axisDataLat.size() > indexLastLat + 2 &&
                               axisDataLat[indexLastLat + 2] == axisFinalLat[iLat]) {
                        indexYfloor = indexLastLat + 2;
                        indexYceil = indexLastLat + 2;
                    } else {
                        // Search for floor and ceil
                        indexYfloor = indexLastLat + asFindFloor(&axisDataLat[indexLastLat], &axisDataLat[axisDataLatEnd],
                                                                 axisFinalLat[iLat]);
                        indexYceil = indexLastLat + asFindCeil(&axisDataLat[indexLastLat], &axisDataLat[axisDataLatEnd],
                                                               axisFinalLat[iLat]);
                    }

                    if (indexYfloor == asOUT_OF_RANGE || indexYfloor == asNOT_FOUND || indexYceil == asOUT_OF_RANGE ||
                        indexYceil == asNOT_FOUND) {
                        wxLogError(_("The desired point is not available in the data for interpolation. Latitude %f was not found in between %f (index %d) to %f (index %d) (size = %d)."),
                                   axisFinalLat[iLat], axisDataLat[indexLastLat], indexLastLat,
                                   axisDataLat[axisDataLatEnd], axisDataLatEnd, (int) axisDataLat.size());
                        return false;
                    }
                    wxASSERT_MSG(indexYfloor >= 0, wxString::Format("%f in %f to %f",
                                                                    axisFinalLat[iLat],
                                                                    axisDataLat[indexLastLat],
                                                                    axisDataLat[axisDataLatEnd]));
                    wxASSERT(indexYceil >= 0);

                    // Save last index
                    indexLastLat = indexYfloor;

                    for (int iLon = 0; iLon < finalLengthLon; iLon++) {
                        // Try the 2 next longitudes
                        if (axisDataLon.size() > indexLastLon + 1 &&
                            axisDataLon[indexLastLon + 1] == axisFinalLon[iLon]) {
                            indexXfloor = indexLastLon + 1;
                            indexXceil = indexLastLon + 1;
                        } else if (axisDataLon.size() > indexLastLon + 2 &&
                                   axisDataLon[indexLastLon + 2] == axisFinalLon[iLon]) {
                            indexXfloor = indexLastLon + 2;
                            indexXceil = indexLastLon + 2;
                        } else {
                            // Search for floor and ceil
                            indexXfloor = indexLastLon +
                                          asFindFloor(&axisDataLon[indexLastLon], &axisDataLon[axisDataLonEnd],
                                                      axisFinalLon[iLon]);
                            indexXceil = indexLastLon +
                                         asFindCeil(&axisDataLon[indexLastLon], &axisDataLon[axisDataLonEnd],
                                                    axisFinalLon[iLon]);
                        }

                        if (indexXfloor == asOUT_OF_RANGE || indexXfloor == asNOT_FOUND ||
                            indexXceil == asOUT_OF_RANGE || indexXceil == asNOT_FOUND) {
                            wxLogError(_("The desired point is not available in the data for interpolation. Longitude %f was not found in between %f to %f."),
                                       axisFinalLon[iLon], axisDataLon[indexLastLon], axisDataLon[axisDataLonEnd]);
                            return false;
                        }

                        wxASSERT(indexXfloor >= 0);
                        wxASSERT(indexXceil >= 0);

                        // Save last index
                        indexLastLon = indexXfloor;

                        // Proceed to the interpolation
                        if (indexXceil == indexXfloor) {
                            dX = 0;
                        } else {
                            dX = (axisFinalLon[iLon] - axisDataLon[indexXfloor]) /
                                 (axisDataLon[indexXceil] - axisDataLon[indexXfloor]);
                        }
                        if (indexYceil == indexYfloor) {
                            dY = 0;
                        } else {
                            dY = (axisFinalLat[iLat] - axisDataLat[indexYfloor]) /
                                 (axisDataLat[indexYceil] - axisDataLat[indexYfloor]);
                        }


                        if (dX == 0 && dY == 0) {
                            latlonTimeData[iTime][iMem](iLat, iLon) = m_data[iTime][iMem](indexYfloor, indexXfloor);
                        } else if (dX == 0) {
                            valLLcorner = m_data[iTime][iMem](indexYfloor, indexXfloor);
                            valULcorner = m_data[iTime][iMem](indexYceil, indexXfloor);

                            latlonTimeData[iTime][iMem](iLat, iLon) =
                                    (1 - dX) * (1 - dY) * valLLcorner + (1 - dX) * (dY) * valULcorner;
                        } else if (dY == 0) {
                            valLLcorner = m_data[iTime][iMem](indexYfloor, indexXfloor);
                            valLRcorner = m_data[iTime][iMem](indexYfloor, indexXceil);

                            latlonTimeData[iTime][iMem](iLat, iLon) =
                                    (1 - dX) * (1 - dY) * valLLcorner + (dX) * (1 - dY) * valLRcorner;
                        } else {
                            valLLcorner = m_data[iTime][iMem](indexYfloor, indexXfloor);
                            valULcorner = m_data[iTime][iMem](indexYceil, indexXfloor);
                            valLRcorner = m_data[iTime][iMem](indexYfloor, indexXceil);
                            valURcorner = m_data[iTime][iMem](indexYceil, indexXceil);

                            latlonTimeData[iTime][iMem](iLat, iLon) =
                                    (1 - dX) * (1 - dY) * valLLcorner + (1 - dX) * (dY) * valULcorner +
                                    (dX) * (1 - dY) * valLRcorner + (dX) * (dY) * valURcorner;
                        }
                    }

                    indexLastLon = 0;
                }

                indexLastLat = 0;
            }
        }

        m_data = latlonTimeData;
        m_latPtsnb = finalLengthLat;
        m_lonPtsnb = finalLengthLon;
    }

    return true;
}

float asPredictor::GetMinValue() const
{
    float minValue = m_data[0][0](0, 0);
    float tmpValue;

    for (const auto &dat : m_data) {
        for (const auto &v : dat) {
            tmpValue = v.minCoeff();
            if (tmpValue < minValue) {
                minValue = tmpValue;
            }
        }
    }

    return minValue;
}

float asPredictor::GetMaxValue() const
{
    float maxValue = m_data[0][0](0, 0);
    float tmpValue;

    for (const auto &dat : m_data) {
        for (const auto &v : dat) {
            tmpValue = v.maxCoeff();
            if (tmpValue > maxValue) {
                maxValue = tmpValue;
            }
        }
    }

    return maxValue;
}

bool asPredictor::IsLatLon(const wxString &datasetId)
{
    if (datasetId.IsSameAs("CORDEX", false)) {
        return false;
    }

    return true;
}

void asPredictor::CheckLevelTypeIsDefined()
{
    if(m_product.IsEmpty()) {
        asThrowException(_("The type of product must be defined for this dataset (prefix to the variable name. Ex: press/hgt)."));
    }
}


