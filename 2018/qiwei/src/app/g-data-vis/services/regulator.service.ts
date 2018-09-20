import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

import { Regulator } from '../models/regulator';

@Injectable()
export class RegulatorService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
     private regulatorUrl = environment.gsqlPre + '/query/regulator_D';  // URL to web api
     private graphUrl = environment.gsqlPre + '/query/graph_vis_D';
    //private regulatorUrl = 'http://localhost:8080/gsqlapi/query/regulator';  // URL to web api
   // private graphUrl = 'http://localhost:8080/gsqlapi/query/graph_vis';
    // private vertexUrl = 'http://localhost:4999/api/v0/fakegsql/'
    constructor(private http: Http) { }
    getRegulators(): Promise<Regulator[]> {
        return this.http.get(this.regulatorUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(data);
                console.log(res);
                for (const reg of data) {
                    res.push({
                        'regulator_name': reg['x.regulator_name'],
                        'tap_bus': reg['x.tap_bus'],
                        'phase': reg['x.phase'],
                        'voltage_level': reg['x.voltage_level'],
                        'R': reg['x.R'],
                        'X': reg['x.X'],
                        'Tap_max': reg['x.Tap_max'],
                        'Tap_min': reg['x.Tap_min'],
                        'Tap_range': reg['x.Tap_range'],
                        'PT': reg['x.PT'],
                        'CT_rating': reg['x.CT_rating'],
                        'bandwidth': reg['x.bandwidth'],
                        'control_phase': reg['x.control_phase_reg'],
                        'startnode': reg['x.startnode']
                    });
                }
                console.log(res);
                return res as Regulator[];
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
