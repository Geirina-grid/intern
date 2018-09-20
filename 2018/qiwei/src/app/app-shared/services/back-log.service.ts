import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class BackLogService {
  constructor(private http: Http) { }

  getlog(file: string): Promise<any> {
    const logUrl = environment.pyPre + '/log/' + file;
    return this.http.get(logUrl)
      .toPromise()
      .then(response => response.json().result)
      .catch(this.handleError);
  }
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}
