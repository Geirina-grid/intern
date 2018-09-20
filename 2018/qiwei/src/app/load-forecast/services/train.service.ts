import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';


@Injectable()
export class TrainService {

  private pyPre = environment.pyPre;
  // private trainUrl = 'http://localhost:5000/api/v0/train/';
  private headers = new Headers({ 'Content-Type': 'application/json' });

  constructor(private http: Http) { }

  Train(normalizer: string, model: string, traintime:number, crossvalidation:string, njobs:number): Promise<any> {
    const trainUrl = this.pyPre + '/train/';
    return this.http
      .post(trainUrl, JSON.stringify({ normalizer: normalizer, model: model,  traintime:traintime, crossvalidation:crossvalidation, njobs:njobs}), { headers: this.headers })
      .toPromise()
      .then(res => { console.log(res.json()); return res.json() })
      .catch(this.handleError);
  }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}

