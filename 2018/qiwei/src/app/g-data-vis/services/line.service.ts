import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

import { Line } from '../models/line';
@Injectable()
export class LineService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
     private lineUrl = environment.gsqlPre + '/query/line_D';  // URL to web api
     private graphUrl = environment.gsqlPre + '/query/graph_vis_D';
   // private lineUrl = 'http://localhost:8080/gsqlapi/query/line';  // URL to web api
  //  private graphUrl = 'http://localhost:8080/gsqlapi/query/graph_vis';
    constructor(
        private http: Http) {
    }
    getLines(): Promise<Line[]> {
        return this.http.get(this.lineUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                for (const line of data) {
                    res.push({
                        'edge_name': line['x.edge_name'],
                        'tap_bus_id': line['x.s_exId'],
                        'z_bus_id': line['x.t_exId'],
                        'linelength': line['x.linelength'],
                        /*
                        'Raa': line['x.RA'],
                        'Rbb': line['x.RB'],
                        'Rcc': line['x.RC'],
                        'Xaa': line['x.XA'],
                        'Xbb': line['x.XB'],
                        'Xcc': line['x.XC'],
                        'Xab': line['x.Xab'],
                        'Xac': line['x.Xac'],
                        'Xbc': line['x.Xbc'],
                        */
                        'Raa': Number(line['x.RA']).toFixed(5),
                        'Rbb': Number(line['x.RB']).toFixed(5),
                        'Rcc': Number(line['x.RC']).toFixed(5),
                        'Xaa': Number(line['x.XA']).toFixed(5),
                        'Xbb': Number(line['x.XB']).toFixed(5),
                        'Xcc': Number(line['x.XC']).toFixed(5),
                        'Xab': Number(line['x.Xab']).toFixed(5),
                        'Xac': Number(line['x.Xac']).toFixed(5),
                        'Xbc': Number(line['x.Xbc']).toFixed(5),
                        'startnode': line['x.startnode'],
                    });
                }
                return res as Line[];
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

