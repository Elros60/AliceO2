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
#ifndef O2_FRAMEWORK_SERVICESPEC_H_
#define O2_FRAMEWORK_SERVICESPEC_H_

#include "Framework/ServiceHandle.h"
#include "Framework/DeviceMetricsInfo.h"
#include "Framework/DeviceInfo.h"
#include <functional>
#include <string>
#include <vector>

#include <boost/program_options/variables_map.hpp>

namespace fair::mq
{
struct ProgOptions;
}

namespace o2::framework
{

struct InitContext;
struct DeviceSpec;
struct ServiceRegistry;
struct DeviceState;
struct ProcessingContext;
class EndOfStreamContext;
struct ConfigContext;
struct WorkflowSpecNode;

class DanglingContext;

/// A callback to create a given Service.
using ServiceInit = std::function<ServiceHandle(ServiceRegistry&, DeviceState&, fair::mq::ProgOptions&)>;
/// A callback invoked whenever we start running, before the user processing callback.
using ServiceStartCallback = std::function<void(ServiceRegistry&, void*)>;
/// A callback invoked whenever we stop running, after the user processing callback.
using ServiceStopCallback = std::function<void(ServiceRegistry&, void*)>;
/// A callback invoked whenever we stop running completely, before we exit.
using ServiceExitCallback = std::function<void(ServiceRegistry&, void*)>;

/// A callback to configure a given Service. Notice that the
/// service itself is type erased and it's responsibility of
/// the configuration itself to cast it to the correct value
using ServiceConfigureCallback = std::function<void*(InitContext&, void*)>;

/// A callback which is executed before each processing loop.
using ServiceProcessingCallback = std::function<void(ProcessingContext&, void*)>;

/// A callback which is executed before each dangling input loop
using ServiceDanglingCallback = std::function<void(DanglingContext&, void*)>;

/// A callback which is executed before the end of stream loop.
using ServiceEOSCallback = std::function<void(EndOfStreamContext&, void*)>;

/// Callback executed before the forking of a given device in the driver
/// Notice the forking can happen multiple times. It's responsibility of
/// the service to track how many times it happens and act accordingly.
using ServicePreFork = std::function<void(ServiceRegistry&, boost::program_options::variables_map const&)>;

/// Callback executed after forking a given device in the driver,
/// but before doing exec / starting the device.
using ServicePostForkChild = std::function<void(ServiceRegistry&)>;

/// Callback executed after forking a given device in the driver,
/// but before doing exec / starting the device.
using ServicePostForkParent = std::function<void(ServiceRegistry&)>;

/// Callback executed before each redeployment of the whole configuration
using ServicePreSchedule = std::function<void(ServiceRegistry&, boost::program_options::variables_map const&)>;

/// Callback executed after each redeployment of the whole configuration
using ServicePostSchedule = std::function<void(ServiceRegistry&, boost::program_options::variables_map const&)>;

/// Callback executed in the driver in order to process a metric.
using ServiceMetricHandling = std::function<void(ServiceRegistry&,
                                                 std::vector<o2::framework::DeviceMetricsInfo>& metrics,
                                                 std::vector<o2::framework::DeviceSpec>& specs,
                                                 std::vector<o2::framework::DeviceInfo>& infos,
                                                 DeviceMetricsInfo& driverMetrics,
                                                 size_t timestamp)>;

/// Callback executed in the child after dispatching happened.
using ServicePostDispatching = std::function<void(ProcessingContext&, void*)>;

/// Callback invoked when the driver enters the init phase.
using ServiceDriverInit = std::function<void(ServiceRegistry&, boost::program_options::variables_map const&)>;

/// Callback invoked when the driver enters the init phase.
using ServiceDriverStartup = std::function<void(ServiceRegistry&, boost::program_options::variables_map const&)>;

/// Callback invoked when we inject internal devices in the topology
using ServiceTopologyInject = std::function<void(WorkflowSpecNode&, ConfigContext&)>;

/// Callback invoked when we amend the topology
using ServiceTopologyAdjust = std::function<void(WorkflowSpecNode&, ConfigContext const&)>;

/// A specification for a Service.
/// A Service is a utility class which does not perform
/// data processing itself, but it can be used by the data processor
/// to carry out common tasks (e.g. monitoring) or by the framework
/// to perform data processing related ancillary work (e.g. send
/// messages after a computation happended).
struct ServiceSpec {
  /// Name of the service
  std::string name = "please specify name";
  /// Callback to initialise the service.
  ServiceInit init = nullptr;
  /// Callback to configure the service.
  ServiceConfigureCallback configure = nullptr;
  /// Callback executed before actual processing happens.
  ServiceProcessingCallback preProcessing = nullptr;
  /// Callback executed once actual processing happened.
  ServiceProcessingCallback postProcessing = nullptr;
  /// Callback executed before the dangling inputs loop
  ServiceDanglingCallback preDangling = nullptr;
  /// Callback executed after the dangling inputs loop
  ServiceDanglingCallback postDangling = nullptr;
  /// Callback executed before the end of stream callback of the user happended
  ServiceEOSCallback preEOS = nullptr;
  /// Callback executed after the end of stream callback of the user happended
  ServiceEOSCallback postEOS = nullptr;
  /// Callback executed before the forking of a given device in the driver
  /// Notice the forking can happen multiple times. It's responsibility of
  /// the service to track how many times it happens and act accordingly.
  ServicePreFork preFork = nullptr;
  /// Callback executed after forking a given device in the driver,
  /// but before doing exec / starting the device.
  ServicePostForkChild postForkChild = nullptr;
  /// Callback executed after forking a given device in the driver.
  ServicePostForkParent postForkParent = nullptr;

