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


#ifndef _CLASSIFY_HPP
#define _CLASSIFY_HPP

#include <string>

#include "distvars.hpp"

using namespace std;

WRAPPER(Classifier)

WRAPPER(EFMDataDescription);

//VECTOR: ClassifierList (of PClassifier)
#define TClassifierList TOrangeVector<PClassifier> 
VWRAPPER(ClassifierList)

/* Classifiers have three methods for classification.
   - operator() returns TValue
   - classDistribution return PDistribution
   - predictionAndDistribution returns both

   At least one of the first two need to be overloaded. If the method
   can return probabilities (or at least something the closely
   ressembles it), it should redefine the second.

   computesProbabilities should be set if classDistribution is overloaded.

   Here are different scenarios:

   1. Derived class overloads only operator() (returning TValue) and
      sets computeProbabilities to false.

      In this case, the inherited PDistribution will call operator()
      and construct a PDistribution in which the returned TValue will
      have a probability of 1.0. Similarly, predictionAndDistribution
      will call operator() and construct PDistribution.

   2. Derived class overloads classDistribution and sets
      computeProbabilities.

      The inherited operator() will call classDistribution and use the
      distribution's method highestProbValue() to select a value to return.
      (Note that it randomly selects one of the values (and not the first
      value) with the highest probability. This can lead to
      different classification of the same example.) Method
      predictionAndDistribution behaves similarly.

   3. operator() and classDistribution are overloaded, while
      predictionAndDistribution is not.

      computesProbabilities decides which of the two methods are used
      by the inherited predictionAndDistribution.

   4. Only predictionAndDistribution is overloaded.

      This is illegal. If prediction cannot be based on probabilities
      (for example, if there is a way to prefer one of predictions with
      the same probability), you should redefine all three methods.
      computesProbabilities can be set to whatever you like.

   Most classifiers will use scenario 2 -- return classDistributions
   and leave the rest to methods inherited from TClassifier.
*/

class TClassifier : public TOrange {
public:
  __REGISTER_CLASS

  PVariable classVar; //P class variable
  bool computesProbabilities; //P set if classifier computes class probabilities (if not, it assigns 1.0 to the predicted)

  TClassifier(const bool &cp=false);
  TClassifier(const PVariable &, const bool &cp=false);
  TClassifier(const TClassifier &old);

  virtual TValue operator ()(const TExample &);
  virtual PDistribution classDistribution(const TExample &);
  virtual void predictionAndDistribution(const TExample &, TValue &, PDistribution &);

  virtual TValue operator()(const TExample &, PEFMDataDescription);
  virtual PDistribution classDistribution(const TExample &, PEFMDataDescription);
};


class TClassifierFD : public TClassifier {
public:
  __REGISTER_CLASS

  PDomain domain; //P domain

  TClassifierFD(const bool &cp =false);
  TClassifierFD(PDomain, const bool &cp =false);
  TClassifierFD(const TClassifierFD &old);

  void afterSet(const char *name);
};


class TDefaultClassifier : public TClassifier {
public:
  __REGISTER_CLASS

  TValue defaultVal; //P default prediction
  PDistribution defaultDistribution; //P default distribution

  TDefaultClassifier();
  TDefaultClassifier(PVariable);
  TDefaultClassifier(PVariable, PDistribution defDis);
  TDefaultClassifier(PVariable, const TValue &defVal, PDistribution defDis);
  TDefaultClassifier(const TDefaultClassifier &old);
  
  virtual TValue operator ()(const TExample &);
  virtual PDistribution classDistribution(const TExample &);
  virtual void predictionAndDistribution(const TExample &, TValue &, PDistribution &);
};


class TRandomClassifier : public TClassifier {
public:
  __REGISTER_CLASS

  PDistribution probabilities; //P probabilities of predictions

  TRandomClassifier(PVariable acv=PVariable());
  TRandomClassifier(const TDistribution &probs);
  TRandomClassifier(PVariable acv, const TDistribution &probs);
  TRandomClassifier(PDistribution);
  TRandomClassifier(PVariable acv, PDistribution);

  TValue operator()(const TExample &);
  PDistribution classDistribution(const TExample &);
  void predictionAndDistribution(const TExample &, TValue &val, PDistribution &dist);
};



WRAPPER(DomainDistributions);
WRAPPER(ExampleGenerator);



class TEFMDataDescription : public TOrange {
public:
  __REGISTER_CLASS

  PDomain domain; //P domain
  PDomainDistributions domainDistributions; //P distributions of values for attributes
  vector<float> averages;
  int originalWeight, missingWeight;

  TEFMDataDescription(PDomain, PDomainDistributions=PDomainDistributions(), int ow=0, int mw=0);
  void getAverages();
};


#endif
