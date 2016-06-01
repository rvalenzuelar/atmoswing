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

#include <wx/filename.h>
#include "asDataPredictorArchive.h"
#include "asGeoAreaCompositeRegularGrid.h"
#include "asTimeArray.h"
#include "gtest/gtest.h"


TEST(DataPredictorArchiveNcepR1v2014Regular, LoadEasy)
{
    double Xmin = 10;
    double Xwidth = 10;
    double Ymin = 35;
    double Ywidth = 5;
    double step = 2.5;
    float level = 1000;
    asGeoAreaCompositeRegularGrid geoarea(Xmin, Xwidth, step, Ymin, Ywidth, step, level);

    double start = asTime::GetMJD(1960, 1, 1, 00, 00);
    double end = asTime::GetMJD(1960, 1, 11, 00, 00);
    double timestephours = 6;
    asTimeArray timearray(start, end, timestephours, asTimeArray::Simple);
    timearray.Init();

    wxString predictorDataDir = wxFileName::GetCwd();
    predictorDataDir.Append("/files/");

    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance("NCEP_Reanalysis_v1", "hgt",
                                                                            predictorDataDir);

    predictor->SetFileNamePattern("NCEP_Reanalysis_v1(2014)_hgt_%d.nc");
    predictor->Load(&geoarea, timearray);

    VArray2DFloat hgt = predictor->GetData();
    // hgt[time](lat,lon)

    /* Values time step 0 (horizontal=Lon, vertical=Lat)
    176.0	175.0	170.0	162.0	151.0
    185.0	180.0	173.0	162.0	144.0
    193.0	183.0	174.0	163.0	143.0
    */
    EXPECT_FLOAT_EQ(176, hgt[0](0, 0));
    EXPECT_FLOAT_EQ(175, hgt[0](0, 1));
    EXPECT_FLOAT_EQ(170, hgt[0](0, 2));
    EXPECT_FLOAT_EQ(162, hgt[0](0, 3));
    EXPECT_FLOAT_EQ(151, hgt[0](0, 4));
    EXPECT_FLOAT_EQ(185, hgt[0](1, 0));
    EXPECT_FLOAT_EQ(193, hgt[0](2, 0));
    EXPECT_FLOAT_EQ(143, hgt[0](2, 4));

    /* Values time step 1 (horizontal=Lon, vertical=Lat)
    159.0	156.0	151.0	146.0	141.0
    171.0	162.0	151.0	140.0	129.0
    182.0	168.0	154.0	143.0	130.0
    */
    EXPECT_FLOAT_EQ(159, hgt[1](0, 0));
    EXPECT_FLOAT_EQ(156, hgt[1](0, 1));
    EXPECT_FLOAT_EQ(151, hgt[1](0, 2));
    EXPECT_FLOAT_EQ(146, hgt[1](0, 3));
    EXPECT_FLOAT_EQ(141, hgt[1](0, 4));
    EXPECT_FLOAT_EQ(171, hgt[1](1, 0));
    EXPECT_FLOAT_EQ(182, hgt[1](2, 0));
    EXPECT_FLOAT_EQ(130, hgt[1](2, 4));

    /* Values time step 11 (horizontal=Lon, vertical=Lat)
    121.0	104.0	98.0	102.0	114.0
    141.0	125.0	115.0	112.0	116.0
    158.0	147.0	139.0	133.0	131.0
    */
    EXPECT_FLOAT_EQ(121, hgt[11](0, 0));
    EXPECT_FLOAT_EQ(104, hgt[11](0, 1));
    EXPECT_FLOAT_EQ(98, hgt[11](0, 2));
    EXPECT_FLOAT_EQ(102, hgt[11](0, 3));
    EXPECT_FLOAT_EQ(114, hgt[11](0, 4));
    EXPECT_FLOAT_EQ(141, hgt[11](1, 0));
    EXPECT_FLOAT_EQ(158, hgt[11](2, 0));
    EXPECT_FLOAT_EQ(131, hgt[11](2, 4));

    /* Values time step 40 (horizontal=Lon, vertical=Lat)
    -2.0	12.0	24.0	33.0	40.0
    -1.0	11.0	24.0	36.0	49.0
    6.0	    16.0	29.0	46.0	65.0
    */
    EXPECT_FLOAT_EQ(-2, hgt[40](0, 0));
    EXPECT_FLOAT_EQ(12, hgt[40](0, 1));
    EXPECT_FLOAT_EQ(24, hgt[40](0, 2));
    EXPECT_FLOAT_EQ(33, hgt[40](0, 3));
    EXPECT_FLOAT_EQ(40, hgt[40](0, 4));
    EXPECT_FLOAT_EQ(-1, hgt[40](1, 0));
    EXPECT_FLOAT_EQ(6, hgt[40](2, 0));
    EXPECT_FLOAT_EQ(65, hgt[40](2, 4));

    wxDELETE(predictor);
}

