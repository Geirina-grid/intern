import { Injectable } from '@angular/core';

import { Observable } from 'rxjs/Observable';
import { of } from 'rxjs/observable/of';
import { Http, Response } from '@angular/http';
import 'rxjs/add/operator/map';


import { environment } from '../../../environments/environment';
import { empty } from 'rxjs/Observer';

@Injectable()
export class DfserService  {

  constructor(private http: Http) { }

  flgValue:boolean[];

  SVC_data: SVCC[] = [];
  SVCVoltage_data:SVCC[]=[];
  SVCCurrent_data:SVCC[]=[];
  SVCdroop_data:SVCC[]=[];
  line_data: lineC[] = [];
  underground_data:lineC[]=[];
  overhead_data:lineC[]=[];

  regulator_data:regulatorC[]=[];
  load_data:loadC[]=[];
  switch_data:switchC[]=[];
  res_data:resC[]=[];
  wind_data:windC[]=[];
  PV_data:PVC[]=[];
  battery_data:batteryC[]=[];
  DataR:any;
  getData() {
  
    if (this.flgValue[0])
    {
      this.DataR=[];
      this.DataR=this.line_data;
    }
    else if(this.flgValue[1]&&this.flgValue[2]==false)
    {
      this.DataR=[];
      this.DataR=this.overhead_data;

    }
    else if(this.flgValue[2]&&this.flgValue[1]==false)
    {
      this.DataR=[];
      this.DataR=this.underground_data;
    }
    else if(this.flgValue[2]&&this.flgValue[1])
    { 
      this.DataR=[];
      this.DataR=this.underground_data.concat(this.overhead_data);
    }



    if (this.flgValue[3])
    {
    this.DataR=[];
    this.DataR=this.SVC_data;
    }

    else if(this.flgValue[4]&&this.flgValue[5]==false)
    {
      this.DataR=[];
      this.DataR=this.SVCVoltage_data

    }
    else if(this.flgValue[5]&&this.flgValue[4]==false)
    {
      this.DataR=[];
      this.DataR=this.SVCCurrent_data;
    }
    else if(this.flgValue[4]&&this.flgValue[5])
    { 
      this.DataR=[];
      this.DataR=this.SVCCurrent_data.concat(this.SVCVoltage_data);
    }





    if (this.regulator_data.length!=0)
    {
    this.DataR=[];
    this.DataR=this.regulator_data;
    }

    if (this.load_data.length!=0)
    {
    this.DataR=[];
    this.DataR=this.load_data;
    }
    if (this.switch_data.length!=0)
    {
    this.DataR=[];
    this.DataR=this.switch_data;
    }
    if (this.flgValue[6]||this.flgValue[7]||this.flgValue[8]||this.flgValue[9])
    {
        this.DataR=[];
        if(this.flgValue[6])
        {
        this.DataR=this.res_data;
        this.res_data=[];
        }
        if(this.flgValue[7]&&!this.flgValue[6])
        {
          this.DataR=this.wind_data;
          this.wind_data=[];
        }
        if(this.flgValue[8]&&!this.flgValue[6])
        {
          this.DataR=this.PV_data;
          this.PV_data=[];
        }
        if(this.flgValue[9]&&!this.flgValue[6])
        {
          this.DataR=this.battery_data;
          this.battery_data=[];
        }
        this.res_data=[];
        this.wind_data=[];
        this.PV_data=[];
        this.battery_data=[];
    }



    return of(this.DataR);


  }


