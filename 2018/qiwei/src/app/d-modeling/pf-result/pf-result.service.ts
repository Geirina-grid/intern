import { Injectable } from '@angular/core';
import { Headers, Http, Response } from '@angular/http';
import { Observable } from 'rxjs/Observable';
import { of } from 'rxjs/observable/of';
import 'rxjs/add/operator/map';

import 'rxjs/add/operator/toPromise';
import { environment } from '../../../environments/environment';
import { empty } from 'rxjs/Observer';

@Injectable()
export class PfResultService  {

  private getTSD = environment.pyPre + '/plot_from_gsql_TSD/'
  private get1Day = environment.pyPre + '/plot_from_gsql_TSD1day/'
  private idUrl = environment.pyPre + '/dq_id/'
  private headers = new Headers({ 'Content-Type': 'application/json' });

  constructor(private http: Http) { }

  flgValue:boolean[];

  SVC_data: SVCC[] = [];

  line_data: lineC[] = [];
  L_over_data:lineC[]=[];
  L_secure_data:lineC[]=[];

  regulator_data:regulatorC[]=[];


  bus_data:busC[]=[];
  B_PAH_data:busC[]=[];
  B_PAL_data:busC[]=[];
  B_PBH_data:busC[]=[];
  B_PBL_data:busC[]=[];
  B_PCH_data:busC[]=[];
  B_PCL_data:busC[]=[];
  B_normal:busC[]=[];


  switch_data:switchC[]=[];
  battery_data:batteryC[]=[];
  DataR:any;
  getData() {
  var temp:busC[]=[];


    if (this.flgValue[0])
    {
      this.DataR=[];
      this.DataR=this.line_data;
    }
    else if(this.flgValue[1]&&this.flgValue[2]==false)
    {
      this.DataR=[];
      this.DataR=this.L_over_data;

    }
    else if(this.flgValue[2]&&this.flgValue[1]==false)
    {
      this.DataR=[];
      this.DataR=this.L_secure_data;
    }
    else if(this.flgValue[2]&&this.flgValue[1])
    { 
      this.DataR=[];
      this.DataR=this.L_over_data.concat(this.L_secure_data);
    }


    if (this.flgValue[3])
    {
    this.DataR=[];
    this.DataR=this.bus_data;
    }
    else if(this.flgValue[4]||this.flgValue[5]||this.flgValue[6]||this.flgValue[7]||this.flgValue[8]||this.flgValue[9]||this.flgValue[10])
    {
      this.DataR=[];
      if(this.flgValue[4])
      {temp=temp.concat(this.B_PAH_data)}
      if(this.flgValue[5])
      {temp=temp.concat(this.B_PAL_data)}
      if(this.flgValue[6])
      {temp=temp.concat(this.B_PBH_data)}
      if(this.flgValue[7])
      {temp=temp.concat(this.B_PBL_data)}
      if(this.flgValue[8])
      {temp=temp.concat(this.B_PCH_data)}
      if(this.flgValue[9])
      {temp=temp.concat(this.B_PCL_data)}
      if(this.flgValue[9])
      {temp=temp.concat(this.B_PCL_data)}
      if(this.flgValue[10])
      {temp=temp.concat(this.B_normal)}
      this.DataR=temp;
    }


 
    if (this.SVC_data.length!=0)
    {
    this.DataR=[];
    this.DataR=this.SVC_data;
    }

    if (this.regulator_data.length!=0)
    {
    this.DataR=[];
    this.DataR=this.regulator_data;
    }
    if (this.switch_data.length!=0)
    {
    this.DataR=[];
    this.DataR=this.switch_data;
    }
    if (this.battery_data.length!=0)
    {
    this.DataR=[];
    this.DataR=this.battery_data;
    }



    return of(this.DataR);


  }


