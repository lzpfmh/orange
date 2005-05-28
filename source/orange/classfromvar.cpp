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


#include "vars.hpp"
#include "domain.hpp"
#include "examples.hpp"
#include "examplegen.hpp"
#include "transval.hpp"

#include "classfromvar.ppp"


inline TValue processValue(PTransformValue &transformer, const TValue &val, const PDistribution &distributionForUnknown, bool transformUnknowns)
{
  if (!val.isSpecial() || transformUnknowns)
    return transformer ? transformer->call(val) : val;

  if (distributionForUnknown) {
    PDistribution distr = CLONE(TDistribution, distributionForUnknown);
    distr->normalize();
    return TValue(distr, val.varType, val.valueType);
  }

  return val;
}


TClassifierFromVar::TClassifierFromVar(PVariable acv, PDistribution dun)
: TClassifier(acv),
  whichVar(acv),
  transformer(),
  distributionForUnknown(dun),
  transformUnknowns(false),
  lastDomainVersion(-1)
{}


TClassifierFromVar::TClassifierFromVar(PVariable acv, PVariable awhichVar, PDistribution dun)
: TClassifier(acv),
  whichVar(awhichVar),
  transformer(),
  distributionForUnknown(dun),
  transformUnknowns(false),
  lastDomainVersion(-1)
{}


TClassifierFromVar::TClassifierFromVar(PVariable acv, PVariable awhichVar, PDistribution dun, PTransformValue trans)
: TClassifier(acv),
  whichVar(awhichVar),
  transformer(trans),
  distributionForUnknown(dun),
  lastDomainVersion(-1)
{}

TClassifierFromVar::TClassifierFromVar(const TClassifierFromVar &old)
: TClassifier(old),
  whichVar(old.whichVar),
  transformer(old.transformer),
  distributionForUnknown(old.distributionForUnknown),
  transformUnknowns(false),
  lastDomainVersion(-1)
{};


TValue TClassifierFromVar::operator ()(const TExample &example)
{ 
  if ((lastDomainVersion != example.domain->version) || (lastWhichVar != whichVar)) {
    checkProperty(whichVar);

    lastDomainVersion = -1;
    lastWhichVar = whichVar;
    position = 0;

    TVarList::const_iterator vi(example.domain->variables->begin()), ei(example.domain->variables->end());
    for(; (vi!=ei) && (*vi!=whichVar); vi++, position++);
    if (vi==ei)
      position = -1;
  }

  if (position>=0)
    return processValue(transformer, example[position], distributionForUnknown, transformUnknowns);

  TMetaVector::const_iterator mi(example.domain->metas.begin()), me(example.domain->metas.end());
  for( ; (mi!=me) && ((*mi).variable!=whichVar); mi++);
  if (mi!=me)
    return processValue(transformer, example[(*mi).id], distributionForUnknown, transformUnknowns);

  if (whichVar->getValueFrom)
    return processValue(transformer, whichVar->computeValue(example), distributionForUnknown, transformUnknowns);

  int varType;
  if (distributionForUnknown->variable)
    varType = distributionForUnknown->variable->varType;
  else if (classVar)
    varType = classVar->varType;
  else if (distributionForUnknown->supportsDiscrete)
    varType = TValue::INTVAR;
  else if (distributionForUnknown->supportsContinuous)
    varType = TValue::FLOATVAR;
  else
    varType = TValue::NONE;
    
  return TValue(CLONE(TDistribution, distributionForUnknown), varType, valueDK);
}



TClassifierFromVarFD::TClassifierFromVarFD(PVariable acv, PDomain dom, const int &p, PDistribution dun, PTransformValue atrans)
: TClassifierFD(dom),
  position(p),
  transformer(atrans),
  distributionForUnknown(dun),
  transformUnknowns(false)
{ classVar = acv; }


TClassifierFromVarFD::TClassifierFromVarFD(const TClassifierFromVarFD &old)
: TClassifierFD(old),
  position(old.position),
  transformer(old.transformer),
  distributionForUnknown(old.distributionForUnknown),
  transformUnknowns(false)
{};


TValue TClassifierFromVarFD::operator ()(const TExample &example)
{ 
  if (position==ILLEGAL_INT)
    raiseError("'position' not set");
  if (position>=example.domain->variables->size())
    raiseError("'position' out of range");
  
  if (example.domain==domain)
    return processValue(transformer, example[position], distributionForUnknown, transformUnknowns);
  else
    return processValue(transformer, example.getValue(domain->getVar(position)), distributionForUnknown, transformUnknowns);
}
