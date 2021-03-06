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
 */

#include "asFramePredictandDB.h"

#include "asPredictandPrecipitation.h"
#include "asPredictandTemperature.h"
#include "asPredictandLightnings.h"

asFramePredictandDB::asFramePredictandDB(wxWindow *parent, wxWindowID id)
        : asFramePredictandDBVirtual(parent, id)
{
    // Set the defaults
    wxConfigBase *pConfig = wxFileConfig::Get();
    long choiceDataParam = pConfig->Read("/PredictandDBToolbox/ChoiceDataParam", 0l);
    m_choiceDataParam->SetSelection((int) choiceDataParam);
    long choiceDataTempResol = pConfig->Read("/PredictandDBToolbox/ChoiceDataTempResol", 0l);
    m_choiceDataTempResol->SetSelection((int) choiceDataTempResol);
    long choiceDataSpatAggreg = pConfig->Read("/PredictandDBToolbox/ChoiceDataSpatAggreg", 0l);
    m_choiceDataSpatAggreg->SetSelection((int) choiceDataSpatAggreg);
    wxString ReturnPeriodNorm = pConfig->Read("/PredictandDBToolbox/ReturnPeriodNorm", "10");
    m_textCtrlReturnPeriod->SetValue(ReturnPeriodNorm);
    bool normalizeByReturnPeriod = true;
    pConfig->Read("/PredictandDBToolbox/NormalizeByReturnPeriod", &normalizeByReturnPeriod);
    m_checkBoxReturnPeriod->SetValue(normalizeByReturnPeriod);
    bool processSquareRoot = false;
    pConfig->Read("/PredictandDBToolbox/ProcessSquareRoot", &processSquareRoot);
    m_checkBoxSqrt->SetValue(processSquareRoot);
    wxString catalogPath = pConfig->Read("/PredictandDBToolbox/CatalogPath", wxEmptyString);
    m_filePickerCatalogPath->SetPath(catalogPath);
    wxString predictandDataDir = pConfig->Read("/PredictandDBToolbox/PredictandDataDir", wxEmptyString);
    m_dirPickerDataDir->SetPath(predictandDataDir);
    wxString destinationDir = pConfig->Read("/PredictandDBToolbox/DestinationDir", wxEmptyString);
    m_dirPickerDestinationDir->SetPath(destinationDir);
    wxString patternsDir = pConfig->Read("/PredictandDBToolbox/PatternsDir", wxEmptyString);
    m_dirPickerPatternsDir->SetPath(patternsDir);

    ToggleProcessing();

    // Icon
#ifdef __WXMSW__
    SetIcon(wxICON(myicon));
#endif
}

void asFramePredictandDB::OnSaveDefault(wxCommandEvent &event)
{
    wxBusyCursor wait;

    // Save as defaults
    wxConfigBase *pConfig = wxFileConfig::Get();
    auto choiceDataParam = (long) m_choiceDataParam->GetSelection();
    pConfig->Write("/PredictandDBToolbox/ChoiceDataParam", choiceDataParam);
    m_choiceDataParam->SetSelection((int) choiceDataParam);
    auto choiceDataTempResol = (long) m_choiceDataTempResol->GetSelection();
    pConfig->Write("/PredictandDBToolbox/ChoiceDataTempResol", choiceDataTempResol);
    m_choiceDataTempResol->SetSelection((int) choiceDataTempResol);
    auto choiceDataSpatAggreg = (long) m_choiceDataSpatAggreg->GetSelection();
    pConfig->Write("/PredictandDBToolbox/ChoiceDataSpatAggreg", choiceDataSpatAggreg);
    m_choiceDataSpatAggreg->SetSelection((int) choiceDataSpatAggreg);
    wxString returnPeriodNorm = m_textCtrlReturnPeriod->GetValue();
    pConfig->Write("/PredictandDBToolbox/ReturnPeriodNorm", returnPeriodNorm);
    bool normalizeByReturnPeriod = m_checkBoxReturnPeriod->GetValue();
    pConfig->Write("/PredictandDBToolbox/NormalizeByReturnPeriod", normalizeByReturnPeriod);
    bool processSquareRoot = m_checkBoxSqrt->GetValue();
    pConfig->Write("/PredictandDBToolbox/ProcessSquareRoot", processSquareRoot);
    wxString catalogPath = m_filePickerCatalogPath->GetPath();
    pConfig->Write("/PredictandDBToolbox/CatalogPath", catalogPath);
    wxString predictandDataDir = m_dirPickerDataDir->GetPath();
    pConfig->Write("/PredictandDBToolbox/PredictandDataDir", predictandDataDir);
    wxString destinationDir = m_dirPickerDestinationDir->GetPath();
    pConfig->Write("/PredictandDBToolbox/DestinationDir", destinationDir);
    wxString patternsDir = m_dirPickerPatternsDir->GetPath();
    pConfig->Write("/PredictandDBToolbox/PatternsDir", patternsDir);

    pConfig->Flush();
}

void asFramePredictandDB::CloseFrame(wxCommandEvent &event)
{
    Close();
}

void asFramePredictandDB::OnDataSelection(wxCommandEvent &event)
{
    ToggleProcessing();
}

