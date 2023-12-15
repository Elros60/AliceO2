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

/// \file alignment-workflow.cxx
/// \brief Implementation of a DPL device to perform alignment for muon spectrometer 
///
/// \author Chi ZHANG, CEA-Saclay

#include "CommonUtils/ConfigurableParam.h"
#include "MCHAlign/AlignmentSpec.h"

using namespace o2::framework;
using namespace std;

// we need to add workflow options before including Framework/runDataProcessing
void customize(vector<ConfigParamSpec>& workflowOptions)
{
  workflowOptions.emplace_back("configKeyValues", VariantType::String, "",
                               ConfigParamSpec::HelpString{"Semicolon separated key=value strings"});
  workflowOptions.emplace_back("disable-input-from-ccdb", VariantType::Bool, false,
                               ConfigParamSpec::HelpString{"Do not read magnetic field and geometry from CCDB"});
}

#include "Framework/runDataProcessing.h"
WorkflowSpec defineDataProcessing(const ConfigContext& configcontext)
{
  o2::conf::ConfigurableParam::updateFromString(configcontext.options().get<std::string>("configKeyValues"));
  bool disableCCDB = configcontext.options().get<bool>("disable-input-from-ccdb");
  return WorkflowSpec{o2::mch::getAlignmentSpec(disableCCDB)};
}
