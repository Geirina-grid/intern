import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class TimeseriesVisService {
  private getTSD = environment.pyPre + '/plot_from_gsql_TSD/'
  private get1Day = environment.pyPre + '/plot_from_gsql_TSD1day/'
  private idUrl = environment.pyPre + '/dq_id/'
  private headers = new Headers({ 'Content-Type': 'application/json' });
  constructor(private http: Http) { }

  Nonservice(hist:any):Promise<any>{
    return new Promise((resolve)=>{
      resolve({hist:hist});
    })
  }

  GetIDList(): Promise<any>{
    return this.http.post(this.idUrl,JSON.stringify({}), { headers: this.headers })
    .toPromise().then(
      response => response.json()
    ).catch(this.handleError)
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
  PlotTSD(nid: number, sd: number, ed: number, phaseNO: number[]): Promise<any> {
    const loadUrl = this.getTSD;
    return this.http
      .post(loadUrl, JSON.stringify({ metId: nid, sd: sd, ed: ed, phaseNO: phaseNO}), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}
