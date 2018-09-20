import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

import { Welcome } from '../models/welcome';

@Injectable()
export class WelcomeService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
     private graphUrl = environment.gsqlPre + '/query/graph_vis_D';
     private welcomeUrl = environment.gsqlPre + '/query/welcome_D';  // URL to web api


    constructor(private http: Http) { }


    getGraph(): Promise<any> {
        return this.http.get(this.graphUrl)
            .toPromise()
            .then(response =>{
                console.log(response.json().results);
                return response.json().results;
            } )
            .catch(this.handleError);
    }
    getWelcome(params:any): Promise<any> {
        let suffix = '?endnode=' + params.endnode;
        return this.http.get(this.welcomeUrl+suffix)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(data);
                res.push(data[0]['@@phaseLoadP']);
                res.push(data[1]['@@phaseLossP']);
                res.push(data[2]['startnode']);
                res.push(data[3]['@@phaseAVoltage']);
                res.push(data[4]['@@phaseBVoltage']);
                res.push(data[5]['@@phaseCVoltage']);
                res.push(data[6]['@@phaseALoad']);
                res.push(data[7]['@@phaseBLoad']);
                res.push(data[8]['@@phaseCLoad']);
                res.push(data[9]['@@phaseABLoad']);
                res.push(data[10]['@@phaseBCLoad']);
                res.push(data[11]['@@phaseCALoad']);
                res.push(data[12]['@@phaseAPower']);
                res.push(data[13]['@@phaseBPower']);
                res.push(data[14]['@@phaseCPower']);
                res.push(data[15]['@@V_unbal']);
                res.push(data[16]['@@Nodeindex']);
                res.push(data[17]['@@Edgeindex']);
                res.push(data[18]['@@LineLengthindex']);
                res.push(data[19]['@@LineLengthindex_line']);
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

