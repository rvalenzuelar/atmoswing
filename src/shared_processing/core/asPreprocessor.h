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
 * Portions Copyright 2013-2014 Pascal Horton, Terranum.
 */

#ifndef ASPREPROCESSOR_H
#define ASPREPROCESSOR_H

#include <asIncludes.h>

class asPredictor;


class asPreprocessor
        : public wxObject
{
public:

    static bool Preprocess(std::vector<asPredictor *> predictors, const wxString &method, asPredictor *result);

    static bool PreprocessGradients(std::vector<asPredictor *> predictors, asPredictor *result);

    static bool PreprocessAddition(std::vector<asPredictor *> predictors, asPredictor *result);

    static bool PreprocessAverage(std::vector<asPredictor *> predictors, asPredictor *result);

    static bool PreprocessDifference(std::vector<asPredictor *> predictors, asPredictor *result);

    static bool PreprocessMultiplication(std::vector<asPredictor *> predictors, asPredictor *result);

    static bool PreprocessFormerHumidityIndex(std::vector<asPredictor *> predictors, asPredictor *result);

    static bool PreprocessMergeByHalfAndMultiply(std::vector<asPredictor *> predictors, asPredictor *result);

    static bool PreprocessHumidityFlux(std::vector<asPredictor *> predictors, asPredictor *result);

    static bool PreprocessWindSpeed(std::vector<asPredictor *> predictors, asPredictor *result);

protected:

private:
};

#endif // ASPREPROCESSOR_H
