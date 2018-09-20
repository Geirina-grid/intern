import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import 'rxjs/add/operator/toPromise';
import {environment} from '../../../environments/environment';

@Injectable()
export class SeriesPowerFlowService {
  private headers = new Headers({ 'Content-Type': 'application/json' });
   private graphUrl = environment.gsqlPre + '/query/GLM_D';
   private gridlabdUrl = environment.pyPre + '/gridlabd/';
   private postgridlabdUrl = environment.pyPre + '/postgridlabd/';
   //private pfresultsUrl = environment.GLDPre + '/xml/node_1/voltage_A';

  constructor(private http: Http) { }
  runPowerFlow(params): Promise<any> {
    //let suffix = '?starttime=' + params.starttime +'&stoptime=' + params.stoptime + '&solver=' + params.solver;
    let suffix = '?starttime=' + params.starttime +'&stoptime=' + params.stoptime + '&solver=' + params.solver + '&inv_gen_mode=CONSTANT_PF'+ '&solar_gen_mode=SUPPLY_DRIVEN'+ '&cap_control=MANUAL'+ '&wind_gen_mode=CONSTANTP'+ '&bat_sch=hg'+ '&floor_area=40'+ '&heating_setpoint=69'+ '&cooling_setpoint=74'+ '&solar_panel_type=SINGLE_CRYSTAL_SILICON'+ '&solar_area=40'+ '&solar_efficiency=0.9'+ '&solar_install_type=ROOF_MOUNTED'+ '&wind_Gen_type=SYNCHRONOUS'+ '&wind_Turbine_Model=VESTAS_V82'+ '&inverter_inverter_type=PWM';
    // console.log(this.graphUrl + suffix);
    return this.http.get(this.graphUrl + suffix)
      .toPromise()
      .then(response => {
        const data = response.json().results;
        const res = [];

        return res;

  })
     .catch(this.handleError);
}

// getPFResults(): Promise<any> {
//   return this.http.get(this.pfresultsUrl)
//       .toPromise()
//       .then(response => {
//         console.log(response);
//         var parser, xmlDoc, test, ret, idx, subret;
//         test = response['_body'].split('<');
//         ret = {};
//         idx = 1;
//         for (var _i = 2; _i < test.length; _i++){
//           if (test[_i].indexOf('/') === 0) {
//             continue;
//           }
//           const temp = test[_i].split('>')
//           if (temp[0] === 'property') {
//             // start of transformation
//             subret = {};
//           } else if (temp.length > 1) {
//             subret[temp[0]] = temp[1];
//           }
//           if (temp[0] === 'value') {
//             // end of transformation
//             ret['property' + idx] = subret;
//             idx++
//           }
//         }
//         console.log(ret);
//         const data = ret.property1;
//         console.log(data);
//         const res = [];
//         return data;
//       })
//       .catch(this.handleError);
// }

rungridlabd() {
    // url, prepare vertices and edges files
    return this.http.get(this.gridlabdUrl)
      .toPromise()
      .then(response => {
        const data = response.json().results;
        const res = [];

        return res;
      })
      .catch(this.handleError);

  }

  // postgridlabd() {
  //   // url, prepare vertices and edges files
  //   return this.http.get(this.postgridlabdUrl)
  //   .toPromise()
  //   .then(response => {
  //     const data = response.json().results;
  //     const res = [];

  //     return res;
  //   })
  //   .catch(this.handleError);

  // }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }

}