TEST(DataPredictorArchiveNcepR1v2014Regular, GetMinMaxValues)
{
    double Xmin = 10;
    double Xwidth = 10;
    double Ymin = 35;
    double Ywidth = 5;
    double step = 2.5;
    float level = 1000;
    asGeoAreaCompositeRegularGrid geoarea(Xmin, Xwidth, step, Ymin, Ywidth, step, level);

    double start = asTime::GetMJD(1960, 1, 1, 00, 00);
    double end = asTime::GetMJD(1960, 1, 11, 00, 00);
    double timestephours = 6;
    asTimeArray timearray(start, end, timestephours, asTimeArray::Simple);
    timearray.Init();

    wxString predictorDataDir = wxFileName::GetCwd();
    predictorDataDir.Append("/files/");

    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance("NCEP_Reanalysis_v1", "hgt",
                                                                            predictorDataDir);

    predictor->SetFileNamePattern("NCEP_Reanalysis_v1(2014)_hgt_%d.nc");
    predictor->Load(&geoarea, timearray);

    EXPECT_FLOAT_EQ(-3, predictor->GetMinValue());
    EXPECT_FLOAT_EQ(199, predictor->GetMaxValue());

    wxDELETE(predictor);
}

TEST(DataPredictorArchiveNcepR1v2014Regular, LoadComposite)
{
    double Xmin = -10;
    double Xwidth = 15;
    double Ymin = 35;
    double Ywidth = 5;
    double step = 2.5;
    float level = 1000;
    asGeoAreaCompositeRegularGrid geoarea(Xmin, Xwidth, step, Ymin, Ywidth, step, level);

    double start = asTime::GetMJD(1960, 1, 1, 00, 00);
    double end = asTime::GetMJD(1960, 1, 11, 00, 00);
    double timestephours = 6;
    asTimeArray timearray(start, end, timestephours, asTimeArray::Simple);
    timearray.Init();

    wxString predictorDataDir = wxFileName::GetCwd();
    predictorDataDir.Append("/files/");

    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance("NCEP_Reanalysis_v1", "hgt",
                                                                            predictorDataDir);

    predictor->SetFileNamePattern("NCEP_Reanalysis_v1(2014)_hgt_%d.nc");
    predictor->Load(&geoarea, timearray);

    VArray2DFloat hgt = predictor->GetData();
    // hgt[time](lat,lon)

    /* Values time step 0 (horizontal=Lon, vertical=Lat)
    187.0	191.0	189.0	183.0   |   175.0	171.0	171.0
    203.0	205.0	201.0	193.0   |   185.0	182.0	184.0
    208.0	209.0	206.0	200.0   |   195.0	196.0	199.0
    */
    EXPECT_FLOAT_EQ(187, hgt[0](0, 0));
    EXPECT_FLOAT_EQ(191, hgt[0](0, 1));
    EXPECT_FLOAT_EQ(189, hgt[0](0, 2));
    EXPECT_FLOAT_EQ(183, hgt[0](0, 3));
    EXPECT_FLOAT_EQ(175, hgt[0](0, 4));
    EXPECT_FLOAT_EQ(171, hgt[0](0, 5));
    EXPECT_FLOAT_EQ(171, hgt[0](0, 6));
    EXPECT_FLOAT_EQ(203, hgt[0](1, 0));
    EXPECT_FLOAT_EQ(208, hgt[0](2, 0));
    EXPECT_FLOAT_EQ(199, hgt[0](2, 6));

    /* Values time step 1 (horizontal=Lon, vertical=Lat)
    188.0	192.0	188.0	180.0    |   170.0	163.0	160.0
    198.0	198.0	194.0	186.0    |   179.0	175.0	175.0
    202.0	202.0	200.0	195.0    |   190.0	189.0	191.0
    */
    EXPECT_FLOAT_EQ(188, hgt[1](0, 0));
    EXPECT_FLOAT_EQ(192, hgt[1](0, 1));
    EXPECT_FLOAT_EQ(188, hgt[1](0, 2));
    EXPECT_FLOAT_EQ(180, hgt[1](0, 3));
    EXPECT_FLOAT_EQ(170, hgt[1](0, 4));
    EXPECT_FLOAT_EQ(163, hgt[1](0, 5));
    EXPECT_FLOAT_EQ(160, hgt[1](0, 6));
    EXPECT_FLOAT_EQ(198, hgt[1](1, 0));
    EXPECT_FLOAT_EQ(202, hgt[1](2, 0));
    EXPECT_FLOAT_EQ(191, hgt[1](2, 6));

    /* Values time step 11 (horizontal=Lon, vertical=Lat)
    227.0	223.0	217.0	211.0    |   203.0	189.0	169.0
    217.0	209.0	203.0	200.0    |   198.0	192.0	179.0
    195.0	187.0	186.0	189.0    |   193.0	191.0	183.0
    */
    EXPECT_FLOAT_EQ(227, hgt[11](0, 0));
    EXPECT_FLOAT_EQ(223, hgt[11](0, 1));
    EXPECT_FLOAT_EQ(217, hgt[11](0, 2));
    EXPECT_FLOAT_EQ(211, hgt[11](0, 3));
    EXPECT_FLOAT_EQ(203, hgt[11](0, 4));
    EXPECT_FLOAT_EQ(189, hgt[11](0, 5));
    EXPECT_FLOAT_EQ(169, hgt[11](0, 6));
    EXPECT_FLOAT_EQ(217, hgt[11](1, 0));
    EXPECT_FLOAT_EQ(195, hgt[11](2, 0));
    EXPECT_FLOAT_EQ(183, hgt[11](2, 6));

    /* Values time step 40 (horizontal=Lon, vertical=Lat)
    45.0	25.0	16.0	11.0    |   4.0	    -5.0	-12.0
    33.0	15.0	6.0	    2.0     |   -2.0	-7.0	-11.0
    53.0	37.0	25.0	15.0    |   7.0	    1.0	    -2.0
    */
    EXPECT_FLOAT_EQ(45, hgt[40](0, 0));
    EXPECT_FLOAT_EQ(25, hgt[40](0, 1));
    EXPECT_FLOAT_EQ(16, hgt[40](0, 2));
    EXPECT_FLOAT_EQ(11, hgt[40](0, 3));
    EXPECT_FLOAT_EQ(4, hgt[40](0, 4));
    EXPECT_FLOAT_EQ(-5, hgt[40](0, 5));
    EXPECT_FLOAT_EQ(-12, hgt[40](0, 6));
    EXPECT_FLOAT_EQ(33, hgt[40](1, 0));
    EXPECT_FLOAT_EQ(53, hgt[40](2, 0));
    EXPECT_FLOAT_EQ(-2, hgt[40](2, 6));

    wxDELETE(predictor);
}

