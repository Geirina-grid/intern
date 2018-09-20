import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import 'rxjs/add/operator/toPromise';

import { environment } from '../../../environments/environment';

@Injectable()
export class LoaddatagraphService {
  private headers = new Headers({ 'Content-Type': 'application/json' });
  private gsqlPre = environment.gsqlPre;
  private graphUrl = this.gsqlPre + '/query/powerflow_graph/read_load_data';

  constructor(private http: Http) { }

  // this function called to post any change of graph, include add/delete/update a node/link

  getMeterID(nodepoint: string, read_type: string): Promise<any> {
    return this.http.get(this.graphUrl + "?nodepoint=" + nodepoint + "&read_type=" + read_type)
      .toPromise()
      .then(response => response.json())
      .catch(this.handleError);
  }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }

}
