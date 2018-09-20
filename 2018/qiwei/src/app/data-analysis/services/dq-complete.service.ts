import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';
import 'rxjs/add/operator/toPromise';

@Injectable()
export class DqCompleteService {
  private headers = new Headers({ 'Content-Type': 'application/json' });
  private peak = environment.pyPre+'/le_service_peak/'
  constructor(private http:Http) { }

  get_peak():Promise<any>{
    return this.http.post(this.peak,JSON.stringify({nothing:"nothing"}), { headers: this.headers })
    .toPromise()
    .then(response => {
      return response.json().result;
    })
    .catch(this.handleError);
  }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}
