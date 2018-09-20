import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';
import 'rxjs/add/operator/toPromise';


@Injectable()
export class DbinquiryService {

  private headers = new Headers({ 'Content-Type': 'application/json' });
  private getTableColsUrl = environment.pyPre + '/gettablecols/';

  constructor(private http: Http) { 
    
  }

  getTableCols(tablename: String): Promise<any[]> {
    const url = `${this.getTableColsUrl}${tablename}`;
    return this.http.get(url)
        .toPromise()
        .then(response => response.json().result)
        .catch(this.handleError);
    }
  
  getNormInfo(normalizerid: String): Promise<any[]> {
    const url = `${environment.pyPre + '/getnorminfo/'}${normalizerid}`;
    return this.http.get(url)
        .toPromise()
        .then(response => response.json().result)
        .catch(this.handleError);
    }
  
  getModelInfo(modelid: String): Promise<any[]> {
    const url = `${environment.pyPre + '/getmodelinfo/'}${modelid}`;
    return this.http.get(url)
        .toPromise()
        .then(response => response.json().result)
        .catch(this.handleError);
    }
    
  getPrevPredict(meterid: String): Promise<any> {
    const url = `${environment.pyPre + '/getprevpredict/'}${meterid}`;
    return this.http.get(url)
        .toPromise()
        .then(response => response.json().result)
        .catch(this.handleError);
    }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
}

}
