import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class BigDataService {
  private getMonth = environment.pyPre + '/plot_from_gsql_month/'
  private getDay = environment.pyPre + '/plot_from_gsql_day/'
  private getHour = environment.pyPre + '/plot_from_gsql_hour/'
  private getEve = environment.pyPre + '/plot_from_gsql_every/'
  private get1Day = environment.pyPre + '/plot_from_gsql_1day/'
  private headers = new Headers({ 'Content-Type': 'application/json' });
  constructor(private http: Http) { }

  Nonservice(hist:any):Promise<any>{
    return new Promise((resolve)=>{
      resolve({hist:hist});
    })
  }

  Plot1Day(nid:number, day:any): Promise<any> {
    return this.http.post(
      this.get1Day,JSON.stringify({nid:nid,day:day}),{headers:this.headers}
    ).toPromise().then(
      res=>res.json().result
    ).catch(
      this.handleError
    );
  }
  PlotMonth(nid: number, rt: any): Promise<any> {
    const loadUrl = this.getMonth;
    return this.http
      .post(loadUrl, JSON.stringify({ metId: nid , rt : rt}), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  PlotEvery(nid: number, sd: number, ed: number): Promise<any> {
    const loadUrl = this.getEve;
    return this.http
      .post(loadUrl, JSON.stringify({ metId: nid, sd: sd, ed: ed }), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  PlotDay(nid: number, rt: any, month: any): Promise<any> {
    const loadUrl = this.getDay;
    return this.http
      .post(loadUrl, JSON.stringify({ metId: nid , rt : rt, month: month}), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  PlotHour(nid: number, rt: any, month: any, day: number): Promise<any> {
    const loadUrl = this.getHour;
    return this.http
      .post(loadUrl, JSON.stringify({ metId: nid , rt : rt, month: month, day: day}), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}