TEST(DataPredictorArchiveNcepR1v2014Regular, LoadBorderLeft)
{
    double Xmin = 0;
    double Xwidth = 5;
    double Ymin = 35;
    double Ywidth = 5;
    double step = 2.5;
    float level = 1000;
    asGeoAreaCompositeRegularGrid geoarea(Xmin, Xwidth, step, Ymin, Ywidth, step, level);

    double start = asTime::GetMJD(1960, 1, 1, 00, 00);
    double end = asTime::GetMJD(1960, 1, 11, 00, 00);
    double timestephours = 6;
    asTimeArray timearray(start, end, timestephours, asTimeArray::Simple);
    timearray.Init();

    wxString predictorDataDir = wxFileName::GetCwd();
    predictorDataDir.Append("/files/");

    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance("NCEP_Reanalysis_v1", "hgt",
                                                                            predictorDataDir);

    predictor->SetFileNamePattern("NCEP_Reanalysis_v1(2014)_hgt_%d.nc");
    predictor->Load(&geoarea, timearray);

    VArray2DFloat hgt = predictor->GetData();
    // hgt[time](lat,lon)

    /* Values time step 0 (horizontal=Lon, vertical=Lat)
    |   175.0	171.0	171.0
    |   185.0	182.0	184.0
    |   195.0	196.0	199.0
    */
    EXPECT_FLOAT_EQ(175, hgt[0](0, 0));
    EXPECT_FLOAT_EQ(171, hgt[0](0, 1));
    EXPECT_FLOAT_EQ(171, hgt[0](0, 2));
    EXPECT_FLOAT_EQ(185, hgt[0](1, 0));
    EXPECT_FLOAT_EQ(195, hgt[0](2, 0));
    EXPECT_FLOAT_EQ(199, hgt[0](2, 2));

    /* Values time step 1 (horizontal=Lon, vertical=Lat)
    |   170.0	163.0	160.0
    |   179.0	175.0	175.0
    |   190.0	189.0	191.0
    */
    EXPECT_FLOAT_EQ(170, hgt[1](0, 0));
    EXPECT_FLOAT_EQ(163, hgt[1](0, 1));
    EXPECT_FLOAT_EQ(160, hgt[1](0, 2));
    EXPECT_FLOAT_EQ(179, hgt[1](1, 0));
    EXPECT_FLOAT_EQ(190, hgt[1](2, 0));
    EXPECT_FLOAT_EQ(191, hgt[1](2, 2));

    /* Values time step 40 (horizontal=Lon, vertical=Lat)
    |   4.0	    -5.0	-12.0
    |   -2.0	-7.0	-11.0
    |   7.0	    1.0	    -2.0
    */
    EXPECT_FLOAT_EQ(4, hgt[40](0, 0));
    EXPECT_FLOAT_EQ(-5, hgt[40](0, 1));
    EXPECT_FLOAT_EQ(-12, hgt[40](0, 2));
    EXPECT_FLOAT_EQ(-2, hgt[40](1, 0));
    EXPECT_FLOAT_EQ(7, hgt[40](2, 0));
    EXPECT_FLOAT_EQ(-2, hgt[40](2, 2));

    wxDELETE(predictor);
}