  processData(data,Vchain){
    console.log(data);
    console.log(Vchain);
    this.SVC_data= [];
    this.SVCCurrent_data=[];
    this.SVCdroop_data=[];
    this.SVCVoltage_data=[];

    this.line_data=[];
    this.overhead_data=[];
    this.underground_data=[];

    this.regulator_data=[];
    this.load_data=[];
    this.switch_data=[];
    this.res_data=[];
    var temp = data['results'];
    var keys = Object.keys(temp[0]); 
    var flg:number;
    if(keys[0]=="x.CapID")
    {
      for(var i=0; i<temp.length; i++) {
        var svcna = temp[i]['x.CapID'];
        var na = temp[i]['x.bus_name'];
        var cap = temp[i]['x.QCap']/1000;
        var pha = temp[i]['x.Phase'];
        var conmeth = temp[i]['x.control'];
        this.SVC_data.push({SVCname: svcna, SVCBus: na, SVCcapacity: cap, SVCcontrol:conmeth,SVCphase:pha});
        if(conmeth=="电压")
        {
          this.SVCVoltage_data.push({SVCname: svcna, SVCBus: na, SVCcapacity: cap, SVCcontrol:conmeth,SVCphase:pha});

        }
        else if(conmeth=="电流")
        {
          this.SVCCurrent_data.push({SVCname: svcna, SVCBus: na, SVCcapacity: cap, SVCcontrol:conmeth,SVCphase:pha});

        }
        else if(conmeth=="迟滞")
        {
          this.SVCdroop_data.push({SVCname: svcna, SVCBus: na, SVCcapacity: cap, SVCcontrol:conmeth,SVCphase:pha});
        }
      }
    };
    if(keys[0]=='x.edge_name')
    {
      for(var i=0; i<temp.length; i++) {
        var Vlinename = temp[i]['x.edge_name'];
        var VstartBus = temp[i]['x.s_exId'];
        var VendBus = temp[i]['x.t_exId'];
        var VlineLength = temp[i]['x.linelength'];

        var VphaseAresistance = Number(temp[i]['x.RA']).toFixed(4);
        var VphaseBresistance = Number(temp[i]['x.RB']).toFixed(4);
        var VphaseCresistance = Number(temp[i]['x.RC']).toFixed(4);
        var VphaseAreactance = Number(temp[i]['x.XA']).toFixed(4);
        var VphaseBreactance = Number(temp[i]['x.XB']).toFixed(4);
        var VphaseCreactance = Number(temp[i]['x.XC']).toFixed(4);

        var VphaseAB = Number(temp[i]['x.Xab']).toFixed(4);
        var VphaseAC = Number(temp[i]['x.Xac']).toFixed(4);
        var VphaseBC = Number(temp[i]['x.Xbc']).toFixed(4);
        var Vconfig =temp[i]['x.config'];

        this.line_data.push({linename: Vlinename, startBus: VstartBus,
          endBus: VendBus, lineLength:VlineLength, phaseAresistance:VphaseAresistance,
          phaseBresistance:VphaseBresistance, phaseCresistance:VphaseCresistance,
          phaseAreactance:VphaseAreactance, phaseBreactance:VphaseBreactance,
          phaseCreactance:VphaseCreactance, phaseAB:VphaseAB, phaseAC:VphaseAC,phaseBC: VphaseBC,config:Vconfig})
        
        if(Vconfig!="12")
        {
          this.overhead_data.push({linename: Vlinename, startBus: VstartBus,
            endBus: VendBus, lineLength:VlineLength, phaseAresistance:VphaseAresistance,
            phaseBresistance:VphaseBresistance, phaseCresistance:VphaseCresistance,
            phaseAreactance:VphaseAreactance, phaseBreactance:VphaseBreactance,
            phaseCreactance:VphaseCreactance, phaseAB:VphaseAB, phaseAC:VphaseAC,phaseBC: VphaseBC,config:Vconfig})
          
        }
        else
        {
          this.underground_data.push({linename: Vlinename, startBus: VstartBus,
            endBus: VendBus, lineLength:VlineLength, phaseAresistance:VphaseAresistance,
            phaseBresistance:VphaseBresistance, phaseCresistance:VphaseCresistance,
            phaseAreactance:VphaseAreactance, phaseBreactance:VphaseBreactance,
            phaseCreactance:VphaseCreactance, phaseAB:VphaseAB, phaseAC:VphaseAC,phaseBC: VphaseBC,config:Vconfig})
          
        }
      }
    };


    if(keys[0]=='x.regulator_name')
    {
      console.log(keys);
      for(var i=0; i<temp.length; i++) {
        var Vrename = temp[i]['x.regulator_name'];
        var Vrebus = temp[i]['x.tap_bus'];
        var Vphase = temp[i]['x.phase'];
        var VreCvoltage = temp[i]['x.voltage_level'];
        var VresistorV = temp[i]['x.R'];
        
        var VreactanceV = temp[i]['x.X'];
        var VmaxShift = temp[i]['x.Tap_max'];
        var VminShift = temp[i]['x.Tap_min'];
        var Vstepsize = temp[i]['x.Tap_range'];
        var VPTratio = temp[i]['x.PT'];

        var VCTcurrent = temp[i]['x.CT_rating'];
        var Vbandwidth = temp[i]['x.bandwidth'];
        var VphaseV = temp[i]['x.control_phase_reg'];

        this.regulator_data.push({rename:Vrename, rebus: Vrebus,
          phase:Vphase,
          reactanceV: VreactanceV, reCvoltage:VreCvoltage, resistorV:VresistorV,
          maxShift:VmaxShift, stepsize:Vstepsize,
          minShift:VminShift, PTratio:VPTratio,
          CTcurrent:VCTcurrent, bandwidth:Vbandwidth, phaseV:VphaseV})
     }
    };

    if(keys[0]=='@@id')
    {
      console.log(keys);
      for(var i=0; i<temp.length; i++) {
        var VloadID = temp[i]['@@id'];
        var Vlbus = temp[i]['x.exId'];
        var VPA = Number((temp[i]['x.PdA']/1000).toFixed(2));
        var VQA = Number((temp[i]['x.QdA']/1000).toFixed(2));
        var VPB = Number((temp[i]['x.PdB']/1000).toFixed(2));
        
        var VQB = Number((temp[i]['x.QdB']/1000).toFixed(2));
        var VPC = Number((temp[i]['x.PdC']/1000).toFixed(2));
        var VQC = Number((temp[i]['x.QdC']/1000).toFixed(2));

        this.load_data.push({loadID:VloadID, lbus: Vlbus,
          PA:VPA,
          QA: VQA, PB:VPB, QB:VQB,
          PC:VPC, QC:VQC})
      }
    }
    if(keys[0]=='@@resI')
    {
      var windn:number=0;
      var pvn:number=0;
      var batteryn:number=0;
      for(var i=0; i<temp[0]['@@resI'].length; i++) {
        var wind_name = temp[0]['@@resI'][i]['wind_name'];
        var wind_Gen_status = temp[0]['@@resI'][i]['wind_flag'];
        // if(wind_Gen_status=="ONLINE")
        // {
        //   windn+=1;
        // }
        windn+=1;
        var wind_Gen_mode = temp[0]['@@resI'][i]['wind_Gen_mode'];

        var wind_Turbine_Model = temp[0]['@@resI'][i]['wind_model'];
        var solar_name = temp[0]['@@resI'][i]['solar_name'];
        var solar_gen_status = temp[0]['@@resI'][i]['PV_flag'];
        pvn+=1;
        // if(solar_gen_status=="ONLINE")
        // {
        //   pvn+=1;
        // }
        var solar_install_type = temp[0]['@@resI'][i]['solar_install_type'];
        var solar_panel_type = temp[0]['@@resI'][i]['solar_panel_type'];
        var solar_gen_mode = temp[0]['@@resI'][i]['solar_gen_mode'];
        var solar_efficiency = temp[0]['@@resI'][i]['solar_efficiency'];
        var solar_area = temp[0]['@@resI'][i]['solar_area'];
        var battery_name = temp[0]['@@resI'][i]['battery_name'];
        var battery_generator_mode = temp[0]['@@resI'][i]['battery_generator_mode'];
        var battery_V_Max = temp[0]['@@resI'][i]['battery_V_Max'];
        var battery_I_Max = temp[0]['@@resI'][i]['battery_I_Max'];

        var battery_P_Max = temp[0]['@@resI'][i]['battery_P_Max'];
        var battery_E_Max = temp[0]['@@resI'][i]['battery_E_Max'];
        var battery_base_efficiency = temp[0]['@@resI'][i]['battery_base_efficiency'];
        var battery_gen_status = temp[0]['@@resI'][i]['battery_flag'];
        // if(battery_gen_status=="ONLINE")
        // {
        //   batteryn+=1;
        // }
        batteryn+=1;
        var battery_parasitic_power_draw = temp[0]['@@resI'][i]['battery_parasitic_power'];



        this.wind_data.push({wind_name:wind_name, wind_Gen_status: wind_Gen_status,
          wind_Gen_mode:wind_Gen_mode,wind_Turbine_Model:wind_Turbine_Model})
        this.PV_data.push({solar_name:solar_name, solar_gen_status: solar_gen_status,
          solar_install_type:solar_install_type,solar_panel_type:solar_panel_type,
          solar_gen_mode:solar_gen_mode,solar_efficiency:solar_efficiency,solar_area:solar_area})
        this.battery_data.push({battery_name:battery_name, battery_generator_mode: battery_generator_mode,
          battery_V_Max:battery_V_Max, battery_I_Max:battery_I_Max,
          battery_P_Max:battery_P_Max, battery_E_Max:battery_E_Max,
          battery_base_efficiency:battery_base_efficiency, battery_gen_status:battery_gen_status,
          battery_parasitic_power_draw:battery_parasitic_power_draw})
      }
      console.log(this.res_data)
      console.log(this.wind_data)
      console.log(this.PV_data)
      console.log(this.battery_data)
      this.res_data.push({windn:windn, PVn: pvn,
        batteryn:batteryn})
    }
    this.flgValue=Vchain;
    // this.processSubdata(Vchain);
//to do: switch and battery after the backend data is obtained
  }
/*   processSubdata(Vchain){
   switch(Vchain[0])
   {
    case true:
      break;
    case false:
      if()
        {}


   }
  }
 */
    
  
  getURL(params): String {
    let gsqlPre = environment.gsqlPre;
    //var url = gsqlPre + "/query/product_graph/getInfoMod?" + params.join("&");
/*      var url = "http://localhost:3000/query/product_graph/getInfoMod?" + params; */
    var url = environment.gsqlPre+"/query/" + params; 

    console.log(url);
    return url;
  }

