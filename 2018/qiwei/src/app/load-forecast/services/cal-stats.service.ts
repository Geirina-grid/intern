import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';


@Injectable()
export class CalStatsService {

  private predictUrl = environment.pyPre + '/calstats/';
  private corrUrl = environment.pyPre + '/calcorr/';
  private headers = new Headers({ 'Content-Type': 'application/json' });

  constructor(private http: Http) { }

  calStats(ape: any[], ae: any[]): Promise<any> {
    return this.http
      .post(this.predictUrl, JSON.stringify({ ape: ape, ae: ae }), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }

  calCorr(data:any[]): Promise<any> {
    return this.http
      .post(this.corrUrl, JSON.stringify({ data:data }), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}


