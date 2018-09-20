import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

import { LoadDisplay } from '../models/loadDisplay';
import { CustomerDisplay } from '../models/customerDisplay';


@Injectable()
export class LoadDisplayService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
    private loadDisplayUrl = environment.gsqlPre + '/query/loadDisplay_D';  // URL to web api
    private customerDisplayUrl = environment.gsqlPre + '/query/customerDisplay_D';  // URL to web api
    private graphUrl = environment.gsqlPre + '/query/graph_vis_D';


    constructor(private http: Http) { }
    getLoadDisplays(): Promise<LoadDisplay[]> {
        return this.http.get(this.loadDisplayUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(data);
                console.log(res);
                for (const loadDis of data) {
                    res.push({
                        'id': loadDis['@@id'],
                        'Bus_name': loadDis['x.exId'],
                        'PdA': Number((loadDis['x.PdA']/1000).toFixed(2)),
                        'QdA': Number((loadDis['x.QdA']/1000).toFixed(2)),
                        'PdB': Number((loadDis['x.PdB']/1000).toFixed(2)),
                        'QdB': Number((loadDis['x.QdB']/1000).toFixed(2)),
                        'PdC': Number((loadDis['x.PdC']/1000).toFixed(2)),
                        'QdC': Number((loadDis['x.QdC']/1000).toFixed(2)),
                        'startnode': loadDis['x.startnode'],                     
                    });
                }
                //   console.log(res);
                return res as LoadDisplay[];
            })
            .catch(this.handleError);
    }

    getCustomerDisplays(params): Promise<CustomerDisplay[]> {
        let suffix = '?loadpoint='+ params.loadpoint;
        return this.http.get(this.customerDisplayUrl + suffix)
        .toPromise()
        .then(response => {
            const data = response.json().results;
            const res = [];
            console.log(data);
            console.log(res);
            for (const loadDis of data) {
                res.push({
                    'customer': loadDis['x.exId'],
                    'load_point': loadDis['x.load_point'],
                    'phase_cust': loadDis['x.phase_cust'],
                    'PcA': Number(loadDis['x.PcA'].toFixed(3)),
                    'QcA': Number(loadDis['x.QcA'].toFixed(3)),
                    'PcB': Number(loadDis['x.PcB'].toFixed(3)),
                    'QcB': Number(loadDis['x.QcB'].toFixed(3)),
                    'PcC': Number(loadDis['x.PcC'].toFixed(3)),
                    'QcC': Number(loadDis['x.QcC'].toFixed(3)),                  
                });
            }
            return res as CustomerDisplay[];
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

