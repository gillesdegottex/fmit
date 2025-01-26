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

#include <vector>

// implements a stack of <order> RC-filters
class IIRLowPass
{
private:
    std::vector<double> m_memory;
    double m_RC;
    double m_dt;

public:
    IIRLowPass(int order = 4, double fC = 10, double dt = 0.1);

    double step(double in);
    void setFC(double fC);
    void setDt(double dt) {m_dt = dt;}
    void resetMemory();
};