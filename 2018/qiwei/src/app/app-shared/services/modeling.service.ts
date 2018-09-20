// For now, it is copied from "graph-manage.service.ts"
import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import 'rxjs/add/operator/toPromise';

import { environment } from '../../../environments/environment';

@Injectable()
export class ModelingService {
  private headers = new Headers({ 'Content-Type': 'application/json' });
  private gsqlPre = environment.gsqlPre;
  private graphUrl = this.gsqlPre + '/graph';
  constructor(private http: Http) { }

  // this function called to post any change of graph, include add/delete/update a node/link
  postGraph(payload: any): Promise<any> {
    return this.http.post(this.graphUrl, payload, { headers: this.headers })
      .toPromise()
      .then(response => response.json())
      .catch(this.handleError);
  }

  // this function called to delete a node
  deleteNode(type: string, id: string): Promise<any> {
    const deleteUrl = this.graphUrl + '/vertices/' + type + '/' + id;
    return this.http.delete(deleteUrl)
      .toPromise()
      .then(resp => resp.json())
      .catch(this.handleError);
  }
  // this function called to delete a link

  deleteLink(sType: string, sId: string, eType: string, tType: string, tId: string): Promise<any> {
    const deleteUrl = this.graphUrl + '/edges/' + sType + '/' + sId + '/' + eType + '/' + tType + '/' + tId;
    return this.http.delete(deleteUrl)
      .toPromise()
      .then(resp => resp.json())
      .catch(this.handleError);
  }

  // getSchema(): Promise<any> {
  //   let schemaUrl = this.graphUrl + '/schema';
  //   return this.http.get(schemaUrl)
  //     .toPromise()
  //     .then(resp => resp.text())
  //     .catch(this.handleError);
  // }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }

}

