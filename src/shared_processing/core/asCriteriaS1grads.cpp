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

#include "asCriteriaS1grads.h"

asCriteriaS1grads::asCriteriaS1grads()
        : asCriteria()
{
    m_criteria = asCriteria::S1grads;
    m_name = "S1grads";
    m_fullName = _("Teweles-Wobus on gradients");
    m_order = Asc;
    m_scaleBest = 0;
    m_scaleWorst = 200;
    m_canUseInline = true;
}

asCriteriaS1grads::~asCriteriaS1grads()
{
    //dtor
}

float asCriteriaS1grads::Assess(const a2f &refData, const a2f &evalData, int rowsNb, int colsNb) const
{
    wxASSERT_MSG(refData.rows() == evalData.rows(),
                 wxString::Format("refData.rows()=%d, evalData.rows()=%d", (int) refData.rows(),
                                  (int) evalData.rows()));
    wxASSERT_MSG(refData.cols() == evalData.cols(),
                 wxString::Format("refData.cols()=%d, evalData.cols()=%d", (int) refData.cols(),
                                  (int) evalData.cols()));
    wxASSERT_MSG(refData.rows() > 0, wxString::Format("refData.rows()=%d", (int) refData.rows()));
    wxASSERT_MSG(refData.cols() > 0, wxString::Format("refData.cols()=%d", (int) refData.cols()));

#ifdef _DEBUG
    if (refData.rows() < 1)
        asThrowException(_("The number of rows of the data is null in the S1grads criteria processing."));
    if (refData.cols() < 1)
        asThrowException(_("The number of cols of the data is null in the S1grads criteria processing."));
#endif

    float dividend = 0, divisor = 0;

    // Note here that the actual gradient data do not fill the entire data blocks,
    // but the rest being 0-filled, we can simplify the sum calculation !

    dividend = ((refData - evalData).abs()).sum();
    divisor = (refData.abs().max(evalData.abs())).sum();

    if (divisor > 0) {
        return 100.0f * (dividend / divisor); // Can be NaN
    } else {
        if (dividend == 0) {
            wxLogWarning(_("Both dividend and divisor are equal to zero in the predictor criteria."));
            return m_scaleBest;
        } else {
            return NaNf;
        }
    }

}