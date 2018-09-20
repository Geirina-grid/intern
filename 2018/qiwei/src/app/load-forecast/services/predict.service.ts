import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';


@Injectable()
export class PredictService {
  private pyPre = environment.pyPre;
  // private predictUrl = 'http://localhost:5000/api/v0/predict/';
  private headers = new Headers({ 'Content-Type': 'application/json' });

  constructor(private http: Http) { }

  Predict(modelid: string, date: string): Promise<any> {
    const predictUrl = this.pyPre + '/predict/';
    return this.http
      .post(predictUrl, JSON.stringify({ modelid: modelid, date: date }), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }

  TrainPredict(modelid: string, train_start: string, train_end: string): Promise<any> {
    const predictUrl = this.pyPre + '/modelplot/';
    return this.http
      .post(predictUrl, JSON.stringify({ modelid: modelid, train_start: train_start, train_end: train_end}), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}

