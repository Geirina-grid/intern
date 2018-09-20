import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import 'rxjs/add/operator/toPromise';
import {environment} from '../../../environments/environment';

@Injectable()
export class PowerFlowService {
  private headers = new Headers({ 'Content-Type': 'application/json' });
   private graphUrl = environment.gsqlPre + '/query/BFS_D';
 // private graphUrl = 'http://localhost:8080/gsqlapi/query/graph_LU_PVPQ_vis';

  constructor(private http: Http) { }
  runPowerFlow(params): Promise<any> {
    let suffix = '?maxIt=' + params.iteration +'&threshold=' + params.maxchange;
    // console.log(this.graphUrl + suffix);
    return this.http.get(this.graphUrl + suffix)
    // return this.http.get(this.graphUrl)
      .toPromise()
      .then(response => {
        const data = response.json().results;
        const res = [];
        res.push(data[0]['@@numBackCompute']);
        res.push(data[1]['Accuracy']);
        res.push(data[2]['VNbase']);
        console.log(res);
        return res;
      
  })
     .catch(this.handleError);
}
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }

}