TEST(DataPredictorArchiveNcepR1v2014Regular, LoadBorderLeftOn720)
{
    double Xmin = 360;
    double Xwidth = 5;
    double Ymin = 35;
    double Ywidth = 5;
    double step = 2.5;
    float level = 1000;
    asGeoAreaCompositeRegularGrid geoarea(Xmin, Xwidth, step, Ymin, Ywidth, step, level);

    double start = asTime::GetMJD(1960, 1, 1, 00, 00);
    double end = asTime::GetMJD(1960, 1, 11, 00, 00);
    double timestephours = 6;
    asTimeArray timearray(start, end, timestephours, asTimeArray::Simple);
    timearray.Init();

    wxString predictorDataDir = wxFileName::GetCwd();
    predictorDataDir.Append("/files/");

    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance("NCEP_Reanalysis_v1", "hgt",
                                                                            predictorDataDir);

    predictor->SetFileNamePattern("NCEP_Reanalysis_v1(2014)_hgt_%d.nc");
    predictor->Load(&geoarea, timearray);

    VArray2DFloat hgt = predictor->GetData();
    // hgt[time](lat,lon)

    /* Values time step 0 (horizontal=Lon, vertical=Lat)
    |   175.0	171.0	171.0
    |   185.0	182.0	184.0
    |   195.0	196.0	199.0
    */
    EXPECT_FLOAT_EQ(175, hgt[0](0, 0));
    EXPECT_FLOAT_EQ(171, hgt[0](0, 1));
    EXPECT_FLOAT_EQ(171, hgt[0](0, 2));
    EXPECT_FLOAT_EQ(185, hgt[0](1, 0));
    EXPECT_FLOAT_EQ(195, hgt[0](2, 0));
    EXPECT_FLOAT_EQ(199, hgt[0](2, 2));

    /* Values time step 1 (horizontal=Lon, vertical=Lat)
    |   170.0	163.0	160.0
    |   179.0	175.0	175.0
    |   190.0	189.0	191.0
    */
    EXPECT_FLOAT_EQ(170, hgt[1](0, 0));
    EXPECT_FLOAT_EQ(163, hgt[1](0, 1));
    EXPECT_FLOAT_EQ(160, hgt[1](0, 2));
    EXPECT_FLOAT_EQ(179, hgt[1](1, 0));
    EXPECT_FLOAT_EQ(190, hgt[1](2, 0));
    EXPECT_FLOAT_EQ(191, hgt[1](2, 2));

    /* Values time step 40 (horizontal=Lon, vertical=Lat)
    |   4.0	    -5.0	-12.0
    |   -2.0	-7.0	-11.0
    |   7.0	    1.0	    -2.0
    */
    EXPECT_FLOAT_EQ(4, hgt[40](0, 0));
    EXPECT_FLOAT_EQ(-5, hgt[40](0, 1));
    EXPECT_FLOAT_EQ(-12, hgt[40](0, 2));
    EXPECT_FLOAT_EQ(-2, hgt[40](1, 0));
    EXPECT_FLOAT_EQ(7, hgt[40](2, 0));
    EXPECT_FLOAT_EQ(-2, hgt[40](2, 2));

    wxDELETE(predictor);
}

