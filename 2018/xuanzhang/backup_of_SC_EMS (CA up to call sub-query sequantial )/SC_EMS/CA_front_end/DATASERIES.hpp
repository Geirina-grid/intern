
#include <iostream>
#include <sstream>
#include <murmurhash/MurmurHash2.h>
#include <gutil/jsonwriter.hpp>
#include <gapi4/graphapi.hpp>
#include <gpelib4/enginedriver/engineservicerequest.hpp>
#include <boost/lexical_cast.hpp>


#ifndef _DATASERIES_
#define _DATASERIES_
namespace UDIMPL {
class DATASERIES {
public:

  double h1;
  double h2;
  double h3;
  double h4;
  double h5;
  double h6;
  double h7;
  double h8;
  double h9;
  double h10;
  double h11;
  double h12;
  double h13;
  double h14;
  double h15;
  double h16;
  double h17;
  double h18;
  double h19;
  double h20;
  double h21;
  double h22;
  double h23;
  double h24;

  DATASERIES():
  h1(),
  h2(),
  h3(),
  h4(),
  h5(),
  h6(),
  h7(),
  h8(),
  h9(),
  h10(),
  h11(),
  h12(),
  h13(),
  h14(),
  h15(),
  h16(),
  h17(),
  h18(),
  h19(),
  h20(),
  h21(),
  h22(),
  h23(),
  h24(){
  }

  DATASERIES(double h1_, double h2_, double h3_, double h4_, double h5_, double h6_, double h7_, double h8_, double h9_, double h10_, double h11_, double h12_, double h13_, double h14_, double h15_, double h16_, double h17_, double h18_, double h19_, double h20_, double h21_, double h22_, double h23_, double h24_){
    h1 = h1_;
    h2 = h2_;
    h3 = h3_;
    h4 = h4_;
    h5 = h5_;
    h6 = h6_;
    h7 = h7_;
    h8 = h8_;
    h9 = h9_;
    h10 = h10_;
    h11 = h11_;
    h12 = h12_;
    h13 = h13_;
    h14 = h14_;
    h15 = h15_;
    h16 = h16_;
    h17 = h17_;
    h18 = h18_;
    h19 = h19_;
    h20 = h20_;
    h21 = h21_;
    h22 = h22_;
    h23 = h23_;
    h24 = h24_;
  }

 bool operator==(DATASERIES const &other) const {
    return h1 == other.h1
        && h2 == other.h2
        && h3 == other.h3
        && h4 == other.h4
        && h5 == other.h5
        && h6 == other.h6
        && h7 == other.h7
        && h8 == other.h8
        && h9 == other.h9
        && h10 == other.h10
        && h11 == other.h11
        && h12 == other.h12
        && h13 == other.h13
        && h14 == other.h14
        && h15 == other.h15
        && h16 == other.h16
        && h17 == other.h17
        && h18 == other.h18
        && h19 == other.h19
        && h20 == other.h20
        && h21 == other.h21
        && h22 == other.h22
        && h23 == other.h23
        && h24 == other.h24
    ;
  }

  friend std::size_t hash_value(const DATASERIES& other){
    std::stringstream os;
    os    << other.h1
    << other.h2
    << other.h3
    << other.h4
    << other.h5
    << other.h6
    << other.h7
    << other.h8
    << other.h9
    << other.h10
    << other.h11
    << other.h12
    << other.h13
    << other.h14
    << other.h15
    << other.h16
    << other.h17
    << other.h18
    << other.h19
    << other.h20
    << other.h21
    << other.h22
    << other.h23
    << other.h24
    ;
    std::string s = os.str();
    return MurmurHash64A(s.c_str(), s.size(), 0);
  }

 DATASERIES&  operator += (const DATASERIES& other){
    this->h1 += other.h1;
    this->h2 += other.h2;
    this->h3 += other.h3;
    this->h4 += other.h4;
    this->h5 += other.h5;
    this->h6 += other.h6;
    this->h7 += other.h7;
    this->h8 += other.h8;
    this->h9 += other.h9;
    this->h10 += other.h10;
    this->h11 += other.h11;
    this->h12 += other.h12;
    this->h13 += other.h13;
    this->h14 += other.h14;
    this->h15 += other.h15;
    this->h16 += other.h16;
    this->h17 += other.h17;
    this->h18 += other.h18;
    this->h19 += other.h19;
    this->h20 += other.h20;
    this->h21 += other.h21;
    this->h22 += other.h22;
    this->h23 += other.h23;
    this->h24 += other.h24;
    return *this;
  }

