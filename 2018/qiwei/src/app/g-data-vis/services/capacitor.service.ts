import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';

import 'rxjs/add/operator/toPromise';

import { Capacitor } from '../models/capacitor';
import { environment } from '../../../environments/environment';

@Injectable()
export class CapacitorService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
    //  private capacitorUrl = 'http://192.168.2.3:9000/query/capacitor';  // URL to web api
    //  private graphUrl = 'http://192.168.2.3:9000/query/graph_vis';
    private gsqlPre = environment.gsqlPre;
    private graphUrl = environment.gsqlPre + '/query/graph_vis_D';
    // private vertexUrl = 'http://localhost:4999/api/v0/fakegsql/'
    constructor(private http: Http) { }
    getCapacitors(): Promise<Capacitor[]> {
        const capacitorUrl = this.gsqlPre + '/query/capacitor_D';
        return this.http.get(capacitorUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                // console.log(data);
                // console.log(res);
                for (const cap of data) {
                    res.push({
                        'Capid': cap['x.CapID'],
                        'bus_name': cap['x.bus_name'],
                     //   'Qcap': cap['x.QCap'] *5000,
                        'Qcap': cap['x.QCap'] /1000,
                        'control': cap['x.control'],
                        'phase': cap['x.Phase'],
                        'startnode': cap['x.startnode']
                    });
                }
                return res as Capacitor[];
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
