#ifndef SNDLHCEVENTHEADER_H
#define SNDLHCEVENTHEADER_H 1

#include <TNamed.h>
#include "SNDLHCEventHeaderConst.h"

#include <ctime>
#include <map>
#include <string>
#include <vector>

using namespace std;

/**
 * Event header class based on FairEventHeader
 */
class SNDLHCEventHeader : public TNamed
{

  public:

    /** Default constructor **/
    SNDLHCEventHeader();

    /** Constructor with arguments **/
    SNDLHCEventHeader(Int_t runN, uint64_t evtNumber, int64_t timestamp, uint64_t flags);

    /** Destructor **/
    virtual ~SNDLHCEventHeader();

    /** Setters **/
    void SetRunId(uint64_t runid) { fRunId = runid; }
    void SetEventTime(int64_t time) { fEventTime = time; }
    void SetInputFileId(int id) { fInputFileId = id; }
    void SetEventNumber(int id) { fEventNumber = id; }
    void SetUTCtimestamp(int64_t UTCtstamp) { fUTCtimestamp = UTCtstamp; };
    void SetFlags(uint64_t flags);

    /** Getters **/
    uint64_t GetRunId() { return fRunId; }
    int64_t GetEventTime() { return fEventTime; }
    int GetInputFileId() { return fInputFileId; }
    int GetEventNumber() { return fEventNumber; }
    string GetTimeAsString(); // GMT time
    int64_t GetUTCtimestamp() const { return fUTCtimestamp; }
    uint16_t GetFillNumber() const { return fFillNumber; }
    int GetAccMode() const { return fAccMode; }
    int GetBeamMode() const { return fBeamMode; }
    map<string, bool> GetFastNoiseFilters();
    map<string, bool> GetAdvNoiseFilters();
    vector<string> GetPassedFastNFCriteria();
    vector<string> GetPassedAdvNFCriteria();

    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;

  protected:
    uint64_t fRunId;
    int64_t fEventTime;
    int fInputFileId;
    int fEventNumber;
    int64_t fUTCtimestamp;
    uint64_t fFlags;
    uint16_t fFillNumber;
    int fAccMode; // enum class
    int fBeamMode; // enum class
    
    /** Copy constructor **/
    SNDLHCEventHeader(const SNDLHCEventHeader& eventHeader);
    SNDLHCEventHeader operator=(const SNDLHCEventHeader& eventHeader);

    ClassDef(SNDLHCEventHeader,1)

};

#endif /* SNDLHCEVENTHEADER_H */