  loadData(url): Observable<String> {
    return this.http.get(url).map((result: Response) => result.json());
  }

  

  
  private dModleUrl = environment.gsqlPre + '/query/summary_D';  // URL to web api
  getDmodles(): Promise<summary[]> {
    return this.http.get(this.dModleUrl)
        .toPromise()
        .then(response => {
            const data = response.json().results;
            const res = [];
            for (const summ of data) {
                res.push({
                    'Sys_id': summ['@@SYSid'],
                     'NoBS':123,
                    //'NoBS': summ['@@BSid'],
                    'NoLN': summ['@@LNid'],
                    'NoCap': summ['@@Capid'],
                    'NoXF': summ['@@XFid'],
                    'NoLD': summ['@@LDid'],
                    'NoGen': summ['@@UNid'],
                    'DegUnbal': Number(summ['@@DegUnbal'].toFixed(2)),
                    'T_Load': Number(summ['@@TotalLoad'].toFixed(2)),
                    'T_Loss': Number(summ['@@TotalLoss'].toFixed(2)),

                    'NoPV':9,
                    'NoWG': 9,
                    'NoEV': 9,
                    'NoES': 9,
                    'NoCCHP': 0,
                    'NoSHEPS': 0,

                    'StartTime':'2:00',
                    'EndTIme':'3:00',
                    'TimeStep':15,
                    'PFmethod':'NR',
                    'Convergence':0.01,
                    'NVoltage':220,

                });
            }

            return res as summary[];
        })
}



getchart(param:any): Promise<summary[]> {
  return this.http.get(param)
      .toPromise()
      .then(response => {
          const data = response.json().results;
          const res = [];
          for (const summ of data) {
            if(param==environment.gsqlPre + '/query/line_D')
            {
              res.push({
                  'XA': summ['x.XA'],
                  'XB': summ['x.XB'],
                  'XC': summ['x.XC'],
                  'config':summ['x.config']
              });
            }
            if(param==environment.gsqlPre + '/query/capacitor_D')
            {
              res.push({
                  'capacity': summ['x.QCap'],
              });
            }
            if(param==environment.gsqlPre + '/query/getres_D')
            {
              for(var i=0;i<summ['@@resI'].length;i++)
              {
                res.push({ 
                  'from': summ['@@resI'][i]['From_Node'],
                  'PVf':summ['@@resI'][i]['PV_flag'],
                  'batteryf':summ['@@resI'][i]['battery_flag'],
                  'windf':summ['@@resI'][i]['wind_flag'],
                  'inv_P':summ['@@resI'][i]['inv_P'],
                  'wind_model':summ['@@resI'][i]['wind_model'],
                  'battery_parasitic_power':summ['@@resI'][i]['battery_parasitic_power']
                });
              }
            }


          }

          return res as summary[];
      })
}



}




