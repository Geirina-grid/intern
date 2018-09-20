import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import 'rxjs/add/operator/toPromise';
import { environment } from '../../environments/environment';
import * as yaml from 'yamljs';
import { Subject } from 'rxjs/Subject';

@Injectable()
export class TopologyService {
  private headers = new Headers({ 'Content-Type': 'application/json' });
  private detailPre = environment.detailPre
  
  private nodelabelAnnouncedSource = new Subject<any>();
  nodelabelAnnounced$ = this.nodelabelAnnouncedSource.asObservable();
  private linklabelAnnouncedSource = new Subject<any>();
  linklabelAnnounced$ = this.linklabelAnnouncedSource.asObservable();
  nodelabel: any;
  linklabel: any;
  constructor(private http: Http) { }


  // get graph data (nodes, links) from gsql
  getGraph(id: number): Promise<any> {
    const graphUrl = this.detailPre + '/query/SubDisplay?Substation_ID=' + id;
    return this.http.get(graphUrl)
               .toPromise()
               .then(response => response.json().results)
               .catch(this.handleError);
  }

  // get schema from gsql
  getSchema(): Promise<any> {
    const schemaUrl = this.detailPre + '/graph/gsql_demo';
    return this.http.get(schemaUrl)
               .toPromise()
               .then(resp => {
        let res = yaml.parse(resp.text());
        return res;
      })
  }


  announceNodeLabel(label: any) {

  }

  announceLinkLabel(label: any) {

  }

  // error handler
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }

}
