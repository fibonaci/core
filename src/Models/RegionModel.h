// RegionModel.h
#if !defined(_REGIONMODEL_H)
#define _REGIONMODEL_H

#include <set>
#include <vector>

#include "TimeAgent.h"
#include "Communicator.h"

/**
   @class RegionModel
   
    The RegionModel class is the abstract class/interface used by all 
   region models
  
   This is all that is known externally about Regions

   @section intro Introduction
   The RegionModel type assists in defining the region-institution-facility 
   hierarchy in Cyclus. A RegionModel region is an actor associated with a 
   set of institutions or facilities for which it is responsible. A 
   RegionModel may be used to help MarketModel implementations to make 
   material routing decisions based on interfacility relationships. 
   Deployment is a primary differentiator between different RegionModel 
   implementations.

   Like all model implementations, there are a number of implementations that 
   are distributed as part of the core Cyclus application as well as 
   implementations contributed by third-party developers. The links below 
   describe additional parameters necessary for the complete definition of a 
   region of that implementation.

   @section functionality Basic Functionality
   All regions perform three major functions:

   -# Schedule the deployment of facilities by either
     -# Determining when new facilities need to be built, or
     -# Deferring to an InstModel to make this determination.
   -# Manage the deployment of facilities by interacting with the 
   Institutions to select a specific facility type and facility parameters
   -# Passing material offers/requests between a prescribed market and 
   related facilities.

   Different regional types will be required to fully define the first two 
   functions while the third is a built-in capability for all region types. 
   For instance, one may wish to include a region which has exponential 
   growth as its driving factor for facility creation or one may wish to 
   have pre-determined building order based on time step (e.g. the JAEA 
   benchmark). Additionally, one may wish for there to be a one-to-one 
   region-to-instituion deployment for simple models and thus demand that 
   each instiution simply build a facility when its region determines the 
   facility's necessity. However, one may instead wish to have two competing 
   instiutions in one region and have the institution which provides the best 
   incentive to the region to build a required facility.

   @section availableCoreImpl Available Core Implementation
   - NullRegion: This region is associated with a set of allowed facilities. 
   It defers to an institution regarding facility deployment, making no 
   demands on facility type or parameter (save the facility's allowability in 
   the region). It makes no alterations to messages passed through it in 
   either the up or down direction.
   
   @section anticipatedCoreImpl Anticipated Core Implementation
   - <a href="http://code.google.com/p/cyclus/wiki/DeploymentRegion">DeploymentRegion</a>:
   This region will issue deployment commands to its institutions according 
   to some user specified schedule. Institutions do not change or negotiate 
   facility types or parameters. It makes no alterations to messages passed 
   through it in either the up or down direction.
   - <a href="http://code.google.com/p/cyclus/wiki/ExpGrowthRegion">ExpGrowthRegion</a>: 
   This region will issue deployment requests to its institutions according 
   to some user-specified initial capacity and exponential growth rate. 
   Institutions do not change or negotiate facility types or parameters. It 
   makes no alterations to messages passed through it in either the up or 
   down direction.
 */
class RegionModel : public TimeAgent, public Communicator {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
   *   Default constructor for RegionModel Class
   */
  RegionModel();
  
  /**
   *   RegionModels should not be indestructible.
   */
  virtual ~RegionModel() {};
    
  /**
   *   every model needs a method to initialize from XML
   */
  virtual void init(xmlNodePtr cur);

  /**
   *   every model needs a method to copy one object to another
   */
  virtual void copy(RegionModel* src);

  /**
   *  This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src)=0;

  /**
   *   every model should be able to print a verbose description
   */
  virtual void print();

 public:
  /**
   *   default RegionModel receiver is to ignore messages
   */
  virtual void receiveMessage(msg_ptr msg);

  /**
   *  Each region is prompted to do its beginning-of-life-step
   * stuff before the simulation begins.
   *
   * Normally, Regions simply hand the command down to institutions.
   *
   */
  virtual void handlePreHistory();

  /**
   *  Each region is prompted to do its beginning-of-time-step
   * stuff at the tick of the timer.
   * The default behavior is to ignore the tick.
   *
   * @param time is the time to perform the tick
   */
  virtual void handleTick(int time);

  /**
   *  Each region is prompted to do its end-of-time-step
   * stuff at the tock of the timer.
   * The default behavior is to ignore the tock.
   *
   * @param time is the time to perform the tock
   */
  virtual void handleTock(int time);

  /**
   *  Each region is prompted to do its daily task.
   *
   * @param time is the month since the start of the simulation
   * @param day is the current day of that month
   */
  virtual void handleDailyTasks(int time, int day);

 public:
  /**
   *   returns if the facility is in this region's allowed facs
   */
  bool isAllowedFacility(Model* test_fac) 
  { return ( allowedFacilities_.find(test_fac) 
	     != allowedFacilities_.end() ); } ;
    
 protected:
  /**
   *   every region has a list of allowed facilities
   */
  std::set<Model*> allowedFacilities_;
      
};

#endif
