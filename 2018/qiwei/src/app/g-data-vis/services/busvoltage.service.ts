import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

import { Busvoltage } from '../models/busvoltage';

@Injectable()
export class BusvoltageService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
     private busvoltageUrl = environment.gsqlPre + '/query/busvoltage_D';  // URL to web api
     private graphUrl = environment.gsqlPre + '/query/graph_vis_D';
    //private busvoltageUrl = 'http://localhost:8080/gsqlapi/query/busvoltage';  // URL to web api
    //private graphUrl = 'http://localhost:8080/gsqlapi/query/graph_vis';


    constructor(private http: Http) { }
    getBusvoltages(): Promise<Busvoltage[]> {
        return this.http.get(this.busvoltageUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(data);
                console.log(res);
                for (const busvol of data) {
                    res.push({
                        'Bus_name': busvol['x.bus_name'],
                        'VAmag': Number((busvol['x.VAmag']/1000).toFixed(3)),
                        'VBmag': Number((busvol['x.VBmag']/1000).toFixed(3)),
                        'VCmag': Number((busvol['x.VCmag']/1000).toFixed(3)),
                        'VAang': Number((busvol['x.VAang']/3.14159*180).toFixed(2)),
                        'VBang': Number((busvol['x.VBang']/3.14159*180-360).toFixed(2)),
                        'VCang': Number((busvol['x.VCang']/3.14159*180).toFixed(2)),
                        'DegUnbal': (busvol['x.DegUnbal']*100).toFixed(2)+"%",
                        'startnode': busvol['x.startnode'],

                    });
                }
                //   console.log(res);
                return res as Busvoltage[];
            })
            .catch(this.handleError);
    }

    getBusvoltagespu(): Promise<Busvoltage[]> {
        return this.http.get(this.busvoltageUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(data);
                console.log(res);
                for (const busvol of data) {
                    res.push({
                        'Bus_name': busvol['x.bus_name'],
                        'VAmag': Number((busvol['x.VAmag']/busvol['x.Vbase']).toFixed(4)),
                        'VBmag': Number((busvol['x.VBmag']/busvol['x.Vbase']).toFixed(4)),
                        'VCmag': Number((busvol['x.VCmag']/busvol['x.Vbase']).toFixed(4)),
                        'VAang': Number((busvol['x.VAang']/3.14159*180).toFixed(2)),
                        'VBang': Number((busvol['x.VBang']/3.14159*180-360).toFixed(2)),
                        'VCang': Number((busvol['x.VCang']/3.14159*180).toFixed(2)),
                        'DegUnbal': (busvol['x.DegUnbal']*100).toFixed(2)+"%",
                        'startnode': busvol['x.startnode'],

                    });
                }
                //   console.log(res);
                return res as Busvoltage[];
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

