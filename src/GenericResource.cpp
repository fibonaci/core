// Resource.cpp
// Implements the Resource Class

#include "GenericResource.h"

#include "CycException.h"
#include "Logger.h"

bool GenericResource::type_is_logged_ = false;

using namespace std;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
GenericResource::GenericResource(string units,
                                 string quality, double quantity) : Resource() {
  units_ = units;
  quality_ = quality;
  quantity_ = quantity;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
GenericResource::GenericResource(const GenericResource& other) {
  units_ = other.units_;
  quality_ = other.quality_;
  quantity_ = other.quantity_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
rsrc_ptr GenericResource::clone() {
  CLOG(LEV_DEBUG2) << "GenericResource ID=" << ID_ << " was cloned.";
  print();
  return rsrc_ptr(new GenericResource(*this));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GenericResource::print() {
  CLOG(LEV_DEBUG3) << "GenericResource ID=" << ID_ << ", quality=" << quality_
                   << ", quantity=" << quantity_ << ", units=" << units_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
bool GenericResource::checkQuality(rsrc_ptr other){
  bool toRet = false;

  toRet = (units_ == other->units());
  return toRet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool GenericResource::checkQuantityEqual(rsrc_ptr other) {
  // KDHFLAG : Should allow epsilon as a parameter to this function?
  bool toRet;
  toRet =( checkQuality(other) && quantity_ == other->quantity());
  return toRet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool GenericResource::checkQuantityGT(rsrc_ptr other) {
  // KDHFLAG : Should allow epsilon as a parameter to this function?
  bool toRet;
  toRet = ( checkQuality(other) && quantity_ < other->quantity());
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericResource::setOriginatorID(int id){
  originatorID_ = id;
  this->Resource::addToTable();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void GenericResource::absorb(gen_rsrc_ptr other) {
  if (! checkQuality(boost::dynamic_pointer_cast<Resource>(other))) {
    throw CycGenResourceIncompatible("incompatible resource types.");
  }

  quantity_ += other->quantity();
  other->setQuantity(0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
gen_rsrc_ptr GenericResource::extract(double quantity) {
  if (quantity > quantity_) {
    throw CycGenResourceOverExtract("Attempted to extract more quantity than exists.");
  }

  quantity_ -= quantity;

  return gen_rsrc_ptr(new GenericResource(units_, quality_, quantity));
}
