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
 
#ifndef ASPREDICTORCRITERIAS1weights_H
#define ASPREDICTORCRITERIAS1weights_H

#include <asIncludes.h>
#include <asPredictorCriteria.h>

class asPredictorCriteriaS1weights: public asPredictorCriteria
{
public:

    /** Default constructor
     * \param criteria The chosen criteria
     */
    asPredictorCriteriaS1weights(int linAlgebraMethod=asLIN_ALGEBRA_NOVAR);

    /** Default destructor */
    ~asPredictorCriteriaS1weights();

    /** Process the Criteria
     * \param refData The target day
     * \param evalData The day to assess
     * \return The Criteria
     */
    float Assess(const Array2DFloat &refData, const Array2DFloat &evalData, int rowsNb=0, int colsNb=0);


protected:

private:
    float m_wU;
    float m_wV;

};

#endif
