import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';
import 'rxjs/add/operator/toPromise';

@Injectable()
export class RetrieveCaseService {
  private headers = new Headers({ 'Content-Type': 'application/json' });
  private retrieveUrl = environment.gsqlPre + '/query/retrieve/';

  constructor(private http: Http) { }

  retrieveCase(payload: any): Promise<any> {
    // console.log(this.headers.toJSON());
    return this.http.post(this.retrieveUrl, payload, { headers: this.headers })
    .toPromise()
    .then(responce => responce.json())
    .catch(this.handleError);
  }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }

}
