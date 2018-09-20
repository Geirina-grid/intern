import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import 'rxjs/add/operator/toPromise';
import { environment } from '../../../environments/environment';
import * as yaml from 'yamljs';
import { Subject } from 'rxjs/Subject';

@Injectable()
export class WholeGraphService {
  private headers = new Headers({ 'Content-Type': 'application/json' });
  //  private graphUrl = 'http://192.168.2.3:9000/query/graph_vis';
  private gsqlPre = environment.gsqlPre;
  private pyPre = environment.pyPre;
  // private graphUrl = 'http://localhost:8080/gsqlapi/query/graph_vis';
  private nodelabelAnnouncedSource = new Subject<any>();
  nodelabelAnnounced$ = this.nodelabelAnnouncedSource.asObservable();
  private linklabelAnnouncedSource = new Subject<any>();
  linklabelAnnounced$ = this.linklabelAnnouncedSource.asObservable();
  subscriptionSource = new Subject<any>();
  newSubscription$ = this.subscriptionSource.asObservable();
  constructor(private http: Http) {
  }

  // get graph data (nodes, links) from gsql
  getGraph(): Promise<any> {
    const graphUrl = this.gsqlPre + '/query/graph_vis_D';
    return this.http.get(graphUrl)
      .toPromise()
      .then(response => response.json().results)
      .catch(this.handleError);
  }

  // get schema from gsql
  getSchema(): Promise<any> {
    const schemaUrl = this.gsqlPre + '/graph/schema';
    return this.http.get(schemaUrl)
      .toPromise()
      .then(resp => {
        let res = yaml.parse(resp.text());
        console.log(res);
        return res;
      })
  }

  uploadGraph(formdata: FormData) {
    const graphUrl = this.pyPre + '/graph/';
    const fileheaders = new Headers();
    fileheaders.append('Content-Type', 'multipart/form-data');
    fileheaders.append('Accept', 'application/json');
    this.http.post(graphUrl, formdata, { headers: fileheaders })
      .toPromise()
      .then(resp => {console.log(resp)})
      .catch(this.handleError);
  }
  downloadGraph() {
    // url, prepare vertices and edges files
    const createUrl = this.pyPre + '/graph/';
    return this.http.get(createUrl)
      .toPromise()
      .then(resp => resp.json().error)
      .catch(this.handleError);

  }
  downloadVertex() {
    const vertexUrl = this.pyPre + '/vertex/';
    console.log('download vertex');
    return this.http.get(vertexUrl)
      .toPromise()
      .then(resp => {console.log(resp)});
  }
  downloadEdge() {
    const edgeUrl = this.pyPre + '/edge/';
    return this.http.get(edgeUrl);
  }


  announceNodeLabel(label: any) {
    console.log(label);
    this.nodelabelAnnouncedSource.next(label);
    console.log('new node label announced');
  }

  announceLinkLabel(label: any) {
    console.log(label);
    this.linklabelAnnouncedSource.next(label);
    console.log('new link label announced');
  }
  announceSubscription(isNotEmpty = true) {
    this.subscriptionSource.next(isNotEmpty);
  }

  // error handler
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }

}
