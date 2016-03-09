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

#include "asDataPredictandPrecipitation.h"
#include "asDataPredictandTemperature.h"
#include "asDataPredictandLightnings.h"

asFramePredictandDB::asFramePredictandDB( wxWindow* parent, wxWindowID id )
:
asFramePredictandDBVirtual( parent, id )
{
    // Set the defaults
    wxConfigBase *pConfig = wxFileConfig::Get();
    long choiceDataParam = pConfig->Read("/PredictandDBToolbox/ChoiceDataParam", 0l);
    m_choiceDataParam->SetSelection((int)choiceDataParam);
    long choiceDataTempResol = pConfig->Read("/PredictandDBToolbox/ChoiceDataTempResol", 0l);
    m_choiceDataTempResol->SetSelection((int)choiceDataTempResol);
    long choiceDataSpatAggreg = pConfig->Read("/PredictandDBToolbox/ChoiceDataSpatAggreg", 0l);
    m_choiceDataSpatAggreg->SetSelection((int)choiceDataSpatAggreg);
    wxString ReturnPeriodNorm = pConfig->Read("/PredictandDBToolbox/ReturnPeriodNorm", "10");
    m_textCtrlReturnPeriod->SetValue(ReturnPeriodNorm);
    bool NormalizeByReturnPeriod = true;
    pConfig->Read("/PredictandDBToolbox/NormalizeByReturnPeriod", &NormalizeByReturnPeriod);
    m_checkBoxReturnPeriod->SetValue(NormalizeByReturnPeriod);
    bool ProcessSquareRoot = false;
    pConfig->Read("/PredictandDBToolbox/ProcessSquareRoot", &ProcessSquareRoot);
    m_checkBoxSqrt->SetValue(ProcessSquareRoot);
    wxString catalogPath = pConfig->Read("/PredictandDBToolbox/CatalogPath", wxEmptyString);
    m_filePickerCatalogPath->SetPath(catalogPath);
    wxString PredictandDataDir = pConfig->Read("/PredictandDBToolbox/PredictandDataDir", wxEmptyString);
    m_dirPickerDataDir->SetPath(PredictandDataDir);
    wxString DestinationDir = pConfig->Read("/PredictandDBToolbox/DestinationDir", wxEmptyString);
    m_dirPickerDestinationDir->SetPath(DestinationDir);
    wxString PatternsDir = pConfig->Read("/PredictandDBToolbox/PatternsDir", wxEmptyString);
    m_dirPickerPatternsDir->SetPath(PatternsDir);

    ToggleProcessing();

    // Icon
#ifdef __WXMSW__
    SetIcon(wxICON(myicon));
#endif
}

void asFramePredictandDB::OnSaveDefault( wxCommandEvent& event )
{
    // Save as defaults
    wxConfigBase *pConfig = wxFileConfig::Get();
    long choiceDataParam = (long)m_choiceDataParam->GetSelection();
    pConfig->Write("/PredictandDBToolbox/ChoiceDataParam", choiceDataParam);
    m_choiceDataParam->SetSelection((int)choiceDataParam);
    long choiceDataTempResol = (long)m_choiceDataTempResol->GetSelection();
    pConfig->Write("/PredictandDBToolbox/ChoiceDataTempResol", choiceDataTempResol);
    m_choiceDataTempResol->SetSelection((int)choiceDataTempResol);
    long choiceDataSpatAggreg = (long)m_choiceDataSpatAggreg->GetSelection();
    pConfig->Write("/PredictandDBToolbox/ChoiceDataSpatAggreg", choiceDataSpatAggreg);
    m_choiceDataSpatAggreg->SetSelection((int)choiceDataSpatAggreg);
    wxString ReturnPeriodNorm = m_textCtrlReturnPeriod->GetValue();
    pConfig->Write("/PredictandDBToolbox/ReturnPeriodNorm", ReturnPeriodNorm);
    bool NormalizeByReturnPeriod = m_checkBoxReturnPeriod->GetValue();
    pConfig->Write("/PredictandDBToolbox/NormalizeByReturnPeriod", NormalizeByReturnPeriod);
    bool ProcessSquareRoot = m_checkBoxSqrt->GetValue();
    pConfig->Write("/PredictandDBToolbox/ProcessSquareRoot", ProcessSquareRoot);
    wxString catalogPath = m_filePickerCatalogPath->GetPath();
    pConfig->Write("/PredictandDBToolbox/CatalogPath", catalogPath);
    wxString PredictandDataDir = m_dirPickerDataDir->GetPath();
    pConfig->Write("/PredictandDBToolbox/PredictandDataDir", PredictandDataDir);
    wxString DestinationDir = m_dirPickerDestinationDir->GetPath();
    pConfig->Write("/PredictandDBToolbox/DestinationDir", DestinationDir);
    wxString PatternsDir = m_dirPickerPatternsDir->GetPath();
    pConfig->Write("/PredictandDBToolbox/PatternsDir", PatternsDir);

    pConfig->Flush();
}

