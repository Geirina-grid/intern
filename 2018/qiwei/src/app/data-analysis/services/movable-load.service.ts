import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class MovableLoadService {
  private pyPre = environment.pyPre;
  private getUserMovableLoad = environment.pyPre + '/user_movable_load/'
  private getSystemMovableLoad = environment.pyPre + '/system_movable_load/'
  private getUserRanking = environment.pyPre + '/user_diff_ranking/'
  private runAlgorithm = environment.pyPre + '/run_large_elec_algorithm/'
  private getStatistics = environment.pyPre + '/statistics_movable_load/'
  private headers = new Headers({ 'Content-Type': 'application/json' });

  constructor(private http: Http) { }

  PlotUserMovableLoad(user:number,sd:number,ed:number,algo:number,para:number,oneday:number): Promise<any> {
    const loadUrl = this.getUserMovableLoad;
    console.log("User start");
    return this.http
      .post(loadUrl, JSON.stringify({user: user, sd:sd, ed:ed, algo:algo, para:para, oneday:oneday}), { headers: this.headers })
      .toPromise()
      .then(res => {
        console.log("User end");
        return res.json().result
      })
      .catch(this.handleError);
  }

  PlotSystemMovableLoad(sd:number, ed:number, algo:number, para:number,oneday:number): Promise<any> {
    const loadUrl = this.getSystemMovableLoad;
    console.log("System start");
    return this.http
      .post(loadUrl, JSON.stringify({sd:sd, ed:ed, algo:algo, para:para, oneday:oneday }), { headers: this.headers })
      .toPromise()
      .then(res => {
        console.log("System end");
        return res.json().result
      })
      .catch(this.handleError);
  }

  PlotUserRanking(type:number,algo:number,para:number): Promise<any> {
    const loadUrl = this.getUserRanking;
    return this.http
      .post(loadUrl, JSON.stringify({type: type, algo:algo, para:para }), { headers: this.headers })
      .toPromise()
      .then(res => {
        console.log(res.json().result);
        return res.json().result
      })
      .catch(this.handleError);
  }

  PlotStatistics(sd:number, ed:number,user:number,algo:number,para:number): Promise<any> {
    const loadUrl = this.getStatistics;
    return this.http
      .post(loadUrl, JSON.stringify({sd:sd, ed:ed,user: user, algo:algo, para:para }), { headers: this.headers })
      .toPromise()
      .then(res => {
        console.log(res.json().result);
        return res.json().result
      })
      .catch(this.handleError);
  }

  RunAlgorithm(chosenprocess: any): Promise<any> {
    const loadUrl = this.runAlgorithm;
    return this.http
      .post(loadUrl, JSON.stringify({ processcount:chosenprocess }), { headers: this.headers })
      .toPromise()
      .then(res => {
        console.log(res.json().result);
        console.log(chosenprocess);
        return res.json().result
      })
      .catch(this.handleError);
  }

  le_dat(nid: any, rt: any, sd: any, ed:any): Promise<any> {
    const tempUrl = this.pyPre + '/le_service/';
    return this.http
      .post(tempUrl, JSON.stringify({ nid: nid, rt: rt, sd:sd, ed:ed }), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}
