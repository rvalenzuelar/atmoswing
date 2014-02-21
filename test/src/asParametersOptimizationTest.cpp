#include "include_tests.h"
#include "asParametersOptimization.h"

#include "UnitTest++.h"

namespace
{

TEST(LoadFromFile)
{
    wxString filepath = wxFileName::GetCwd();
    filepath.Append("/files/asParametersOptimizationTestFile01.xml");

    asParametersOptimization params;
    params.LoadFromFile(filepath);

    CHECK_EQUAL(1962, params.GetArchiveYearStart());
    CHECK_EQUAL(2008, params.GetArchiveYearEnd());
    CHECK_EQUAL(1970, params.GetCalibrationYearStart());
    CHECK_EQUAL(2000, params.GetCalibrationYearEnd());
    CHECK_EQUAL(24, params.GetTimeArrayAnalogsTimeStepHours());
    CHECK_EQUAL(24, params.GetTimeArrayTargetTimeStepHours());
    CHECK_EQUAL(1, params.GetTimeArrayAnalogsIntervalDaysIteration());
    CHECK_EQUAL(10, params.GetTimeArrayAnalogsIntervalDaysLowerLimit());
    CHECK_EQUAL(182, params.GetTimeArrayAnalogsIntervalDaysUpperLimit());
    CHECK_EQUAL(false, params.IsTimeArrayAnalogsIntervalDaysLocked());
    CHECK_EQUAL(true, params.IsTimeArrayAnalogsIntervalDaysRandomInit());
    CHECK_EQUAL(60, params.GetTimeArrayAnalogsExcludeDays());
    CHECK_EQUAL(true, params.GetTimeArrayAnalogsMode().IsSameAs("DaysInterval"));
    CHECK_EQUAL(true, params.GetTimeArrayTargetMode().IsSameAs("Simple"));

    CHECK_EQUAL(true, params.GetMethodName(0).IsSameAs("Analogs"));
    CHECK_EQUAL(100, params.GetAnalogsNumber(0));
    CHECK_EQUAL(1, params.GetAnalogsNumberIteration(0));
    CHECK_EQUAL(5, params.GetAnalogsNumberLowerLimit(0));
    CHECK_EQUAL(200, params.GetAnalogsNumberUpperLimit(0));
    CHECK_EQUAL(true, params.IsAnalogsNumberLocked(0));
    CHECK_EQUAL(false, params.IsAnalogsNumberRandomInit(0));

    CHECK_EQUAL(false, params.NeedsPreprocessing(0,0));
    CHECK_EQUAL(true, params.GetPredictorDatasetId(0,0).IsSameAs("NCEP_R-1"));
    CHECK_EQUAL(true, params.GetPredictorDataId(0,0).IsSameAs("hgt"));
    CHECK_EQUAL(500, params.GetPredictorLevel(0,0));
    CHECK_EQUAL(24, params.GetPredictorDTimeHours(0,0));
    CHECK_EQUAL(6, params.GetPredictorDTimeHoursIteration(0,0));
    CHECK_EQUAL(-48, params.GetPredictorDTimeHoursLowerLimit(0,0));
    CHECK_EQUAL(48, params.GetPredictorDTimeHoursUpperLimit(0,0));
    CHECK_EQUAL(true, params.IsPredictorDTimeHoursLocked(0,0));
    CHECK_EQUAL(false, params.IsPredictorDTimeHoursRandomInit(0,0));
    CHECK_EQUAL(1, params.GetPredictorDTimeDays(0,0));
    CHECK_EQUAL(0, params.GetPredictorTimeHour(0,0));
    CHECK_EQUAL(true, params.GetPredictorGridType(0,0).IsSameAs("Regular"));
    CHECK_EQUAL(2.5, params.GetPredictorUminIteration(0,0));
    CHECK_EQUAL(300, params.GetPredictorUminLowerLimit(0,0));
    CHECK_EQUAL(450, params.GetPredictorUminUpperLimit(0,0));
    CHECK_EQUAL(false, params.IsPredictorUminLocked(0,0));
    CHECK_EQUAL(true, params.IsPredictorUminRandomInit(0,0));
    CHECK_EQUAL(1, params.GetPredictorUptsnbIteration(0,0));
    CHECK_EQUAL(1, params.GetPredictorUptsnbLowerLimit(0,0));
    CHECK_EQUAL(21, params.GetPredictorUptsnbUpperLimit(0,0));
    CHECK_EQUAL(false, params.IsPredictorUptsnbLocked(0,0));
    CHECK_EQUAL(true, params.IsPredictorUptsnbRandomInit(0,0));
    CHECK_EQUAL(2.5, params.GetPredictorUstep(0,0));
    CHECK_EQUAL(2.5, params.GetPredictorVminIteration(0,0));
    CHECK_EQUAL(0, params.GetPredictorVminLowerLimit(0,0));
    CHECK_EQUAL(70, params.GetPredictorVminUpperLimit(0,0));
    CHECK_EQUAL(false, params.IsPredictorVminLocked(0,0));
    CHECK_EQUAL(true, params.IsPredictorVminRandomInit(0,0));
    CHECK_EQUAL(1, params.GetPredictorVptsnbIteration(0,0));
    CHECK_EQUAL(1, params.GetPredictorVptsnbLowerLimit(0,0));
    CHECK_EQUAL(13, params.GetPredictorVptsnbUpperLimit(0,0));
    CHECK_EQUAL(false, params.IsPredictorVptsnbLocked(0,0));
    CHECK_EQUAL(true, params.IsPredictorVptsnbRandomInit(0,0));
    CHECK_EQUAL(2.5, params.GetPredictorVstep(0,0));
    CHECK_EQUAL(true, params.GetPredictorCriteria(0,0).IsSameAs("S1"));
    CHECK_CLOSE(0.01, params.GetPredictorWeightIteration(0,0), 0.0001);
    CHECK_CLOSE(0, params.GetPredictorWeightLowerLimit(0,0), 0.0001);
    CHECK_CLOSE(1, params.GetPredictorWeightUpperLimit(0,0), 0.0001);
    CHECK_EQUAL(false, params.IsPredictorWeightLocked(0,0));
    CHECK_EQUAL(true, params.IsPredictorWeightRandomInit(0,0));

    CHECK_EQUAL(true, params.NeedsPreprocessing(0,1));
    CHECK_EQUAL(true, params.GetPreprocessMethod(0,1).IsSameAs("Gradients"));
    CHECK_EQUAL(true, params.GetPreprocessDatasetId(0,1,0).IsSameAs("NCEP_R-1"));
    CHECK_EQUAL(true, params.GetPreprocessDataId(0,1,0).IsSameAs("hgt"));
    CHECK_EQUAL(1000, params.GetPreprocessLevel(0,1,0));
    CHECK_EQUAL(12, params.GetPreprocessDTimeHours(0,1,0));
    CHECK_EQUAL(true, params.GetPredictorGridType(0,1).IsSameAs("Regular"));
    CHECK_EQUAL(2.5, params.GetPredictorUminIteration(0,1));
    CHECK_EQUAL(300, params.GetPredictorUminLowerLimit(0,1));
    CHECK_EQUAL(450, params.GetPredictorUminUpperLimit(0,1));
    CHECK_EQUAL(false, params.IsPredictorUminLocked(0,1));
    CHECK_EQUAL(true, params.IsPredictorUminRandomInit(0,1));
    CHECK_EQUAL(1, params.GetPredictorUptsnbIteration(0,1));
    CHECK_EQUAL(3, params.GetPredictorUptsnbLowerLimit(0,1));
    CHECK_EQUAL(19, params.GetPredictorUptsnbUpperLimit(0,1));
    CHECK_EQUAL(false, params.IsPredictorUptsnbLocked(0,1));
    CHECK_EQUAL(true, params.IsPredictorUptsnbRandomInit(0,1));
    CHECK_EQUAL(2.5, params.GetPredictorUstep(0,1));
    CHECK_EQUAL(2.5, params.GetPredictorVminIteration(0,1));
    CHECK_EQUAL(0, params.GetPredictorVminLowerLimit(0,1));
    CHECK_EQUAL(70, params.GetPredictorVminUpperLimit(0,1));
    CHECK_EQUAL(false, params.IsPredictorVminLocked(0,1));
    CHECK_EQUAL(true, params.IsPredictorVminRandomInit(0,1));
    CHECK_EQUAL(1, params.GetPredictorVptsnbIteration(0,1));
    CHECK_EQUAL(1, params.GetPredictorVptsnbLowerLimit(0,1));
    CHECK_EQUAL(9, params.GetPredictorVptsnbUpperLimit(0,1));
    CHECK_EQUAL(false, params.IsPredictorVptsnbLocked(0,1));
    CHECK_EQUAL(true, params.IsPredictorVptsnbRandomInit(0,1));
    CHECK_EQUAL(2.5, params.GetPredictorVstep(0,1));
    CHECK_EQUAL(true, params.GetPredictorCriteria(0,1).IsSameAs("S1"));
    CHECK_CLOSE(0.01, params.GetPredictorWeightIteration(0,1), 0.0001);
    CHECK_CLOSE(0, params.GetPredictorWeightLowerLimit(0,1), 0.0001);
    CHECK_CLOSE(1, params.GetPredictorWeightUpperLimit(0,1), 0.0001);
    CHECK_EQUAL(false, params.IsPredictorWeightLocked(0,1));
    CHECK_EQUAL(true, params.IsPredictorWeightRandomInit(0,1));

    CHECK_EQUAL(40, params.GetPredictandStationId());

    CHECK_EQUAL(true, params.GetForecastScoreName().IsSameAs("CRPSAR"));
    CHECK_EQUAL(1, params.GetForecastScoreAnalogsNumberIteration());
    CHECK_EQUAL(5, params.GetForecastScoreAnalogsNumberLowerLimit());
    CHECK_EQUAL(150, params.GetForecastScoreAnalogsNumberUpperLimit());
    CHECK_EQUAL(false, params.IsForecastScoreAnalogsNumberLocked());
    CHECK_EQUAL(true, params.IsForecastScoreAnalogsNumberRandomInit());

    CHECK_EQUAL(true, params.ForecastScoreNeedsPostprocessing());
    CHECK_EQUAL(true, params.GetForecastScorePostprocessMethod().IsSameAs("DuplicationOnCriteriaExponent"));
    CHECK_CLOSE(2.1, params.GetForecastScorePostprocessDupliExp(), 0.0001);

    CHECK_EQUAL(true, params.GetForecastScoreTimeArrayMode().IsSameAs("Simple"));
    CHECK_CLOSE(357434, params.GetForecastScoreTimeArrayDate(), 0.0001);
    CHECK_CLOSE(60, params.GetForecastScoreTimeArrayIntervalDays(), 0.0001);
}

TEST(LoadFromFileAndInitRandomValues)
{
    wxString filepath = wxFileName::GetCwd();
    filepath.Append("/files/asParametersOptimizationTestFile01.xml");

    asParametersOptimization params;
    params.LoadFromFile(filepath);

    params.InitRandomValues();

    CHECK_EQUAL(1962, params.GetArchiveYearStart());
    CHECK_EQUAL(2008, params.GetArchiveYearEnd());
    CHECK_EQUAL(1970, params.GetCalibrationYearStart());
    CHECK_EQUAL(2000, params.GetCalibrationYearEnd());
    CHECK_EQUAL(24, params.GetTimeArrayAnalogsTimeStepHours());
    CHECK_EQUAL(24, params.GetTimeArrayTargetTimeStepHours());
    CHECK_EQUAL(1, params.GetTimeArrayAnalogsIntervalDaysIteration());
    CHECK_EQUAL(10, params.GetTimeArrayAnalogsIntervalDaysLowerLimit());
    CHECK_EQUAL(182, params.GetTimeArrayAnalogsIntervalDaysUpperLimit());
    CHECK_EQUAL(false, params.IsTimeArrayAnalogsIntervalDaysLocked());
    CHECK_EQUAL(true, params.IsTimeArrayAnalogsIntervalDaysRandomInit());
    CHECK_EQUAL(60, params.GetTimeArrayAnalogsExcludeDays());
    CHECK_EQUAL(true, params.GetTimeArrayAnalogsMode().IsSameAs("DaysInterval"));
    CHECK_EQUAL(true, params.GetTimeArrayTargetMode().IsSameAs("Simple"));

    CHECK_EQUAL(true, params.GetMethodName(0).IsSameAs("Analogs"));
    CHECK_EQUAL(100, params.GetAnalogsNumber(0));
    CHECK_EQUAL(1, params.GetAnalogsNumberIteration(0));
    CHECK_EQUAL(5, params.GetAnalogsNumberLowerLimit(0));
    CHECK_EQUAL(200, params.GetAnalogsNumberUpperLimit(0));
    CHECK_EQUAL(true, params.IsAnalogsNumberLocked(0));
    CHECK_EQUAL(false, params.IsAnalogsNumberRandomInit(0));

    CHECK_EQUAL(false, params.NeedsPreprocessing(0,0));
    CHECK_EQUAL(true, params.GetPredictorDatasetId(0,0).IsSameAs("NCEP_R-1"));
    CHECK_EQUAL(true, params.GetPredictorDataId(0,0).IsSameAs("hgt"));
    CHECK_EQUAL(500, params.GetPredictorLevel(0,0));
    CHECK_EQUAL(24, params.GetPredictorDTimeHours(0,0));
    CHECK_EQUAL(6, params.GetPredictorDTimeHoursIteration(0,0));
    CHECK_EQUAL(-48, params.GetPredictorDTimeHoursLowerLimit(0,0));
    CHECK_EQUAL(48, params.GetPredictorDTimeHoursUpperLimit(0,0));
    CHECK_EQUAL(true, params.IsPredictorDTimeHoursLocked(0,0));
    CHECK_EQUAL(false, params.IsPredictorDTimeHoursRandomInit(0,0));
    CHECK_EQUAL(1, params.GetPredictorDTimeDays(0,0));
    CHECK_EQUAL(0, params.GetPredictorTimeHour(0,0));
    CHECK_EQUAL(true, params.GetPredictorGridType(0,0).IsSameAs("Regular"));
    CHECK_EQUAL(2.5, params.GetPredictorUminIteration(0,0));
    CHECK_EQUAL(300, params.GetPredictorUminLowerLimit(0,0));
    CHECK_EQUAL(450, params.GetPredictorUminUpperLimit(0,0));
    CHECK_EQUAL(false, params.IsPredictorUminLocked(0,0));
    CHECK_EQUAL(true, params.IsPredictorUminRandomInit(0,0));
    CHECK_EQUAL(1, params.GetPredictorUptsnbIteration(0,0));
    CHECK_EQUAL(1, params.GetPredictorUptsnbLowerLimit(0,0));
    CHECK_EQUAL(21, params.GetPredictorUptsnbUpperLimit(0,0));
    CHECK_EQUAL(false, params.IsPredictorUptsnbLocked(0,0));
    CHECK_EQUAL(true, params.IsPredictorUptsnbRandomInit(0,0));
    CHECK_EQUAL(2.5, params.GetPredictorUstep(0,0));
    CHECK_EQUAL(2.5, params.GetPredictorVminIteration(0,0));
    CHECK_EQUAL(0, params.GetPredictorVminLowerLimit(0,0));
    CHECK_EQUAL(70, params.GetPredictorVminUpperLimit(0,0));
    CHECK_EQUAL(false, params.IsPredictorVminLocked(0,0));
    CHECK_EQUAL(true, params.IsPredictorVminRandomInit(0,0));
    CHECK_EQUAL(1, params.GetPredictorVptsnbIteration(0,0));
    CHECK_EQUAL(1, params.GetPredictorVptsnbLowerLimit(0,0));
    CHECK_EQUAL(13, params.GetPredictorVptsnbUpperLimit(0,0));
    CHECK_EQUAL(false, params.IsPredictorVptsnbLocked(0,0));
    CHECK_EQUAL(true, params.IsPredictorVptsnbRandomInit(0,0));
    CHECK_EQUAL(2.5, params.GetPredictorVstep(0,0));
    CHECK_EQUAL(true, params.GetPredictorCriteria(0,0).IsSameAs("S1"));
    CHECK_CLOSE(0.01, params.GetPredictorWeightIteration(0,0), 0.0001);
    CHECK_CLOSE(0, params.GetPredictorWeightLowerLimit(0,0), 0.0001);
    CHECK_CLOSE(1, params.GetPredictorWeightUpperLimit(0,0), 0.0001);
    CHECK_EQUAL(false, params.IsPredictorWeightLocked(0,0));
    CHECK_EQUAL(true, params.IsPredictorWeightRandomInit(0,0));

    CHECK_EQUAL(true, params.NeedsPreprocessing(0,1));
    CHECK_EQUAL(true, params.GetPreprocessMethod(0,1).IsSameAs("Gradients"));
    CHECK_EQUAL(true, params.GetPreprocessDatasetId(0,1,0).IsSameAs("NCEP_R-1"));
    CHECK_EQUAL(true, params.GetPreprocessDataId(0,1,0).IsSameAs("hgt"));
    CHECK_EQUAL(1000, params.GetPreprocessLevel(0,1,0));
    CHECK_EQUAL(12, params.GetPreprocessDTimeHours(0,1,0));
    CHECK_EQUAL(true, params.GetPredictorGridType(0,1).IsSameAs("Regular"));
    CHECK_EQUAL(2.5, params.GetPredictorUminIteration(0,1));
    CHECK_EQUAL(300, params.GetPredictorUminLowerLimit(0,1));
    CHECK_EQUAL(450, params.GetPredictorUminUpperLimit(0,1));
    CHECK_EQUAL(false, params.IsPredictorUminLocked(0,1));
    CHECK_EQUAL(true, params.IsPredictorUminRandomInit(0,1));
    CHECK_EQUAL(1, params.GetPredictorUptsnbIteration(0,1));
    CHECK_EQUAL(3, params.GetPredictorUptsnbLowerLimit(0,1));
    CHECK_EQUAL(19, params.GetPredictorUptsnbUpperLimit(0,1));
    CHECK_EQUAL(false, params.IsPredictorUptsnbLocked(0,1));
    CHECK_EQUAL(true, params.IsPredictorUptsnbRandomInit(0,1));
    CHECK_EQUAL(2.5, params.GetPredictorUstep(0,1));
    CHECK_EQUAL(2.5, params.GetPredictorVminIteration(0,1));
    CHECK_EQUAL(0, params.GetPredictorVminLowerLimit(0,1));
    CHECK_EQUAL(70, params.GetPredictorVminUpperLimit(0,1));
    CHECK_EQUAL(false, params.IsPredictorVminLocked(0,1));
    CHECK_EQUAL(true, params.IsPredictorVminRandomInit(0,1));
    CHECK_EQUAL(1, params.GetPredictorVptsnbIteration(0,1));
    CHECK_EQUAL(1, params.GetPredictorVptsnbLowerLimit(0,1));
    CHECK_EQUAL(9, params.GetPredictorVptsnbUpperLimit(0,1));
    CHECK_EQUAL(false, params.IsPredictorVptsnbLocked(0,1));
    CHECK_EQUAL(true, params.IsPredictorVptsnbRandomInit(0,1));
    CHECK_EQUAL(2.5, params.GetPredictorVstep(0,1));
    CHECK_EQUAL(true, params.GetPredictorCriteria(0,1).IsSameAs("S1"));
    CHECK_CLOSE(0.01, params.GetPredictorWeightIteration(0,1), 0.0001);
    CHECK_CLOSE(0, params.GetPredictorWeightLowerLimit(0,1), 0.0001);
    CHECK_CLOSE(1, params.GetPredictorWeightUpperLimit(0,1), 0.0001);
    CHECK_EQUAL(false, params.IsPredictorWeightLocked(0,1));
    CHECK_EQUAL(true, params.IsPredictorWeightRandomInit(0,1));

    CHECK_EQUAL(40, params.GetPredictandStationId());

    CHECK_EQUAL(true, params.GetForecastScoreName().IsSameAs("CRPSAR"));
    CHECK_EQUAL(1, params.GetForecastScoreAnalogsNumberIteration());
    CHECK_EQUAL(5, params.GetForecastScoreAnalogsNumberLowerLimit());
    CHECK_EQUAL(150, params.GetForecastScoreAnalogsNumberUpperLimit());
    CHECK_EQUAL(false, params.IsForecastScoreAnalogsNumberLocked());
    CHECK_EQUAL(true, params.IsForecastScoreAnalogsNumberRandomInit());

    CHECK_EQUAL(true, params.ForecastScoreNeedsPostprocessing());
    CHECK_EQUAL(true, params.GetForecastScorePostprocessMethod().IsSameAs("DuplicationOnCriteriaExponent"));
    CHECK_CLOSE(2.1, params.GetForecastScorePostprocessDupliExp(), 0.0001);

    CHECK_EQUAL(true, params.GetForecastScoreTimeArrayMode().IsSameAs("Simple"));
    CHECK_CLOSE(357434, params.GetForecastScoreTimeArrayDate(), 0.0001);
    CHECK_CLOSE(60, params.GetForecastScoreTimeArrayIntervalDays(), 0.0001);
}

}