TEST(DataPredictorArchiveNcepR1v2014Regular, LoadBorderRight)
{
    double Xmin = 350;
    double Xwidth = 10;
    double Ymin = 35;
    double Ywidth = 5;
    double step = 2.5;
    float level = 1000;
    asGeoAreaCompositeRegularGrid geoarea(Xmin, Xwidth, step, Ymin, Ywidth, step, level);

    double start = asTime::GetMJD(1960, 1, 1, 00, 00);
    double end = asTime::GetMJD(1960, 1, 11, 00, 00);
    double timestephours = 6;
    asTimeArray timearray(start, end, timestephours, asTimeArray::Simple);
    timearray.Init();

    wxString predictorDataDir = wxFileName::GetCwd();
    predictorDataDir.Append("/files/");

    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance("NCEP_Reanalysis_v1", "hgt",
                                                                            predictorDataDir);

    predictor->SetFileNamePattern("NCEP_Reanalysis_v1(2014)_hgt_%d.nc");
    predictor->Load(&geoarea, timearray);

    VArray2DFloat hgt = predictor->GetData();
    // hgt[time](lat,lon)

    /* Values time step 0 (horizontal=Lon, vertical=Lat)
    187.0	191.0	189.0	183.0   |   175.0
    203.0	205.0	201.0	193.0   |   185.0
    208.0	209.0	206.0	200.0   |   195.0
    */
    EXPECT_FLOAT_EQ(187, hgt[0](0, 0));
    EXPECT_FLOAT_EQ(191, hgt[0](0, 1));
    EXPECT_FLOAT_EQ(189, hgt[0](0, 2));
    EXPECT_FLOAT_EQ(183, hgt[0](0, 3));
    EXPECT_FLOAT_EQ(175, hgt[0](0, 4));
    EXPECT_FLOAT_EQ(203, hgt[0](1, 0));
    EXPECT_FLOAT_EQ(208, hgt[0](2, 0));
    EXPECT_FLOAT_EQ(195, hgt[0](2, 4));

    /* Values time step 1 (horizontal=Lon, vertical=Lat)
    188.0	192.0	188.0	180.0    |   170.0
    198.0	198.0	194.0	186.0    |   179.0
    202.0	202.0	200.0	195.0    |   190.0
    */
    EXPECT_FLOAT_EQ(188, hgt[1](0, 0));
    EXPECT_FLOAT_EQ(192, hgt[1](0, 1));
    EXPECT_FLOAT_EQ(188, hgt[1](0, 2));
    EXPECT_FLOAT_EQ(180, hgt[1](0, 3));
    EXPECT_FLOAT_EQ(170, hgt[1](0, 4));
    EXPECT_FLOAT_EQ(198, hgt[1](1, 0));
    EXPECT_FLOAT_EQ(202, hgt[1](2, 0));
    EXPECT_FLOAT_EQ(190, hgt[1](2, 4));

    /* Values time step 40 (horizontal=Lon, vertical=Lat)
    45.0	25.0	16.0	11.0    |   4.0
    33.0	15.0	6.0	    2.0     |   -2.0
    53.0	37.0	25.0	15.0    |   7.0
    */
    EXPECT_FLOAT_EQ(45, hgt[40](0, 0));
    EXPECT_FLOAT_EQ(25, hgt[40](0, 1));
    EXPECT_FLOAT_EQ(16, hgt[40](0, 2));
    EXPECT_FLOAT_EQ(11, hgt[40](0, 3));
    EXPECT_FLOAT_EQ(4, hgt[40](0, 4));
    EXPECT_FLOAT_EQ(33, hgt[40](1, 0));
    EXPECT_FLOAT_EQ(53, hgt[40](2, 0));
    EXPECT_FLOAT_EQ(7, hgt[40](2, 4));

    wxDELETE(predictor);
}

