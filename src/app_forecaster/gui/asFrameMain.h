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
 * The Original Software is AtmoSwing. The Initial Developer of the 
 * Original Software is Pascal Horton of the University of Lausanne. 
 * All Rights Reserved.
 * 
 */

/*
 * Portions Copyright 2008-2013 University of Lausanne.
 */
 
#ifndef __asFrameMain__
#define __asFrameMain__

/**
@file
Subclass of asFrameMainVirtual, which is generated by wxFormBuilder.
*/

#include "AtmoswingForecasterGui.h"
#include "asIncludes.h"
#include "asLogWindow.h"
#include "asMethodForecasting.h"
#include "asPanelsManagerForecastingModels.h"
#include "asBatchForecasts.h"
#include <wx/awx/led.h>

class asPanelsManagerForecastingModels;

/** Implementing asFrameMainVirtual */
class asFrameMain : public asFrameMainVirtual
{

public:
    /** Constructor */
    asFrameMain( wxWindow* parent );
    ~asFrameMain();

    void OnInit();

    double GetForecastDate();
    void SetForecastDate( double date );


protected:
    asLogWindow *m_LogWindow;
    asMethodForecasting *m_Forecaster;
	awxLed* m_LedDownloading;
	awxLed* m_LedLoading;
	awxLed* m_LedProcessing;
	awxLed* m_LedSaving;
    asBatchForecasts m_BatchForecasts;

    void OnOpenBatchForecasts(wxCommandEvent & event);
    void OnSaveBatchForecasts(wxCommandEvent & event);
    void OnSaveBatchForecastsAs(wxCommandEvent & event);
    bool SaveBatchForecasts();
    void OnNewBatchForecasts(wxCommandEvent & event);
    bool OpenBatchForecasts();
    void Update();
    void OpenFrameXmlEditor( wxCommandEvent& event );
    void OpenFramePredictandDB( wxCommandEvent& event );
    void OnConfigureDirectories( wxCommandEvent& event );
    void OpenFramePreferences( wxCommandEvent& event );
    void OpenFrameAbout( wxCommandEvent& event );
    void OnShowLog( wxCommandEvent& event );
    void OnLogLevel1( wxCommandEvent& event );
    void OnLogLevel2( wxCommandEvent& event );
    void OnLogLevel3( wxCommandEvent& event );
    void OnStatusMethodUpdate( wxCommandEvent& event );
    void OnSetPresentDate( wxCommandEvent& event );
    void DisplayLogLevelMenu();
    void LaunchForecasting( wxCommandEvent& event );
    void CancelForecasting( wxCommandEvent& event );
    void AddForecastingModel( wxCommandEvent& event );
    void SetPresentDate();

private:
    asPanelsManagerForecastingModels* m_PanelsManager;

    DECLARE_EVENT_TABLE()

};

#endif // __asFrameMain__