void asFramePredictandDB::CloseFrame( wxCommandEvent& event )
{
    Close();
}

void asFramePredictandDB::OnDataSelection( wxCommandEvent& event )
{
    ToggleProcessing();
}

void asFramePredictandDB::ToggleProcessing()
{
    switch (m_choiceDataParam->GetSelection())
    {
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

void asFramePredictandDB::BuildDatabase( wxCommandEvent& event )
{
    try
    {
        // Get paths
        wxString catalogFilePath = m_filePickerCatalogPath->GetPath();
        if (catalogFilePath.IsEmpty())
        {
            asLogError(_("The given path for the predictand catalog is empty."));
            return;
        }
        wxString pathDataDir = m_dirPickerDataDir->GetPath();
        if (pathDataDir.IsEmpty())
        {
            asLogError(_("The given path for the data directory is empty."));
            return;
        }
        wxString pathDestinationDir = m_dirPickerDestinationDir->GetPath();
        if (pathDestinationDir.IsEmpty())
        {
            asLogError(_("The given path for the output destination is empty."));
            return;
        }
        wxString pathPatternsDir = m_dirPickerPatternsDir->GetPath();
        if (pathPatternsDir.IsEmpty())
        {
            asLogError(_("The given path for the patterns directory is empty."));
            return;
        }

        // Get temporal resolution
        DataTemporalResolution dataTemporalResolution = Daily;
        switch (m_choiceDataTempResol->GetSelection())
        {
            case wxNOT_FOUND:
            {
                asLogError(_("Wrong selection of the temporal resolution option."));
                break;
            }
            case 0: // 24 hours
            {
                dataTemporalResolution = Daily;
                break;
            }
            case 1: // 6 hours
            {
                dataTemporalResolution = SixHourly;
                break;
            }
            case 2: // Moving temporal window (6/24 hours)
            {
                dataTemporalResolution = SixHourlyMovingDailyTemporalWindow;
                break;
            }
            default:
                asLogError(_("Wrong selection of the temporal resolution option."));
        }

        // Get temporal resolution
        DataSpatialAggregation dataSpatialAggregation = Station;
        switch (m_choiceDataSpatAggreg->GetSelection())
        {
            case wxNOT_FOUND:
            {
                asLogError(_("Wrong selection of the spatial aggregation option."));
                break;
            }
            case 0: // Station
            {
                dataSpatialAggregation = Station;
                break;
            }
            case 1: // Groupment
            {
                dataSpatialAggregation = Groupment;
                break;
            }
            case 2: // Catchment
            {
                dataSpatialAggregation = Catchment;
                break;
            }
            default:
                asLogError(_("Wrong selection of the spatial aggregation option."));
        }

        // Get data parameter
        switch (m_choiceDataParam->GetSelection())
        {
            case wxNOT_FOUND:
            {
                asLogError(_("Wrong selection of the data parameter option."));
                break;
            }
            case 0: // Precipitation
            {
                // Get data processing options
                // Return period
                double valReturnPeriod = 0;
                if (m_checkBoxReturnPeriod->GetValue())
                {
                    wxString valReturnPeriodString = m_textCtrlReturnPeriod->GetValue();
                    valReturnPeriodString.ToDouble(&valReturnPeriod);
                    if ( (valReturnPeriod<1) | (valReturnPeriod>1000) )
                    {
                        asLogError(_("The given return period is not consistent."));
                        return;
                    }
                }

                // Sqrt option
                bool makeSqrt = false;
                if (m_checkBoxSqrt->GetValue())
                {
                    makeSqrt = true;
                }

                // Instiantiate a predictand object
                asDataPredictandPrecipitation predictand(Precipitation, dataTemporalResolution, dataSpatialAggregation);
                predictand.SetIsSqrt(makeSqrt);
                predictand.BuildPredictandDB(catalogFilePath, pathDataDir, pathPatternsDir, pathDestinationDir);
                break;
            }
            case 1: // Temperature
            {
                // Instiantiate a predictand object
                asDataPredictandTemperature predictand(AirTemperature, dataTemporalResolution, dataSpatialAggregation);
                predictand.BuildPredictandDB(catalogFilePath, pathDataDir, pathPatternsDir, pathDestinationDir);
                break;
            }
            case 2: // Lightnings
            {
                // Instiantiate a predictand object
                asDataPredictandLightnings predictand(Lightnings, dataTemporalResolution, dataSpatialAggregation);
                predictand.BuildPredictandDB(catalogFilePath, pathDataDir, pathPatternsDir, pathDestinationDir);
                break;
            }
            case 3: // Other
            {
                asLogError(_("Generic predictand database not yet implemented."));
                break;
            }
            default:
                asLogError(_("Wrong selection of the data parameter option."));
        }
    }
    catch(asException& e)
    {
        wxString fullMessage = e.GetFullMessage();
        if (!fullMessage.IsEmpty())
        {
            asLogError(fullMessage);
        }
    }
}