TEST(DataPredictorArchiveNcepR1v2014Regular, LoadCompositeStepLon)
{
    double Xmin = -10;
    double Xwidth = 15;
    double Ymin = 35;
    double Ywidth = 5;
    double steplon = 5;
    double steplat = 2.5;
    float level = 1000;
    asGeoAreaCompositeRegularGrid geoarea(Xmin, Xwidth, steplon, Ymin, Ywidth, steplat, level);

    double start = asTime::GetMJD(1960, 1, 1, 00, 00);
    double end = asTime::GetMJD(1960, 1, 11, 00, 00);
    double timestephours = 6;
    asTimeArray timearray(start, end, timestephours, asTimeArray::Simple);
    timearray.Init();

    wxString predictorDataDir = wxFileName::GetCwd();
    predictorDataDir.Append("/files/");

    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance("NCEP_Reanalysis_v1", "hgt",
                                                                            predictorDataDir);

    predictor->SetFileNamePattern("NCEP_Reanalysis_v1(2014)_hgt_%d.nc");
    predictor->Load(&geoarea, timearray);

    VArray2DFloat hgt = predictor->GetData();
    // hgt[time](lat,lon)

    /* Values time step 0 (horizontal=Lon, vertical=Lat)
    187.0	189.0   |   175.0	171.0
    203.0	201.0   |   185.0	184.0
    208.0	206.0   |   195.0	199.0
    subset of :
    187.0	191.0	189.0	183.0   |   175.0	171.0	171.0
    203.0	205.0	201.0	193.0   |   185.0	182.0	184.0
    208.0	209.0	206.0	200.0   |   195.0	196.0	199.0
    */
    EXPECT_FLOAT_EQ(187, hgt[0](0, 0));
    EXPECT_FLOAT_EQ(189, hgt[0](0, 1));
    EXPECT_FLOAT_EQ(175, hgt[0](0, 2));
    EXPECT_FLOAT_EQ(171, hgt[0](0, 3));
    EXPECT_FLOAT_EQ(203, hgt[0](1, 0));
    EXPECT_FLOAT_EQ(208, hgt[0](2, 0));
    EXPECT_FLOAT_EQ(199, hgt[0](2, 3));

    /* Values time step 1 (horizontal=Lon, vertical=Lat)
    188.0	188.0	|   170.0	160.0
    198.0	194.0	|   179.0	175.0
    202.0	200.0	|   190.0	191.0
    */
    EXPECT_FLOAT_EQ(188, hgt[1](0, 0));
    EXPECT_FLOAT_EQ(188, hgt[1](0, 1));
    EXPECT_FLOAT_EQ(170, hgt[1](0, 2));
    EXPECT_FLOAT_EQ(160, hgt[1](0, 3));
    EXPECT_FLOAT_EQ(198, hgt[1](1, 0));
    EXPECT_FLOAT_EQ(202, hgt[1](2, 0));
    EXPECT_FLOAT_EQ(191, hgt[1](2, 3));

    /* Values time step 40 (horizontal=Lon, vertical=Lat)
    45.0	16.0	|   4.0	    -12.0
    33.0	6.0	    |   -2.0	-11.0
    53.0	25.0	|   7.0	    -2.0
    */
    EXPECT_FLOAT_EQ(45, hgt[40](0, 0));
    EXPECT_FLOAT_EQ(16, hgt[40](0, 1));
    EXPECT_FLOAT_EQ(4, hgt[40](0, 2));
    EXPECT_FLOAT_EQ(-12, hgt[40](0, 3));
    EXPECT_FLOAT_EQ(33, hgt[40](1, 0));
    EXPECT_FLOAT_EQ(53, hgt[40](2, 0));
    EXPECT_FLOAT_EQ(-2, hgt[40](2, 3));

    wxDELETE(predictor);
}

TEST(DataPredictorArchiveNcepR1v2014Regular, LoadCompositeStepLonMoved)
{
    double Xmin = -7.5;
    double Xwidth = 10;
    double Ymin = 35;
    double Ywidth = 5;
    double steplon = 5;
    double steplat = 2.5;
    float level = 1000;
    asGeoAreaCompositeRegularGrid geoarea(Xmin, Xwidth, steplon, Ymin, Ywidth, steplat, level);

    double start = asTime::GetMJD(1960, 1, 1, 00, 00);
    double end = asTime::GetMJD(1960, 1, 11, 00, 00);
    double timestephours = 6;
    asTimeArray timearray(start, end, timestephours, asTimeArray::Simple);
    timearray.Init();

    wxString predictorDataDir = wxFileName::GetCwd();
    predictorDataDir.Append("/files/");

    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance("NCEP_Reanalysis_v1", "hgt",
                                                                            predictorDataDir);

    predictor->SetFileNamePattern("NCEP_Reanalysis_v1(2014)_hgt_%d.nc");
    predictor->Load(&geoarea, timearray);

    VArray2DFloat hgt = predictor->GetData();
    // hgt[time](lat,lon)

    /* Values time step 0 (horizontal=Lon, vertical=Lat)
    191.0	183.0   |   171.0
    205.0	193.0   |   182.0
    209.0	200.0   |   196.0
    */
    EXPECT_FLOAT_EQ(191, hgt[0](0, 0));
    EXPECT_FLOAT_EQ(183, hgt[0](0, 1));
    EXPECT_FLOAT_EQ(171, hgt[0](0, 2));
    EXPECT_FLOAT_EQ(205, hgt[0](1, 0));
    EXPECT_FLOAT_EQ(209, hgt[0](2, 0));
    EXPECT_FLOAT_EQ(196, hgt[0](2, 2));

    /* Values time step 40 (horizontal=Lon, vertical=Lat)
    25.0	11.0    |   -5.0
    15.0	2.0     |   -7.0
    37.0	15.0    |    1.0
    */
    EXPECT_FLOAT_EQ(25, hgt[40](0, 0));
    EXPECT_FLOAT_EQ(11, hgt[40](0, 1));
    EXPECT_FLOAT_EQ(-5, hgt[40](0, 2));
    EXPECT_FLOAT_EQ(15, hgt[40](1, 0));
    EXPECT_FLOAT_EQ(37, hgt[40](2, 0));
    EXPECT_FLOAT_EQ(1, hgt[40](2, 2));

    wxDELETE(predictor);
}