export interface SVCC {
  SVCname: string;
  SVCBus: number;
  SVCcapacity: number;
  SVCcontrol: string;
  SVCphase:string;
}
export interface lineC {
 linename:string;
 startBus:number;
 endBus:number;
 lineLength:number;
 phaseAresistance:string;
 phaseBresistance: string;
 phaseCresistance: string;
 phaseAreactance: string;
 phaseBreactance: string;
 phaseCreactance: string;

 phaseAB: string;
 phaseAC: string;
 phaseBC: string;

 config: string;

}

export interface regulatorC{
rename:string;
rebus:number;
phase:string;
reCvoltage:number;
resistorV:number;
reactanceV:number;
maxShift:number;
minShift:number;
stepsize:number;
PTratio:number;
CTcurrent:number;
bandwidth:number;
phaseV:string;
}


export interface loadC{
  loadID:number;
  lbus:number;
  PA:number;
  QA:number;
  PB:number;
  QB:number;
  PC:number;
  QC:number;
  }

  export interface resC{
    windn:number;
    PVn:number;
    batteryn:number;}
  export interface windC{
    wind_name:string;
    wind_Gen_status:string;
    wind_Gen_mode:string;
    wind_Turbine_Model:string;
  }
  export interface PVC{
    solar_name:string;
    solar_gen_status:string;
    solar_install_type:string;
    solar_panel_type:string;
    solar_gen_mode:string;
    solar_efficiency:number;
    solar_area:number;
  }
  export interface batteryC{
    battery_name:string;
    battery_generator_mode:string;
    battery_V_Max:number;
    battery_I_Max:number;
    battery_P_Max:number;
    battery_E_Max:number;
    battery_base_efficiency:number;
    battery_gen_status:string;
    battery_parasitic_power_draw:number;
    }

    export interface switchC{
      Swname:string;
      startBus:string;
      endBus:number;
      status:number;
      }

export interface IGeometry {
    type: string;
    coordinates: number[];
}

export interface IGeoJson {
    type: string;
    geometry: IGeometry;
    properties?: any;
}

export class GeoJson implements IGeoJson {
  type = 'Feature';
  geometry: IGeometry;
  properties: any;

  constructor(coordinates, properties) {
    this.geometry = {
      type: 'Point',
      coordinates: coordinates
    }
    this.properties = properties;
  }
}

export interface Site {
  country: number[],
  quantity: number,
}
//lat,lng
export interface Region {
  country: number[];
  area: number[];
}

export class summary {
  Sys_id: string;
  NoBS: number;
  NoLN: number;
  NoCap:number;
  NoXF: number;
  NoLD: number;
  NoGen: number;
  DegUnbal: number;
  T_Load: number;
  T_Loss: number;

  NoPV:number;
  NoWG: number;
  NoEV: number;
  NoES: number;
  NoCCHP: number;
  NoSHEPS: number;
  
  StartTime:string;
  EndTIme:string;
  TimeStep:number;
  PFmethod:string;
  Convergence:number;
  NVoltage:number;

}