void asFramePredictandDB::ToggleProcessing()
{
    switch (m_choiceDataParam->GetSelection()) {
        case 0: // precipitation
        {
            m_panelDataProcessing->Enable();
            m_checkBoxReturnPeriod->Enable();
            m_textCtrlReturnPeriod->Enable();
            m_staticTextYears->Enable();
            m_checkBoxSqrt->Enable();
            break;
        }
        default: // other
        {
            m_panelDataProcessing->Disable();
            m_checkBoxReturnPeriod->Disable();
            m_textCtrlReturnPeriod->Disable();
            m_staticTextYears->Disable();
            m_checkBoxSqrt->Disable();
            break;
        }
    }
}

void asFramePredictandDB::BuildDatabase(wxCommandEvent &event)
{
    wxBusyCursor wait;

    try {
        // Get paths
        wxString catalogFilePath = m_filePickerCatalogPath->GetPath();
        if (catalogFilePath.IsEmpty()) {
            wxLogError(_("The given path for the predictand catalog is empty."));
            return;
        }
        wxString pathDataDir = m_dirPickerDataDir->GetPath();
        if (pathDataDir.IsEmpty()) {
            wxLogError(_("The given path for the data directory is empty."));
            return;
        }
        wxString pathDestinationDir = m_dirPickerDestinationDir->GetPath();
        if (pathDestinationDir.IsEmpty()) {
            wxLogError(_("The given path for the output destination is empty."));
            return;
        }
        wxString pathPatternsDir = m_dirPickerPatternsDir->GetPath();
        if (pathPatternsDir.IsEmpty()) {
            wxLogError(_("The given path for the patterns directory is empty."));
            return;
        }

        // Get temporal resolution
        asPredictand::TemporalResolution temporalResolution = asPredictand::Daily;
        switch (m_choiceDataTempResol->GetSelection()) {
            case wxNOT_FOUND: {
                wxLogError(_("Wrong selection of the temporal resolution option."));
                break;
            }
            case 0: // 24 hours
            {
                temporalResolution = asPredictand::Daily;
                break;
            }
            case 1: // 6 hours
            {
                temporalResolution = asPredictand::SixHourly;
                break;
            }
            case 2: // Moving temporal window (1-hourly)
            {
                temporalResolution = asPredictand::OneHourlyMTW;
                break;
            }
            case 3: // Moving temporal window (3-hourly)
            {
                temporalResolution = asPredictand::ThreeHourlyMTW;
                break;
            }
            case 4: // Moving temporal window (6-hourly)
            {
                temporalResolution = asPredictand::SixHourlyMTW;
                break;
            }
            case 5: // Moving temporal window (12-hourly)
            {
                temporalResolution = asPredictand::TwelveHourlyMTW;
                break;
            }
            default:
                wxLogError(_("Wrong selection of the temporal resolution option."));
        }

        // Get temporal resolution
        asPredictand::SpatialAggregation spatialAggregation = asPredictand::Station;
        switch (m_choiceDataSpatAggreg->GetSelection()) {
            case wxNOT_FOUND: {
                wxLogError(_("Wrong selection of the spatial aggregation option."));
                break;
            }
            case 0: // Station
            {
                spatialAggregation = asPredictand::Station;
                break;
            }
            case 1: // Groupment
            {
                spatialAggregation = asPredictand::Groupment;
                break;
            }
            case 2: // Catchment
            {
                spatialAggregation = asPredictand::Catchment;
                break;
            }
            default:
                wxLogError(_("Wrong selection of the spatial aggregation option."));
        }

        // Get data parameter
        switch (m_choiceDataParam->GetSelection()) {
            case wxNOT_FOUND: {
                wxLogError(_("Wrong selection of the data parameter option."));
                break;
            }
            case 0: // Precipitation
            {
                // Get data processing options
                // Return period
                double valReturnPeriod = 0;
                if (m_checkBoxReturnPeriod->GetValue()) {
                    wxString valReturnPeriodString = m_textCtrlReturnPeriod->GetValue();
                    valReturnPeriodString.ToDouble(&valReturnPeriod);
                    if ((valReturnPeriod < 1) | (valReturnPeriod > 1000)) {
                        wxLogError(_("The given return period is not consistent."));
                        return;
                    }
                }

                // Instantiate a predictand object
                asPredictandPrecipitation predictand(asPredictand::Precipitation, temporalResolution,
                                                     spatialAggregation);
                predictand.SetHasReferenceValues(m_checkBoxReturnPeriod->GetValue());
                predictand.SetIsSqrt(m_checkBoxSqrt->GetValue());
                predictand.BuildPredictandDB(catalogFilePath, pathDataDir, pathPatternsDir, pathDestinationDir);
                break;
            }
            case 1: // Temperature
            {
                // Instantiate a predictand object
                asPredictandTemperature predictand(asPredictand::AirTemperature, temporalResolution,
                                                   spatialAggregation);
                predictand.BuildPredictandDB(catalogFilePath, pathDataDir, pathPatternsDir, pathDestinationDir);
                break;
            }
            case 2: // Lightnings
            {
                // Instantiate a predictand object
                asPredictandLightnings predictand(asPredictand::Lightnings, temporalResolution, spatialAggregation);
                predictand.BuildPredictandDB(catalogFilePath, pathDataDir, pathPatternsDir, pathDestinationDir);
                break;
            }
            case 3: // Other
            {
                wxLogError(_("Generic predictand database not yet implemented."));
                break;
            }
            default:
                wxLogError(_("Wrong selection of the data parameter option."));
        }
    } catch (asException &e) {
        wxString fullMessage = e.GetFullMessage();
        if (!fullMessage.IsEmpty()) {
            wxLogError(fullMessage);
        }
    }
}
