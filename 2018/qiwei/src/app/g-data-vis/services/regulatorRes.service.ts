import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

import { RegulatorRes } from '../models/regulatorRes';

@Injectable()
export class RegulatorResService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
     private regulatorResUrl = environment.gsqlPre + '/query/regulatorRes_D';  // URL to web api
     private graphUrl = environment.gsqlPre + '/query/graph_vis_D';
    //private regulatorResUrl = 'http://localhost:8080/gsqlapi/query/regulatorRes';  // URL to web api
   // private graphUrl = 'http://localhost:8080/gsqlapi/query/graph_vis';
    // private vertexUrl = 'http://localhost:4999/api/v0/fakegsql/'
    constructor(private http: Http) { }
    getRegulatorRess(): Promise<RegulatorRes[]> {
        return this.http.get(this.regulatorResUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(data);
                console.log(res);
                for (const reg of data) {
                    res.push({
                        'regulatorRes_name': reg['x.regulatorRes_name'],
                        'tap_bus': reg['x.tap_bus'],
                        'phase': reg['x.phase'],
                        'voltage_level': reg['x.voltage_level'],
                        'R': reg['x.R'],
                        'X': reg['x.X'],
                        'PT': reg['x.PT'],
                        'CT_rating': reg['x.CT_rating'],                           
                        'Tap': reg['x.Tap'],
                        'startnode': reg['x.startnode']
                    });
                }
                console.log(res);
                return res as RegulatorRes[];
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
