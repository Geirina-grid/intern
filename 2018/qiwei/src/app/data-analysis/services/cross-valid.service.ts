import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class CrossValidService {
  private pe = environment.pyPre + '/cross_val_pe/'
  private pqui = environment.pyPre + '/cross_val_pqui/'
  private cross_valid_algo = environment.pyPre + '/cross_valid_algo/'
  private cross_valid_ui_pq = environment.pyPre + '/plot_ui_pq/'
  private cross_valid_pf_ps = environment.pyPre + '/plot_pf_ps/'
  private cross_valid_load_energy = environment.pyPre + '/plot_load_energy/'
  private plot_corss_valid_ranking = environment.pyPre + '/plot_corss_valid_ranking/'
  private headers = new Headers({ 'Content-Type': 'application/json' });
  constructor(private http:Http) { }

  p_e_com(nid:number, sd:number, ed:number, rtype:any): Promise<any>{
    const loadUrl = this.pe;
    return this.http
    .post(loadUrl,JSON.stringify({metId:nid, sd:sd, ed:ed, rt:rtype}), { headers: this.headers })
    .toPromise()
    .then(res => res.json().result)
    .catch(this.handleError);
  }

  pq_ui_com(nid:number, sd:number, ed:number, rtype:any): Promise<any>{
    const loadUrl = this.pqui;
    return this.http
    .post(loadUrl,JSON.stringify({metId:nid, sd:sd, ed:ed, rt:rtype}), { headers: this.headers })
    .toPromise()
    .then(res => res.json().result)
    .catch(this.handleError);
  }

  PlotUIPG(startdate: number, enddate: number,user:number, rtype:number, oneday:number): Promise<any>{
    const loadUrl = this.cross_valid_ui_pq;
    return this.http
    .post(loadUrl,JSON.stringify({startdate:startdate, enddate:enddate, user:user, rtype:rtype, oneday:oneday}), { headers: this.headers })
    .toPromise()
    .then(res => res.json().result)
    .catch(this.handleError);
  }

  PlotPFPS(startdate: number, enddate: number,user:number, rtype:number, oneday:number): Promise<any>{
    const loadUrl = this.cross_valid_pf_ps;
    return this.http
    .post(loadUrl,JSON.stringify({startdate:startdate, enddate:enddate, user:user, rtype:rtype, oneday:oneday}), { headers: this.headers })
    .toPromise()
    .then(res => res.json().result)
    .catch(this.handleError);
  }

  PlotLoadEnergy(startdate: number, enddate: number,user:number, oneday:number): Promise<any>{
    const loadUrl = this.cross_valid_load_energy;
    return this.http
    .post(loadUrl,JSON.stringify({startdate:startdate, enddate:enddate, user:user, oneday:oneday}), { headers: this.headers })
    .toPromise()
    .then(res => res.json().result)
    .catch(this.handleError);
  }

  CrossValidAlgo(): Promise<any>{
    const loadUrl = this.cross_valid_algo;
    return this.http
    .post(loadUrl,JSON.stringify({}), { headers: this.headers })
    .toPromise()
    .then(res => res.json().result)
    .catch(this.handleError);
  }

  PlotUserRanking(algo:number, type:number): Promise<any>{
    const loadUrl = this.plot_corss_valid_ranking;
    return this.http
    .post(loadUrl,JSON.stringify({algo:algo, type:type}), { headers: this.headers })
    .toPromise()
    .then(res => res.json().result)
    .catch(this.handleError);
  }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }  
}