  /// Callback executed before and after we schedule a topology
  ServicePreSchedule preSchedule = nullptr;
  ServicePostSchedule postSchedule = nullptr;

  ///Callback executed after each metric is received by the driver.
  ServiceMetricHandling metricHandling = nullptr;

  /// Callback executed after a given input record has been successfully
  /// dispatched.
  ServicePostDispatching postDispatching = nullptr;

  /// Callback invoked on Start
  ServiceStartCallback start = nullptr;
  /// Callback invoked on Start
  ServiceStopCallback stop = nullptr;
  /// Callback invoked on exit
  ServiceExitCallback exit = nullptr;
  /// Callback invoked on driver entering the INIT state
  ServiceDriverInit driverInit = nullptr;
  /// Callback invoked when starting the driver
  ServiceDriverStartup driverStartup = nullptr;

  /// Callback invoked when doing topology creation
  ServiceTopologyInject injectTopology = nullptr;

  /// Callback invoked when finalising topology creation
  ServiceTopologyAdjust adjustTopology = nullptr;

  /// Kind of service being specified.
  ServiceKind kind = ServiceKind::Serial;
};

struct ServiceConfigureHandle {
  ServiceSpec const& spec;
  ServiceConfigureCallback callback;
  void* service;
};

struct ServiceProcessingHandle {
  ServiceSpec const& spec;
  ServiceProcessingCallback callback;
  void* service;
};

struct ServiceDanglingHandle {
  ServiceSpec const& spec;
  ServiceDanglingCallback callback;
  void* service;
};

struct ServiceEOSHandle {
  ServiceSpec const& spec;
  ServiceEOSCallback callback;
  void* service;
};

struct ServiceDispatchingHandle {
  ServiceSpec const& spec;
  ServicePostDispatching callback;
  void* service;
};

struct ServiceStartHandle {
  ServiceSpec const& spec;
  ServiceStartCallback callback;
  void* service;
};

struct ServiceStopHandle {
  ServiceSpec const& spec;
  ServiceStopCallback callback;
  void* service;
};

struct ServiceExitHandle {
  ServiceSpec const& spec;
  ServiceExitCallback callback;
  void* service;
};

struct ServicePlugin {
  virtual ServiceSpec* create() = 0;
};

} // namespace o2::framework

#endif // O2_FRAMEWORK_SERVICESPEC_H_
