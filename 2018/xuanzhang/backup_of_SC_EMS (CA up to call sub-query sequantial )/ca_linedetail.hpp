
#include <iostream>
#include <sstream>
#include <murmurhash/MurmurHash2.h>
#include <gutil/jsonwriter.hpp>
#include <gapi4/graphapi.hpp>
#include <gpelib4/enginedriver/engineservicerequest.hpp>
#include <boost/lexical_cast.hpp>


#ifndef _CA_LINEDETAIL_
#define _CA_LINEDETAIL_
namespace UDIMPL {
class ca_linedetail {
public:

  int32_t caFromBus;
  int32_t caToBus;
  int32_t violFromBus;
  int32_t violToBus;
  double violation_perc;

  ca_linedetail():
  caFromBus(),
  caToBus(),
  violFromBus(),
  violToBus(),
  violation_perc(){
  }

  ca_linedetail(int32_t caFromBus_, int32_t caToBus_, int32_t violFromBus_, int32_t violToBus_, double violation_perc_){
    caFromBus = caFromBus_;
    caToBus = caToBus_;
    violFromBus = violFromBus_;
    violToBus = violToBus_;
    violation_perc = violation_perc_;
  }

 bool operator==(ca_linedetail const &other) const {
    return caFromBus == other.caFromBus
        && caToBus == other.caToBus
        && violFromBus == other.violFromBus
        && violToBus == other.violToBus
        && violation_perc == other.violation_perc
    ;
  }

  friend std::size_t hash_value(const ca_linedetail& other){
    std::stringstream os;
    os    << other.caFromBus
    << other.caToBus
    << other.violFromBus
    << other.violToBus
    << other.violation_perc
    ;
    std::string s = os.str();
    return MurmurHash64A(s.c_str(), s.size(), 0);
  }

 ca_linedetail&  operator += (const ca_linedetail& other){
    this->caFromBus += other.caFromBus;
    this->caToBus += other.caToBus;
    this->violFromBus += other.violFromBus;
    this->violToBus += other.violToBus;
    this->violation_perc += other.violation_perc;
    return *this;
  }

 bool operator<(ca_linedetail const &other) const {
    if (caFromBus < other.caFromBus) {
      return true;
    } else if (caFromBus > other.caFromBus) {
      return false;
    }

    if (caToBus < other.caToBus) {
      return true;
    } else if (caToBus > other.caToBus) {
      return false;
    }

    if (violFromBus < other.violFromBus) {
      return true;
    } else if (violFromBus > other.violFromBus) {
      return false;
    }

    if (violToBus < other.violToBus) {
      return true;
    } else if (violToBus > other.violToBus) {
      return false;
    }

    if (violation_perc < other.violation_perc) {
      return true;
    } else if (violation_perc > other.violation_perc) {
      return false;
    }

    return false;
  }


  void json_printer (gutil::JSONWriter& writer, gpelib4::EngineServiceRequest& _request,
gapi4::UDFGraphAPI* graphAPI, bool verbose = false) const {

    writer.WriteStartObject();
    writer.WriteName("caFromBus");
      writer.WriteInt(caFromBus);
          writer.WriteName("caToBus");
      writer.WriteInt(caToBus);
          writer.WriteName("violFromBus");
      writer.WriteInt(violFromBus);
          writer.WriteName("violToBus");
      writer.WriteInt(violToBus);
          writer.WriteName("violation_perc");
      writer.WriteFloat(violation_perc);
      
    writer.WriteEndObject();
  }
  gutil::JSONWriter& json_write_name (
    gutil::JSONWriter& writer, gpelib4::EngineServiceRequest& _request,
    gapi4::UDFGraphAPI* graphAPI, bool verbose = false) const {
    
    std::string ss = boost::lexical_cast<std::string>(*this);
    return writer.WriteName(ss.c_str());
  }

  friend std::ostream& operator<<(std::ostream& os, const ca_linedetail& m) {
    os<<"[";
    os<<"caFromBus "<<m.caFromBus<<"|";
    os<<"caToBus "<<m.caToBus<<"|";
    os<<"violFromBus "<<m.violFromBus<<"|";
    os<<"violToBus "<<m.violToBus<<"|";
    os<<"violation_perc "<<m.violation_perc<<"]";
      return os ;
  }


  template <class ARCHIVE>
   void serialize(ARCHIVE& ar) {

    struct TempTuple {
      int32_t caFromBus;
      int32_t caToBus;
      int32_t violFromBus;
      int32_t violToBus;
      double violation_perc;
    };
    TempTuple tp;
    //initialize tp from this->data
    tp.caFromBus = caFromBus;
    tp.caToBus = caToBus;
    tp.violFromBus = violFromBus;
    tp.violToBus = violToBus;
    tp.violation_perc = violation_perc;
    //serialization for TempTuple
    ar (tp.caFromBus,tp.caToBus,tp.violFromBus,tp.violToBus,tp.violation_perc);
    //recover this->data from tp
    caFromBus = tp.caFromBus;
    caToBus = tp.caToBus;
    violFromBus = tp.violFromBus;
    violToBus = tp.violToBus;
    violation_perc = tp.violation_perc;

   }

}__attribute__((__packed__));
}//END namespace UDIMPL
#endif

