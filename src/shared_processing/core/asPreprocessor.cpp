/** 
 *
 *  This file is part of the AtmoSwing software.
 *
 *  Copyright (c) 2008-2012  University of Lausanne, Pascal Horton (pascal.horton@unil.ch). 
 *  All rights reserved.
 *
 *  THIS CODE, SOFTWARE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY  
 *  OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 */
 
#include "asPreprocessor.h"

#include <asDataPredictor.h>
#include <asDataPredictorArchive.h>
#ifndef MINIMAL_LINKS
    #include <asDataPredictorRealtime.h>
#endif
#include <asThreadPreprocessorGradients.h>


bool asPreprocessor::Preprocess(std::vector < asDataPredictorArchive > predictors, const wxString& method, asDataPredictor *result)
{
    std::vector < asDataPredictor* > ptorsPredictors;

    for (unsigned int i=0; i<predictors.size(); i++)
    {
        ptorsPredictors.push_back(&predictors[i]);
    }

    return Preprocess(ptorsPredictors, method, result);
}

#ifndef MINIMAL_LINKS
bool asPreprocessor::Preprocess(std::vector < asDataPredictorRealtime > predictors, const wxString& method, asDataPredictor *result)
{
    std::vector < asDataPredictor* > ptorsPredictors;

    for (unsigned int i=0; i<predictors.size(); i++)
    {
        ptorsPredictors.push_back(&predictors[i]);
    }

    return Preprocess(ptorsPredictors, method, result);
}
#endif

bool asPreprocessor::Preprocess(std::vector < asDataPredictor* > predictors, const wxString& method, asDataPredictor *result)
{
    wxASSERT(result);

    result->SetPreprocessMethod(method);

    if (method.IsSameAs("Gradients"))
    {
        return PreprocessGradients(predictors, result);
    }
    else if (method.IsSameAs("Difference"))
    {
        return PreprocessDifference(predictors, result);
    }
    else if (method.IsSameAs("Multiplication"))
    {
        return PreprocessMultiplication(predictors, result);
    }
    else if (method.IsSameAs("MergeCouplesAndMultiply"))
    {
        return PreprocessMergeCouplesAndMultiply(predictors, result);
    }
    else if (method.IsSameAs("MergeByHalfAndMultiply"))
    {
        return PreprocessMergeByHalfAndMultiply(predictors, result);
    }
    else if (method.IsSameAs("HumidityFlux"))
    {
        return PreprocessHumidityFlux(predictors, result);
    }
    else if (method.IsSameAs("WindSpeed"))
    {
        return PreprocessWindSpeed(predictors, result);
    }
    else
    {
        asLogError(_("The preprocessing method was not correctly defined."));
        return false;
    }
}

bool asPreprocessor::PreprocessGradients(std::vector < asDataPredictor* > predictors, asDataPredictor *result)
{
    // Get the processing method
    ThreadsManager().CritSectionConfig().Enter();
    wxConfigBase *pConfig = wxFileConfig::Get();
    bool allowMultithreading;
    pConfig->Read("/Standard/AllowMultithreading", &allowMultithreading, false);
    ThreadsManager().CritSectionConfig().Leave();

    // Only one predictor
    wxASSERT(predictors.size()>0);
    if(predictors.size()>1) asThrowException(_("The number of predictors cannot be superior to 1 in asPreprocessor::PreprocessGradients"));

    // Get sizes
    wxASSERT(predictors[0]);
    int rowsNb = predictors[0]->GetLatPtsnb();
    int colsNb = predictors[0]->GetLonPtsnb();
    int timeSize = predictors[0]->GetSizeTime();

    wxASSERT(rowsNb>1);
    wxASSERT(colsNb>1);
    wxASSERT(timeSize>0);

    // Create container
    VArray2DFloat gradients(timeSize);
//    gradients.reserve(timeSize*tmpgrad.rows()*tmpgrad.cols());

// FIXME (Pascal#1#): Error under Linux
/*
    if (allowMultithreading)
    {
        // Get threads number
        int threadsNb = ThreadsManager().GetAvailableThreadsNb();

        int threadType = -1;

        // Adapt to the number of targets
        if (2*threadsNb>timeSize)
        {
            threadsNb = 1;
        }

        // Create and give data
        int start = 0, end = -1;
        for (int i_threads=0; i_threads<threadsNb; i_threads++)
        {
            start = end+1;
            end = ceil(((float)(i_threads+1)*(float)(timeSize-1)/(float)threadsNb));
            wxASSERT_MSG(end>=start, wxString::Format(_("start = %d, end = %d, timeSize = %d"), start, end, timeSize));

            asThreadPreprocessorGradients* thread = new asThreadPreprocessorGradients(&gradients, &predictors, start, end);
            threadType = thread->GetType();
            ThreadsManager().AddThread(thread);
        }

        // Wait until all done
        ThreadsManager().Wait(threadType);
    }
    else
    {*/
        Array1DFloat tmpgrad = Array1DFloat::Constant((rowsNb-1)*colsNb+rowsNb*(colsNb-1), NaNFloat);

        for (int i_time=0; i_time<timeSize; i_time++)
        {
            int counter=0;

            // Vertical gradients
            for (int i_row=0; i_row<rowsNb-1; i_row++)
            {
                for (int i_col=0; i_col<colsNb; i_col++)
                {
                    tmpgrad(counter) = predictors[0]->GetData()[i_time](i_row+1,i_col)-predictors[0]->GetData()[i_time](i_row,i_col);
                    counter++;
                }
            }

            // Horizontal gradients
            for (int i_row=0; i_row<rowsNb; i_row++)
            {
                for (int i_col=0; i_col<colsNb-1; i_col++)
                {
                    // The matrix is transposed to be coherent with the dimensions
                    tmpgrad(counter) = predictors[0]->GetData()[i_time](i_row,i_col+1)-predictors[0]->GetData()[i_time](i_row,i_col);
                    counter++;
                }
            }

            //gradients.push_back(tmpgrad.transpose());
            gradients[i_time] = tmpgrad.transpose();
        }
    //}

    // Overwrite the data in the predictor object
    result->SetData(gradients);
    result->SetIsPreprocessed(true);
    result->SetCanBeClipped(false);

    return true;
}