  processData(data,Vchain){
    console.log(Vchain);
    this.SVC_data= [];

    this.line_data=[];
    var maxL=5000/3; 


    this.L_over_data=[];
    this.L_secure_data=[];
    

    this.regulator_data=[];

    this.bus_data=[];
    this.B_PAH_data=[];
    this.B_PAL_data=[];
    this.B_PBH_data=[];
    this.B_PBL_data=[];
    this.B_PCH_data=[];
    this.B_PCL_data=[];


    this.switch_data=[];
    this.battery_data=[];
    var temp = data['results'];
    var keys = Object.keys(temp[0]); 
    var flg:number;
    if(keys[0]=="x.CapID")
    {
      for(var i=0; i<temp.length; i++) {
        var svcna = temp[i]['x.CapID'];
        var na = temp[i]['x.bus_name'];
        var cap =Number(temp[i]['x.QCap_cal']/1000).toFixed(3);
        var pha = temp[i]['x.Phase'];
        var conmeth = temp[i]['x.control'];
        this.SVC_data.push({SVCname: svcna, SVCBus: na, SVCoutput: cap,SVCphase:pha});
      }
    };
    if(keys[0]=='x.edge_name')
    {
      for(var i=0; i<temp.length; i++) {
        var Vlinename = temp[i]['x.edge_name'];
        var VstartBus = temp[i]['x.s_exId'];
        var VendBus = temp[i]['x.t_exId'];

        var VphaseAP =Number(temp[i]['x.PA']/1000).toFixed(3);
        var VphaseBP= Number(temp[i]['x.PB']/1000).toFixed(3);
        var VphaseCP = Number(temp[i]['x.PC']/1000).toFixed(3);
        var VphaseAQ = Number(temp[i]['x.QA']/1000).toFixed(3);
        var VphaseBQ = Number( temp[i]['x.QB']/1000).toFixed(3);
        var VphaseCQ = Number( temp[i]['x.QC']/1000).toFixed(3);

        var VphaseACu =Number(temp[i]['x.IA']).toFixed(3);
        var VphaseBCu = Number(temp[i]['x.IB']).toFixed(3);
        var VphaseCCu = Number(temp[i]['x.IC']).toFixed(3);

        var VphaseACan =Number( temp[i]['x.IA_ang']).toFixed(3);
        var VphaseBCan = Number(temp[i]['x.IB_ang']).toFixed(3);
        var VphaseCCan =Number( temp[i]['x.IC_ang']).toFixed(3);

        var VphaseAloss= Number (temp[i]['x.lossA']/1000).toFixed(3);
        var VphaseBloss= Number(temp[i]['x.lossB']/1000).toFixed(3);
        var VphaseCloss=Number(temp[i]['x.lossC']/1000).toFixed(3);

        this.line_data.push({linename: Vlinename, startBus: VstartBus,
          endBus: VendBus, phaseAP:VphaseAP,
          phaseBP:VphaseBP, phaseCP:VphaseCP,
          phaseAQ:VphaseAQ, phaseBQ:VphaseBQ,
          phaseCQ:VphaseCQ, phaseACu:VphaseACu, phaseBCu:VphaseBCu, phaseCCu: VphaseCCu,
          phaseACan:VphaseACan,phaseBCan:VphaseBCan, phaseCCan:VphaseCCan,phaseAloss:VphaseAloss, phaseBloss:VphaseBloss,
          phaseCloss:VphaseCloss
        })
        var k1=+VphaseAP;
        var k2=+VphaseBP;
        var k3=+VphaseCP;
        if(k1>maxL||k2>maxL||k3>maxL)
        {
          this.L_over_data.push({linename: Vlinename, startBus: VstartBus,
            endBus: VendBus, phaseAP:VphaseAP,
            phaseBP:VphaseBP, phaseCP:VphaseCP,
            phaseAQ:VphaseAQ, phaseBQ:VphaseBQ,
            phaseCQ:VphaseCQ, phaseACu:VphaseACu, phaseBCu:VphaseBCu, phaseCCu: VphaseCCu,
            phaseACan:VphaseACan,phaseBCan:VphaseBCan, phaseCCan:VphaseCCan,phaseAloss:VphaseAloss, phaseBloss:VphaseBloss,
            phaseCloss:VphaseCloss
          })
        }
        else
        {
          this.L_secure_data.push({linename: Vlinename, startBus: VstartBus,
            endBus: VendBus, phaseAP:VphaseAP,
            phaseBP:VphaseBP, phaseCP:VphaseCP,
            phaseAQ:VphaseAQ, phaseBQ:VphaseBQ,
            phaseCQ:VphaseCQ, phaseACu:VphaseACu, phaseBCu:VphaseBCu, phaseCCu: VphaseCCu,
            phaseACan:VphaseACan,phaseBCan:VphaseBCan, phaseCCan:VphaseCCan,phaseAloss:VphaseAloss, phaseBloss:VphaseBloss,
            phaseCloss:VphaseCloss
          })
      
      };
     }
    }

    if(keys[0]=='x.regulatorRes_name')
    {
      console.log(keys);
      for(var i=0; i<temp.length; i++) {
        var Vrename = temp[i]['x.regulatorRes_name'];
        var Vrebus = temp[i]['x.tap_bus'];
        var Vphase = temp[i]['x.phase'];
        var VresistorV = temp[i]['x.R'];
        var VreactanceV = temp[i]['x.X'];

        var VPTratio = temp[i]['x.PT'];

        var VCTcurrent = temp[i]['x.CT_rating'];

        var VCurrentS = temp[i]['x.Tap'];

        this.regulator_data.push({rename:Vrename, rebus: Vrebus,
          phase:Vphase,
          reactanceV: VreactanceV, resistorV:VresistorV,
           PTratio:VPTratio,
          CTcurrent:VCTcurrent,CurrentS:VCurrentS})
     }
    };

    if(keys[0]=='@@id')
    {
      console.log(keys);
      for(var i=0; i<temp.length; i++) {
        var VloadID = temp[i]['@@id'];
        var Vlbus = temp[i]['x.bus_name'];
        var VphaseAV = Number(temp[i]['x.VAmag']/temp[i]['x.Vbase']).toFixed(3);
        var VphaseBV = Number(temp[i]['x.VBmag']/temp[i]['x.Vbase']).toFixed(3);
        var VphaseCV = Number(temp[i]['x.VCmag']/temp[i]['x.Vbase']).toFixed(3);
        
        var VphaseAang = Number(temp[i]['x.VAang']/3.14159*180).toFixed(3);
        var VphaseBang = Number(temp[i]['x.VBang']/3.14159*180-360).toFixed(3);
        var VphaseCang = Number(temp[i]['x.VCang']/3.14159*180).toFixed(3);
        var Vunbalan = Number (temp[i]['x.DegUnbal']*100).toFixed(3)+"%";

        this.bus_data.push({loadID:VloadID, lbus: Vlbus,
          phaseAV:VphaseAV,
          phaseBV: VphaseBV, phaseCV:VphaseCV, phaseAang:VphaseAang,
          phaseBang:VphaseBang, phaseCang:VphaseCang,
          unbalan:Vunbalan
        })
        var k1=+VphaseAV;
        var k2=+VphaseBV;
        var k3=+VphaseCV;

        
        if(k1>1.15)
        {
          this.B_PAH_data.push({loadID:VloadID, lbus: Vlbus,
            phaseAV:VphaseAV,
            phaseBV: VphaseBV, phaseCV:VphaseCV, phaseAang:VphaseAang,
            phaseBang:VphaseBang, phaseCang:VphaseCang,
            unbalan:Vunbalan})
        }
        if(k1<0.85&&k1>0)
        {
          this.B_PAL_data.push({loadID:VloadID, lbus: Vlbus,
            phaseAV:VphaseAV,
            phaseBV: VphaseBV, phaseCV:VphaseCV, phaseAang:VphaseAang,
            phaseBang:VphaseBang, phaseCang:VphaseCang,
            unbalan:Vunbalan})
        }
        if(k2>1.15)
        {
          this.B_PBH_data.push({loadID:VloadID, lbus: Vlbus,
            phaseAV:VphaseAV,
            phaseBV: VphaseBV, phaseCV:VphaseCV, phaseAang:VphaseAang,
            phaseBang:VphaseBang, phaseCang:VphaseCang,
            unbalan:Vunbalan})
        }
        if(k2<0.85&&k2>0)
        {
          this.B_PBL_data.push({loadID:VloadID, lbus: Vlbus,
            phaseAV:VphaseAV,
            phaseBV: VphaseBV, phaseCV:VphaseCV, phaseAang:VphaseAang,
            phaseBang:VphaseBang, phaseCang:VphaseCang,
            unbalan:Vunbalan})
        }
        if(k3>1.15)
        {
          this.B_PCH_data.push({loadID:VloadID, lbus: Vlbus,
            phaseAV:VphaseAV,
            phaseBV: VphaseBV, phaseCV:VphaseCV, phaseAang:VphaseAang,
            phaseBang:VphaseBang, phaseCang:VphaseCang,
            unbalan:Vunbalan})
        }
        if(k3<0.85&&k3>0)
        {
          this.B_PCL_data.push({loadID:VloadID, lbus: Vlbus,
            phaseAV:VphaseAV,
            phaseBV: VphaseBV, phaseCV:VphaseCV, phaseAang:VphaseAang,
            phaseBang:VphaseBang, phaseCang:VphaseCang,
            unbalan:Vunbalan})
        }

        if(k1<1.15&&k1>0.85&&k2<1.15&&k2>0.85&&k3<1.15&&k3>0.85)
        {
          this.B_normal.push({loadID:VloadID, lbus: Vlbus,
            phaseAV:VphaseAV,
            phaseBV: VphaseBV, phaseCV:VphaseCV, phaseAang:VphaseAang,
            phaseBang:VphaseBang, phaseCang:VphaseCang,
            unbalan:Vunbalan})
        }
      }
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


  getchart(param:any): Promise<summary[]> {
    return this.http.get(param)
        .toPromise()
        .then(response => {
            const data = response.json().results;
            const res = [];
            for (const summ of data) {
              if(true)
              {
                res.push({
                    'PA': summ['x.PA'],
                    'PB': summ['x.PB'],
                    'PC': summ['x.PC'],
                    'QA': summ['x.QA'],
                    'QB': summ['x.QB'],
                    'QC': summ['x.QC'],
                });
              }
            }
  
            return res as summary[];
        })
  }
  

   GetIDList(): Promise<any>{
     return this.http.post(this.idUrl,JSON.stringify({}), { headers: this.headers })
     .toPromise().then(
       response => response.json()
     ).catch(this.handleError)
   }
   Plot1Day(nid:number, day:any): Promise<any> {
     return this.http.post(
       this.get1Day,JSON.stringify({nid:nid,day:day}),{headers:this.headers}
     ).toPromise().then(
       res=>res.json().result
     ).catch(
       this.handleError
     );
   }
   PlotTSD(nid: string, sd: number, ed: number, phaseNO: number[]): Promise<any> {
     const loadUrl = this.getTSD;
     return this.http
       .post(loadUrl, JSON.stringify({ metId: nid, sd: sd, ed: ed, phaseNO: phaseNO }), { headers: this.headers })
       .toPromise()
       .then(res => res.json().result)
       .catch(this.handleError);
   }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }

}




export interface SVCC {
  SVCname: string;
  SVCBus: number;
  SVCoutput: string;
  SVCphase:string;
}
export interface lineC {
 linename:string;
 startBus:number;
 endBus:number;
 phaseAP:string;
 phaseBP: string;
 phaseCP: string;
 phaseAQ: string;
 phaseBQ: string;
 phaseCQ: string;

 phaseACu: string;
 phaseBCu: string;
 phaseCCu: string;
 phaseACan: string;
 phaseBCan: string;
 phaseCCan: string;

 phaseAloss:string;
 phaseBloss:string;
 phaseCloss:string;

}

export interface regulatorC{
rename:string;
rebus:number;
phase:string;

resistorV:number;
reactanceV:number;

PTratio:number;
CTcurrent:number;

CurrentS:number;

}


export interface busC{
  loadID:number;
  lbus:number;
  phaseAV:string;
  phaseBV:string;
  phaseCV:string;
  phaseAang:string;
  phaseBang:string;
  phaseCang:string;
  unbalan:string;
  }

  export interface batteryC{
    baname:string;
    batype:string;
    P:number;
    Q:number;
    Qmax:number;
    Qmin:number;
    controlM:string;
    phase:string;
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
  StartTime:string;
  EndTIme:string;
  TimeStep:number;
  PFmethod:string;
  Convergence:number;
  NVoltage:number;}
