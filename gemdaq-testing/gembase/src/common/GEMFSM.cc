/**
 * class: GEMFSM
 * description: Generic FSM interface for GEM applications and devices
 *              structure borrowed from TCDS core, with nods to HCAL and EMU code
 * author: 
 * date: 
 */

#include "gem/base/GEMFSM.h"

#include "gem/base/GEMFSMApplication.h"

#include "gem/base/utils/GEMLogging.h"
#include "gem/base/utils/GEMSOAPToolBox.h"
#include "gem/base/utils/exception/Exception.h"

#include "toolbox/fsm/AsynchronousFiniteStateMachine.h"
#include "toolbox/fsm/InvalidInputEvent.h"
#include "toolbox/string.h"
#include "xercesc/dom/DOMNode.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/util/XercesDefs.hpp"
#include "xcept/tools.h"

gem::base::GEMFSM::GEMFSM(GEMFSMApplication* const gemAppP
			  //,gem::base::utils::ApplicationStateInfoSpaceHandler* const appStateInfoSpaceHanderP
			  ) :
  //appStateInfoSpaceHandlerP_(appStateInfoSpaceHanderP),
  gemfsmP_(0),
  gemAppP_(gemAppP),
  gemLogger_(gemAppP->getApplicationLogger()),
  gemRCMSNotifier_(gemAppP_->getApplicationLogger(),
		   gemAppP_->getApplicationDescriptor(),
		   gemAppP_->getApplicationContext())
{
  DEBUG("GEMFSM ctor begin");
  
  // Create the underlying Finite State Machine itself.
  std::stringstream commandLoopName;
  uint32_t instanceNumber = gemAppP_->getApplicationDescriptor()->getInstance();

  //also want to get the name of the GEM FSM aplication to put it into this commandLoopName
  commandLoopName << "urn:toolbox-task-workloop:gemFSMCommandLoop:" << instanceNumber;
  gemfsmP_ = new toolbox::fsm::AsynchronousFiniteStateMachine(commandLoopName.str());

  // A map to look up the names of the 'intermediate' state transitions.
  lookupMap_["Initializing"] = "Initialized";// Initialized
  lookupMap_["Configuring"]  = "Configured" ;// Configured
  lookupMap_["Halting"]      = "Halted"     ;// Halted
  lookupMap_["Starting"]     = "Running"    ;// Running
  lookupMap_["Pausing"]      = "Paused"     ;// Paused
  lookupMap_["Resuming"]     = "Running"    ;// Running
  lookupMap_["Stopping"]     = "Stopped"    ;// Configured

  // Define all states and transitions.
  /* intermediate states (states entered when a transition is requested*/
  gemfsmP_->addState(STATE_INITIALIZING, "Initializing", this, &gem::base::GEMFSM::stateChanged);
  gemfsmP_->addState(STATE_CONFIGURING,  "Configuring",  this, &gem::base::GEMFSM::stateChanged);
  gemfsmP_->addState(STATE_HALTING,      "Halting",      this, &gem::base::GEMFSM::stateChanged);
  gemfsmP_->addState(STATE_STARTING,     "Starting",     this, &gem::base::GEMFSM::stateChanged);
  gemfsmP_->addState(STATE_PAUSING,      "Pausing",      this, &gem::base::GEMFSM::stateChanged);
  gemfsmP_->addState(STATE_RESUMING,     "Resuming",     this, &gem::base::GEMFSM::stateChanged);
  gemfsmP_->addState(STATE_STOPPING,     "Stopping",     this, &gem::base::GEMFSM::stateChanged);

  /*terminal states*/
  gemfsmP_->addState(STATE_INITIAL,    "Initial",     this, &gem::base::GEMFSM::stateChanged);
  gemfsmP_->addState(STATE_HALTED,     "Halted",      this, &gem::base::GEMFSM::stateChanged);
  gemfsmP_->addState(STATE_CONFIGURED, "Configured",  this, &gem::base::GEMFSM::stateChanged);
  gemfsmP_->addState(STATE_RUNNING,    "Running",     this, &gem::base::GEMFSM::stateChanged);
  gemfsmP_->addState(STATE_PAUSED,     "Paused",      this, &gem::base::GEMFSM::stateChanged);

  /*State transitions*/
  // Initialize: I -> H., connect hardware, perform basic checks, (load firware?)
  gemfsmP_->addStateTransition(STATE_INITIAL, STATE_HALTED, "Initialize", gemAppP_, &gem::base::GEMFSMApplication::initializeAction);

  // Configure: H/C/E/P -> C., configure hardware, set default parameters
  gemfsmP_->addStateTransition(STATE_HALTED,     STATE_CONFIGURED, "Configure", gemAppP_, &gem::base::GEMFSMApplication::configureAction);
  gemfsmP_->addStateTransition(STATE_CONFIGURED, STATE_CONFIGURED, "Configure", gemAppP_, &gem::base::GEMFSMApplication::configureAction);
  gemfsmP_->addStateTransition(STATE_RUNNING,    STATE_CONFIGURED, "Configure", gemAppP_, &gem::base::GEMFSMApplication::configureAction);
  gemfsmP_->addStateTransition(STATE_PAUSED,     STATE_CONFIGURED, "Configure", gemAppP_, &gem::base::GEMFSMApplication::configureAction);

  // Start: C -> E., enable links for data to flow from front ends to back ends
  gemfsmP_->addStateTransition(STATE_CONFIGURED, STATE_RUNNING, "Start", gemAppP_, &gem::base::GEMFSMApplication::startAction);
  
  // Pause: E -> P. pause data flow, links stay alive, TTC/TTS counters stay active
  gemfsmP_->addStateTransition(STATE_RUNNING, STATE_PAUSED, "Pause", gemAppP_, &gem::base::GEMFSMApplication::pauseAction);
  
  // Resume: P -> E., resume data flow
  gemfsmP_->addStateTransition(STATE_PAUSED, STATE_RUNNING, "Resume", gemAppP_, &gem::base::GEMFSMApplication::resumeAction);
  
  // Stop: C/E/P -> C., stop data flow, disable links
  gemfsmP_->addStateTransition(STATE_CONFIGURED, STATE_CONFIGURED, "Stop", gemAppP_, &gem::base::GEMFSMApplication::stopAction);
  gemfsmP_->addStateTransition(STATE_RUNNING,    STATE_CONFIGURED, "Stop", gemAppP_, &gem::base::GEMFSMApplication::stopAction);
  gemfsmP_->addStateTransition(STATE_PAUSED,     STATE_CONFIGURED, "Stop", gemAppP_, &gem::base::GEMFSMApplication::stopAction);
  
  // Halt: C/E/F/H/P/ -> H., reset hardware state to pre-configured state
  gemfsmP_->addStateTransition(STATE_CONFIGURED, STATE_HALTED, "Halt", gemAppP_, &gem::base::GEMFSMApplication::haltAction);
  gemfsmP_->addStateTransition(STATE_RUNNING,    STATE_HALTED, "Halt", gemAppP_, &gem::base::GEMFSMApplication::haltAction);
  gemfsmP_->addStateTransition(STATE_FAILED,     STATE_HALTED, "Halt", gemAppP_, &gem::base::GEMFSMApplication::haltAction);
  gemfsmP_->addStateTransition(STATE_HALTED,     STATE_HALTED, "Halt", gemAppP_, &gem::base::GEMFSMApplication::haltAction);
  gemfsmP_->addStateTransition(STATE_PAUSED,     STATE_HALTED, "Halt", gemAppP_, &gem::base::GEMFSMApplication::haltAction);
  
  // Error: I/H/C/E/P -> F.
  /*
    gemfsmP_->addStateTransition(STATE_INITIAL,    STATE_FAILED, "Error", gemAppP_, &GEMFSMApplication::);
    gemfsmP_->addStateTransition(STATE_HALTED,     STATE_FAILED, "Error", gemAppP_, &GEMFSMApplication::);
    gemfsmP_->addStateTransition(STATE_CONFIGURED, STATE_FAILED, "Error", gemAppP_, &GEMFSMApplication::);
    gemfsmP_->addStateTransition(STATE_RUNNING,    STATE_FAILED, "Error", gemAppP_, &GEMFSMApplication::);
    gemfsmP_->addStateTransition(STATE_PAUSED,     STATE_FAILED, "Error", gemAppP_, &GEMFSMApplication::);
  */
  
  gemfsmP_->setStateName(STATE_FAILED, "Error");
  gemfsmP_->setFailedStateTransitionAction(      gemAppP_, &gem::base::GEMFSMApplication::failAction);
  gemfsmP_->setFailedStateTransitionChanged(     this,     &gem::base::GEMFSM::stateChanged);
  gemfsmP_->setInvalidInputStateTransitionAction(this,     &gem::base::GEMFSM::invalidAction);
  
  // Start out with the FSM in its initial state: Initial.
  gemfsmP_->setInitialState(STATE_INITIAL);
  gemfsmP_->reset();

  // Find connection to RCMS.
  gemRCMSNotifier_.findRcmsStateListener();

}