bool asPreprocessor::PreprocessDifference(std::vector < asDataPredictor* > predictors, asDataPredictor *result)
{
    // More than one predictor
    if(predictors.size()!=2) asThrowException(_("The number of predictors must be equal to 2 in asPreprocessor::PreprocessDifference"));

    // Get sizes
    wxASSERT(predictors[0]);
    wxASSERT(predictors[1]);
    int rowsNb = predictors[0]->GetLatPtsnb();
    int colsNb = predictors[0]->GetLonPtsnb();
    int timeSize = predictors[0]->GetSizeTime();

    wxASSERT(rowsNb>1);
    wxASSERT(colsNb>1);
    wxASSERT(timeSize>0);

    // Create container
    Array2DFloat tmpdiff = Array2DFloat::Constant(rowsNb, colsNb, 1);
    VArray2DFloat resdiff;
    resdiff.reserve(timeSize*rowsNb*colsNb);

    for (int i_time=0; i_time<timeSize; i_time++)
    {
        tmpdiff.fill(1);

        for (int i_row=0; i_row<rowsNb; i_row++)
        {
            for (int i_col=0; i_col<colsNb; i_col++)
            {
                tmpdiff(i_row, i_col) = predictors[0]->GetData()[i_time](i_row,i_col)-predictors[1]->GetData()[i_time](i_row,i_col);
            }
        }

        resdiff.push_back(tmpdiff);
    }

    // Overwrite the data in the predictor object
    result->SetData(resdiff);
    result->SetIsPreprocessed(true);
    result->SetCanBeClipped(true);

    return true;
}

bool asPreprocessor::PreprocessMultiplication(std::vector < asDataPredictor* > predictors, asDataPredictor *result)
{
    // More than one predictor
    if(predictors.size()<2) asThrowException(_("The number of predictors must be superior to 2 in asPreprocessor::PreprocessMultiplication"));

    // Get sizes
    wxASSERT(predictors[0]);
    int rowsNb = predictors[0]->GetLatPtsnb();
    int colsNb = predictors[0]->GetLonPtsnb();
    int timeSize = predictors[0]->GetSizeTime();

    wxASSERT(rowsNb>1);
    wxASSERT(colsNb>1);
    wxASSERT(timeSize>0);

    // Create container
    Array2DFloat tmpmulti = Array2DFloat::Constant(rowsNb, colsNb, 1);
    VArray2DFloat multi;
    multi.reserve(timeSize*rowsNb*colsNb);

    for (int i_time=0; i_time<timeSize; i_time++)
    {
        tmpmulti.fill(1);

        for (int i_row=0; i_row<rowsNb; i_row++)
        {
            for (int i_col=0; i_col<colsNb; i_col++)
            {
                for (unsigned int i_dat=0; i_dat<predictors.size(); i_dat++)
                {
                    wxASSERT(predictors[i_dat]);
                    tmpmulti(i_row, i_col) *= predictors[i_dat]->GetData()[i_time](i_row,i_col);
                }
            }
        }

        multi.push_back(tmpmulti);
    }

    // Overwrite the data in the predictor object
    result->SetData(multi);
    result->SetIsPreprocessed(true);
    result->SetCanBeClipped(true);

    return true;
}

