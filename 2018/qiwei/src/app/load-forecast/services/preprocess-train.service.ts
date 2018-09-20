import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import { Subject } from 'rxjs/Subject';

@Injectable()
export class PreprocessTrainService {

  private trainParam = new Subject<any>();
  
    // pageIndex$ = this.pageIndex.asObservable();
  
    // constructor() { }
  
    changeTrainParam (trainParam:any) {
      this.trainParam.next(trainParam);
    }
  
    getTrainParam (): Observable<any> {
      return this.trainParam.asObservable()
    }

}
