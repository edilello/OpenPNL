/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//                INTEL CORPORATION PROPRIETARY INFORMATION                //
//   This software is supplied under the terms of a license agreement or   //
//  nondisclosure agreement with Intel Corporation and may not be copied   //
//   or disclosed except in accordance with the terms of that agreement.   //
//       Copyright (c) 2003 Intel Corporation. All Rights Reserved.        //
//                                                                         //
//  File:      pnlSoftMaxCPD.hpp                                           //
//                                                                         //
//  Purpose:   CSoftMaxCPD class definition                                //
//                                                                         //
//  Author(s): (in alphabetical order)                                     //
//             Abrosimova, Bader, Chernishova, Gergel, Senin, Sidorov,     //
//             Sysoyev, Vinogradov                                         //
//             NNSU                                                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef __PNLSOFTMAXCPD__HPP__
#define __PNLSOFTMAXCPD__HPP__

#include "pnlCPD.hpp"
#include "pnlTypeDefs.hpp"
#include "pnlSoftMaxDistribFun.hpp"

PNL_BEGIN

class PNL_API CSoftMaxCPD : public CCPD
{
public:
  static CSoftMaxCPD* Create(const intVector& domainIn, CModelDomain* pMD);

  static CSoftMaxCPD* Copy(const CSoftMaxCPD* pSMCPD);

  void InitLearnData();

  void CopyLearnDataToDistrib();

  virtual CFactor* Clone() const;

  virtual CFactor* CloneWithSharedMatrices();

  void AllocDistribution(const floatVector& weightsIn, 
    const floatVector& offsetIn,
    const intVector& parentCombination = intVector());

#ifdef PNL_OBSOLETE
  static CSoftMaxCPD* Create(const int *domain, int nNodes, CModelDomain* pMD);

  void AllocDistribution(const float* pWeights, 
    const float* pOffsets, const int* parentCombination = NULL);

#endif //PNL_OBSOLETE

  virtual ~CSoftMaxCPD(){}

  virtual CPotential *ConvertToPotential() const;

  virtual void NormalizeCPD();

  virtual float ProcessingStatisticalData(int numberOfEvidences);

  virtual void UpdateStatisticsEM(const CPotential *pMargPot, 
    const CEvidence *pEvidence = NULL);

  virtual void GenerateSample(CEvidence* evidenceIO, int maximize = 0) const;

  virtual CPotential* ConvertStatisticToPot(int numOfSamples) const;

  virtual float GetLogLik(const CEvidence* pEv, 
    const CPotential* pShrInfRes = NULL) const;

  void SetMaximizingMethod(EMaximizingMethod met);

  EMaximizingMethod GetMaximizingMethod();

  void MaximumLikelihood(float **Observation, int NumberOfObservations, 
    float Accuracy, float step = 0.1);

#ifdef PNL_OBSOLETE
  virtual void UpdateStatisticsML(const CEvidence* const* pEvidencesIn, 
    int EvidenceNumber);
#endif

protected:
  CSoftMaxCPD(const int *domain, int nNodes, CModelDomain* pMD);

  CSoftMaxCPD(const CSoftMaxCPD& SMCPD);

  CSoftMaxCPD(const CSoftMaxCPD* pSMCPD);

private:
  EMaximizingMethod m_MaximizingMethod;
  //defines Method, which is used for Likelihood Maximizing
};

PNL_END

#endif// __PNLSOFTMAXCPD__HPP__