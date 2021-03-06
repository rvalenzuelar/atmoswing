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
 */

#include "asFileAscii.h"

asFileAscii::asFileAscii(const wxString &fileName, const FileMode &fileMode)
        : asFile(fileName, fileMode)
{

}

bool asFileAscii::Open()
{
    if (!Find())
        return false;

    switch (m_fileMode) {
        case (ReadOnly):
            m_file.open(m_fileName.GetFullPath().mb_str(), std::fstream::in);
            break;

        case (Write):
            m_file.open(m_fileName.GetFullPath().mb_str(), std::fstream::out);
            break;

        case (Replace):
            m_file.open(m_fileName.GetFullPath().mb_str(), std::fstream::trunc | std::fstream::out);
            break;

        case (New):
            m_file.open(m_fileName.GetFullPath().mb_str(), std::fstream::out);
            break;

        case (Append):
            m_file.open(m_fileName.GetFullPath().mb_str(), std::fstream::app | std::fstream::out);
            break;
    }

    if (!m_file.is_open())
        return false;

    m_opened = true;

    return true;
}

bool asFileAscii::Close()
{
    wxASSERT(m_opened);

    m_file.close();
    return true;
}

void asFileAscii::AddLineContent(const wxString &lineContent)
{
    wxASSERT(m_opened);

    wxString lineContentCopy = lineContent;
    lineContentCopy.Append("\n");

    m_file << lineContentCopy.mb_str();

    // Check the state flags
    if (m_file.fail())
        asThrowException(wxString::Format(_("An error occured while trying to write in file %s"),
                                          m_fileName.GetFullPath()));
}

const wxString asFileAscii::GetLineContent()
{
    wxASSERT(m_opened);

    std::string tmpLineContent;

    if (!m_file.eof()) {
        getline(m_file, tmpLineContent);

        // Check the state flags
        if ((!m_file.eof()) && (m_file.fail()))
            asThrowException(wxString::Format(_("An error occured while trying to write in file %s"),
                                              m_fileName.GetFullPath()));
    } else {
        asThrowException(wxString::Format(_("You are trying to read a line after the end of the file %s"),
                                          m_fileName.GetFullPath()));
    }

    wxString lineContent = wxString(tmpLineContent.c_str(), wxConvUTF8);

    return lineContent;
}

const wxString asFileAscii::GetFullContent()
{
    wxASSERT(m_opened);

    std::string tmpContent;
    std::string apptmpContent;

    while (!m_file.eof()) {
        getline(m_file, tmpContent);
        apptmpContent.append(tmpContent);

        // Check the state flags
        if ((!m_file.eof()) && (m_file.fail()))
            asThrowException(wxString::Format(_("An error occured while trying to read in file %s"),
                                              m_fileName.GetFullPath()));
    }

    return wxString(apptmpContent.c_str(), wxConvUTF8);
}

const wxString asFileAscii::GetFullContentWhithoutReturns()
{
    wxASSERT(m_opened);

    std::string tmpContent;
    std::string apptmpContent;

    while (!m_file.eof()) {
        getline(m_file, tmpContent);
        apptmpContent.append(tmpContent);
    }

    // Check the state flags
    if ((!m_file.eof()) && (m_file.fail()))
        asThrowException(wxString::Format(_("An error occured while trying to read in file %s"),
                                          m_fileName.GetFullPath()));

    return wxString(apptmpContent.c_str(), wxConvUTF8);
}

int asFileAscii::GetInt()
{
    wxASSERT(m_opened);

    int tmp;
    m_file >> tmp;
    return tmp;
}

float asFileAscii::GetFloat()
{
    wxASSERT(m_opened);

    float tmp;
    m_file >> tmp;
    return tmp;
}

double asFileAscii::GetDouble()
{
    wxASSERT(m_opened);

    double tmp;
    m_file >> tmp;
    return tmp;
}

bool asFileAscii::SkipLines(int linesNb)
{
    wxASSERT(m_opened);

    for (int iLine = 0; iLine < linesNb; iLine++) {
        if (!m_file.eof()) {
            GetLineContent();
        } else {
            wxLogError(_("Reached the end of the file while skipping lines."));
            return false;
        }
    }

    return true;
}

bool asFileAscii::SkipElements(int elementNb)
{
    wxASSERT(m_opened);

    float tmp;

    for (int iEl = 0; iEl < elementNb; iEl++) {
        if (!m_file.eof()) {
            m_file >> tmp;
        } else {
            wxLogError(_("Reached the end of the file while skipping lines."));
            return false;
        }
    }

    return true;
}

bool asFileAscii::EndOfFile() const
{
    wxASSERT(m_opened);

    return m_file.eof();
}
