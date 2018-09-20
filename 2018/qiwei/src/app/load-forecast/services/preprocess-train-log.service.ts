import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import { Subject } from 'rxjs/Subject';

@Injectable()
export class PreprocessTrainLogService {

  private status = new Subject<any>();
  private result_preprocess = new Subject<any>();
  private result_tr = new Subject<any>();
  private result_tr2 = new Subject<any>();

  status$ = this.status.asObservable();
  result_preprocess$ = this.result_preprocess.asObservable();
  result_tr$ = this.result_tr.asObservable();
  result_tr2$ = this.result_tr2.asObservable();
  
    updateStatus (msg:any) {
      this.status.next(msg);
    }
  
    // getStatus (): Observable<any> {
    //   return this.status.asObservable()
    // }

    updateResultPreprocess (msg:any) {
      this.result_preprocess.next(msg);
    }
  
    // getResultPreprocess (): Observable<any> {
    //   return this.result_preprocess.asObservable()
    // }

    updateResultTr (msg:any) {
      this.result_tr.next(msg);
    }
  
    // getResultTr (): Observable<any> {
    //   return this.result_tr.asObservable()
    // }

    updateResultTr2 (msg:any) {
      this.result_tr2.next(msg);
    }
  
    // getResultTr2 (): Observable<any> {
    //   return this.result_tr2.asObservable()
    // }

}
