import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

import { LineResult } from '../models/lineResult';

@Injectable()
export class LineResultService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
     private lineResultUrl = environment.gsqlPre + '/query/lineResult_D';  // URL to web api
     private graphUrl = environment.gsqlPre + '/query/graph_vis_D';
    //private lineResultUrl = 'http://localhost:8080/gsqlapi/query/lineResult';  // URL to web api
    //private graphUrl = 'http://localhost:8080/gsqlapi/query/graph_vis';

    constructor(private http: Http) { }
    getLineResults(): Promise<LineResult[]> {
        return this.http.get(this.lineResultUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(data);
                console.log(res);
                for (const lineResult of data) {
                    res.push({
                        'edge_name': lineResult['x.edge_name'],
                        'tap_bus_name': lineResult['x.s_exId'],
                        'z_bus_name': lineResult['x.t_exId'],
                        'PA': Number((lineResult['x.PA']/1000).toFixed(2)),
                        'PB': Number((lineResult['x.PB']/1000).toFixed(2)),
                        'PC': Number((lineResult['x.PC']/1000).toFixed(2)),
                        'QA': Number((lineResult['x.QA']/1000).toFixed(2)),
                        'QB': Number((lineResult['x.QB']/1000).toFixed(2)),
                        'QC': Number((lineResult['x.QC']/1000).toFixed(2)),
                        'IA': Number(lineResult['x.IA'].toFixed(2)),
                        'IB': Number(lineResult['x.IB'].toFixed(2)),
                        'IC': Number(lineResult['x.IC'].toFixed(2)),
                        'IA_ang': Number(lineResult['x.IA_ang'].toFixed(2)),
                        'IB_ang': Number(lineResult['x.IB_ang'].toFixed(2)),
                        'IC_ang': Number(lineResult['x.IC_ang'].toFixed(2)),
                        'lossA': Number((lineResult['x.lossA']/1000).toFixed(4)),
                        'lossB': Number((lineResult['x.lossB']/1000).toFixed(4)),
                        'lossC': Number((lineResult['x.lossC']/1000).toFixed(4)),
                        'startnode': lineResult['x.startnode'],

                    });
                }
                console.log(res);
                return res as LineResult[];
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

