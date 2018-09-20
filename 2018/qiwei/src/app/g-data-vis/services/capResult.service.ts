import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

import { CapResult } from '../models/capResult';

@Injectable()
export class CapResultService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
     private capResultUrl = environment.gsqlPre + '/query/capResult_D';  // URL to web api
     private graphUrl = environment.gsqlPre + '/query/graph_vis_D';
    //private capResultUrl = 'http://localhost:8080/gsqlapi/query/capResult';  // URL to web api
    //private graphUrl = 'http://localhost:8080/gsqlapi/query/graph_vis';

    // private vertexUrl = 'http://localhost:4999/api/v0/fakegsql/'
    constructor(private http: Http) { }
    getCapResults(): Promise<CapResult[]> {
        return this.http.get(this.capResultUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(data);
                console.log(res);
                for (const cap of data) {
                    res.push({
                        // 'id': cap['@@id'],
                        'CP_ID': cap['x.CapID'],
                        'Bus': cap['x.bus_name'],
                      //  'Q': Number((cap['x.QCap_cal']*5000).toFixed(2)),
                        'Q': Number((cap['x.QCap_cal']/1000).toFixed(2)),
                        'phase': cap['x.Phase'],
                        'startnode': cap['x.startnode'],
                    });
                }
                console.log(res);
                return res as CapResult[];
            })
            .catch(this.handleError);
    }

    getGraph(): Promise<any> {
        return this.http.get(this.graphUrl)
            .toPromise()
            .then(response => response.json().results)
            .catch(this.handleError);
    }
    private handleError(error: any): Promise<any> {
        console.error('An error occurred', error); // for demo purposes only
        return Promise.reject(error.message || error);
    }
}
