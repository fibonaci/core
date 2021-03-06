// SinkFacility.cpp
// Implements the SinkFacility class
#include <iostream>

#include "SinkFacility.h"

#include "Logger.h"
#include "GenericResource.h"
#include "CycException.h"
#include "InputXML.h"
#include "MarketModel.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SinkFacility::SinkFacility(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SinkFacility::~SinkFacility(){ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::init(xmlNodePtr cur) {
  FacilityModel::init(cur);

  /// Sink facilities can have many input/output commodities
  /// move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SinkFacility");

  /// all facilities require commodities - possibly many
  string commod;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"incommodity");
  for (int i=0;i<nodes->nodeNr;i++) {
    commod = (const char*)(nodes->nodeTab[i]->children->content);
    in_commods_.push_back(commod);
  }

  // get monthly capacity
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);

  double inv_size = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL);
  inventory_.setCapacity(inv_size);

  // get commodity price
  commod_price_ = strtod(XMLinput->get_xpath_content(cur,"commodprice"), NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::copy(SinkFacility* src) {
  FacilityModel::copy(src);

  in_commods_ = src->in_commods_;
  capacity_ = src->capacity_;
  inventory_.setCapacity(src->inventory_.capacity());
  commod_price_ = src->commod_price_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::copyFreshModel(Model* src) {
  copy(dynamic_cast<SinkFacility*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::print() {
  FacilityModel::print();

  string msg = "";

  msg += "accepts commodities ";

  for (vector<string>::iterator commod=in_commods_.begin();
       commod != in_commods_.end();
       commod++) {
    msg += (commod == in_commods_.begin() ? "{" : ", " );
    msg += (*commod);
  }
  msg += "} until its inventory is full at ";
  LOG(LEV_DEBUG2, "SnkFac") << msg << inventory_.capacity() << " kg.";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::handleTick(int time){
  LOG(LEV_INFO3, "SnkFac") << facName() << " is ticking {";

  double requestAmt = getRequestAmt(); 
  double minAmt = 0;

  if (requestAmt>EPS_KG){
    // for each potential commodity, make a request
    for (vector<string>::iterator commod = in_commods_.begin();
        commod != in_commods_.end();
        commod++) {
      LOG(LEV_INFO4, "SnkFac") << " requests "<< requestAmt << " kg of " << *commod << ".";

      MarketModel* market = MarketModel::marketForCommod(*commod);
      Communicator* recipient = dynamic_cast<Communicator*>(market);

      // create a generic resource
      gen_rsrc_ptr request_res = gen_rsrc_ptr(new GenericResource((*commod), "kg", requestAmt));

      // build the transaction and message
      Transaction trans;
      trans.commod = *commod;
      trans.minfrac = minAmt/requestAmt;
      trans.is_offer = false;
      trans.price = commod_price_;
      trans.resource = request_res;

      msg_ptr request(new Message(this, recipient, trans)); 
      request->setNextDest(facInst());
      request->sendOn();

    }
  }
  LOG(LEV_INFO3, "SnkFac") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::handleTock(int time){
  LOG(LEV_INFO3, "SnkFac") << facName() << " is tocking {";

  // On the tock, the sink facility doesn't really do much. 
  // Maybe someday it will record things.
  // For now, lets just print out what we have at each timestep.
  LOG(LEV_INFO4, "SnkFac") << "SinkFacility " << this->ID()
                  << " is holding " << inventory_.quantity()
                  << " units of material at the close of month " << time
                  << ".";
  LOG(LEV_INFO3, "SnkFac") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::addResource(msg_ptr msg, vector<rsrc_ptr> manifest) {
  inventory_.pushAll(ResourceBuff::toMat(manifest));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
const double SinkFacility::getRequestAmt(){
  // The sink facility should ask for as much stuff as it can reasonably receive.
  double requestAmt;
  // get current capacity
  double space = inventory_.space();

  if (space <= 0 ){
    requestAmt=0;
  } else if (space < capacity_){
    requestAmt = space/in_commods_.size();
  } else if (space >= capacity_){
    requestAmt = capacity_/in_commods_.size();
  }
  return requestAmt;
}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructSinkFacility() {
  return new SinkFacility();
}

/* ------------------- */ 