TEST(DataPredictorArchiveNcepR1v2014Regular, LoadCompositeStepLonLat)
{
    double Xmin = -10;
    double Xwidth = 15;
    double Ymin = 35;
    double Ywidth = 5;
    double steplon = 5;
    double steplat = 5;
    float level = 1000;
    asGeoAreaCompositeRegularGrid geoarea(Xmin, Xwidth, steplon, Ymin, Ywidth, steplat, level);

    double start = asTime::GetMJD(1960, 1, 1, 00, 00);
    double end = asTime::GetMJD(1960, 1, 11, 00, 00);
    double timestephours = 6;
    asTimeArray timearray(start, end, timestephours, asTimeArray::Simple);
    timearray.Init();

    wxString predictorDataDir = wxFileName::GetCwd();
    predictorDataDir.Append("/files/");

    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance("NCEP_Reanalysis_v1", "hgt",
                                                                            predictorDataDir);

    predictor->SetFileNamePattern("NCEP_Reanalysis_v1(2014)_hgt_%d.nc");
    predictor->Load(&geoarea, timearray);

    VArray2DFloat hgt = predictor->GetData();
    // hgt[time](lat,lon)

    /* Values time step 0 (horizontal=Lon, vertical=Lat)
    187.0	189.0   |   175.0	171.0
    208.0	206.0   |   195.0	199.0
    */
    EXPECT_FLOAT_EQ(187, hgt[0](0, 0));
    EXPECT_FLOAT_EQ(189, hgt[0](0, 1));
    EXPECT_FLOAT_EQ(175, hgt[0](0, 2));
    EXPECT_FLOAT_EQ(171, hgt[0](0, 3));
    EXPECT_FLOAT_EQ(208, hgt[0](1, 0));
    EXPECT_FLOAT_EQ(199, hgt[0](1, 3));

    /* Values time step 1 (horizontal=Lon, vertical=Lat)
    188.0	188.0	|   170.0	160.0
    202.0	200.0	|   190.0	191.0
    */
    EXPECT_FLOAT_EQ(188, hgt[1](0, 0));
    EXPECT_FLOAT_EQ(188, hgt[1](0, 1));
    EXPECT_FLOAT_EQ(170, hgt[1](0, 2));
    EXPECT_FLOAT_EQ(160, hgt[1](0, 3));
    EXPECT_FLOAT_EQ(202, hgt[1](1, 0));
    EXPECT_FLOAT_EQ(191, hgt[1](1, 3));

    /* Values time step 40 (horizontal=Lon, vertical=Lat)
    45.0	16.0	|   4.0	    -12.0
    53.0	25.0	|   7.0	    -2.0
    */
    EXPECT_FLOAT_EQ(45, hgt[40](0, 0));
    EXPECT_FLOAT_EQ(16, hgt[40](0, 1));
    EXPECT_FLOAT_EQ(4, hgt[40](0, 2));
    EXPECT_FLOAT_EQ(-12, hgt[40](0, 3));
    EXPECT_FLOAT_EQ(53, hgt[40](1, 0));
    EXPECT_FLOAT_EQ(-2, hgt[40](1, 3));

    wxDELETE(predictor);
}

