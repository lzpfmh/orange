/*
    This file is part of Orange.

    Orange is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Orange is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Orange; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Authors: Janez Demsar, Blaz Zupan, 1996--2002
    Contact: janez.demsar@fri.uni-lj.si
*/


#ifndef __BASSTAT_HPP
#define __BASSTAT_HPP

#include "root.hpp"
#include <vector>
using namespace std;

// Minimal, maximal, average value of attribute, and deviation
class TBasicAttrStat : public TOrange {
public:
  __REGISTER_CLASS

  float sum; //PR sum of values
  float sum2; //PR sum of squares of values
  float n; //PR number of examples for which the attribute is defined

  float min; //P the lowest value of the attribute
  float max; //P the highest value of the attribute
  float avg; //P the average value of the attribute
  float dev; //P the deviation of the value of the attribute
  PVariable variable; //P the attribute to which the data applies
  bool holdRecomputation; //P temporarily disables recomputation of avg and dev while adding values

  TBasicAttrStat(PVariable var, const bool &ahold=false);

  void add(float f, float p=1);
  void recompute();
};

WRAPPER(BasicAttrStat);
WRAPPER(ExampleGenerator);

// Minimal, maximal, average value of attribute, and deviation for all attributes from the generator
//VECTOR: BasicAttrStatList (of PBasicAttrStat)
class TDomainBasicAttrStat : public TOrangeVector<PBasicAttrStat> {
public:
  __REGISTER_CLASS

  TDomainBasicAttrStat();
  TDomainBasicAttrStat(PExampleGenerator gen, const long &weightID=0);
  void purge();
};

WRAPPER(DomainBasicAttrStat);

#endif

