import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class LeafletMapLatLngService {
  private getMapLatLng = environment.pyPre + '/getLanlng'
  private headers = new Headers({ 'Content-Type': 'application/json' });
  constructor(private http: Http) { }

  GetMapLatLng(): Promise<any> {
    const loadUrl = this.getMapLatLng;
    return this.http
      .post(loadUrl, JSON.stringify({ }), { headers: this.headers })
      .toPromise()
      .then(res => {
        console.log(res.json());
        return res.json()
      })
      .catch(this.handleError);
  }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}
