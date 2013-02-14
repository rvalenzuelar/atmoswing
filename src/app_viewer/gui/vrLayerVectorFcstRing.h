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
 
#ifndef _VRLAYERVECTORSFCSTRING_H_
#define _VRLAYERVECTORSFCSTRING_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// Include wxWidgets' headers
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


#include "asIncludes.h"
#include "vrlayervector.h"

class vrRender;
class vrLabel;

//For dealing with GIS data stored into Fcst projects.
class vrLayerVectorFcstRing : public vrLayerVectorOGR
{
public:
    vrLayerVectorFcstRing();
    virtual ~vrLayerVectorFcstRing();

    virtual long AddFeature(OGRGeometry * geometry, void * data = NULL);

    void SetMaxValue(double val)
    {
        if (val<0.1)
        {
            asLogWarning(_("The given maximum value for the vrLayerVectorFcstRing class was too small, so it has been increased."));
            val = 0.1;
        }
        m_ValueMax = val;
    }

    double GetMaxValue()
    {
        return m_ValueMax;
    }

protected:
    double m_ValueMax;

    virtual bool _DrawPoints(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
                             const vrRender * render, vrLabel * label, double pxsize);

    virtual bool _DrawLines(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
                            const vrRender * render, const vrLabel * label, double pxsize);

    virtual bool _DrawPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
                               const vrRender * render, const vrLabel * label, double pxsize);

    virtual bool _DrawMultiPolygons(wxGraphicsContext * gdc, const wxRect2DDouble & coord,
                               const vrRender * render, const vrLabel * label, double pxsize);

    void _CreatePath(wxGraphicsPath & path, const wxPoint & center, int segmentsTotNb, int segmentNb);

    void _Paint(wxGraphicsContext * gdc, wxGraphicsPath & path, double value);
};
#endif

