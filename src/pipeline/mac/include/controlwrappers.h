#ifndef EP_CONTROLWRAPPERS_H
#define EP_CONTROLWRAPPERS_H

#include "Controller_Protocol.ph"
#include "eventwrappers.h"

typedef LOFAR::TYPES::uint16 uint16;
typedef LOFAR::TYPES::uint32 uint32;

// Sendable messages
class CONTROLStartedEventWrapper : public GenericEventWrapper {
private:
    CONTROLStartedEvent* my_event;
public:
    CONTROLStartedEventWrapper(std::string cntlrName, bool successful) {
        this->my_event = new CONTROLStartedEvent;
        this->my_event->cntlrName = cntlrName;
        this->my_event->successful = successful;
    }
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    bool get_successful() { return this->my_event->successful; }
    virtual uint16 get_signal() { return this->my_event->signal; }
    virtual CONTROLStartedEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLConnectedEventWrapper : public GenericEventWrapper {
private:
    CONTROLConnectedEvent* my_event;
public:
    CONTROLConnectedEventWrapper(std::string cntlrName, uint16 result) {
        this->my_event = new CONTROLConnectedEvent;
        this->my_event->cntlrName = cntlrName;
        this->my_event->result = result;
    }
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    uint16 get_result() { return this->my_event->result; }

    virtual CONTROLConnectedEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLResyncedEventWrapper : public GenericEventWrapper {
private:
    CONTROLResyncedEvent* my_event;
public:
    CONTROLResyncedEventWrapper(std::string cntlrName, uint16 result) {
        this->my_event = new CONTROLResyncedEvent;
        this->my_event->cntlrName = cntlrName;
        this->my_event->result = result;
    }
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    uint16 get_result() { return this->my_event->result; }

    virtual CONTROLResyncedEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLClaimedEventWrapper : public GenericEventWrapper {
private:
    CONTROLClaimedEvent* my_event;
public:
    CONTROLClaimedEventWrapper(std::string cntlrName, uint16 result) {
        this->my_event = new CONTROLClaimedEvent;
        this->my_event->cntlrName = cntlrName;
        this->my_event->result = result;
    }
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    uint16 get_result() { return this->my_event->result; }

    virtual CONTROLClaimedEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLPreparedEventWrapper : public GenericEventWrapper {
private:
    CONTROLPreparedEvent* my_event;
public:
    CONTROLPreparedEventWrapper(std::string cntlrName, uint16 result) {
        this->my_event = new CONTROLPreparedEvent;
        this->my_event->cntlrName = cntlrName;
        this->my_event->result = result;
    }
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    uint16 get_result() { return this->my_event->result; }

    virtual CONTROLPreparedEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLResumedEventWrapper : public GenericEventWrapper {
private:
    CONTROLResumedEvent* my_event;
public:
    CONTROLResumedEventWrapper(std::string cntlrName, uint16 result) {
        this->my_event = new CONTROLResumedEvent;
        this->my_event->cntlrName = cntlrName;
        this->my_event->result = result;
    }
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    uint16 get_result() { return this->my_event->result; }

    virtual CONTROLResumedEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLSuspendedEventWrapper : public GenericEventWrapper {
private:
    CONTROLSuspendedEvent* my_event;
public:
    CONTROLSuspendedEventWrapper(std::string cntlrName, uint16 result) {
        this->my_event = new CONTROLSuspendedEvent;
        this->my_event->cntlrName = cntlrName;
        this->my_event->result = result;
    }
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    uint16 get_result() { return this->my_event->result; }

    virtual CONTROLSuspendedEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLReleasedEventWrapper : public GenericEventWrapper {
private:
    CONTROLReleasedEvent* my_event;
public:
    CONTROLReleasedEventWrapper(std::string cntlrName, uint16 result) {
        this->my_event = new CONTROLReleasedEvent;
        this->my_event->cntlrName = cntlrName;
        this->my_event->result = result;
    }
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    uint16 get_result() { return this->my_event->result; }

    virtual CONTROLReleasedEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLQuitedEventWrapper : public GenericEventWrapper {
private:
    CONTROLQuitedEvent* my_event;
public:
    CONTROLQuitedEventWrapper(std::string cntlrName, uint32 treeID, uint16 result, std::string errorMsg) {
        this->my_event = new CONTROLQuitedEvent;
        this->my_event->cntlrName = cntlrName;
        this->my_event->result = result;
        this->my_event->treeID = treeID;
        this->my_event->errorMsg = errorMsg;
    }
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    uint16 get_result() { return this->my_event->result; }
    uint32 get_treeID() { return this->my_event->treeID; }
    std::string get_errorMsg() { return this->my_event->errorMsg; }

    virtual CONTROLQuitedEvent* get_event_ptr() { return this->my_event; }
};

// Receivable messages

// First the simple: connect, claim, prepare, resume, suspend, release, quit
class CONTROLConnectEventWrapper : public GenericEventWrapper {
private:
    CONTROLConnectEvent* my_event;
public:
    CONTROLConnectEventWrapper(GenericEventWrapper& my_gev) {
        LOFAR::MACIO::GCFEvent* event_ptr;
        event_ptr = my_gev.get_event_ptr();
        this->my_event = new CONTROLConnectEvent(*event_ptr);
    }
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    virtual uint16 get_signal() { return this->my_event->signal; }
    virtual CONTROLConnectEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLClaimEventWrapper : public GenericEventWrapper {
private:
    CONTROLClaimEvent* my_event;
public:
    CONTROLClaimEventWrapper(GenericEventWrapper& my_gev) {
        LOFAR::MACIO::GCFEvent* event_ptr;
        event_ptr = my_gev.get_event_ptr();
        this->my_event = new CONTROLClaimEvent(*event_ptr);
    }
    CONTROLClaimEventWrapper();
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    virtual uint16 get_signal() { return this->my_event->signal; }
    virtual CONTROLClaimEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLPrepareEventWrapper : public GenericEventWrapper {
private:
    CONTROLPrepareEvent* my_event;
public:
    CONTROLPrepareEventWrapper(GenericEventWrapper& my_gev) {
        LOFAR::MACIO::GCFEvent* event_ptr;
        event_ptr = my_gev.get_event_ptr();
        this->my_event = new CONTROLPrepareEvent(*event_ptr);
    }
    CONTROLPrepareEventWrapper();
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    virtual uint16 get_signal() { return this->my_event->signal; }
    virtual CONTROLPrepareEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLResumeEventWrapper : public GenericEventWrapper {
private:
    CONTROLResumeEvent* my_event;
public:
    CONTROLResumeEventWrapper(GenericEventWrapper& my_gev) {
        LOFAR::MACIO::GCFEvent* event_ptr;
        event_ptr = my_gev.get_event_ptr();
        this->my_event = new CONTROLResumeEvent(*event_ptr);
    }
    CONTROLResumeEventWrapper();
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    virtual uint16 get_signal() { return this->my_event->signal; }
    virtual CONTROLResumeEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLSuspendEventWrapper : public GenericEventWrapper {
private:
    CONTROLSuspendEvent* my_event;
public:
    CONTROLSuspendEventWrapper(GenericEventWrapper& my_gev) {
        LOFAR::MACIO::GCFEvent* event_ptr;
        event_ptr = my_gev.get_event_ptr();
        this->my_event = new CONTROLSuspendEvent(*event_ptr);
    }
    CONTROLSuspendEventWrapper();
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    virtual uint16 get_signal() { return this->my_event->signal; }
    virtual CONTROLSuspendEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLReleaseEventWrapper : public GenericEventWrapper {
private:
    CONTROLReleaseEvent* my_event;
public:
    CONTROLReleaseEventWrapper(GenericEventWrapper& my_gev) {
        LOFAR::MACIO::GCFEvent* event_ptr;
        event_ptr = my_gev.get_event_ptr();
        this->my_event = new CONTROLReleaseEvent(*event_ptr);
    }
    CONTROLReleaseEventWrapper();
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    virtual uint16 get_signal() { return this->my_event->signal; }
    virtual CONTROLReleaseEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLQuitEventWrapper : public GenericEventWrapper {
private:
    CONTROLQuitEvent* my_event;
public:
    CONTROLQuitEventWrapper(GenericEventWrapper& my_gev) {
        LOFAR::MACIO::GCFEvent* event_ptr;
        event_ptr = my_gev.get_event_ptr();
        this->my_event = new CONTROLQuitEvent(*event_ptr);
    }
    CONTROLQuitEventWrapper();
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    virtual uint16 get_signal() { return this->my_event->signal; }
    virtual CONTROLQuitEvent* get_event_ptr() { return this->my_event; }
};

// ...then the more awkward: resync, schedule.

class CONTROLResyncEventWrapper : public GenericEventWrapper {
private:
    CONTROLResyncEvent* my_event;
public:
    CONTROLResyncEventWrapper(GenericEventWrapper& my_gev) {
        LOFAR::MACIO::GCFEvent* event_ptr;
        event_ptr = my_gev.get_event_ptr();
        this->my_event = new CONTROLResyncEvent(*event_ptr);
    }
    CONTROLResyncEventWrapper();
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    uint16 get_curState() { return this->my_event->curState; }
    std::string get_hostname() { return this->my_event->hostname; }

    virtual uint16 get_signal() { return this->my_event->signal; }
    virtual CONTROLResyncEvent* get_event_ptr() { return this->my_event; }
};

class CONTROLScheduleEventWrapper : public GenericEventWrapper {
private:
    CONTROLScheduleEvent* my_event;
public:
    CONTROLScheduleEventWrapper(GenericEventWrapper& my_gev) {
        LOFAR::MACIO::GCFEvent* event_ptr;
        event_ptr = my_gev.get_event_ptr();
        this->my_event = new CONTROLScheduleEvent(*event_ptr);
    }
    std::string get_cntlrName() { return this->my_event->cntlrName; }
    time_t get_startTime() { return this->my_event->startTime; }
    time_t get_stopTime() { return this->my_event->stopTime; }

    virtual uint16 get_signal() { return this->my_event->signal; }
    virtual CONTROLScheduleEvent* get_event_ptr() { return this->my_event; }
};


#endif

