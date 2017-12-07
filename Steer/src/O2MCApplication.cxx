#include <Steer/O2MCApplication.h>
#include <FairMQChannel.h>
#include <FairMQMessage.h>
#include <FairMQDevice.h>
#include <FairMQParts.h>
#include <ITSMFTSimulation/Hit.h>
#include <TPCSimulation/Point.h>
#include <SimulationDataFormat/PrimaryChunk.h>
#include <TMessage.h>
#include <sstream>

namespace o2 {
namespace steer {

  template<typename T>
  void TypedVectorSender(const char* name, FairMQChannel& channel, const o2::Data::SubEventInfo& info) {
    static auto mgr = FairRootManager::Instance();
    auto vector = mgr->InitObjectAs<const std::vector<T>*>(name);

    o2::Data::addTrailingObjectToVector(
					*(const_cast<std::vector<T>*>(vector)), info);
    
    if (vector) {
      auto buffer = (char*)&(*vector)[0];
      auto buffersize = vector->size()*sizeof(T);
      FairMQMessagePtr message(channel.NewMessage(buffer, buffersize,
						     [](void* data, void* hint) { }, buffer));
      channel.Send(message);
    }
  }

  // sending std::vector as a TMessage (whenever this cannot be avoided)
  template<typename T>
  void TMessageVectorSender(const char* name, FairMQChannel& channel, const o2::Data::SubEventInfo& info) {
    static auto mgr = FairRootManager::Instance();
    auto vector = mgr->InitObjectAs<const std::vector<T>*>(name);

    FairMQParts composedmessage;
    composedmessage.AddPart(std::move(channel.NewSimpleMessage(info)));

    if (vector) {
      LOG(INFO) << "SENDING TPC HITS " << vector->size() << "\n";
      TMessage* tmsg = new TMessage();
      tmsg->WriteObjectAny((void*)vector, TClass::GetClass("vector<o2::TPC::HitGroup>"));
      auto free_tmessage = [](void* data, void* hint) { /* delete static_cast<TMessage*>(hint); */ };

      std::unique_ptr<FairMQMessage> message(channel.NewMessage(tmsg->Buffer(), tmsg->BufferSize(), free_tmessage, tmsg));
      composedmessage.AddPart(std::move(message));

      auto bytes = channel.Send(composedmessage);
      LOG(INFO) << "SEND " << bytes << " BYTES\n";
    }
  }

  void O2MCApplication::assembleTPCSectors(FairMQParts& parts)
  {
	parts.AddPart(std::move(mTPCChannel->NewSimpleMessage(mSubEventInfo)));
	static auto mgr = FairRootManager::Instance();

    for(int s=0;s<36;++s) {
      std::stringstream name;
      name << "TPCHitsSector" << s;
      auto vector = mgr->InitObjectAs<const std::vector<o2::TPC::HitGroup>*>(name.str().c_str());

      if (vector) {
        LOG(INFO) << "SENDING TPC HITS " << vector->size() << "\n";
        TMessage* tmsg = new TMessage();
        tmsg->WriteObjectAny((void*)vector, TClass::GetClass("vector<o2::TPC::HitGroup>"));
        auto free_tmessage = [](void* data, void* hint) { /* delete static_cast<TMessage*>(hint); */ };

        std::unique_ptr<FairMQMessage> message(
          mTPCChannel->NewMessage(tmsg->Buffer(), tmsg->BufferSize(), free_tmessage, tmsg));
        parts.AddPart(std::move(message));
     }
    }
  }

  void O2MCApplication::sendTPCData() {
	FairMQParts parts;
	assembleTPCSectors(parts);
	mTPCChannel->Send(parts);
  }

  void O2MCApplication::SendData() {
    // Question do we delegate to detectors?
    // ... or iterate over FairRootMgr branch addresses

    // Send meta information: can we still send this atomically with the data??

    //

    // Send primaries + secondaries produced
    TypedVectorSender<o2::MCTrack>("MCTrack", *mMCTrackChannel, mSubEventInfo);
    
    TypedVectorSender<o2::ITSMFT::Hit>("ITSHit", *mITSChannel, mSubEventInfo);
    // HOW TO LOOP OVER THE DETECTOR PRODUCTS -- it would be nice having a template vector
    // CAN WE SEND IN PARALLEL ?

    sendTPCData();
    //TMessageVectorSender<o2::TPC::HitGroup>("TPCHitsSector0", *mTPCChannel, mSubEventInfo);
    //%sHitsSector%d
  }

}
}
