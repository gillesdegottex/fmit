// Copyright 2007 "Gilles Degottex"

// This file is part of "CppAddons"

// "CppAddons" is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "CppAddons" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef _Fit_h_
#define _Fit_h_

namespace Math
{
    inline void FitParabola(double x1, double y1, double x2, double y2, double x3, double y3,
                        double& a, double& b, double& c, double& xapex, double& yapex)
    {
        assert(x1!=x2 && x1!=x3 && x2!=x3);

        double h31 = (y3-y1)/(x3-x1);
        double h21 = (y2-y1)/(x2-x1);

        a = (h31-h21)/(x3-x2);
        b = h21 - a*(x2+x1);
        c = y1 - a*x1*x1 - b*x1;

        xapex = -b/2/a;
        yapex = c - b*b/4/a;
    }
}

#endif
