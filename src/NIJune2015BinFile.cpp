// File: NIJune2015BinFile.hh
// Name: Leah Broussard
// Date: 2015/5/6
// Purpose: Opens binary files in NI June 2015 format
//
// Revision History:
// 2015/5/6:   LJB  Find and open file given run #
// 2015/5/7:   LJB  Open multiple RIO files, requires consecutive RIO 
//                  files starting with RIO0
// 2015/5/11:  LJB  Reads events into a NI_event structure
// 2015/7/15:  LJB  Specialized to June 2015 file format
// 2015/11/21: LJB  Open(filenum)

#ifndef NI_JUN15_BIN_FILE_CPP__
#define NI_JUN15_BIN_FILE_CPP__

#include "NIJune2015BinFile.hh"

/*************************************************************************/
//                           Constructors
/*************************************************************************/
NIJune2015BinFile::NIJune2015BinFile() {
}

NIJune2015BinFile::NIJune2015BinFile(std::string path, std::string name) {
  Open(path,name);
}

NIJune2015BinFile::NIJune2015BinFile(std::string filename) {
  Open(filename);
}


/*************************************************************************/
//                             Destructor
/*************************************************************************/
NIJune2015BinFile::~NIJune2015BinFile() {
  Close();
}

/*************************************************************************/
//                                Open   
/*************************************************************************/
bool NIJune2015BinFile::Open(int filenum, int rionum){
	char tempstr[255];
	sprintf(tempstr,"Run_%d.bin",filenum);
	std::string filename = tempstr;
    return Open(filename);
}

/*************************************************************************/
//                             ReadEvent
/*************************************************************************/
bool NIJune2015BinFile::ReadNextEvent(BinEv_t& NI_event) {
  JuneBinEv_t* June_event = dynamic_cast<JuneBinEv_t*>(&NI_event);
  if (!June_event) {
    cout << "NIJune2015BinFile::ReadNextEvent() requires JuneBinEv_t types" << endl;
    return false;
  }

  if (!IsOpen())
    return false;
  if (!CheckLength())
    return false;

  June_event->result = 0;
  fFileStream.read(reinterpret_cast<char *>(&(June_event->result)),1);
  fFileStream.read(reinterpret_cast<char *>(&(June_event->eventID)),4);
  fFileStream.read(reinterpret_cast<char *>(&(June_event->board)),4);
  fFileStream.read(reinterpret_cast<char *>(&(June_event->channel)),4);
  fFileStream.read(reinterpret_cast<char *>(&(June_event->timestamp)),8);
  int wavelen;
  fFileStream.read(reinterpret_cast<char *>(&wavelen),4);
  if (wavelen > MAXWAVE) {
    cout << endl;
    cout << GetPosition() << endl;
    cout << June_event->result << endl;
    cout << June_event->eventID << endl;
    cout << June_event->board << endl;
    cout << June_event->channel << endl;
    cout << June_event->timestamp << endl;
    cout << "Error in wavelength " << wavelen << endl;
    return false;
  }
  June_event->wave.resize(wavelen);
  if (!fFileStream.read(reinterpret_cast<char *>(&(June_event->wave[0])),wavelen*2))
    return false;
  return true;
}



#endif // __NI_JUN15_BIN_FILE_CPP__

