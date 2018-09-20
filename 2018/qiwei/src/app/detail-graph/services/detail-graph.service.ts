import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';
import 'rxjs/add/operator/toPromise';

@Injectable()
export class DetailGraphService {
  private headers = new Headers({ 'Content-Type': 'application/json' });
  private graphUrl = environment.detailPre + '/query/graph_vis'
  private detailUrl = environment.detailPre + '/query/SubDisplay';

  constructor(private http: Http) { }

  getDetail(id: number): Promise<any> {
    let reqUrl = this.detailUrl + '?subbarid=' + id;
    return this.http.get(reqUrl)
      .toPromise()
      .then(response => response.json().results)
      .catch(this.handleError);
  }

  getGraph(): Promise<any> {
    console.log(this.graphUrl);
    return this.http.get(this.graphUrl)
      .toPromise()
      .then(response => response.json().results)
      .catch(this.handleError);
  }
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }

}
