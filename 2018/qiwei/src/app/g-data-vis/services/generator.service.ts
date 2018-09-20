import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

import { Generator } from '../models/generator';

@Injectable()
export class GeneratorService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
     private generatorUrl = environment.gsqlPre + '/query/generator_D';  // URL to web api
     private graphUrl = environment.gsqlPre + '/query/graph_vis_D';
    //private generatorUrl = 'http://localhost:8080/gsqlapi/query/generator';  // URL to web api
    //private graphUrl = 'http://localhost:8080/gsqlapi/query/graph_vis';

    // private vertexUrl = 'http://localhost:4999/api/v0/fakegsql/'
    constructor(private http: Http) { }
    getGenerators(): Promise<Generator[]> {
        return this.http.get(this.generatorUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                // console.log(data);
                // console.log(res);
                for (const gen of data) {
                    res.push({
                        // 'id': gen['@@id'],
                        'UN_ID': gen['x.bus_name'],
                        'Bus': gen['x.exId'],
                        'Pg': Number(gen['GenP'].toFixed(2)),
                        'Qg':Number(gen['GenQ'].toFixed(2)),
                        'Qmax': Number(gen['qUp'].toFixed(2)),
                        'Qmin': Number(gen['qLower'].toFixed(2)),
                        'Reg_bus': gen['x.control_bus_number'],
                        'desired_voltage': Number(gen['desired_volts'].toFixed(2)),
                    });
                }
                // console.log(res);
                return res as Generator[];
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

