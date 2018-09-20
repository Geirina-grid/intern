import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

import { Summary } from '../models/summary';

@Injectable()
export class SummaryService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
     private summaryUrl = environment.gsqlPre + '/query/summary_D';  // URL to web api
   //  private graphUrl = 'http://192.168.4.2:9000/query/graph_vis';
     private graphUrl = environment.gsqlPre + '/query/graph_vis_D';
    //private summaryUrl = 'http://localhost:8080/gsqlapi/query/summary';  // URL to web api
   // private graphUrl = 'http://localhost:8080/gsqlapi/query/graph_vis';

    constructor(private http: Http) { }
    getSummarys(): Promise<Summary[]> {
        return this.http.get(this.summaryUrl)
            .toPromise()
            .then(response => {
                const data = response.json().results;
                const res = [];
                console.log(data);
                console.log(res);
                for (const summ of data) {
                    res.push({
                        'Sys_id': summ['@@SYSid'],
                        'NoBS': summ['@@BSid'],
                        'NoLN': summ['@@LNid'],
                        'NoCap': summ['@@Capid'],
                        'NoXF': summ['@@XFid'],
                        'NoLD': summ['@@LDid'],
                        'NoGen': summ['@@UNid'],
                        'DegUnbal': Number(summ['@@DegUnbal'].toFixed(2)),
                        'T_Load': Number(summ['@@TotalLoad'].toFixed(2)),
                        'T_Loss': Number(summ['@@TotalLoss'].toFixed(2)),
                    });
                }
                console.log(res);
                return res as Summary[];
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