TEST(DataPredictorArchiveNcepR1v2014Regular, LoadCompositeStepLonLatTime)
{
    double Xmin = -10;
    double Xwidth = 15;
    double Ymin = 35;
    double Ywidth = 5;
    double steplon = 5;
    double steplat = 5;
    float level = 1000;
    asGeoAreaCompositeRegularGrid geoarea(Xmin, Xwidth, steplon, Ymin, Ywidth, steplat, level);

    double start = asTime::GetMJD(1960, 1, 1, 00, 00);
    double end = asTime::GetMJD(1960, 1, 11, 00, 00);
    double timestephours = 24;
    asTimeArray timearray(start, end, timestephours, asTimeArray::Simple);
    timearray.Init();

    wxString predictorDataDir = wxFileName::GetCwd();
    predictorDataDir.Append("/files/");

    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance("NCEP_Reanalysis_v1", "hgt",
                                                                            predictorDataDir);

    predictor->SetFileNamePattern("NCEP_Reanalysis_v1(2014)_hgt_%d.nc");
    predictor->Load(&geoarea, timearray);

    VArray2DFloat hgt = predictor->GetData();
    // hgt[time](lat,lon)

    /* Values time step 0 (horizontal=Lon, vertical=Lat)
    187.0	189.0   |   175.0	171.0
    208.0	206.0   |   195.0	199.0
    */
    EXPECT_FLOAT_EQ(187, hgt[0](0, 0));
    EXPECT_FLOAT_EQ(189, hgt[0](0, 1));
    EXPECT_FLOAT_EQ(175, hgt[0](0, 2));
    EXPECT_FLOAT_EQ(171, hgt[0](0, 3));
    EXPECT_FLOAT_EQ(208, hgt[0](1, 0));
    EXPECT_FLOAT_EQ(199, hgt[0](1, 3));

    /* Values time step 10 (horizontal=Lon, vertical=Lat)
    45.0	16.0	|   4.0	    -12.0
    53.0	25.0	|   7.0	    -2.0
    */
    EXPECT_FLOAT_EQ(45, hgt[10](0, 0));
    EXPECT_FLOAT_EQ(16, hgt[10](0, 1));
    EXPECT_FLOAT_EQ(4, hgt[10](0, 2));
    EXPECT_FLOAT_EQ(-12, hgt[10](0, 3));
    EXPECT_FLOAT_EQ(53, hgt[10](1, 0));
    EXPECT_FLOAT_EQ(-2, hgt[10](1, 3));

    wxDELETE(predictor);
}

TEST(DataPredictorArchiveNcepR1v2014Regular, SetData)
{
    double Xmin = -10;
    double Xwidth = 15;
    double Ymin = 35;
    double Ywidth = 5;
    double steplon = 5;
    double steplat = 5;
    float level = 1000;
    asGeoAreaCompositeRegularGrid geoarea(Xmin, Xwidth, steplon, Ymin, Ywidth, steplat, level);

    double start = asTime::GetMJD(1960, 1, 1, 00, 00);
    double end = asTime::GetMJD(1960, 1, 5, 00, 00);
    double timestephours = 24;
    asTimeArray timearray(start, end, timestephours, asTimeArray::Simple);
    timearray.Init();

    wxString predictorDataDir = wxFileName::GetCwd();
    predictorDataDir.Append("/files/");

    asDataPredictorArchive *predictor = asDataPredictorArchive::GetInstance("NCEP_Reanalysis_v1", "hgt",
                                                                            predictorDataDir);

    predictor->SetFileNamePattern("NCEP_Reanalysis_v1(2014)_hgt_%d.nc");
    predictor->Load(&geoarea, timearray);

    Array2DFloat tmp;
    tmp.resize(1, 4);
    VArray2DFloat newdata;

    tmp << 1, 2, 3, 4;
    newdata.push_back(tmp);
    tmp << 11, 12, 13, 14;
    newdata.push_back(tmp);
    tmp << 21, 22, 23, 24;
    newdata.push_back(tmp);
    tmp << 31, 32, 33, 34;
    newdata.push_back(tmp);
    tmp << 41, 42, 43, 44;
    newdata.push_back(tmp);

    predictor->SetData(newdata);

    EXPECT_FLOAT_EQ(1, predictor->GetLatPtsnb());
    EXPECT_FLOAT_EQ(4, predictor->GetLonPtsnb());
    EXPECT_FLOAT_EQ(1, predictor->GetData()[0](0, 0));
    EXPECT_FLOAT_EQ(2, predictor->GetData()[0](0, 1));
    EXPECT_FLOAT_EQ(4, predictor->GetData()[0](0, 3));
    EXPECT_FLOAT_EQ(14, predictor->GetData()[1](0, 3));
    EXPECT_FLOAT_EQ(41, predictor->GetData()[4](0, 0));
    EXPECT_FLOAT_EQ(44, predictor->GetData()[4](0, 3));

    wxDELETE(predictor);
}