gem::base::GEMFSM::~GEMFSM()
{
  if (gemfsmP_)
    delete gemfsmP_;
  gemfsmP_ = 0;
}


xoap::MessageReference gem::base::GEMFSM::changeState(xoap::MessageReference msg)
//throw (toolbox::fsm::exception::Exception)
{
  if (msg.isNull()) {
    XCEPT_RAISE(xoap::exception::Exception,"Null message received!");
  }
  
  std::string commandName = "undefined";
  try {
    commandName = gem::base::utils::GEMSOAPToolBox::extractFSMCommandName(msg);
  }
  catch(xoap::exception::Exception& err) {
    std::string msgBase =
      toolbox::toString("Unable to extract command from GEMFSM SOAP message");
    ERROR(toolbox::toString("%s: %s.", msgBase.c_str(),
			    xcept::stdformat_exception_history(err).c_str()));
    XCEPT_DECLARE_NESTED(gem::base::utils::exception::SOAPTransitionProblem, top,
			 toolbox::toString("%s.", msgBase.c_str()), err);
    gemAppP_->notifyQualified("error", top);
    std::string faultString = toolbox::toString("%s failed", commandName.c_str());
    std::string faultCode   = "Client";
    std::string detail      = toolbox::toString("%s: %s.",
						msgBase.c_str(),
						err.message().c_str());
    std::string faultActor = gemAppP_->getFullURL();
    xoap::MessageReference reply =
      gem::base::utils::GEMSOAPToolBox::makeSoapFaultReply(faultString, faultCode, detail, faultActor);
    return reply;
  }
  
  DEBUG(toolbox::toString("GEMFSM::changeState() received command '%s'.",
			  commandName.c_str()));
  
  try {
    toolbox::Event::Reference event(new toolbox::Event(commandName, this));
    gemfsmP_->fireEvent(event);
  }
  catch(toolbox::fsm::exception::Exception& err) {
    std::string msgBase =
      toolbox::toString("Problem executing the GEMFSM '%s' command",
			commandName.c_str());
    ERROR(toolbox::toString("%s: %s.",
			    msgBase.c_str(),
			    xcept::stdformat_exception(err).c_str()));
    XCEPT_DECLARE_NESTED(gem::base::utils::exception::SOAPTransitionProblem, top,
			 toolbox::toString("%s.", msgBase.c_str()), err);
    gemAppP_->notifyQualified("error", top);
    std::string faultString = toolbox::toString("%s failed", commandName.c_str());
    std::string faultCode   = "Server";
    std::string detail      = toolbox::toString("%s: %s.",
						msgBase.c_str(),
						err.message().c_str());
    std::string faultActor = gemAppP_->getFullURL();
    xoap::MessageReference reply =
      gem::base::utils::GEMSOAPToolBox::makeSoapFaultReply(faultString, faultCode, detail, faultActor);
    return reply;
  }
  
  //best way?  tcds had questions about this part
  std::string newStateName = commandName + "Triggered";

  // Once we get here, the state transition has been triggered. Notify
  // the requestor of the new state.
  try {
    xoap::MessageReference reply =
      gem::base::utils::GEMSOAPToolBox::makeFSMSoapReply(commandName, newStateName);
    return reply;
  }
  catch(xcept::Exception& err) {
    std::string msgBase =
      toolbox::toString("Failed to create GEMFSM SOAP reply for command '%s'",
			commandName.c_str());
    ERROR(toolbox::toString("%s: %s.",
			    msgBase.c_str(),
			    xcept::stdformat_exception(err).c_str()));
    XCEPT_DECLARE_NESTED(gem::base::utils::exception::SoftwareProblem, top,
			 toolbox::toString("%s.", msgBase.c_str()), err);
    gemAppP_->notifyQualified("error", top);
    XCEPT_RETHROW(xoap::exception::Exception, msgBase, err);
  }
  
  //XCEPT_RAISE(xoap::exception::Exception,"command not found");              
}