 bool operator<(DATASERIES const &other) const {
    if (h1 < other.h1) {
      return true;
    } else if (h1 > other.h1) {
      return false;
    }

    if (h2 < other.h2) {
      return true;
    } else if (h2 > other.h2) {
      return false;
    }

    if (h3 < other.h3) {
      return true;
    } else if (h3 > other.h3) {
      return false;
    }

    if (h4 < other.h4) {
      return true;
    } else if (h4 > other.h4) {
      return false;
    }

    if (h5 < other.h5) {
      return true;
    } else if (h5 > other.h5) {
      return false;
    }

    if (h6 < other.h6) {
      return true;
    } else if (h6 > other.h6) {
      return false;
    }

    if (h7 < other.h7) {
      return true;
    } else if (h7 > other.h7) {
      return false;
    }

    if (h8 < other.h8) {
      return true;
    } else if (h8 > other.h8) {
      return false;
    }

    if (h9 < other.h9) {
      return true;
    } else if (h9 > other.h9) {
      return false;
    }

    if (h10 < other.h10) {
      return true;
    } else if (h10 > other.h10) {
      return false;
    }

    if (h11 < other.h11) {
      return true;
    } else if (h11 > other.h11) {
      return false;
    }

    if (h12 < other.h12) {
      return true;
    } else if (h12 > other.h12) {
      return false;
    }

    if (h13 < other.h13) {
      return true;
    } else if (h13 > other.h13) {
      return false;
    }

    if (h14 < other.h14) {
      return true;
    } else if (h14 > other.h14) {
      return false;
    }

    if (h15 < other.h15) {
      return true;
    } else if (h15 > other.h15) {
      return false;
    }

    if (h16 < other.h16) {
      return true;
    } else if (h16 > other.h16) {
      return false;
    }

    if (h17 < other.h17) {
      return true;
    } else if (h17 > other.h17) {
      return false;
    }

    if (h18 < other.h18) {
      return true;
    } else if (h18 > other.h18) {
      return false;
    }

    if (h19 < other.h19) {
      return true;
    } else if (h19 > other.h19) {
      return false;
    }

    if (h20 < other.h20) {
      return true;
    } else if (h20 > other.h20) {
      return false;
    }

    if (h21 < other.h21) {
      return true;
    } else if (h21 > other.h21) {
      return false;
    }

    if (h22 < other.h22) {
      return true;
    } else if (h22 > other.h22) {
      return false;
    }

    if (h23 < other.h23) {
      return true;
    } else if (h23 > other.h23) {
      return false;
    }

    if (h24 < other.h24) {
      return true;
    } else if (h24 > other.h24) {
      return false;
    }

    return false;
  }


  void json_printer (gutil::JSONWriter& writer, gpelib4::EngineServiceRequest& _request,
gapi4::UDFGraphAPI* graphAPI, bool verbose = false) const {

    writer.WriteStartObject();
    writer.WriteName("h1");
      writer.WriteFloat(h1);
          writer.WriteName("h2");
      writer.WriteFloat(h2);
          writer.WriteName("h3");
      writer.WriteFloat(h3);
          writer.WriteName("h4");
      writer.WriteFloat(h4);
          writer.WriteName("h5");
      writer.WriteFloat(h5);
          writer.WriteName("h6");
      writer.WriteFloat(h6);
          writer.WriteName("h7");
      writer.WriteFloat(h7);
          writer.WriteName("h8");
      writer.WriteFloat(h8);
          writer.WriteName("h9");
      writer.WriteFloat(h9);
          writer.WriteName("h10");
      writer.WriteFloat(h10);
          writer.WriteName("h11");
      writer.WriteFloat(h11);
          writer.WriteName("h12");
      writer.WriteFloat(h12);
          writer.WriteName("h13");
      writer.WriteFloat(h13);
          writer.WriteName("h14");
      writer.WriteFloat(h14);
          writer.WriteName("h15");
      writer.WriteFloat(h15);
          writer.WriteName("h16");
      writer.WriteFloat(h16);
          writer.WriteName("h17");
      writer.WriteFloat(h17);
          writer.WriteName("h18");
      writer.WriteFloat(h18);
          writer.WriteName("h19");
      writer.WriteFloat(h19);
          writer.WriteName("h20");
      writer.WriteFloat(h20);
          writer.WriteName("h21");
      writer.WriteFloat(h21);
          writer.WriteName("h22");
      writer.WriteFloat(h22);
          writer.WriteName("h23");
      writer.WriteFloat(h23);
          writer.WriteName("h24");
      writer.WriteFloat(h24);
      
    writer.WriteEndObject();
  }
  gutil::JSONWriter& json_write_name (
    gutil::JSONWriter& writer, gpelib4::EngineServiceRequest& _request,
    gapi4::UDFGraphAPI* graphAPI, bool verbose = false) const {
    
    std::string ss = boost::lexical_cast<std::string>(*this);
    return writer.WriteName(ss.c_str());
  }

