import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class PreprocessService {

  private pyPre = environment.pyPre;
  // private preprocessUrl = 'http://localhost:5000/api/v0/preprocess/';
  private headers = new Headers({ 'Content-Type': 'application/json' });

  constructor(private http: Http) { }

  dataPreprocess(id: number, start: string, end: string, outliermin: number, outliermax:number, 
  numnorm:string, catnorm:string, calendarfea:any[], weatherfea:any[], prevhr:string, prevhravg:string, prevhrmaxmin:string): Promise<any> {
    const preprocessUrl = this.pyPre + '/preprocess/';
    return this.http
      // tslint:disable-next-line:max-line-length
      .post(preprocessUrl, JSON.stringify({ id: id, startdate: start, enddate: end, outliermin: outliermin, outliermax:outliermax, 
        numnorm:numnorm, catnorm:catnorm, calendarfea:calendarfea, weatherfea:weatherfea, prevhr:prevhr, prevhravg:prevhravg, prevhrmaxmin:prevhrmaxmin}), { headers: this.headers })
      .toPromise()
      .then(res => {return res.json()})
      .catch(this.handleError);
  }



  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}

