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
 * Portions Copyright 2014-2015 Pascal Horton, Terranum.
 */

#ifndef ASTOTALSCORERANKHISTOGRAM_H
#define ASTOTALSCORERANKHISTOGRAM_H

#include <asIncludes.h>
#include <asTotalScore.h>

class asTotalScoreRankHistogram
        : public asTotalScore
{
public:
    asTotalScoreRankHistogram(Period period);

    asTotalScoreRankHistogram(const wxString &periodString);

    virtual ~asTotalScoreRankHistogram();

    float Assess(const a1f &targetDates, const a1f &scores, const asTimeArray &timeArray) const;

    a1f AssessOnArray(const a1f &targetDates, const a1f &scores, const asTimeArray &timeArray) const;

protected:

private:

};

#endif // ASTOTALSCORERANKHISTOGRAM_H