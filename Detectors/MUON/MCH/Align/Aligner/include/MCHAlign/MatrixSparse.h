// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file MatrixSparse.h
/// \brief Sparse matrix class (from AliROOT), used as a global matrix for MillePede2
/// \author ruben.shahoyan@cern.ch

#ifndef ALICEO2_MCH_MATRIXSPARSE_H
#define ALICEO2_MCH_MATRIXSPARSE_H

#include "MCHAlign/MatrixSq.h"
#include "MCHAlign/VectorSparse.h"

namespace o2
{
namespace mch
{

/// \class MatrixSparse
class MatrixSparse : public MatrixSq
{
 public:
  MatrixSparse() = default;

  MatrixSparse(int size);

  MatrixSparse(const MatrixSparse& mat);

  ~MatrixSparse() override { Clear(); }

  VectorSparse* GetRow(int ir) const { return (ir < fNcols) ? fVecs[ir] : nullptr; }
  VectorSparse* GetRowAdd(int ir);

  int GetSize() const override { return fNrows; }
  virtual int GetNRows() const { return fNrows; }
  virtual int GetNCols() const { return fNcols; }

  void Clear(Option_t* option = "") override;
  void Reset() override
  {
    for (int i = fNcols; i--;) {
      GetRow(i)->Reset();
    }
  }
  void Print(Option_t* option = "") const override;
  MatrixSparse& operator=(const MatrixSparse& src);
  double& operator()(int row, int col) override;
  double operator()(int row, int col) const override;
  void SetToZero(int row, int col);

  float GetDensity() const override;

  double DiagElem(int r) const override;
  double& DiagElem(int r) override;

  void SortIndices(bool valuesToo = false);

  void MultiplyByVec(const TVectorD& vecIn, TVectorD& vecOut) const override;

  void MultiplyByVec(const double* vecIn, double* vecOut) const override;

  void AddToRow(int r, double* valc, int* indc, int n) override;

 protected:
  VectorSparse** fVecs = nullptr;

  ClassDefOverride(MatrixSparse, 0)
};

//___________________________________________________
inline void MatrixSparse::MultiplyByVec(const TVectorD& vecIn, TVectorD& vecOut) const
{
  // multiplication
  MultiplyByVec((double*)vecIn.GetMatrixArray(), (double*)vecOut.GetMatrixArray());
}

//___________________________________________________
inline void MatrixSparse::SetToZero(int row, int col)
{
  //  set existing element to 0
  if (IsSymmetric() && col > row) {
    Swap(row, col);
  }

  VectorSparse* rowv = GetRow(row);

  if (rowv) {
    rowv->SetToZero(col);
  }
}

//___________________________________________________
inline double MatrixSparse::operator()(int row, int col) const
{
  if (IsSymmetric() && col > row) {
    Swap(row, col);
  }

  VectorSparse* rowv = GetRow(row);

  if (!rowv) {
    return 0;
  }
  return rowv->FindIndex(col);
}

//___________________________________________________
inline double& MatrixSparse::operator()(int row, int col)
{
  if (IsSymmetric() && col > row) {
    Swap(row, col);
  }

  VectorSparse* rowv = GetRowAdd(row);

  if (col >= fNcols) {
    fNcols = col + 1;
  }
  return rowv->FindIndexAdd(col);
}

//___________________________________________________
inline double MatrixSparse::DiagElem(int row) const
{
  // get diag elem
  VectorSparse* rowv = GetRow(row);
  if (!rowv) {
    return 0;
  }
  if (IsSymmetric()) {
    return (rowv->GetNElems() > 0 && rowv->GetLastIndex() == row) ? rowv->GetLastElem() : 0.;
  } else {
    return rowv->FindIndex(row);
  }
}

//___________________________________________________
inline double& MatrixSparse::DiagElem(int row)
{
  // get diag elem
  VectorSparse* rowv = GetRowAdd(row);
  if (row >= fNcols) {
    fNcols = row + 1;
  }
  if (IsSymmetric()) {
    return (rowv->GetNElems() > 0 && rowv->GetLastIndex() == row) ? rowv->GetLastElem() : rowv->FindIndexAdd(row);
  } else {
    return rowv->FindIndexAdd(row);
  }
}

} // namespace mch
} // namespace o2

#endif
