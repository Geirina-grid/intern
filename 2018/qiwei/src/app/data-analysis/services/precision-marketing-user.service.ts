import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class PrecisionMarketingUserService {
  private getUserElasticity = environment.pyPre + '/prec_market_user_elasticity/'
  private getUserEntropy = environment.pyPre + '/prec_market_user_entropy/'
  private getUserLoad = environment.pyPre + '/prec_market_user_load/'
  private getSystemLoad = environment.pyPre + '/prec_market_system_load/'
  private getUserAnnulElasticity = environment.pyPre + '/prec_market_user_annul_elasticity/'
  private headers = new Headers({ 'Content-Type': 'application/json' });
  constructor(private http: Http) { }

  PlotUserElasticity(): Promise<any> {
    const loadUrl = this.getUserElasticity;
    return this.http
      .post(loadUrl, JSON.stringify({ }), { headers: this.headers })
      .toPromise()
      .then(res => {
        console.log(res.json().result);
        return res.json().result
      })
      .catch(this.handleError);
  }
  PlotUserEntropy(): Promise<any> {
    const loadUrl = this.getUserEntropy;
    return this.http
      .post(loadUrl, JSON.stringify({ }), { headers: this.headers })
      .toPromise()
      .then(res => {
        console.log(res.json().result);
        return res.json().result
      })
      .catch(this.handleError);
  }
  PlotUserAnnulElasticity(user:number): Promise<any> {
    const loadUrl = this.getUserAnnulElasticity;
    return this.http
      .post(loadUrl, JSON.stringify({user: user }), { headers: this.headers })
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
  PlotUserLoad(user:number, day: number): Promise<any> {
    const loadUrl = this.getUserLoad;
    return this.http
      .post(loadUrl, JSON.stringify({user:user, date: day}), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}