std::string gem::base::GEMFSM::getCurrentState() const
{
  return gemfsmP_->getStateName(gemfsmP_->getCurrentState());
}

void gem::base::GEMFSM::notifyRCMS(toolbox::fsm::FiniteStateMachine &fsm, std::string const msg)
  throw (toolbox::fsm::exception::Exception)
{
  // Notify RCMS of a state change.
  // NOTE: Should only be used for state _changes_.

  //toolbox::fsm::State currentState = fsm.getCurrentState();
  //std::string stateName            = fsm.getStateName(currentState);
  std::string stateName = fsm.getStateName(fsm.getCurrentState());
  DEBUG("notifyRCMS() called with msg = " << msg);
  try {
    gemRCMSNotifier_.stateChanged(stateName, msg);
  }
  catch(xcept::Exception& err) {
    ERROR("Failed to notify RCMS of state change: "
	  << xcept::stdformat_exception_history(err));
    XCEPT_DECLARE_NESTED(gem::base::utils::exception::RCMSNotificationError, top,
			 "Failed to notify RCMS of state change.", err);
    gemAppP_->notifyQualified("error", top);
  }
}


void gem::base::GEMFSM::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
  throw (toolbox::fsm::exception::Exception)
{
  std::string state_ = fsm.getStateName(fsm.getCurrentState());
  gemFSMState_ = state_;
  //appStateInfoSpaceHandlerP_->setFSMState(state_);
  DEBUG("Current state is: [" << state_ << "]");
  // Send notification to Run Control
  notifyRCMS(fsm, "Normal state change.");

  std::map<std::string, std::string>::const_iterator iter = lookupMap_.find(state_);
  if (iter != lookupMap_.end()) {
    std::string commandName = iter->second;
    DEBUG("DEBUG JGH '"
	  << state_
	  << "' is an intermediate state --> forwarding to '"
	  << commandName
	  << "'");
    
    // // BUG BUG BUG
    // // Slow things down a bit during development.
    // ::sleep(2);
    // // BUG BUG BUG end
    
    try {
      toolbox::Event::Reference event(new toolbox::Event(commandName, this));
      gemfsmP_->fireEvent(event);
    }
    catch(toolbox::fsm::exception::Exception& err) {
      std::string msgBase =
	toolbox::toString("Problem executing the GEMFSM '%s' command",
			  commandName.c_str());
      ERROR(toolbox::toString("%s: %s.",
			      msgBase.c_str(),
			      xcept::stdformat_exception(err).c_str()));
      XCEPT_DECLARE_NESTED(gem::base::utils::exception::TransitionProblem, top,
			   toolbox::toString("%s.", msgBase.c_str()), err);
      gemAppP_->notifyQualified("error", top);
    }
  }
  else {
    DEBUG("DEBUG JGH '" << state_ << "' is not an intermediate state");
  }
  DEBUG("stateChanged() end");

}


void gem::base::GEMFSM::invalidAction(toolbox::Event::Reference event)
//throw (toolbox::fsm::exception::Exception)
{
  /* what's the point of this action?  
   * should we go to failed or try to ensure no action is taken and the initial state is preserved?
   */
  toolbox::fsm::InvalidInputEvent& invalidInputEvent = dynamic_cast<toolbox::fsm::InvalidInputEvent&>(*event);
  std::string initialState   = gemfsmP_->getStateName(invalidInputEvent.getFromState());
  std::string requestedState = invalidInputEvent.getInput();
  
  std::string message = toolbox::toString("An invalid state transition has been received:"
					  "requested transition to '%s' from '%s'.",
					  requestedState.c_str(), initialState.c_str());
  ERROR(message);
  gotoFailed(message);
}
