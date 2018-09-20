import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class ClusteringService {
  private pie = environment.pyPre+'/tl_service_pie/'
  private scatter = environment.pyPre+'/tl_service_scatter/'
  private line = environment.pyPre+'/tl_service_line/'
  private pie1 = environment.pyPre+'/tl_service_pie1/'
  private runalgo = environment.pyPre+'/run_tl/'
  private all_cent = environment.pyPre+'/tl_show_all/'
  private one_pie = environment.pyPre+'/typical_load_onepie/'
  private headers = new Headers({ 'Content-Type': 'application/json' });
  private phasechange = environment.pyPre+'/tl_service_idlisten/'
  constructor(private http:Http) { }

  get_phase(nid:number):Promise<any>{
    return this.http.post(
      this.phasechange,JSON.stringify({nid:nid}),{headers:this.headers}
    ).toPromise().then(
      res=>res.json().result
    ).catch(
      this.handleError
    )
  }
  plot_one_pie(cind:number):Promise<any>{
    return this.http.post(
      this.one_pie,JSON.stringify({cind:cind}),{headers:this.headers}
    ).toPromise().then(
      res=>res.json().result
    ).catch(
      this.handleError
    )
  }
  plot_all(nclus:number):Promise<any>{
    return this.http.post(
      this.all_cent,JSON.stringify({nclus:nclus}),{headers:this.headers}
    ).toPromise().then(
      res=>res.json().result
    ).catch(
      this.handleError
    );
  }

  plot_pie1(nid:number, rtype: number):Promise<any>{
    return this.http.post(
      this.pie1, JSON.stringify({nid:nid, rtype:rtype}),{headers:this.headers}
    ).toPromise().then(
      res=>res.json().result
    ).catch(
      this.handleError
    );
  }

  plot_line(nid:number, rtype: number, day: string, cind:number):Promise<any>{
    return this.http.post(
      this.line,JSON.stringify({nid:nid, rtype:rtype, day:day, cind:cind}),{ headers: this.headers }
    ).toPromise().then(
      res=>res.json().result
    ).catch(
      this.handleError
    );
  }
  plot_pie():Promise<any> {
    return this.http.post(
      this.pie,JSON.stringify({nothing:'nothing'}),{ headers: this.headers }
    ).toPromise()
    .then(res => res.json().result)
    .catch(this.handleError);
  }
  plot_scatter(nid:number, rtype:number):Promise<any>{
    return this.http.post(
      this.scatter,JSON.stringify({nid:nid,rtype:rtype}),{ headers: this.headers }
    ).toPromise()
    .then(res=>res.json().result)
    .catch(this.handleError);
  }
  run_algo(nclus:number):Promise<any>{
    return this.http.post(
      this.runalgo,JSON.stringify({nclus:nclus}),{ headers: this.headers }
    ).toPromise()
    .then(res=>{
      //alert("Success!")
      return res.json().result
      })
    .catch(this.handleError);
  }
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }  
}