bool asPreprocessor::PreprocessMergeCouplesAndMultiply(std::vector < asDataPredictor* > predictors, asDataPredictor *result)
{
    // More than one predictor
    int inputSize = predictors.size();
    if(inputSize<2) asThrowException(_("The number of predictors must be superior to 2 in asPreprocessor::PreprocessMergeCouplesAndMultiply"));
    if(!(fmod((float)inputSize,(float)2)==0)) asThrowException(_("The number of predictors must be a pair in asPreprocessor::PreprocessMergeCouplesAndMultiply"));

    // Merge
    wxASSERT(predictors[0]);
    VVArray2DFloat copyData = VVArray2DFloat(inputSize/2);
    copyData.reserve(2*predictors[0]->GetLatPtsnb()*predictors[0]->GetLonPtsnb()*predictors[0]->GetSizeTime()*inputSize);
    int counter = 0;
    int prevTimeSize = 0;

    for(unsigned int i_dat=0; i_dat<predictors.size(); i_dat+=2)
    {
        wxASSERT(predictors[i_dat]);
        wxASSERT(predictors[i_dat+1]);

        // Get sizes
        int rowsNb1 = predictors[i_dat]->GetLatPtsnb();
        int colsNb1 = predictors[i_dat]->GetLonPtsnb();
        int rowsNb2 = predictors[i_dat+1]->GetLatPtsnb();
        int colsNb2 = predictors[i_dat+1]->GetLonPtsnb();
        int timeSize = predictors[i_dat]->GetSizeTime();

        if (i_dat>0)
        {
            wxASSERT(prevTimeSize==timeSize);
        }
        prevTimeSize = timeSize;

        wxASSERT(rowsNb1>0);
        wxASSERT(colsNb1>0);
        wxASSERT(rowsNb2>0);
        wxASSERT(colsNb2>0);
        wxASSERT(timeSize>0);

        bool putBelow;
        int rowsNew, colsNew;
        if(colsNb1==colsNb2)
        {
            colsNew = colsNb1;
            rowsNew = rowsNb1+rowsNb2;
            putBelow = true;
        }
        else if(rowsNb1==rowsNb2)
        {
            rowsNew = rowsNb1;
            colsNew = colsNb1+colsNb2;
            putBelow = false;
        }
        else
        {
            asThrowException(_("The predictors sizes make them impossible to merge."));
        }

        Array2DFloat tmp(rowsNew,colsNew);

        for(int i_time=0; i_time<timeSize; i_time++)
        {
            tmp.topLeftCorner(rowsNb1,colsNb1) = predictors[i_dat]->GetData()[i_time];

            if(putBelow)
            {
                tmp.block(rowsNb1,0,rowsNb2,colsNb2) = predictors[i_dat+1]->GetData()[i_time];
            }
            else
            {
                tmp.block(0,colsNb1,rowsNb2,colsNb2) = predictors[i_dat+1]->GetData()[i_time];
            }

            copyData[counter].push_back(tmp);
        }

        counter++;
    }

    // Get sizes
    int rowsNb = copyData[0][0].rows();
    int colsNb = copyData[0][0].cols();
    int timeSize = copyData[0].size();

    wxASSERT(rowsNb>0);
    wxASSERT(colsNb>0);
    wxASSERT(timeSize>0);

    // Create container
    Array2DFloat tmpmulti = Array2DFloat::Constant(rowsNb, colsNb, 1);
    VArray2DFloat multi;
    multi.reserve(timeSize*rowsNb*colsNb);

    for (int i_time=0; i_time<timeSize; i_time++)
    {
        tmpmulti.fill(1);

        for (int i_row=0; i_row<rowsNb; i_row++)
        {
            for (int i_col=0; i_col<colsNb; i_col++)
            {
                for (unsigned int i_dat=0; i_dat<copyData.size(); i_dat++)
                {
                    tmpmulti(i_row, i_col) *= copyData[i_dat][i_time](i_row,i_col);
                }
            }
        }

        multi.push_back(tmpmulti);
    }

    // Overwrite the data in the predictor object
    result->SetData(multi);
    result->SetIsPreprocessed(true);
    result->SetCanBeClipped(true);

    return true;
}

