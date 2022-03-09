// Copyright 2004 "Gilles Degottex"

// This file is part of "fmit"

// "fmit" is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// "fmit" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <cmath>
#include "IIRLowPass.h"

IIRLowPass::IIRLowPass(int order, double fC, double dt)
{
    setFC(fC);
    m_memory.resize(order);
    resetMemory();
    m_dt = dt;
}

void IIRLowPass::resetMemory()
{
    for (std::size_t i = 0; i < m_memory.size(); i++) {
        m_memory[i] = 0;
    }
}

void IIRLowPass::setFC(double fC)
{
    m_RC = 1 / (2 * M_PI * fC);
}

double IIRLowPass::step(double in)
{
    double alpha = m_dt / (m_RC + m_dt);
    for (std::size_t i = 0; i < m_memory.size(); i++)
    {
        if (i == 0)
            m_memory[0] = alpha * in + (1 - alpha) * m_memory[0];
        else
            m_memory[i] = alpha * m_memory[i - 1] + (1 - alpha) * m_memory[i];
    }
    return m_memory[m_memory.size() - 1];
}
