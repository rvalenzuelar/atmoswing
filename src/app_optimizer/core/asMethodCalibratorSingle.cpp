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

#include "asMethodCalibratorSingle.h"

asMethodCalibratorSingle::asMethodCalibratorSingle()
        : asMethodCalibrator()
{

}

asMethodCalibratorSingle::~asMethodCalibratorSingle()
{

}

bool asMethodCalibratorSingle::Calibrate(asParametersCalibration &params)
{
    // Check that we really handle a single case
    bool checkSizes = true;
    wxString errorField = wxEmptyString;
    if (params.GetTimeArrayAnalogsIntervalDaysVector().size() > 1) {
        checkSizes = false;
        errorField.Append("IntervalDays, ");
    }
    if (params.GetForecastScoreNameVector().size() > 1) {
        checkSizes = false;
        errorField.Append("ForecastScoreName, ");
    }
    if (params.GetForecastScoreTimeArrayModeVector().size() > 1) {
        checkSizes = false;
        errorField.Append("ForecastScoreTimeArrayMode, ");
    }
    if (params.GetForecastScoreTimeArrayDateVector().size() > 1) {
        checkSizes = false;
        errorField.Append("ForecastScoreTimeArrayDate ,");
    }
    if (params.GetForecastScoreTimeArrayIntervalDaysVector().size() > 1) {
        checkSizes = false;
        errorField.Append("ForecastScoreTimeArrayIntervalDays, ");
    }
    if (params.GetForecastScorePostprocessDupliExpVector().size() > 1) {
        checkSizes = false;
        errorField.Append("ForecastScorePostprocessDupliExp, ");
    }

    for (int i_step = 0; i_step < params.GetStepsNb(); i_step++) {
        if (params.GetAnalogsNumberVector(i_step).size() > 1) {
            checkSizes = false;
            errorField.Append(wxString::Format("AnalogsNumber (step %d), ", i_step));
        }
        for (int i_predictor = 0; i_predictor < params.GetPredictorsNb(i_step); i_predictor++) {
            if (params.NeedsPreprocessing(i_step, i_predictor)) {
                for (int i_pre = 0; i_pre < params.GetPreprocessSize(i_step, i_predictor); i_pre++) {
                    if (params.GetPreprocessDataIdVector(i_step, i_predictor, i_pre).size() > 1) {
                        checkSizes = false;
                        errorField.Append(
                                wxString::Format("PreprocessDataId (step %d, predictor %d, preprocess %d), ", i_step,
                                                 i_predictor, i_pre));
                    }
                    if (params.GetPreprocessLevelVector(i_step, i_predictor, i_pre).size() > 1) {
                        checkSizes = false;
                        errorField.Append(
                                wxString::Format("PreprocessLevel (step %d, predictor %d, preprocess %d), ", i_step,
                                                 i_predictor, i_pre));
                    }
                    if (params.GetPreprocessTimeHoursVector(i_step, i_predictor, i_pre).size() > 1) {
                        checkSizes = false;
                        errorField.Append(
                                wxString::Format("PreprocessTimeHours (step %d, predictor %d, preprocess %d), ", i_step,
                                                 i_predictor, i_pre));
                    }
                }
            }

            // Do the other ones anyway
            if (params.GetPredictorDataIdVector(i_step, i_predictor).size() > 1) {
                checkSizes = false;
                errorField.Append(wxString::Format("PredictorDataId (step %d, predictor %d), ", i_step, i_predictor));
            }
            if (params.GetPredictorLevelVector(i_step, i_predictor).size() > 1) {
                checkSizes = false;
                errorField.Append(wxString::Format("PredictorLevel (step %d, predictor %d), ", i_step, i_predictor));
            }
            if (params.GetPredictorTimeHoursVector(i_step, i_predictor).size() > 1) {
                checkSizes = false;
                errorField.Append(
                        wxString::Format("PredictorTimeHours (step %d, predictor %d), ", i_step, i_predictor));
            }
            if (params.GetPredictorXminVector(i_step, i_predictor).size() > 1) {
                checkSizes = false;
                errorField.Append(wxString::Format("PredictorXmin (step %d, predictor %d), ", i_step, i_predictor));
            }
            if (params.GetPredictorXptsnbVector(i_step, i_predictor).size() > 1) {
                checkSizes = false;
                errorField.Append(wxString::Format("PredictorXptsnb (step %d, predictor %d), ", i_step, i_predictor));
            }
            if (params.GetPredictorYminVector(i_step, i_predictor).size() > 1) {
                checkSizes = false;
                errorField.Append(wxString::Format("PredictorYmin (step %d, predictor %d), ", i_step, i_predictor));
            }
            if (params.GetPredictorYptsnbVector(i_step, i_predictor).size() > 1) {
                checkSizes = false;
                errorField.Append(wxString::Format("PredictorYptsnb (step %d, predictor %d), ", i_step, i_predictor));
            }
            if (params.GetPredictorCriteriaVector(i_step, i_predictor).size() > 1) {
                checkSizes = false;
                errorField.Append(wxString::Format("PredictorCriteria (step %d, predictor %d), ", i_step, i_predictor));
            }
            if (params.GetPredictorWeightVector(i_step, i_predictor).size() > 1) {
                checkSizes = false;
                errorField.Append(wxString::Format("PredictorWeight (step %d, predictor %d), ", i_step, i_predictor));
            }

            if (params.NeedsPreprocessing(i_step, i_predictor)) {
                for (int i_dataset = 0; i_dataset < params.GetPreprocessSize(i_step, i_predictor); i_dataset++) {
                    if (params.GetPreprocessLevelVector(i_step, i_predictor, i_dataset).size() > 1) {
                        checkSizes = false;
                        errorField.Append(
                                wxString::Format("PreprocessLevel (step %d, predictor %d), ", i_step, i_predictor));
                    }
                    if (params.GetPreprocessTimeHoursVector(i_step, i_predictor, i_dataset).size() > 1) {
                        checkSizes = false;
                        errorField.Append(wxString::Format("PreprocessTimeHoursV (step %d, predictor %d), ", i_step,
                                                           i_predictor));
                    }
                }
            }
        }
    }

    if (!checkSizes) {
        errorField = errorField.Remove(errorField.Length() - 3, 2); // Removes the last coma
        wxString errorMessage =
                _("The following parameters are not compatible with the single assessment: ") + errorField;
        asLogError(errorMessage);
        return false;
    }

    // Extract the stations IDs
    VVectorInt stationsId = params.GetPredictandStationIdsVector();

    // Create result object to save the final parameters sets
    asResultsParametersArray results_all;
    results_all.Init(_("all_station_parameters"));

    // Create a analogsdate object to save previous analogs dates selection.
    asResultsAnalogsDates anaDatesPrevious;

    for (unsigned int i_stat = 0; i_stat < stationsId.size(); i_stat++) {
        ClearAll();

        VectorInt stationId = stationsId[i_stat];

        // Reset the score of the climatology
        m_scoreClimatology.clear();

        // Create results objects
        asResultsAnalogsDates anaDates;
        asResultsAnalogsValues anaValues;
        asResultsAnalogsForecastScores anaScores;
        asResultsAnalogsForecastScoreFinal anaScoreFinal;

        // Create result objects to save the parameters sets
        asResultsParametersArray results_tested;
        results_tested.Init(
                wxString::Format(_("station_%s_tested_parameters"), GetPredictandStationIdsList(stationId)));

        // Set the next station ID
        params.SetPredictandStationIds(stationId);

        // Process every step one after the other
        int stepsNb = params.GetStepsNb();
        for (int i_step = 0; i_step < stepsNb; i_step++) {
            bool containsNaNs = false;
            if (i_step == 0) {
                if (!GetAnalogsDates(anaDates, params, i_step, containsNaNs))
                    return false;
            } else {
                if (!GetAnalogsSubDates(anaDates, params, anaDatesPrevious, i_step, containsNaNs))
                    return false;
            }
            if (containsNaNs) {
                asLogError(_("The dates selection contains NaNs"));
                return false;
            }
            if (!GetAnalogsValues(anaValues, params, anaDates, i_step))
                return false;
            if (!GetAnalogsForecastScores(anaScores, params, anaValues, i_step))
                return false;
            if (!GetAnalogsForecastScoreFinal(anaScoreFinal, params, anaScores, i_step))
                return false;

            // Store the result
            results_tested.Add(params, anaScoreFinal.GetForecastScore(), m_scoreValid);

            // Keep the analogs dates of the best parameters set
            anaDatesPrevious = anaDates;
        }

        // Validate
        m_parameters.push_back(params);
        Validate();

        // Keep the best parameters set
        results_all.Add(params, anaScoreFinal.GetForecastScore(), m_scoreValid);
        if (!results_all.Print())
            return false;
        if (!results_tested.Print())
            return false;
    }

    return true;
}