bool asPreprocessor::PreprocessMergeByHalfAndMultiply(std::vector < asDataPredictor* > predictors, asDataPredictor *result)
{
    // More than one predictor
    int inputSize = predictors.size();
    int factorSize = inputSize/2;
    if(inputSize<2) asThrowException(_("The number of predictors must be superior to 2 in asPreprocessor::PreprocessMergeByHalfAndMultiply"));
    if(!(fmod((float)inputSize,(float)2)==0)) asThrowException(_("The number of predictors must be a pair in asPreprocessor::PreprocessMergeByHalfAndMultiply"));

    // Handle sizes
    wxASSERT(predictors[0]);
    int originalRowsNb = predictors[0]->GetLatPtsnb();
    int originalColsNb = predictors[0]->GetLonPtsnb();
    int timeSize = predictors[0]->GetSizeTime();
    wxASSERT(originalRowsNb>0);
    wxASSERT(originalColsNb>0);
    wxASSERT(timeSize>0);

    int newRowsNb = originalRowsNb*factorSize;
    int newColsNb = originalColsNb;

    // Initialize
    wxASSERT(predictors[0]);
    VVArray2DFloat copyData = VVArray2DFloat(2);
    copyData.reserve(newRowsNb*newColsNb*timeSize*2);

    // Merge
    for(unsigned int i_half=0; i_half<2; i_half++)
    {
        Array2DFloat tmp(newRowsNb,newColsNb);

        for(int i_time=0; i_time<timeSize; i_time++)
        {
            for(int i_dat=0; i_dat<inputSize/2; i_dat++)
            {
                int i_curr = i_half*inputSize/2+i_dat;
                wxASSERT(predictors[i_curr]);
                wxASSERT(predictors[i_curr]->GetLatPtsnb()==originalRowsNb);
                wxASSERT(predictors[i_curr]->GetLonPtsnb()==originalColsNb);
                wxASSERT(predictors[i_curr]->GetSizeTime()==timeSize);

                tmp.block(i_dat*originalRowsNb,0,originalRowsNb,originalColsNb) = predictors[i_curr]->GetData()[i_time];
            }
            copyData[i_half].push_back(tmp);
        }
    }

    wxASSERT(copyData.size()==2);

    // Create container
    Array2DFloat tmpmulti = Array2DFloat::Zero(newRowsNb, newColsNb);
    VArray2DFloat multi;
    multi.reserve(timeSize*newRowsNb*newColsNb);

    for (int i_time=0; i_time<timeSize; i_time++)
    {
        for (int i_row=0; i_row<newRowsNb; i_row++)
        {
            for (int i_col=0; i_col<newColsNb; i_col++)
            {
                tmpmulti(i_row, i_col) = copyData[0][i_time](i_row,i_col)*copyData[1][i_time](i_row,i_col);
            }
        }

        multi.push_back(tmpmulti);
    }

    wxASSERT(multi.size()==timeSize);

    // Overwrite the data in the predictor object
    result->SetData(multi);
    result->SetIsPreprocessed(true);
    result->SetCanBeClipped(false);

    return true;
}

