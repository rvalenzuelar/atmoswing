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

#ifndef ASTIMEARRAY_H
#define ASTIMEARRAY_H

#include <asIncludes.h>
#include <asTime.h>

class asPredictand;


class asTimeArray
        : public asTime
{
public:

    enum Mode //!< Enumaration of the period selection mode
    {
        SingleDay,      // A single day
        Simple,         // A simple full time array
        SameSeason,        // Into the same season in reference to a date
        SeasonDJF,      // The DJF season
        SeasonMAM,      // The MAM season
        SeasonJJA,      // The JJA season
        SeasonSON,      // The SON season
        DaysInterval,    // + or - an amount of days in reference to a date
        PredictandThresholds, Custom
    };

    asTimeArray(double start, double end, double timestephours, Mode slctmode);

    asTimeArray(double start, double end, double timestephours, const wxString &slctModeString);

    asTimeArray();

    asTimeArray(double date, Mode slctmode);

    explicit asTimeArray(vd &timeArray);

    explicit asTimeArray(a1d &timeArray);

    ~asTimeArray() override = default;

    bool Init();

    bool Init(double forecastdate, double exclusiondays);

    bool Init(double forecastdate, double intervaldays, double exclusiondays);

    bool Init(asPredictand &predictand, const wxString &serieName, int stationId, float minThreshold,
              float maxThreshold);

    bool Init(double forecastdate, double intervaldays, double exclusiondays, asPredictand &predictand,
              const wxString &serieName, int stationId, float minThreshold, float maxThreshold);

    double operator[](unsigned int i)
    {
        wxASSERT(m_initialized);
        wxASSERT(i < (unsigned) GetSize());
        return m_timeArray[i];
    }

    bool BuildArraySimple();

    bool BuildArrayDaysInterval(double forecastDate);

    bool BuildArraySeasons(double forecastDate);

    bool BuildArrayPredictandThresholds(asPredictand &predictand, const wxString &serieName, int stationId,
                                        float minThreshold, float maxThreshold);

    bool HasForbiddenYears() const;

    bool IsYearForbidden(int year) const;

    vi GetForbiddenYears() const
    {
        return m_forbiddenYears;
    }

    void SetForbiddenYears(const vi &years)
    {
        m_forbiddenYears = years;
    }

    bool RemoveYears(const vi &years);

    bool KeepOnlyYears(const vi &years);

    bool IsSimpleMode() const
    {
        return (m_mode == Simple) || (m_mode == SingleDay);
    }

    double GetStart() const
    {
        return m_start;
    }

    int GetStartingYear() const
    {
        return GetYear(m_start);
    }

    int GetStartingMonth() const
    {
        return GetMonth(m_start);
    }

    int GetStartingDay() const
    {
        return GetDay(m_start);
    }

    double GetStartingHour() const
    {
        double fractpart, intpart;
        fractpart = modf(m_start, &intpart);
        return fractpart * 24;
    }

    double GetEnd() const
    {
        return m_end;
    }

    int GetEndingYear() const
    {
        return GetYear(m_end);
    }

    int GetEndingMonth() const
    {
        return GetMonth(m_end);
    }

    double GetEndingHour() const
    {
        double fractpart, intpart;
        fractpart = modf(m_end, &intpart);
        return fractpart * 24;
    }

    double GetTimeStepHours() const
    {
        return m_timeStepDays * 24;
    }

    double GetTimeStepDays() const
    {
        return m_timeStepDays;
    }

    double GetIntervalHours() const
    {
        return m_intervalDays * 24;
    }

    double GetIntervalDays() const
    {
        return m_intervalDays;
    }

    a1d GetTimeArray() const
    {
        return m_timeArray;
    }

    int GetSize() const
    {
        return (int) m_timeArray.size();
    }

    double GetFirst() const
    {
        wxASSERT(m_initialized);
        return m_timeArray(0);
    }

    double GetLast() const
    {
        wxASSERT(m_initialized);
        return m_timeArray(m_timeArray.rows() - 1);
    }

    int GetClosestIndex(double date) const;

    int GetIndexFirstAfter(double date, double dataTimeStep) const;

    int GetIndexFirstBefore(double date, double dataTimeStep) const;

protected:

private:
    bool m_initialized;
    Mode m_mode;
    a1d m_timeArray;
    double m_start;
    double m_end;
    double m_timeStepDays;
    double m_intervalDays;
    double m_exclusionDays;
    vi m_forbiddenYears;

};

#endif