  friend std::ostream& operator<<(std::ostream& os, const DATASERIES& m) {
    os<<"[";
    os<<"h1 "<<m.h1<<"|";
    os<<"h2 "<<m.h2<<"|";
    os<<"h3 "<<m.h3<<"|";
    os<<"h4 "<<m.h4<<"|";
    os<<"h5 "<<m.h5<<"|";
    os<<"h6 "<<m.h6<<"|";
    os<<"h7 "<<m.h7<<"|";
    os<<"h8 "<<m.h8<<"|";
    os<<"h9 "<<m.h9<<"|";
    os<<"h10 "<<m.h10<<"|";
    os<<"h11 "<<m.h11<<"|";
    os<<"h12 "<<m.h12<<"|";
    os<<"h13 "<<m.h13<<"|";
    os<<"h14 "<<m.h14<<"|";
    os<<"h15 "<<m.h15<<"|";
    os<<"h16 "<<m.h16<<"|";
    os<<"h17 "<<m.h17<<"|";
    os<<"h18 "<<m.h18<<"|";
    os<<"h19 "<<m.h19<<"|";
    os<<"h20 "<<m.h20<<"|";
    os<<"h21 "<<m.h21<<"|";
    os<<"h22 "<<m.h22<<"|";
    os<<"h23 "<<m.h23<<"|";
    os<<"h24 "<<m.h24<<"]";
      return os ;
  }


  template <class ARCHIVE>
   void serialize(ARCHIVE& ar) {

    struct TempTuple {
      double h1;
      double h2;
      double h3;
      double h4;
      double h5;
      double h6;
      double h7;
      double h8;
      double h9;
      double h10;
      double h11;
      double h12;
      double h13;
      double h14;
      double h15;
      double h16;
      double h17;
      double h18;
      double h19;
      double h20;
      double h21;
      double h22;
      double h23;
      double h24;
    };
    TempTuple tp;
    //initialize tp from this->data
    tp.h1 = h1;
    tp.h2 = h2;
    tp.h3 = h3;
    tp.h4 = h4;
    tp.h5 = h5;
    tp.h6 = h6;
    tp.h7 = h7;
    tp.h8 = h8;
    tp.h9 = h9;
    tp.h10 = h10;
    tp.h11 = h11;
    tp.h12 = h12;
    tp.h13 = h13;
    tp.h14 = h14;
    tp.h15 = h15;
    tp.h16 = h16;
    tp.h17 = h17;
    tp.h18 = h18;
    tp.h19 = h19;
    tp.h20 = h20;
    tp.h21 = h21;
    tp.h22 = h22;
    tp.h23 = h23;
    tp.h24 = h24;
    //serialization for TempTuple
    ar (tp.h1,tp.h2,tp.h3,tp.h4,tp.h5,tp.h6,tp.h7,tp.h8,tp.h9,tp.h10,tp.h11,tp.h12,tp.h13,tp.h14,tp.h15,tp.h16,tp.h17,tp.h18,tp.h19,tp.h20,tp.h21,tp.h22,tp.h23,tp.h24);
    //recover this->data from tp
    h1 = tp.h1;
    h2 = tp.h2;
    h3 = tp.h3;
    h4 = tp.h4;
    h5 = tp.h5;
    h6 = tp.h6;
    h7 = tp.h7;
    h8 = tp.h8;
    h9 = tp.h9;
    h10 = tp.h10;
    h11 = tp.h11;
    h12 = tp.h12;
    h13 = tp.h13;
    h14 = tp.h14;
    h15 = tp.h15;
    h16 = tp.h16;
    h17 = tp.h17;
    h18 = tp.h18;
    h19 = tp.h19;
    h20 = tp.h20;
    h21 = tp.h21;
    h22 = tp.h22;
    h23 = tp.h23;
    h24 = tp.h24;

   }

}__attribute__((__packed__));
}//END namespace UDIMPL
#endif