bool asPreprocessor::PreprocessHumidityFlux(std::vector < asDataPredictor* > predictors, asDataPredictor *result)
{
    // More than one predictor
    int inputSize = predictors.size();
    if(inputSize!=8 && inputSize!=6) asThrowException(_("The number of predictors must be equal to 6 or 8 in asPreprocessor::PreprocessHumidityFlux"));

    // Merge
    wxASSERT(predictors[0]);
    VVArray2DFloat copyData = VVArray2DFloat(inputSize/2);
    copyData.reserve(2*predictors[0]->GetLatPtsnb()*predictors[0]->GetLonPtsnb()*predictors[0]->GetSizeTime()*inputSize);
    int counter = 0;
    int prevTimeSize = 0;

    for(unsigned int i_dat=0; i_dat<predictors.size(); i_dat+=2)
    {
        wxASSERT(predictors[i_dat]);
        wxASSERT(predictors[i_dat+1]);

        // Get sizes
        int rowsNb1 = predictors[i_dat]->GetLatPtsnb();
        int colsNb1 = predictors[i_dat]->GetLonPtsnb();
        int rowsNb2 = predictors[i_dat+1]->GetLatPtsnb();
        int colsNb2 = predictors[i_dat+1]->GetLonPtsnb();
        int timeSize = predictors[i_dat]->GetSizeTime();

        if (i_dat>0)
        {
            wxASSERT(prevTimeSize==timeSize);
        }
        prevTimeSize = timeSize;

        wxASSERT(rowsNb1>0);
        wxASSERT(colsNb1>0);
        wxASSERT(rowsNb2>0);
        wxASSERT(colsNb2>0);
        wxASSERT(timeSize>0);

        bool putBelow;
        int rowsNew, colsNew;
        if(colsNb1==colsNb2)
        {
            colsNew = colsNb1;
            rowsNew = rowsNb1+rowsNb2;
            putBelow = true;
        }
        else if(rowsNb1==rowsNb2)
        {
            rowsNew = rowsNb1;
            colsNew = colsNb1+colsNb2;
            putBelow = false;
        }
        else
        {
            asThrowException(_("The predictors sizes make them impossible to merge."));
        }

        Array2DFloat tmp(rowsNew,colsNew);

        for(int i_time=0; i_time<timeSize; i_time++)
        {
            tmp.topLeftCorner(rowsNb1,colsNb1) = predictors[i_dat]->GetData()[i_time];

            if(putBelow)
            {
                tmp.block(rowsNb1,0,rowsNb2,colsNb2) = predictors[i_dat+1]->GetData()[i_time];
            }
            else
            {
                tmp.block(0,colsNb1,rowsNb2,colsNb2) = predictors[i_dat+1]->GetData()[i_time];
            }

            copyData[counter].push_back(tmp);
        }

        counter++;
    }

    // Get sizes
    int rowsNb = copyData[0][0].rows();
    int colsNb = copyData[0][0].cols();
    int timeSize = copyData[0].size();

    wxASSERT(rowsNb>0);
    wxASSERT(colsNb>0);
    wxASSERT(timeSize>0);

    // Create container
    Array2DFloat tmpmulti = Array2DFloat::Constant(rowsNb, colsNb, 1);
    VArray2DFloat multi;
    multi.reserve(timeSize*rowsNb*colsNb);

    float wind = NaNFloat;

    for (int i_time=0; i_time<timeSize; i_time++)
    {
        tmpmulti.fill(1);

        for (int i_row=0; i_row<rowsNb; i_row++)
        {
            for (int i_col=0; i_col<colsNb; i_col++)
            {
                // Get wind value
                wind = sqrt(copyData[0][i_time](i_row,i_col)*copyData[0][i_time](i_row,i_col) + copyData[1][i_time](i_row,i_col)*copyData[1][i_time](i_row,i_col));
                tmpmulti(i_row, i_col) = wind;

                for (unsigned int i_dat=2; i_dat<copyData.size(); i_dat++)
                {
                    tmpmulti(i_row, i_col) *= copyData[i_dat][i_time](i_row,i_col);
                }
            }
        }

        multi.push_back(tmpmulti);
    }

    // Overwrite the data in the predictor object
    result->SetData(multi);
    result->SetIsPreprocessed(true);
    result->SetCanBeClipped(true);

    return true;
}

bool asPreprocessor::PreprocessWindSpeed(std::vector < asDataPredictor* > predictors, asDataPredictor *result)
{
    // More than one predictor
    int inputSize = predictors.size();
    if(inputSize!=2) asThrowException(_("The number of predictors must be equal to 2 in asPreprocessor::PreprocessWindSpeed"));

    // Get sizes
    wxASSERT(predictors[0]);
    wxASSERT(predictors[1]);
    int rowsNb = predictors[0]->GetLatPtsnb();
    int colsNb = predictors[0]->GetLonPtsnb();
    int timeSize = predictors[0]->GetSizeTime();
    wxASSERT(rowsNb>0);
    wxASSERT(colsNb>0);
    wxASSERT(timeSize>0);

    // Create container
    Array2DFloat tmpmulti = Array2DFloat::Constant(rowsNb, colsNb, 1);
    VArray2DFloat multi;
    multi.reserve(timeSize*rowsNb*colsNb);

    float wind = NaNFloat;

    for (int i_time=0; i_time<timeSize; i_time++)
    {
        tmpmulti.fill(1);

        for (int i_row=0; i_row<rowsNb; i_row++)
        {
            for (int i_col=0; i_col<colsNb; i_col++)
            {
                // Get wind value
                wind = sqrt(predictors[0]->GetData()[i_time](i_row,i_col)*predictors[0]->GetData()[i_time](i_row,i_col) + predictors[1]->GetData()[i_time](i_row,i_col)*predictors[1]->GetData()[i_time](i_row,i_col));
                tmpmulti(i_row, i_col) = wind;
            }
        }
        multi.push_back(tmpmulti);
    }

    // Overwrite the data in the predictor object
    result->SetData(multi);
    result->SetIsPreprocessed(true);
    result->SetCanBeClipped(true);

    return true;
}
