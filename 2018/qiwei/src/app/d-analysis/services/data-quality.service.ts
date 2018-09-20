import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';
import 'rxjs/add/operator/toPromise';

@Injectable()
export class DataQualityService {
  private headers = new Headers({ 'Content-Type': 'application/json' });
  private runUrl = environment.pyPre + '/dq_run/'
  private disUrl = environment.pyPre + '/dq/'
  private idUrl = environment.pyPre + '/dq_id/'

  constructor(private http:Http) { 

  }

  rundq(runtype: any): Promise<any> {
    let reqUrl = this.runUrl;
    return this.http.post(reqUrl,JSON.stringify({runtype:runtype}), { headers: this.headers })
      .toPromise()
      .then(response => response.json().results)
      .catch(this.handleError);
  }

  dqimprove(runtype: any, startdate: number, enddate: number, meterid: any, phase: any): Promise<any>{
    return this.http.post(this.disUrl,JSON.stringify({runtype:runtype, startdate:startdate, enddate:enddate, meterid:meterid, phase:phase}), { headers: this.headers })
    .toPromise().then(
      response => response.json()
    ).catch(this.handleError)
  }

  GetIDList(): Promise<any>{
    return this.http.post(this.idUrl,JSON.stringify({}), { headers: this.headers })
    .toPromise().then(
      response => response.json()
    ).catch(this.handleError)
  }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }

}
