// MarketModel.cpp
// Implements the MarketModel Class

#include <string>

#include "MarketModel.h"

#include "InputXML.h"
#include "CycException.h"
#include "Timer.h"
#include "Logger.h"

using namespace std;

list<MarketModel*> MarketModel::markets_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MarketModel::MarketModel() {
  setModelType("Market");

  // register the model

  TI->registerResolveListener(this);
  markets_.push_back(this);
  setIsTemplate(false);
  commodity_ = "none";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MarketModel::~MarketModel() {
  LOG(LEV_DEBUG2, "none!") << "removing market from static list of markets...";
  list<MarketModel*>::iterator mkt;
  for (mkt=markets_.begin(); mkt!=markets_.end(); ++mkt) {
    if (this == *mkt) {
      LOG(LEV_DEBUG2, "none!") << "  found match in static list";
      markets_.erase(mkt);
      LOG(LEV_DEBUG2, "none!") << "  match is removed";
      break;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MarketModel* MarketModel::marketForCommod(string commod) {
  MarketModel* market = NULL;
  list<MarketModel*>::iterator mkt;
  for (mkt=markets_.begin(); mkt!=markets_.end(); ++mkt){
    if ((*mkt)->commodity() == commod) {
      market = *mkt;
      break;
    }
  }

  if (market == NULL) {
    string err_msg = "No market found for commodity '";
    err_msg += commod + "'.";
    throw CycIndexException(err_msg);
  }
  return market;
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::init(xmlNodePtr cur) {
  Model::init(cur);
  
  /** 
   *  Specific initialization for MarketModels
   */

  /// all markets require commodities
  commodity_ = XMLinput->get_xpath_content(cur,"mktcommodity");

  // region models do not currently follow the template/not template
  // paradigm of insts and facs, so log this as its own parent
  this->setParent(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::copy(MarketModel* src) {
  Model::copy(src);
  Communicator::copy(src);

   /** 
   *  Specific initialization for MarketModels
   */

  commodity_ = src->commodity();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::print() { 
  Model::print(); 

  LOG(LEV_DEBUG2, "none!") << "    trades commodity " << commodity_;

};

