import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class PrecisionMarketingService {
  private runElasticity = environment.pyPre + '/run_cluster_elasticity/'
  private getElasticity = environment.pyPre + '/prec_market_elasticity/'
  private getSystemLoad = environment.pyPre + '/prec_market_system_load/'
  private getClusterElasticity = environment.pyPre + '/prec_market_cluster_elasticity/'
  private headers = new Headers({ 'Content-Type': 'application/json' });
  constructor(private http: Http) { }

  RunElasticity(selectedYear: number): Promise<any> {
    const loadUrl = this.runElasticity;
    return this.http
      .post(loadUrl, JSON.stringify({ year: selectedYear }), { headers: this.headers })
      .toPromise()
      .then(res => {
        return res.json().result
      })
      .catch(this.handleError);
  }
  PlotElasticity(): Promise<any> {
    const loadUrl = this.getElasticity;
    return this.http
      .post(loadUrl, JSON.stringify({ }), { headers: this.headers })
      .toPromise()
      .then(res => {
        console.log(res.json().result);
        return res.json().result
      })
      .catch(this.handleError);
  }
  PlotClusterElasticity(clus:number): Promise<any> {
    const loadUrl = this.getClusterElasticity;
    return this.http
      .post(loadUrl, JSON.stringify({cluster: clus }), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  PlotSystemLoad(day: number): Promise<any> {
    const loadUrl = this.getSystemLoad;
    return this.http
      .post(loadUrl, JSON.stringify({ date: day}), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}
